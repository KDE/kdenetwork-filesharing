/***************************************************************************
                          kcmnfsmodule.h  -  description
                             -------------------
    begin                : Mon Apr 29 2002
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

#ifndef KCMNFSMODULE_H
#define KCMNFSMODULE_H

#include <kcmodule.h>


class NFSFile;
class KAboutData;
class CMDialogImpl;
class NFSConfigWidget;




class KCmNfsModule : public KCModule
{
Q_OBJECT
public:
	KCmNfsModule(QWidget *parent=0,const char *name=0, const QStringList & = QStringList());
	virtual ~KCmNfsModule();

	virtual void load();
	virtual void save();
	virtual void defaults();
	virtual QString quickHelp() const;
	virtual const KAboutData* aboutData() const;

protected:
	QString findExports();

	NFSFile* _file;
  CMDialogImpl* _cmDlg;
  NFSConfigWidget* _configWidget;
  QString _exports;
  
protected slots:
	void setModified();
	void exportsSpecifiedSlot(QString);
};

#endif
