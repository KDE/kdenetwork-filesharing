/***************************************************************************
                          nfshostdlg.h  -  description
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

#ifndef NFSHOSTDLG_H
#define NFSHOSTDLG_H

#include <kdialogbase.h>
#include "nfsentry.h"

class NFSHost;
class HostProps;
class NFSEntry;
class QCheckBox;
class QLineEdit;


class NFSHostDlg : public KDialogBase
{
Q_OBJECT
public: 
	NFSHostDlg(QWidget* parent, HostList* hosts, NFSEntry* entry);
	virtual ~NFSHostDlg();
    bool isModified();
protected:
  HostList* m_hosts;
  NFSEntry* m_nfsEntry;
  HostProps* m_gui;
  bool m_modified;
  
  void init();
protected slots:
  virtual void slotOk();
  void setModified();
  
private:
  void setHostValues(NFSHost* host);
  void setEditValue(QLineEdit* edit, const QString & value);
  void setCheckBoxValue(QCheckBox* chk, bool value);
  bool saveName(NFSHost* host);
  void saveValues(NFSHost* host);
  void saveEditValue(int & value, QLineEdit* edit);
  void saveCheckBoxValue(bool & value, QCheckBox* chk, bool neg);
};

#endif
