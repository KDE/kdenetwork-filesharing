/*
  Copyright (c) 2002-2004 Jan Schaefer <j_schaef@informatik.uni-kl.de>

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

#include <qfile.h>
#include <qprocess.h>
#include <qfileinfo.h>

#include <ksimpleconfig.h>
#include <kdebug.h>
#include <qfileinfo.h>
#include <kio/job.h>
#include <kprocess.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <ktempfile.h>
#include <ksambashare.h>

#include <pwd.h>
#include <time.h>
#include <unistd.h>

#include "sambafile.h"

#define FILESHARE_DEBUG 5009

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

SambaShare* SambaConfigFile::addShare(const QString & name)
{
  SambaShare* newShare = new SambaShare(name,this);
  addShare(name,newShare);
  return newShare;
}


void SambaConfigFile::addShare(const QString & name, SambaShare* share)
{
  insert(name,share),
  _shareList.append(name);
}

void SambaConfigFile::removeShare(const QString & name)
{
  remove(name);
  _shareList.remove(name);
}


QStringList SambaConfigFile::getShareList()
{
  return _shareList;
}

SambaFile::SambaFile(const QString & _path, bool _readonly)
  : readonly(_readonly), 
    changed(false), 
    path(_path), 
    localPath(_path), 
    _sambaConfig(0),
    _testParmValues(0),
    _sambaVersion(-1),
    _tempFile(0)
{
}

SambaFile::~SambaFile()
{
  delete _sambaConfig;
  delete _testParmValues;
  delete _tempFile;

}

bool SambaFile::isRemoteFile() {
  return ! KURL(path).isLocalFile();
}

/** No descriptions */
QString SambaFile::findShareByPath(const QString & path) const
{
  QDictIterator<SambaShare> it(*_sambaConfig);
  KURL url(path);
  url.adjustPath(-1);

  for (  ; it.current(); ++it )
  {
    SambaShare* share = it.current();

    QString *s = share->find("path");
    if (s) {
        KURL curUrl(*s);
        curUrl.adjustPath(-1);

        kdDebug(5009) << url.path() << " =? " << curUrl.path() << endl;

        if (url.path() == curUrl.path())
            return it.currentKey();
    }
  }

  return QString::null;
}

bool SambaFile::save() {
  return slotApply();
}


bool SambaFile::slotApply()
{
  if (readonly) {
      kdDebug(FILESHARE_DEBUG) << "SambaFile::slotApply: readonly=true" << endl;
      return false;
  }      

  // If we have write access to the smb.conf
  // we simply save the values to it
  // if not we have to save the results in
  // a temporary file and copy it afterwards
  // over the smb.conf file with kdesu.
  if (QFileInfo(path).isWritable())
  {
    saveTo(path);
    changed = false;
    return true;
  }

  // Create a temporary smb.conf file
   delete _tempFile;
  _tempFile = new KTempFile();
  _tempFile->setAutoDelete(true);

  if (!saveTo(_tempFile->name())) {
    kdDebug(5009) << "SambaFile::slotApply: Could not save to temporary file" << endl; 
    delete _tempFile;
    _tempFile = 0;
    return false;
  }

  QFileInfo fi(path);
  KURL url(path);

  if (KURL(path).isLocalFile()) {
    KProcess proc;
    kdDebug(5009) << "SambaFile::slotApply: is local file!" << endl;
    
    QString suCommand=QString("cp %1 %2; rm %3")
              .arg(_tempFile->name())
              .arg(path)
              .arg(_tempFile->name());
    proc << "kdesu" << "-d" << suCommand;

    if (! proc.start(KProcess::Block)) {
        kdDebug(5009) << "SambaFile::slotApply: saving to " << path << " failed!" << endl;
        //KMessageBox::sorry(0,i18n("Saving the results to %1 failed.").arg(path));
        delete _tempFile;
        _tempFile = 0;
        return false;
    }        
    else {
        changed = false;
        delete _tempFile;
        _tempFile = 0;
        kdDebug(5009) << "SambaFile::slotApply: changes successfully saved!" << endl;
        return true;
    }        
  } else {
    kdDebug(5009) << "SambaFile::slotApply: is remote file!" << endl;
    _tempFile->setAutoDelete(true);
    KURL srcURL;
    srcURL.setPath( _tempFile->name() );

    KIO::FileCopyJob * job =  KIO::file_copy( srcURL, url, -1, true  );
    connect( job, SIGNAL( result( KIO::Job * ) ), 
             this, SLOT( slotSaveJobFinished ( KIO::Job * ) ) );
    return (job->error()==0);
  }

  return true;
}

  /**
  * Returns a name which isn't already used for a share
  **/
QString SambaFile::getUnusedName(const QString alreadyUsedName) const
{

  QString init = i18n("Unnamed");
  if (alreadyUsedName != QString::null)
    init = alreadyUsedName;

  QString s = init;

  int i = 2;

  while (_sambaConfig->find(s))
  {
    s = init+QString::number(i);
    i++;
  }

  return s;
}



SambaShare* SambaFile::newShare(const QString & name)
{
  if (_sambaConfig->find(name))
    return 0L;

  SambaShare* share = new SambaShare(name,_sambaConfig);
  _sambaConfig->addShare(name,share);

  changed = true;

  return share;

}

SambaShare* SambaFile::newShare(const QString & name, const QString & path)
{
  SambaShare* share = newShare(name);
  if (share)
  {
    share->setValue("path",path);
  }

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

  _sambaConfig->removeShare(share);
}

void SambaFile::removeShare(SambaShare* share)
{
  removeShare(share->getName());
}

void SambaFile::removeShareByPath(const QString & path) {
  QString share = findShareByPath(path);
  removeShare(share);
}

/** No descriptions */
SambaShare* SambaFile::getShare(const QString & share) const
{
  SambaShare *s = _sambaConfig->find(share);

  return s;
}

/**
* Returns a list of all shared directories
**/
SambaShareList* SambaFile::getSharedDirs() const
{
  SambaShareList* list = new SambaShareList();

  QDictIterator<SambaShare> it(*_sambaConfig);

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

  QDictIterator<SambaShare> it(*_sambaConfig);

  for( ; it.current(); ++it )
  {
    if (it.current()->isPrinter())
      list->append(it.current());
  }

  return list;
}

int SambaFile::getSambaVersion() {
  if (_sambaVersion > -1)
    return _sambaVersion;
    
  KProcess testParam;
  testParam << "testparm";
  testParam << "-V";
  _parmOutput = QString("");
  _sambaVersion = 2;
      
  connect( &testParam, SIGNAL(receivedStdout(KProcess*,char*,int)),
          this, SLOT(testParmStdOutReceived(KProcess*,char*,int)));

          
          
  if (testParam.start(KProcess::Block,KProcess::Stdout)) {
    if (_parmOutput.find("3") > -1)
      _sambaVersion = 3;
  } 

  kdDebug(5009) << "Samba version = " << _sambaVersion << endl;
    
  return _sambaVersion;
}


SambaShare* SambaFile::getTestParmValues(bool reload)
{
  if (_testParmValues && !reload)
    return _testParmValues;


  KProcess testParam;
  testParam << "testparm";
  testParam << "-s"; 
  
  if (getSambaVersion() == 3)
     testParam << "-v";


  testParam << "/dev/null";
  _parmOutput = QString("");
  
  connect( &testParam, SIGNAL(receivedStdout(KProcess*,char*,int)),
          this, SLOT(testParmStdOutReceived(KProcess*,char*,int)));

  if (testParam.start(KProcess::Block,KProcess::Stdout))
  {
    parseParmStdOutput();
  } else
    _testParmValues = new SambaShare(_sambaConfig);

  return _testParmValues;
}

void SambaFile::testParmStdOutReceived(KProcess *, char *buffer, int buflen)
{
  _parmOutput+=QString::fromLatin1(buffer,buflen);
}

void SambaFile::parseParmStdOutput()
{

  QTextIStream s(&_parmOutput);

  if (_testParmValues)
    delete _testParmValues;
  _testParmValues = new SambaShare(_sambaConfig);

  QString section="";

  while (!s.atEnd())
  {
    QString line = s.readLine().stripWhiteSpace();

    // empty lines
    if (line.isEmpty())
      continue;

    // comments
    if ('#' == line[0])
      continue;

    // sections
    if ('[' == line[0])
    {
      // get the name of the section
      section = line.mid(1,line.length()-2).lower();
      continue;
    }

    // we are only interested in the global section
    if (section != KGlobal::staticQString("global"))
      continue;

    // parameter
    // parameter
    int i = line.find('=');

    if (i>-1) {
      QString name = line.left(i).stripWhiteSpace();
      QString value = line.mid(i+1).stripWhiteSpace();
      _testParmValues->setValue(name,value,false,false);
    }
    
  }



}

/**
* Try to find the samba config file position
* First tries the config file, then checks
* several common positions
* If nothing is found returns QString::null
**/
QString SambaFile::findSambaConf()
{
    return KSambaShare::instance()->smbConfPath();
}

void SambaFile::slotSaveJobFinished( KIO::Job * job ) {
  delete _tempFile;
  _tempFile = 0;
}

void SambaFile::slotJobFinished( KIO::Job * job )
{
  if (job->error())
    emit canceled( job->errorString() );
  else
  {
    openFile();
    emit completed();
  }
}

bool SambaFile::load()
{
  if (path.isNull() || path.isEmpty())
      return false;
      
  kdDebug(FILESHARE_DEBUG) << "SambaFile::load: path=" << path << endl;
  KURL url(path);

  if (!url.isLocalFile()) {
    KTempFile tempFile;
    localPath = tempFile.name();
    KURL destURL;
    destURL.setPath( localPath );
    KIO::FileCopyJob * job =  KIO::file_copy( url, destURL, 0600, true, false, true );
//    emit started( d->m_job );
    connect( job, SIGNAL( result( KIO::Job * ) ), this, SLOT( slotJobFinished ( KIO::Job * ) ) );
    return true;
  } else {
    localPath = path;
    bool ret = openFile();
    if (ret)
        emit completed();
    return ret;
  }
}

bool SambaFile::openFile() {

  QFile f(localPath);

  if (!f.open(IO_ReadOnly)) {
    //throw SambaFileLoadException(QString("<qt>Could not open file <em>%1</em> for reading.</qt>").arg(path));
    return false;
  }

  QTextStream s(&f);

  delete _sambaConfig;

  _sambaConfig = new SambaConfigFile(this);

  SambaShare *currentShare = 0L;
  bool continuedLine = false; // is true if the line before ended with a backslash
  QString completeLine;
  QStringList comments;

  while (!s.eof())
  {
    QString currentLine = s.readLine().stripWhiteSpace();

    if (continuedLine)
    {
      completeLine += currentLine;
      continuedLine = false;
    } else
      completeLine = currentLine;
            
    // is the line continued in the next line ?
    if ( completeLine[completeLine.length()-1] == '\\' )
    {
      continuedLine = true;
      // remove the ending backslash
      completeLine.truncate( completeLine.length()-1 ); 
      continue;
    }

    // comments or empty lines
    if (completeLine.isEmpty() ||
        '#' == completeLine[0] ||
        ';' == completeLine[0])
    {
      comments.append(completeLine);
      continue;
    }


    // sections
    if ('[' == completeLine[0])
    {
      // get the name of the section
      QString section = completeLine.mid(1,completeLine.length()-2);
      currentShare = _sambaConfig->addShare(section);
      currentShare->setComments(comments);
      comments.clear();

      continue;
    }

    // parameter
    int i = completeLine.find('=');

    if (i>-1)
    {
      QString name = completeLine.left(i).stripWhiteSpace();
      QString value = completeLine.mid(i+1).stripWhiteSpace();

      if (currentShare)
      {
        currentShare->setComments(name,comments);
        currentShare->setValue(name,value,true,true);

        comments.clear();
      }
    }
  }

  f.close();

  // Make sure there is a global share
  if (!getShare("global")) {
     _sambaConfig->addShare("global");
  }
  
  return true;  
}

bool SambaFile::saveTo(const QString & path)
{
  QFile f(path);

  if (!f.open(IO_WriteOnly))
    return false;

  QTextStream s(&f);

  QStringList shareList = _sambaConfig->getShareList();

  for ( QStringList::Iterator it = shareList.begin(); it != shareList.end(); ++it )
  {
    SambaShare* share = _sambaConfig->find(*it);

    // First add all comments of the share to the file
    QStringList comments = share->getComments();
    for ( QStringList::Iterator cmtIt = comments.begin(); cmtIt != comments.end(); ++cmtIt )
    {
      s << *cmtIt << endl;

      kdDebug(5009) << *cmtIt << endl;
    }

    // If there are no lines before the section add
    // a blank line
    if (comments.isEmpty())
      s << endl;

    // Add the name of the share / section
    s << "[" << share->getName() << "]" << endl;

    // Add all options of the share 
    QStringList optionList = share->getOptionList();

    for ( QStringList::Iterator optionIt = optionList.begin(); optionIt != optionList.end(); ++optionIt )
    {

      // Add the comments of the option
      comments = share->getComments(*optionIt);
      for ( QStringList::Iterator cmtIt = comments.begin(); cmtIt != comments.end(); ++cmtIt )
      {
        s << *cmtIt << endl;
      }

      // Add the option
      s << *optionIt << " = " << *share->find(*optionIt) << endl;
    }


  }

  f.close();

  return true;     
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
      if (!it2.data().isEmpty())
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
