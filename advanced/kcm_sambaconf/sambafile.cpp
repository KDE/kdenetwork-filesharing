/***************************************************************************
                          sambafile.cpp  -  description
                             -------------------
    begin                : Son Apr 14 2002
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



SambaConfigFile::SambaConfigFile()
{
  QDict<QString>(10,false);
  setAutoDelete(true);
}

SambaFile::SambaFile(const QString & _path, bool _readonly)
{
  path = _path;
  readonly = _readonly;
  changed = false;

	tmpFilename = "";

  if (!readonly && !QFileInfo(path).isWritable())
  {
    tmpFilename= getTempFileName();
		KProcess *proc = new KProcess();
    *proc << "cp" << path << tmpFilename;
    if (!proc->start(KProcess::Block))
    {
      KMessageBox::error(0,i18n("Error while trying to create %1.").arg(tmpFilename));
      readonly = true;
    	config = new KSimpleConfig(path,true);
    }
    else
    	config = new KSimpleConfig(tmpFilename,false);

  } else
    config = new KSimpleConfig(path,readonly);

  sambaConfig = getSambaConfigFile(config);
}

SambaFile::~SambaFile()
{
  if (tmpFilename!="")
    unlink(QFile::encodeName(tmpFilename));

}

QString SambaFile::getTempFileName()
{
  QString username("???");
  struct passwd *user = getpwuid( getuid() );
  if ( user )
       username=user->pw_name;
  return QString("/tmp/ksambakonqiplugin-%1-%2-%3").arg(username).arg(getpid()).arg(time(0));
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
  if (!changed || readonly)
      return;

	config->sync();

  if (!QFileInfo(path).isWritable() )
  {
    QString suCommand=QString("cp %1 %2; chmod 644 %3").arg(tmpFilename).arg(path).arg(path);
    KProcess *proc = new KProcess();
    connect(proc, SIGNAL(processExited(KProcess *)), this, SLOT(slotSaveDone(KProcess *)));
    *proc<<"kdesu"<<"-c"<<suCommand;
    if (!proc->start(KProcess::Block))
      KMessageBox::sorry(0,i18n("Saving the results to %1 failed.").arg(path));

    delete(proc);
  }

  changed = false;
}


void SambaFile::slotSaveDone(KProcess* proc)
{
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


void SambaFile::writeValue(const QString & share, const QString & optionName, bool value)
{
	writeValue(share,optionName,textFromBool(value));
}

void SambaFile::writeValue(const QString & share, const QString & optionName, const QString & value)
{
  SambaShare* s = sambaConfig->find(share);

  if (!s) return;

  QString *str = s->find(optionName);

  if (str && *str == value)
    return;

  config->setGroup(share);

  if (str)
  {
    if ( (optionName.lower()=="read only" && value.lower()=="yes") ||
         (optionName.lower()=="writeble" && value.lower()=="no") ||
         (optionName.lower()=="write ok" && value.lower()=="no") ||
         (optionName.lower()=="available" && value.lower()=="yes") ||
         (optionName.lower()=="guest ok" && value.lower()=="no") ||
         (optionName.lower()=="browseable" && value.lower()=="yes") ||
         (optionName.lower()=="browsable" && value.lower()=="yes"))
    {
      s->remove(optionName);
     	config->deleteEntry(optionName);

      changed = true;
      return;
    }
  }


  s->replace(optionName,new QString(value));
 	config->writeEntry(optionName,value);

  changed = true;
}

void SambaFile::renameShare(const QString & oldName, const QString & newName)
{
	if (sambaConfig->find(newName))
  	 return;

  SambaShare *oldShare = sambaConfig->find(oldName);

  if (!oldShare)
  	 return;

	config->deleteGroup(oldName);

  SambaShare *newShare = new SambaShare(sambaConfig);
  sambaConfig->insert(newName,newShare);
  config->setGroup(newName);
  
	QDictIterator<QString> it2(*oldShare);

  for (; it2.current(); ++it2 )
  {
     config->writeEntry(it2.currentKey(), *it2.current());
     newShare->insert(it2.currentKey(), new QString(*it2.current()));
  }
  
  sambaConfig->remove(oldName);



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
  config->deleteGroup(share);
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


bool SambaFile::readBoolValue(const QString & share, const QString & optionName)
{
	return boolFromText(readValue(share,optionName));
}

QString SambaFile::readValue(const QString & share, const QString & optionName)
{
  QString result = "";

  SambaShare *s = sambaConfig->find(share);

  if (s)
  {
      QString *tmp = s->find(optionName);

      if (tmp && !tmp->isEmpty() && *tmp != "")
      {
         result = QString(*tmp);
      }
      else	// If we didn't find a value try if any of the synonyms match
      {     // if this doesn't work take the default value
        if (optionName.lower()=="read only")
        {
        	// Try synonyms
  	 			result = readValue(share,"writeable");

          if (result=="")
		 			   result = readValue(share,"write ok");

          if (result != "")
          {
          	if (boolFromText(result.lower())==true )
             	result="no";
            else
             	result="yes";
          }

					// Take default value
         	if (result=="")
          	 result="yes";
  			}

				if (optionName.lower()=="hosts deny")
        {
        	 result = readValue(share,"deny hosts");
        }

				if (optionName.lower()=="hosts allow")
        {
        	 result = readValue(share,"allow hosts");
        }

        if (optionName.lower()=="browseable")
        {
        	result = readValue(share,"browsable");

         	if (result=="")
 					   result = "yes";
        }


        if (optionName.lower()=="available") result = "yes";

        if (optionName.lower()=="guest ok")
        {
          result = readValue(share,"public");
        	if (result=="")
          	result = "no";
        }

        if (optionName.lower()=="guest account") result = "nobody";
      }
  } else
    kdDebug() << "share : " << share << "not found" << endl;

  return result;
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

  SambaConfigFile* samba = new SambaConfigFile();

  for ( QStringList::Iterator it = groups.begin(); it != groups.end(); ++it )
  {
    QMap<QString,QString> entries = config->entryMap(*it);

    SambaShare *share = new SambaShare(*it,samba);
    samba->insert(*it,share);

    for (QMap<QString,QString>::Iterator it2 = entries.begin(); it2 != entries.end(); ++it2 )
    {
       if (it2.data()!="")
          share->setValue(it2.key(),QString(it2.data()));
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
