/***************************************************************************
                          cmdialogimpl.h  -  description
                             -------------------
    begin                : Don Apr 25 2002
    copyright            : (C) 2002 by Jan Schäfer
    email                : janschaefer@users.sourceforge.net
 ***************************************************************************/

/******************************************************************************
 *                                                                            *
 *  This file is part of KNfsPlugin.                                          *
 *                                                                            *
 *  Foobar is free software; you can redistribute it and/or modify            *
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation; either version 2 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  Foobar is distributed in the hope that it will be useful,                 *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with Foobar; if not, write to the Free Software                     *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA  *
 *                                                                            *
 ******************************************************************************/

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
