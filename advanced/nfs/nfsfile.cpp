/*
  Copyright (c) 2004 Jan Schaefer <j_schaef@informatik.uni-kl.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include <pwd.h>
#include <time.h>
#include <unistd.h>

#include <qfileinfo.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qstringlist.h>

#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <knfsshare.h>
#include <ktempfile.h>
#include <kprocio.h>

#include "nfsfile.h"

NFSFile::NFSFile(const KURL & url, bool readonly)
{
  _lines.setAutoDelete(true);
  _entries.setAutoDelete(false);
  _url = url;
  _readonly = readonly;
}

NFSFile::~NFSFile()
{
}

void NFSFile::addEntry(NFSEntry *entry)
{
  _lines.append(entry);
  _entries.append(entry);
}

void NFSFile::removeEntry(NFSEntry *entry)
{
  _entries.remove(entry);
  _lines.remove(entry);
}

bool NFSFile::hasEntry(NFSEntry *entry)
{
  return 0 < _entries.contains(entry);
}


NFSEntry* NFSFile::getEntryByPath(const QString & path)
{
  QString testPath = path.stripWhiteSpace();
  if ( testPath[testPath.length()-1] != '/' )
       testPath += '/';
  
  for (NFSEntry* entry = _entries.first(); entry; entry = _entries.next())
  {
    if (entry->path()==testPath)
       return entry;
  }

  return 0L;
}

bool NFSFile::removeEntryByPath(const QString & path) {
  NFSEntry* entry = getEntryByPath(path);
  if (!entry) 
      return false;
      
  removeEntry(entry);      
  return true;
}

EntryIterator NFSFile::getEntries()
{
  return EntryIterator(_entries);
}




bool NFSFile::load()
{
  QFile f(_url.path());

  if ( !f.open(IO_ReadOnly) ) {
    kdError() << "NFSFile::load: Could not open " << _url.path() << endl;
    return false;
  }
  
  _entries.clear();
  _lines.clear();

  QTextStream s( &f );
    
  bool continuedLine = false; // is true if the line before ended with a backslash
  QString completeLine;
  
  while ( !s.eof() )
  {
    QString currentLine = s.readLine().stripWhiteSpace();

    if (continuedLine) {
      completeLine += currentLine;
      continuedLine = false;
    }      
    else
      completeLine = currentLine;
    
    // is the line continued in the next line ?
    if ( completeLine[completeLine.length()-1] == '\\' )
    {
      continuedLine = true;
      // remove the ending backslash
      completeLine.truncate( completeLine.length()-1 ); 
      continue;
    }
      
    // empty lines
    if (completeLine.isEmpty()) {
      _lines.append(new NFSEmptyLine()); 
      continue;
    }
    
    // comments
    if ('#' == completeLine[0]) {
      _lines.append(new NFSComment(completeLine));
      continue;
    }
    
    QString path;
    QString hosts;
    
    // Handle quotation marks
    if ( completeLine[0] == '"' ) {
      int i = completeLine.find('"',1);
      if (i == -1) {
        kdError() << "NFSFile: Parse error: Missing quotation mark: " 
                  << completeLine << endl;   
        continue;
      }
      path = completeLine.mid(1,i-1);
      hosts = completeLine.mid(i+1);
      
    } else { // no quotation marks
      int i = completeLine.find(' ');
      if (i == -1)
          i = completeLine.find('\t');
      
      if (i == -1) 
        path = completeLine;
      else {
        path = completeLine.left(i);
        hosts = completeLine.mid(i+1).stripWhiteSpace();
      }          
    }        
    
    // normalize path
    if ( path[path.length()-1] != '/' )
            path += '/';
    
    kdDebug(5009) << "KNFSShare: Found path: '" << path << "'" << endl;
    NFSEntry *entry = new NFSEntry(path);
    QStringList hostList = QStringList::split(' ', hosts);
    
    if (hostList.isEmpty()) {
        NFSHost* host = new NFSHost("*");          
        entry->addHost(host);
    } else {
      QStringList::iterator it;
      for ( it = hostList.begin(); it != hostList.end(); ++it ) {
         NFSHost* host = new NFSHost((*it).stripWhiteSpace());          
         entry->addHost(host);
         kdDebug(5009) << "KNFSShare: Found host: " << (*it) << " name='"
                  << host->name << "'" << endl;
      }          
    }
    
    kdDebug(5009) << "KNFSShare: Found hosts: " << hosts << "'" << endl;
    this->addEntry(entry);
  }

  f.close();
  

  return true;

}

void NFSFile::saveTo(QTextStream * stream) {
  QPtrListIterator<NFSLine> it(_lines);
  
  NFSLine *line;
  while ( (line = it.current()) != 0 ) {
    ++it;
    *stream << line->toString() << endl;
  }
}

bool NFSFile::saveTo(const QString& fileName) {
    QFile file(fileName);
    if (!file.open(IO_WriteOnly))
        return false;
        
    QTextStream stream(&file);        
    saveTo(&stream);
    file.close();
    return true;
}

bool NFSFile::save()
{
  if (QFileInfo(_url.path()).isWritable() ) {
    saveTo(_url.path());
  } else
  {
  
    KTempFile tempFile;
    saveTo(tempFile.name());
    tempFile.close();
    tempFile.setAutoDelete( true );

    KProcIO proc;

    QString command = QString("cp %1 %2")
        .arg(KProcess::quote( tempFile.name() ))
        .arg(KProcess::quote( _url.path() ));

    if (restartNFSServer)
      command +=";exportfs -ra";

    if (!QFileInfo(_url.path()).isWritable() )
      proc<<"kdesu" << "-d" << "-c"<<command;

    if (!proc.start(KProcess::Block, true)) {
      return false;
    }      


  }    
  return true;
}

