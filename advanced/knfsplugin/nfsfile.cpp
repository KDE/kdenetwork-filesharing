/***************************************************************************
                          nfsfile.cpp  -  description
                             -------------------
    begin                : Don Apr 25 2002
    copyright            : (C) 2002 by Jan Schäfer
    email                : janschaefer@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <pwd.h>
#include <time.h>
#include <unistd.h>

#include <qfileinfo.h>
#include <qfile.h>
#include <qtextstream.h>

#include <kdebug.h>
#include <kprocess.h>
#include <kmessagebox.h>
#include <klocale.h>


#include "nfsfile.h"

NFSFile::NFSFile(const KURL & url, bool readonly = true)
{
//  _entries.setAutoDelete(true);
  _url = url;
  _readonly = readonly;
}

NFSFile::~NFSFile()
{
}

void NFSFile::addEntry(NFSEntry *entry)
{
  _entries.append(entry);
}

void NFSFile::removeEntry(NFSEntry *entry)
{
  _entries.remove(entry);
}

bool NFSFile::hasEntry(NFSEntry *entry)
{
  return 0 < _entries.contains(entry);
}


NFSEntry* NFSFile::getEntryByPath(const QString & path)
{
	QString testPath = path.stripWhiteSpace();
  if (testPath.right(1)=="/")
  	  testPath.truncate(testPath.length()-1);

  for (NFSEntry* entry = _entries.first(); entry; entry = _entries.next())
  {
    if (entry->getPath()==testPath)
       return entry;
  }

  return 0L;
}

EntryIterator NFSFile::getEntries()
{
  return EntryIterator(_entries);
}


void NFSFile::load()
{
  QFile f(_url.path());

  if ( f.open(IO_ReadOnly) )
  {
    _entries.clear();

    QTextStream t( &f );
    QString s;
    QString ts;
    while ( !t.eof() )
    {

      QString s= "";
      do
      {

        ts = t.readLine().stripWhiteSpace();

        if (ts=="") continue; // Empty line ?

        if (ts.startsWith("#")) continue; // Comment ?


        s+=ts;

        if (s.endsWith("\\"))
           s=s.left(s.length()-1);
      }
      while (ts.endsWith("\\"));  // If line ends with \ append the next line

      QString path = s.left( s.find(" ",0)); // Get the path

      s = s.right( s.length()-path.length() ).stripWhiteSpace();

      if (s=="")
         continue;

      NFSEntry *entry = new NFSEntry(path, this);

      // Get the hosts
      while (s.length()>0)
      {
         QString hostStr = s.left( s.find(" ",0) );  // get next host

         if (hostStr.length()==0)
            break;

         new NFSHost(hostStr, entry);

         s = s.right( s.length()-hostStr.length() ).stripWhiteSpace();
      }

    }

    f.close();
  }



}

QString NFSFile::getTempFileName()
{
  QString username("???");
  struct passwd *user = getpwuid( getuid() );
  if ( user )
       username=user->pw_name;
  return QString("/tmp/knfsplugin-%1-%2-%3").arg(username).arg(getpid()).arg(time(0));
}


void NFSFile::save()
{

  QString tempFileName = getTempFileName();

  QFile tempF(tempFileName);
  QFile f(_url.path());

  // I use two files to preserve the user comments

  if ( f.open(IO_ReadOnly) )
  {
    if (tempF.open(IO_ReadWrite) )
    {
      QTextStream t( &f );
      QTextStream ttmp( &tempF );

      QString s;
      QString ts;
      QString origLine;

      EntryList list(_entries);

      while ( !t.eof() )
      {

        QString s= "";
        do
        {
          origLine = t.readLine();
          ts = origLine.stripWhiteSpace();

          // preserve comments
          if (ts.startsWith("#"))
          {
            ttmp << origLine << endl;
            continue;
          }

          s+=ts;
          if (s.endsWith("\\"))
             s=s.left(s.length()-1);
        }
        while (ts.endsWith("\\"));  // If line ends with \ append the next line

        if (s.stripWhiteSpace() == "")
           continue;

        QString path = s.left( s.find(" ",0)); // Get the path

        NFSEntry* entry = getEntryByPath(path);

        if (entry)
        {
          list.remove(entry);
          ttmp << entry->toString() << endl;;
        }
      }

      NFSEntry *entry;
      for ( entry = list.first(); entry; entry = list.next() )
          ttmp << entry->toString() << endl;

      tempF.close();
    } //~ tempF.open

    f.close();
  } //~ f.open



  if (QFileInfo(_url.path()).exists() && QFileInfo(tempFileName).exists())
  {

    QString command=QString("cp %1 %2; chmod 644 %3").arg(tempFileName).arg(_url.path()).arg(_url.path());

    if (restartNFSServer)
       command +=";exportfs -ra";

    KProcess *proc = new KProcess();

    if (!QFileInfo(_url.path()).isWritable() )
       *proc<<"kdesu"<<"-c"<<command;

    if (!proc->start(KProcess::Block))
      KMessageBox::sorry(0,i18n("Saving the results to %1 failed.").arg(_url.path()));

    delete(proc);
  }

  unlink(tempFileName);


}

QString NFSFile::guessPath()
{
  if (QFileInfo("/etc/exports").exists())
      return "/etc/exports";

  return "";
}
