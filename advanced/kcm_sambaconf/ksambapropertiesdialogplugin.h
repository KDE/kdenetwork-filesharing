/***************************************************************************
                   ksambapropertiesdialogplugin.h  -  description
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

#ifndef KSAMBAPROPERTIES_DIALOG_PLUGIN_H
#define KSAMBAPROPERTIES_DIALOG_PLUGIN_H


/**
  *@author Jan Schäfer
  */

#include <qstring.h>
#include <kpropertiesdialog.h>

class SambaFile;
class QRadioButton;
class QLineEdit;
class QVGroupBox;
class QCheckBox;
class QWidgetStack;
class KonqInterface;
class SambaShare;

class KSambaPropertiesDialogPlugin : public KPropsDlgPlugin
{
Q_OBJECT
public:
  KSambaPropertiesDialogPlugin( KPropertiesDialog *dlg,const char *, const QStringList &  );
  virtual ~KSambaPropertiesDialogPlugin();
  virtual void applyChanges();

protected:
  void initValues();
  void saveValuesToShare();
  void initGUI();
  bool checkValues();
  
  void createShareWidget(QWidget* parent);
  void createConfigWidget(QWidget* parent);
  
  QString getSambaConf();
  SambaFile* getSambaFile();
  SambaShare* getGlobalShare();
  QString getSharePath();
  SambaShare* getActiveShare();
  QString getNetbiosName();
  QString getLocalPathFromUrl(const KURL & url);

private:
  bool m_wasShared;
  QString m_sambaConf;
  QString m_sharePath;
  SambaShare* m_activeShare;
  SambaFile* m_sambaFile;
  KonqInterface* m_shareWidget;
  QWidget* m_configWidget;
  QWidgetStack *m_stack;


protected slots:
  void slotSharedChanged(int state);
  void slotSpecifySmbConf();
  void moreOptionsBtnPressed();
};

#endif
