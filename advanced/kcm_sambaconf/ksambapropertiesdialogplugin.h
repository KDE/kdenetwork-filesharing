/***************************************************************************
                          sambapage.h  -  description
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

class KSambaPropertiesDialogPlugin : public KPropsDlgPlugin
{
Q_OBJECT
public:
  KSambaPropertiesDialogPlugin( KPropertiesDialog *dlg,const char *, const QStringList &  );
  virtual ~KSambaPropertiesDialogPlugin();

  QString addGlobalText(const QString & value, const QString & option, const SambaFile & sambaFile);

  virtual void applyChanges();

protected:
  KPropertiesDialog* propDialog;

  QFrame* frame;
  
  QWidget* initWidget;
  KonqInterface* configWidget;

  QWidgetStack *stack;

  /**  */
  QString smbconf;
  QString sharePath;
  void initValues(const QString & share, SambaFile & sambaFile);
  void init();
  
  QWidget* createInitWidget(QWidget* parent);
  QWidget* createConfigWidget(QWidget* parent);

protected slots:

  void slotSharedChanged(int state);
  void slotSpecifySmbConf();

private: // Private methods
  QString findSambaConf() const;

};

#endif
