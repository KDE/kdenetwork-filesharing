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

#ifndef GROUPCONFIGDLG_H
#define GROUPCONFIGDLG_H

#include <kdialogbase.h>
#include <qvaluelist.h>
#include <kuser.h>

class GroupConfigGUI;

class GroupConfigDlg : public KDialogBase
{
Q_OBJECT
public: 
  GroupConfigDlg(QWidget * parent, const QString & fileShareGroup, bool restricted, 
                 bool rootPassNeeded, bool simpleSharing);
  ~GroupConfigDlg();
  KUserGroup fileShareGroup() { return m_fileShareGroup; }
  bool restricted();
  bool rootPassNeeded();
protected:
  GroupConfigGUI* m_gui;
  
  void initGUI();
  void initUsers();
protected slots:
  void slotAddUser();
  void slotRemoveUser();
  void slotChangeGroup();
  void updateListBox();
  virtual void slotOk();

private:
  bool createFileShareGroup(const QString & s);
  bool deleteGroup(const QString & s);
  bool emptyGroup(const QString & s);
  bool addUser(const KUser & user, const KUserGroup & group);
  bool removeUser(const KUser & user, const KUserGroup & group);
  bool addUsersToGroup(QValueList<KUser> users,const KUserGroup & group);
  void setFileShareGroup(const KUserGroup & group);
  
  QValueList<KUser> m_origUsers;
  QValueList<KUser> m_users;
  KUserGroup m_fileShareGroup;
  bool m_restricted;
  bool m_rootPassNeeded;
  bool m_simpleSharing;
};

#endif
