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

class QFrame;
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

  QString addGlobalText(const QString & value, const QString & option, const SambaFile & sambaFile);

  virtual void applyChanges();

protected:
  bool _wasShared;

  KPropertiesDialog* propDialog;

  QFrame* frame;

  KonqInterface* shareWidget;
  QWidget* configWidget;

  QWidgetStack *stack;

  /**  */
  QString smbconf;
  QString sharePath;
  SambaShare* _share;
  SambaFile* _sambaFile;

  void initValues();
  void saveValuesToShare();
  void init();
  
  /**
   * Checks wether or not the entered Values are correct
   * @return fals if not correct otherwise true
   */
  bool checkValues();
  
  KonqInterface* createShareWidget(QWidget* parent);
  QWidget* createConfigWidget(QWidget* parent);

protected slots:

  void slotSharedChanged(int state);
  void slotSpecifySmbConf();
  void moreOptionsBtnPressed();
  void changedSlot();

private: // Private methods
  QString findSambaConf() const;

};

#endif
