/***************************************************************************
                          sambafile.h  -  description
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

#ifndef SAMBAFILE_H
#define SAMBAFILE_H


/**
  *@author Jan Schäfer
  */

#include <qdict.h>
#include <qstring.h>
#include <qobject.h>


class KSimpleConfig;
class KProcess;
class KConfig;


class SambaShare :public QDict<QString>
{
public:
  SambaShare();
};

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

  /** No descriptions */
  void writeValue(const QString & share, const QString & optionName, const QString & value);
  void writeValue(const QString & share, const QString & optionName, bool value);
  
  bool newShare(const QString & name, const QString & path);
  void removeShare(const QString & share);
	void renameShare(const QString & oldName, const QString & newName);

  SambaShare getShare(const QString & share) const;

  /** No descriptions */
  QString readValue(const QString & share, const QString & optionName);
	bool readBoolValue(const QString & share, const QString & optionName);


protected:
  bool readonly;
  bool changed;
  QString path;
  QString tmpFilename;
  SambaConfigFile *sambaConfig;
	KSimpleConfig *config;

  SambaConfigFile* getSambaConfigFile(KSimpleConfig* config);
  KSimpleConfig* getSimpleConfig(SambaConfigFile* sambaConfig, const QString & filename);

	static bool boolFromText(const QString & value);
  static QString textFromBool(bool value);

private:
  void copyConfigs(KConfig* first, KConfig* second);
	QString getTempFileName();

public slots: // Public slots
  /** No descriptions */
  void slotApply();
  void slotSaveDone(KProcess* proc);
};

#endif
