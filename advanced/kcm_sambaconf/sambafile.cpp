/***************************************************************************
                          sambafile.cpp  -  description
                             -------------------
    begin                : Son Apr 14 2002
    copyright            : (C) 2002 by Jan Schäfer
    email                : janschaefer@users.sourceforge.net
 ***************************************************************************/

/******************************************************************************
 *                                                                            *
 *  This file is part of KSambaPlugin.                                        *
 *                                                                            *
 *  KSambaPlugin is free software; you can redistribute it and/or modify      *
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation; either version 2 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  KSambaPlugin is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with KSambaPlugin; if not, write to the Free Software                     *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA  *
 *                                                                            *
 ******************************************************************************/

#include <ksimpleconfig.h>
#include <kdebug.h>
#include <qfileinfo.h>
#include <kio/job.h>
#include <kprocess.h>
#include <kmessagebox.h>
#include <klocale.h>

#include <pwd.h>
#include <time.h>
#include <unistd.h>

#include "sambafile.h"



SambaConfigFile::SambaConfigFile(SambaFile* sambaFile)
{
  QDict<QString>(10,false);
  setAutoDelete(true);
  _sambaFile = sambaFile;
}

QString SambaConfigFile::getDefaultValue(const QString & name)
{
  SambaShare* defaults = _sambaFile->getTestParmValues();
  QString s = defaults->getValue(name,false,false);

  return s;
}

SambaFile::SambaFile(const QString & _path, bool _readonly)
{
  path = _path;
  readonly = _readonly;
  changed = false;
  _testParmValues = 0L;

  KSimpleConfig *config = new KSimpleConfig(path,readonly);

  sambaConfig = getSambaConfigFile(config);
}

SambaFile::~SambaFile()
{
	delete sambaConfig;
  if (_testParmValues)
     delete _testParmValues;

}

QString SambaFile::getTempFileName()
{
  QString username("???");
  struct passwd *user = getpwuid( getuid() );
  if ( user )
       username=user->pw_name;
  return QString("/tmp/ksambaplugin-%1-%2-%3").arg(username).arg(getpid()).arg(time(0));
}

/** No descriptions */
QString SambaFile::findShareByPath(const QString & path) const
{
  QDictIterator<SambaShare> it(*sambaConfig);
  for (  ; it.current(); ++it )
  {
    SambaShare* share = it.current();

    QString *s = share->find("path");

    if (s && *s==path)
        return it.currentKey();
  }

  return QString();
}

void SambaFile::slotApply()
{
  if (readonly)
      return;


  // Create a temporary smb.conf file
  QString tmpFilename= getTempFileName();

  KSimpleConfig* config = 0L;

/*
  KProcess *proc = new KProcess();
  *proc << "cp" << path << tmpFilename;
  if (!proc->start(KProcess::Block))
  {
    KMessageBox::error(0,i18n("Error while trying to create %1.").arg(tmpFilename));
    return;
  }
  else
*/
  config = getSimpleConfig(sambaConfig, tmpFilename);
  config->sync();

  // Override the original smb.conf with the temporary file
  QString suCommand=QString("cp %1 %2; chmod 644 %3").arg(tmpFilename).arg(path).arg(path);
  KProcess* proc = new KProcess();

  // if file is not writable do a kdesu
  if (!QFileInfo(path).isWritable() )
    *proc<<"kdesu"<<"-c"<<suCommand;

  if (!proc->start(KProcess::Block))
    KMessageBox::sorry(0,i18n("Saving the results to %1 failed.").arg(path));

  delete(proc);

  // Remove the temporary file
  unlink(QFile::encodeName(tmpFilename));
  
  delete(config);
  changed = false;
}



  /**
   * Returns a name which isn't already used for a share
   **/
QString SambaFile::getUnusedName() const
{
	QString init = i18n("Unnamed");
  QString s = init;

  int i = 1;

  while (sambaConfig->find(s))
  {
		s = init+QString("%1").arg(i);
    i++;
  }
  
  return s;
}



SambaShare* SambaFile::newShare(const QString & name)
{
  if (sambaConfig->find(name))
     return 0L;

  SambaShare* share = new SambaShare(name,sambaConfig);
  sambaConfig->insert(name,share);

  changed = true;

  return share;

}

SambaShare* SambaFile::newShare(const QString & name, const QString & path)
{
	SambaShare* share = newShare(name);

	if (share)
	   share->setValue("path",path);

  return share;
}

SambaShare* SambaFile::newPrinter(const QString & name, const QString & printer)
{
	SambaShare* share = newShare(name);

	if (share)
  {
	  share->setValue("printable",true);
  	share->setValue("printer name",printer);
  }

  return share;
}


/** No descriptions */
void SambaFile::removeShare(const QString & share)
{
  changed = true;

  sambaConfig->remove(share);
}

void SambaFile::removeShare(SambaShare* share)
{
	removeShare(share->getName());
}

/** No descriptions */
SambaShare* SambaFile::getShare(const QString & share) const
{
	SambaShare *s = sambaConfig->find(share);

  return s;
}

/**
 * Returns a list of all shared directories
 **/
SambaShareList* SambaFile::getSharedDirs() const
{
	SambaShareList* list = new SambaShareList();

	QDictIterator<SambaShare> it(*sambaConfig);

  for( ; it.current(); ++it )
  {
    if (!it.current()->isPrinter() &&
    		 it.current()->getName() != "global")
    {
			list->append(it.current());
    }
  }

  return list;
}

/**
 * Returns a list of all shared printers
 **/
SambaShareList* SambaFile::getSharedPrinters() const
{
	SambaShareList* list = new SambaShareList();

	QDictIterator<SambaShare> it(*sambaConfig);

  for( ; it.current(); ++it )
  {
    if (it.current()->isPrinter())
			list->append(it.current());
  }

  return list;
}


SambaShare* SambaFile::getTestParmValues(bool reload)
{
  if (_testParmValues && !reload)
     return _testParmValues;


  KProcess testParam;
  testParam.setExecutable("testparm");
  testParam << "-s";

  connect( &testParam, SIGNAL(receivedStdout(KProcess*,char*,int)),
           this, SLOT(testParmStdOutReceived(KProcess*,char*,int)));

  if (testParam.start(KProcess::Block,KProcess::Stdout))
  {
    kdDebug() << "testparm successfully started" << endl;
    parseParmStdOutput();
  }
  else
    kdDebug() << "testparm failed !" << endl;

  return _testParmValues;
}

void SambaFile::testParmStdOutReceived(KProcess *proc, char *buffer, int buflen)
{
  _parmOutput+=QString::fromLatin1(buffer,buflen);
}

void SambaFile::parseParmStdOutput()
{

  QTextIStream s(&_parmOutput);

  if (_testParmValues)
     delete _testParmValues;
  _testParmValues = new SambaShare(sambaConfig);

  QString section="";



  while (!s.atEnd())
  {
    QString line = s.readLine().stripWhiteSpace();
    QString name="";
    QString value="";

    // empty lines
    if (line.isEmpty())
       continue;

    // comments
    if ("#" == line.left(1))
       continue;

    // sections
    if ("[" == line.left(1))
    {
      // get the name of the section
      section = line.mid(1,line.length()-2);
      section = section.lower();
      continue;
    }

    // we are only interested in the global section
    if (section != "global")
       continue;

    // parameter
    name = QStringList::split("=",line)[0].stripWhiteSpace();
    value = QStringList::split("=",line)[1].stripWhiteSpace();

    _testParmValues->setValue(name,value,false,false);
  }



}

/**
 * Try to find the samba config file position
 * First tries the config file, then checks
 * several common positions
 * If nothing is found returns ""
 **/
QString SambaFile::findSambaConf()
{
	KConfig config("ksambaplugin");
	// Perhaps the user has defined the path
	config.setGroup("KSambaKonqiPlugin");
  QString sambaConf = config.readEntry("smb.conf");

  if ( QFileInfo(sambaConf).exists() )
     return sambaConf;

  if ( QFileInfo("/etc/samba/smb.conf").exists() )
     return "/etc/samba/smb.conf";

  if ( QFileInfo("/etc/smb.conf").exists() )
     return "/etc/smb.conf";

  if ( QFileInfo("/usr/local/samba/lib/smb.conf").exists() )
     return "/usr/local/samba/lib/smb.conf";

  if ( QFileInfo("/usr/samba/lib/smb.conf").exists() )
     return "/usr/samba/lib/smb.conf";


  return "";
}



bool SambaFile::boolFromText(const QString & value)
{
	if (value.lower()=="yes" ||
  	  value.lower()=="1" ||
      value.lower()=="true")
    return true;
	else
  	return false;
}

QString SambaFile::textFromBool(bool value)
{
	if (value)
  	 return "yes";
  else
  	 return "no";
}

SambaConfigFile* SambaFile::getSambaConfigFile(KSimpleConfig* config)
{
  QStringList groups = config->groupList();

  SambaConfigFile* samba = new SambaConfigFile(this);

  for ( QStringList::Iterator it = groups.begin(); it != groups.end(); ++it )
  {
    QMap<QString,QString> entries = config->entryMap(*it);

    SambaShare *share = new SambaShare(*it,samba);
    samba->insert(*it,share);

    for (QMap<QString,QString>::Iterator it2 = entries.begin(); it2 != entries.end(); ++it2 )
    {
       if (it2.data()!="")
          share->setValue(it2.key(),QString(it2.data()),false,false);
    }

  }

  return samba;

}

KSimpleConfig* SambaFile::getSimpleConfig(SambaConfigFile* sambaConfig, const QString & path)
{
  KSimpleConfig *config = new KSimpleConfig(path,false);

  QDictIterator<SambaShare> it(*sambaConfig);

  for ( ; it.current(); ++it )
  {
    SambaShare* share = it.current();

    config->setGroup(it.currentKey());

    QDictIterator<QString> it2(*share);

    for (; it2.current(); ++it2 )
    {
       config->writeEntry(it2.currentKey(), *it2.current());
    }

  }

  return config;
}

#include "sambafile.moc"
