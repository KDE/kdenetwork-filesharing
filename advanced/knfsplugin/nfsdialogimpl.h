/***************************************************************************
                          nfsdialogimpl.h  -  description
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
  void virtual specifyPathSlot();
  void virtual setModified();
  void virtual shareStateChanged( int state );
signals:
	void modified();
};

#endif
