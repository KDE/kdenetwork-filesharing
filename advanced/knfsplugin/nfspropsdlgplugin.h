/***************************************************************************
                          nfspropsdlgplugin.h  -  description
                             -------------------
    begin                : Don Apr 25 2002
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

#ifndef NFSPROPSDLGPLUGIN_H
#define NFSPROPSDLGPLUGIN_H

#include <kpropertiesdialog.h>

/**
  *@author Jan Schäfer
  */

class QStringList;
class QWidgetStack;
class NFSFile;

class NFSPropsDlgPlugin : public KPropsDlgPlugin
{
Q_OBJECT
public:
	NFSPropsDlgPlugin( KPropertiesDialog *dlg,const char *, const QStringList & );

	virtual ~NFSPropsDlgPlugin();

  virtual void applyChanges();

protected:
  KPropertiesDialog *_dialog;
  QWidgetStack* _stack;
  QWidget* nfsWidget;
  QWidget* configWidget;
  QString exports;
  NFSFile* nfsFile;

  QWidget* createNFSWidget(QWidget*);
  QWidget* createConfigWidget(QWidget*);

  QString findExports();

protected slots:
	void exportsSpecifiedSlot(QString);  

};

#endif
