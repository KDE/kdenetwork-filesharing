/***************************************************************************
                          usertabimpl.h  -  description
                             -------------------
    begin                : Mon Jul 15 2002
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

#ifndef USERTABIMPL_H
#define USERTABIMPL_H

#include "usertab.h"
#include "common.h"
#include "qstringlist.h"

/**
  *@author Jan Schäfer
  */

class KListViewItem;
class SambaShare;

/**
 * Implements the usertab.ui interface
 * A Widget where you can add SambaUsers to the valid users, rejected users,
 * write list, read list and admin list parameters of the smb.conf file
 **/
class UserTabImpl : public UserTab
{
Q_OBJECT
public:
	UserTabImpl(QWidget* parent, SambaShare* share);
	~UserTabImpl();

  void load();
  void save();
private:
  SambaShare* _share;
  QStringList _specifiedUsers;
  QStringList _specifiedGroups;

  void setAllowedUser(int, const QString &);
  void removeDuplicates( QStringList & validUsers, QStringList & readList,
                         QStringList & writeList, QStringList & adminUsers,
                         QStringList & invalidUsers);

  void removeAll(QStringList & entryList, QStringList & fromList);
  void addListToUserTable(const QStringList & list, int accessRight);
  void loadForceCombos();

  void loadUsers(const QString & validUsersStr,
                 const QString & readListStr,
                 const QString & writeListStr,
                 const QString & adminUsersStr,
                 const QString & invalidUsersStr);

  void saveUsers(QString & validUsersStr,
                 QString & readListStr,
                 QString & writeListStr,
                 QString & adminUsersStr,
                 QString & invalidUsersStr);

  bool nameIsGroup(const QString & name);
  QString removeGroupTag(const QString & name);
  QString removeQuotationMarks(const QString & name);
  void addUserToUserTable(const QString & user, int accessRight);

protected slots:
  virtual void addUserBtnClicked();
  virtual void addGroupBtnClicked();
  virtual void removeSelectedBtnClicked();
  virtual void expertBtnClicked();
};

#endif
