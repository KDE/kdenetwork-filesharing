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

#ifndef SAMBAFILE_H
#define SAMBAFILE_H


#include <qdict.h>
#include <qstring.h>
#include <qobject.h>
#include <kio/job.h>

#include "sambashare.h"


class KSimpleConfig;
class KProcess;
class KConfig;
class KTempFile;

class SambaFile;

class SambaFileLoadException
{
  public:
    SambaFileLoadException(const QString & msg) { message = msg; };
    
    QString getMessage() { return message; };
  protected:
    QString message;
};

class SambaConfigFile : public QDict<SambaShare>
{
public:
  SambaConfigFile(SambaFile*);
  QString getDefaultValue(const QString & name);
  QStringList getShareList();
  void addShare(const QString & name, SambaShare* share);
  SambaShare* addShare(const QString & name);
  void removeShare(const QString & name);
protected:
  SambaFile* _sambaFile;
  QStringList _shareList;
};

class SambaFile : public QObject
{
Q_OBJECT
public: 
	SambaFile(const QString & _path, bool _readonly=true);
	~SambaFile();

  /** No descriptions */
  QString findShareByPath(const QString & path) const;
  void removeShareByPath(const QString & path);
  
  
  SambaShare* newShare(const QString & name);
  SambaShare* newShare(const QString & name, const QString & path);
  SambaShare* newPrinter(const QString & name, const QString & printer);

  void removeShare(const QString & share);
  void removeShare(SambaShare* share);

	void renameShare(const QString & oldName, const QString & newName);

  SambaShare* getShare(const QString & share) const;
  
  /**
   * Returns a list of all shared directories
   **/
  SambaShareList* getSharedDirs() const;

  /**
   * Returns a list of all shared printers
   **/
  SambaShareList* getSharedPrinters() const;

  /**
   * Returns a name which isn't already used for a share
   * if the alreadyUsedName parameter is given, then
   * a name based on this name is returned.
   * E.g.: if public is already used, the method could return 
   * public2
   **/
	QString getUnusedName(const QString alreadyUsedName=QString::null) const;

  /**
   * Returns all values of the global section
   * which are returned by the testparam program
   * if the values were already loaded then these
   * values are returned except the reload parameter
   * is true
   **/
  SambaShare* getTestParmValues(bool reload=false);

  static QString findSambaConf();


  int getSambaVersion();

  /**
   * Load all data from the smb.conf file
   * Can load a remote file
   **/
  bool load();

  bool save();
  
  /**
   * Reads the local smb.conf
   */
  bool openFile();
  
  bool isRemoteFile();

  /**
   * Save all data to the specified file
   * if successful returns true otherwise false
   **/
  bool saveTo(const QString & path);

protected:
  bool readonly;
  bool changed;
  QString path;
  QString localPath;
  SambaConfigFile *_sambaConfig;
  SambaShare* _testParmValues;
  QString _parmOutput;
  int _sambaVersion;

  void parseParmStdOutput();
  SambaConfigFile* getSambaConfigFile(KSimpleConfig* config);
  KSimpleConfig* getSimpleConfig(SambaConfigFile* sambaConfig, const QString & filename);



private:
  void copyConfigs(KConfig* first, KConfig* second);
  KTempFile * _tempFile;
public slots:

  /**
   * Saves all changes to the smb.conf file
   * if the user is not allowed she'll be asked
   * for a root password
   **/
  bool slotApply();
protected slots:
  void testParmStdOutReceived(KProcess *proc, char *buffer, int buflen);
  void slotJobFinished( KIO::Job *);
  void slotSaveJobFinished( KIO::Job *);

signals:
  void canceled(const QString &);
  void completed();  
  
};

#endif
