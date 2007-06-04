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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

#ifndef NFSDIALOG_H
#define NFSDIALOG_H

#include <kdialog.h>

class NFSEntry;
class NFSHost;
class Q3ListViewItem;
class NFSDialogGUI;

class NFSDialog : public KDialog
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
  Q3ListViewItem* createItemFromHost(NFSHost* host);
  void updateItem(Q3ListViewItem* item, NFSHost* host);
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
