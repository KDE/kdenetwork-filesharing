/***************************************************************************
                          kcmnfsmodule.h  -  description
                             -------------------
    begin                : Mon Apr 29 2002
    copyright            : (C) 2002 by Jan Schäfer
    email                : janschaefer@users.sourceforge.net
 ***************************************************************************/

/******************************************************************************
 *                                                                            *
 *  This file is part of KNfsPlugin.                                          *
 *                                                                            *
 *  KNfsPlugin is free software; you can redistribute it and/or modify            *
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation; either version 2 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  KNfsPlugin is distributed in the hope that it will be useful,                 *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with KNfsPlugin; if not, write to the Free Software                     *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA  *
 *                                                                            *
 ******************************************************************************/

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
