/***************************************************************************
                          nfsdialogimpl.h  -  description
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

#ifndef NFSDIALOGIMPL_H
#define NFSDIALOGIMPL_H

#include "nfsdialog.h"

/**
  *@author Jan Schäfer
  */

class NFSEntry;
class NFSHost;
class QListViewItem;
class QListView;
class NFSFile;

class NFSDialogImpl : public NFSDialog
{
Q_OBJECT
public:
	NFSDialogImpl(QWidget * parent, NFSEntry* entry, bool showPath=false);
	NFSDialogImpl(QWidget * parent, NFSFile* file, const QString & path,bool showPath=false);
	~NFSDialogImpl();
  bool _modified;
  void applyChanges();
protected:
  NFSEntry * nfsEntry;
  NFSFile* nfsFile;
  QString path;
	bool _showPath;

  QListViewItem* createItemFromHost(NFSHost* host);
  void updateItem(QListViewItem* item, NFSHost* host);
  void init(NFSEntry * entry);
  void initSlots();
protected slots:
  void slotAddHost();
  void slotRemoveHost();
  void slotModifyHost();
  virtual void specifyPathSlot();
  virtual void setModified();
  virtual void shareStateChanged( int state );
signals:
  void modified();
};

#endif
