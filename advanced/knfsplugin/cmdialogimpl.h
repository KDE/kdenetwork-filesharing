/***************************************************************************
                          cmdialogimpl.h  -  description
                             -------------------
    begin                : Don Apr 25 2002
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

#ifndef CMDIALOGIMPL_H
#define CMDIALOGIMPL_H

#include "cmdialog.h"

/**
  *@author Jan Schäfer
  */

class NFSFile;
class NFSEntry;
class QListViewItem;
class KDialogBase;
class NFSDialogImpl;

class CMDialogImpl : public CMDialog
{
Q_OBJECT
public:
	CMDialogImpl(QWidget* parent, NFSFile* file);
  ~CMDialogImpl();
protected:
	NFSFile* nfsFile;
	NFSDialogImpl* nfsDlg;

  QListViewItem* createItemFromEntry(NFSEntry* entry);
  KDialogBase* createEntryDialog(NFSEntry* entry);
	void updateItem(QListViewItem* item, NFSEntry* entry);

protected slots:
	void setModified();
  virtual void addEntrySlot();
  virtual void removeEntrySlot();
  virtual void modifyEntrySlot();

signals:
	void changed(bool);
};


#endif
