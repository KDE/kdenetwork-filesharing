/*
  Copyright (c) 2004 Jan Schaefer <j_schaef@informatik.uni-kl.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#ifndef NFSDIALOG_H
#define NFSDIALOG_H

#include <kdialogbase.h>

class NFSEntry;
class NFSHost;
class QListViewItem;
class NFSFile;
class NFSDialogGUI;

class NFSDialog : public KDialogBase
{
Q_OBJECT
public: 
  NFSDialog(QWidget * parent, NFSEntry* entry);
  ~NFSDialog();
  bool modified();
protected:
  NFSEntry * m_nfsEntry;
  NFSEntry * m_workEntry;
  NFSDialogGUI* m_gui;
  
  bool m_modified;
  QListViewItem* createItemFromHost(NFSHost* host);
  void updateItem(QListViewItem* item, NFSHost* host);
  void initGUI();
  void initListView();
  void initSlots();
  void initWorkEntry();
protected slots:
  void slotAddHost();
  void slotRemoveHost();
  void slotModifyHost();
  virtual void slotOk();
  void setModified();
};

#endif
