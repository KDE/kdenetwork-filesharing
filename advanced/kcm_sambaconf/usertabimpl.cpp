/***************************************************************************
                          usertabimpl.cpp  -  description
                             -------------------
    begin                : Mon Jul 15 2002
    copyright            : (C) 2002 by Jan Sch�er
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

#include <unistd.h> // for getuid

#include <qcombobox.h>
#include <qstringlist.h>
#include <passwd.h>
#include <qregexp.h>
#include <qtable.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>

#include <klistview.h>
#include <klocale.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kinputdialog.h>

#include "usertabimpl.h"
#include "sambashare.h"
#include "smbpasswdfile.h"
#include "passwd.h"
#include "groupselectdlg.h"
#include "userselectdlg.h"
#include "expertuserdlg.h"

#include <assert.h>

/**
 * @pre share is not null
 * @post _share = share
 */
UserTabImpl::UserTabImpl(QWidget* parent, SambaShare* share)
  : UserTab(parent)
{
  if (share == 0L) {
    kdWarning() << "WARNING: UserTabImpl constructor: share parameter is null!" << endl;
    return;
  }

  _share = share;
  userTable->setLeftMargin(0);
 // userTable->setColumnStretchable ( 3, true );

}

UserTabImpl::~UserTabImpl()
{
}

void UserTabImpl::load()
{
  if (_share == 0L)
    return;

  loadForceCombos();
  loadUsers(_share->getValue("valid users"),
            _share->getValue("read list"),
            _share->getValue("write list"),
            _share->getValue("admin users"),
            _share->getValue("invalid users"));
}

void UserTabImpl::loadUsers(const QString & validUsersStr,
                            const QString & readListStr,
                            const QString & writeListStr,
                            const QString & adminUsersStr,
                            const QString & invalidUsersStr)
{
  userTable->setNumRows(0);
  QStringList validUsers = QStringList::split(QRegExp("[,\\s]+"),validUsersStr);
  QStringList readList = QStringList::split(QRegExp("[,\\s]+"),readListStr);
  QStringList writeList = QStringList::split(QRegExp("[,\\s]+"),writeListStr);
  QStringList adminUsers = QStringList::split(QRegExp("[,\\s]+"),adminUsersStr);
  QStringList invalidUsers = QStringList::split(QRegExp("[,\\s]+"),invalidUsersStr);

  // if the valid users list contains no entries
  // then all users are allowed, except those that are
  // in the invalid list
  if (validUsers.empty())
    allUnspecifiedUsersCombo->setCurrentItem(0);
  else
    allUnspecifiedUsersCombo->setCurrentItem(1);

  removeDuplicates(validUsers,readList,writeList,adminUsers,invalidUsers);

  addListToUserTable(adminUsers,3);
  addListToUserTable(writeList,2);
  addListToUserTable(readList,1);
  addListToUserTable(validUsers,0);
  addListToUserTable(invalidUsers,4);

}


void UserTabImpl::loadForceCombos() {
  forceUserCombo->insertItem("");
  forceGroupCombo->insertItem("");

  QStringList unixGroups = getUnixGroups();

  forceUserCombo->insertStringList( getUnixUsers() );
  forceGroupCombo->insertStringList( unixGroups );

  setComboToString(forceUserCombo, _share->getValue("force user"));
  setComboToString(forceGroupCombo, _share->getValue("force group"));
}

/**
 * Remove all duplicates of the different list, so that
 * all lists are disjunct.
 */
void UserTabImpl::removeDuplicates( QStringList & validUsers, QStringList & readList,
                       QStringList & writeList, QStringList & adminUsers,
                       QStringList & invalidUsers)
{
  removeAll(adminUsers, writeList);

  removeAll(writeList, readList);
  removeAll(adminUsers, readList);

  removeAll(readList, validUsers);
  removeAll(writeList, validUsers);
  removeAll(adminUsers, validUsers);

  removeAll(invalidUsers, validUsers);
  removeAll(invalidUsers, readList);
  removeAll(invalidUsers, writeList);
  removeAll(invalidUsers, adminUsers);
}

/**
 * Remove all entries of entryList from the fromList
 */
void UserTabImpl::removeAll(QStringList & entryList, QStringList & fromList) {
  for (QStringList::Iterator it = entryList.begin(); it != entryList.end(); ++it) {
    fromList.remove((*it));
  }
}


void UserTabImpl::addListToUserTable(const QStringList & list, int accessRight) {

  for (QStringList::ConstIterator it = list.begin(); it != list.end(); ++it) {
    addUserToUserTable(*it,accessRight);
  }
}

void UserTabImpl::addUserToUserTable(const QString & user, int accessRight) {
    int row = userTable->numRows();
    userTable->setNumRows(row+1);
    setAllowedUser(row, user);

    QComboTableItem* comboItem = static_cast<QComboTableItem*>(userTable->item(row,3));
    comboItem->setCurrentItem(accessRight);
}

QString UserTabImpl::removeQuotationMarks(const QString & name) {
    QString result = name;
    if (name.left(1) == "\"") {
        result = result.left(result.length()-1);
        result = result.right(result.length()-1);
    }
    return result;
}

QString UserTabImpl::removeGroupTag(const QString & name) {
  QString result = name;


  if (nameIsGroup(result)) {
    result = result.right(result.length()-1);
    if (nameIsGroup(result))
      result = result.right(result.length()-1);
  }

  return result;
}


bool UserTabImpl::nameIsGroup(const QString & name) {
  QString quoteless = removeQuotationMarks(name);
  if (quoteless.left(1) == "@" ||
      quoteless.left(1) == "+" ||
      quoteless.left(1) == "&")
      return true;

  return false;

}

void UserTabImpl::setAllowedUser(int i, const QString & name)
{
    QStringList accessRights;
    accessRights << i18n("Default") << i18n("Read only") << i18n("Writeable") << i18n("Admin") << i18n("Reject");

    QString uid;
    QString gid;

    QString name2 = name;

    if (nameIsGroup(name2))
    {
      QString name3 = removeGroupTag(name2);
      uid = "";
      gid = QString::number(getGroupGID(name3));
      _specifiedGroups << name3;
    }
    else
    {
      uid = QString::number(getUserUID(name2));
      gid = QString::number(getUserGID(name2));
      _specifiedUsers << name2;
    }

    if (name2.contains(' '))
        name2 = "\""+name2+"\"";


    QTableItem* item = new QTableItem( userTable,QTableItem::Never, name2 );
    userTable->setItem(i,0,item);

    item = new QTableItem( userTable,QTableItem::Never, uid );
    userTable->setItem(i,1,item);

    item = new QTableItem( userTable,QTableItem::Never, gid );
    userTable->setItem(i,2,item);

    QComboTableItem* comboItem = new QComboTableItem( userTable,accessRights);
    userTable->setItem(i,3,comboItem);

}

void UserTabImpl::addUserBtnClicked()
{
  if (getuid() == 0) {
    UserSelectDlg *dlg = new UserSelectDlg();
    dlg->init(_specifiedUsers,_share);

    QStringList selectedUsers = dlg->getSelectedUsers();

    if (dlg->exec()) {
        for (QStringList::Iterator it = selectedUsers.begin(); it != selectedUsers.end(); ++it)
        {
            addUserToUserTable(*it,dlg->getAccess());
        }
    }

    delete dlg;
  } else {
    bool ok;
    QString name = KInputDialog::getText(i18n("Add User"),i18n("Name:"),
                                         QString::null,&ok );
                                         
    if (ok)                                         
        addUserToUserTable(name,0);
  }
}

void UserTabImpl::removeSelectedBtnClicked()
{
  QMemArray<int>rows;

  int j=0;

  for (int i=0; i<userTable->numRows(); i++)
  {
    if (userTable->isRowSelected(i))
    {
      if (nameIsGroup(userTable->item(i,0)->text())) {
        _specifiedGroups.remove( removeGroupTag(removeQuotationMarks(userTable->item(i,0)->text())));
      } else
        _specifiedUsers.remove(userTable->item(i,0)->text());

      rows.resize(j+1);
      rows[j] = i;
      j++;
    }
  }

  userTable->removeRows(rows);
}


void UserTabImpl::addGroupBtnClicked()
{
  GroupSelectDlg *dlg = new GroupSelectDlg();
  dlg->init(_specifiedGroups);
  QStringList selectedGroups = dlg->getSelectedGroups();
  if (dlg->exec()) {
    for (QStringList::Iterator it = selectedGroups.begin(); it != selectedGroups.end(); ++it)
    {
        kdDebug(5009) << "GroupKind: " << dlg->getGroupKind() << endl;
        QString name = dlg->getGroupKind() + (*it);
        addUserToUserTable(name,dlg->getAccess());
    }
  }

  delete dlg;
}

void UserTabImpl::expertBtnClicked() {
    ExpertUserDlg *dlg = new ExpertUserDlg();
    QString validUsersStr;
    QString readListStr;
    QString writeListStr;
    QString adminUsersStr;
    QString invalidUsersStr;

    saveUsers(validUsersStr,readListStr,writeListStr,adminUsersStr,invalidUsersStr);

    dlg->validUsersEdit->setText(validUsersStr);
    dlg->readListEdit->setText(readListStr);
    dlg->writeListEdit->setText(writeListStr);
    dlg->adminUsersEdit->setText(adminUsersStr);
    dlg->invalidUsersEdit->setText(invalidUsersStr);

    if (dlg->exec()) {
        loadUsers(dlg->validUsersEdit->text(),
                  dlg->readListEdit->text(),
                  dlg->writeListEdit->text(),
                  dlg->adminUsersEdit->text(),
                  dlg->invalidUsersEdit->text());

    }

    delete dlg;
}

void UserTabImpl::save()
{
  QString validUsersStr;
  QString readListStr;
  QString writeListStr;
  QString adminUsersStr;
  QString invalidUsersStr;

  saveUsers(validUsersStr,readListStr,writeListStr,adminUsersStr,invalidUsersStr);

  _share->setValue("valid users",validUsersStr);
  _share->setValue("read list", readListStr);
  _share->setValue("write list", writeListStr);
  _share->setValue("admin users", adminUsersStr);
  _share->setValue("invalid users", invalidUsersStr);

  _share->setValue("force user",forceUserCombo->currentText( ) );
  _share->setValue("force group",forceGroupCombo->currentText( ) );
}

void UserTabImpl::saveUsers(QString & validUsersStr,
                            QString & readListStr,
                            QString & writeListStr,
                            QString & adminUsersStr,
                            QString & invalidUsersStr)
{
  QStringList validUsers;
  QStringList writeList;
  QStringList readList;
  QStringList adminUsers;
  QStringList invalidUsers;

  bool allowAllUsers = allUnspecifiedUsersCombo->currentItem() == 0;

  for (int i=0; i<userTable->numRows(); i++)
  {
    QTableItem* item = userTable->item(i,0);
    QComboTableItem* comboItem = static_cast<QComboTableItem*>(userTable->item(i,3));

    if (! allowAllUsers && comboItem->currentItem() < 4)
       validUsers.append(item->text());

    switch (comboItem->currentItem())
    {
      case 0 : break;
      case 1 : readList.append(item->text());break;
      case 2 : writeList.append(item->text());break;
      case 3 : adminUsers.append(item->text());break;
      case 4 : invalidUsers.append(item->text());break;
    }
  }

  validUsersStr = validUsers.join(",");
  readListStr = readList.join(",");
  writeListStr = writeList.join(",");
  adminUsersStr = adminUsers.join(",");
  invalidUsersStr = invalidUsers.join(",");
}



#include "usertabimpl.moc"
