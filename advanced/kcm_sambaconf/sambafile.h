/***************************************************************************
                          sambafile.h  -  description
                             -------------------
    begin                : Son Apr 14 2002
    copyright            : (C) 2002 by Jan Schäfer
    email                : janschaefer@users.sourceforge.net
 ***************************************************************************/

/******************************************************************************
 *                                                                            *
 *  This file is part of KSambaPlugin.                                          *
 *                                                                            *
 *  KSambaPlugin is free software; you can redistribute it and/or modify            *
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation; either version 2 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  KSambaPlugin is distributed in the hope that it will be useful,                 *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with Foobar; if not, write to the Free Software                     *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA  *
 *                                                                            *
 ******************************************************************************/

#ifndef SAMBAFILE_H
#define SAMBAFILE_H


/**
  *@author Jan Schäfer
  */

#include <qdict.h>
#include <qstring.h>
#include <qobject.h>
#include <sambashare.h>

class KSimpleConfig;
class KProcess;
class KConfig;



class SambaConfigFile : public QDict<SambaShare>
{
public:
  SambaConfigFile();
};



class SambaFile : public QObject
{
Q_OBJECT
public: 
	SambaFile(const QString & _path, bool _readonly=true);
	~SambaFile();

  /** No descriptions */
  QString findShareByPath(const QString & path) const;

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
   **/
	QString getUnusedName() const;

  static QString findSambaConf();

	static bool boolFromText(const QString & value);
  static QString textFromBool(bool value);

protected:
  bool readonly;
  bool changed;
  QString path;
  SambaConfigFile *sambaConfig;

  SambaConfigFile* getSambaConfigFile(KSimpleConfig* config);
  KSimpleConfig* getSimpleConfig(SambaConfigFile* sambaConfig, const QString & filename);


private:
  void copyConfigs(KConfig* first, KConfig* second);
	QString getTempFileName();

public slots: // Public slots

  /**
   * Saves all changes to the smb.conf file
   * if the user is not allowed she'll be asked
   * for a root password
   **/
  void slotApply();
};

#endif
