/***************************************************************************
                          usertabimpl.cpp  -  description
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

#include <qcombobox.h>
#include <qstringlist.h>
#include <passwd.h>
#include <qregexp.h>
#include <qtable.h>
#include <qcheckbox.h>

#include <klistview.h>
#include <klocale.h>
#include <kdebug.h>
#include <kiconloader.h>

#include "usertabimpl.h"
#include "sambashare.h"
#include "smbpasswdfile.h"
#include "passwd.h"

#include <assert.h>

UserTabImpl::UserTabImpl(QWidget* parent, SambaShare* share)
  : UserTab(parent)
{
  _share = share;
  assert(_share);
}

UserTabImpl::~UserTabImpl()
{
}

void UserTabImpl::load()
{
  kdDebug() << "loading..." << endl;

  userTable->setColumnWidth(1, possibleUserListView->columnWidth(1));
  userTable->setColumnWidth(2, possibleUserListView->columnWidth(2));

  forceUserCombo->insertItem("");
  forceGroupCombo->insertItem("");

  QStringList unixGroups = getUnixGroups();

  forceUserCombo->insertStringList( getUnixUsers() );
  forceGroupCombo->insertStringList( unixGroups );

  setComboToString(forceUserCombo, _share->getValue("force user"));
  setComboToString(forceGroupCombo, _share->getValue("force group"));


  possibleUserListView->setSelectionMode(QListView::Extended);
  possibleUserListView->setAlternateBackground( Qt::white );

  QStringList validUsers = QStringList::split(QRegExp("[,\\s]+"),_share->getValue("valid users"));
  QStringList invalidUsers = QStringList::split(QRegExp("[,\\s]+"),_share->getValue("invalid users"));
  QStringList readList = QStringList::split(QRegExp("[,\\s]+"),_share->getValue("read list"));
  QStringList writeList = QStringList::split(QRegExp("[,\\s]+"),_share->getValue("write list"));
  QStringList adminUsers = QStringList::split(QRegExp("[,\\s]+"),_share->getValue("admin users"));

  QStringList added;


  int row=0;

  userTable->setLeftMargin(0);

  SmbPasswdFile passwd( KURL(_share->getValue("smb passwd file",true,true)) );
  SambaUserList sambaList = passwd.getSambaUserList();

  // if the valid users list contains no entries
  // then all users are allowed !

  allowAllUsersChk->setChecked(validUsers.empty());

  QStringList all = validUsers+readList+writeList+adminUsers;


  for ( QStringList::Iterator it = all.begin(); it != all.end(); ++it )
  {
    if (added.find(*it)!=added.end())
       continue;

    added.append(*it);

    userTable->setNumRows(row+1);

    setAllowedUser(row, *it);

    QComboTableItem* comboItem = static_cast<QComboTableItem*>(userTable->item(row,3));

    QStringList::Iterator it2;

    it2=readList.find(*it);
    if (it2 != readList.end())
    {
      comboItem->setCurrentItem(1);
      readList.remove(it2);
    }

    it2=writeList.find(*it);
    if (it2 != writeList.end())
    {
      comboItem->setCurrentItem(2);
      writeList.remove(it2);
    }

    it2=adminUsers.find(*it);
    if (it2 != adminUsers.end())
    {
      comboItem->setCurrentItem(3);
      adminUsers.remove(it2);
    }

    row++;
  }

  _usersFolder = new KListViewItem(possibleUserListView, i18n("Samba Users"));
  _groupsFolder = new KListViewItem(possibleUserListView, i18n("Unix Groups"));

  _usersFolder->setPixmap(0,SmallIcon("folder"));
  _groupsFolder->setPixmap(0,SmallIcon("folder"));


  SambaUser *user;
  for ( user = sambaList.first(); user; user = sambaList.next() )
  {
    QStringList::Iterator it;

    it=added.find(user->name);
    if (it == added.end())
        new KListViewItem(_usersFolder, user->name, QString::number(user->uid), QString::number(user->gid));
  }

  for (QStringList::Iterator it = unixGroups.begin(); it != unixGroups.end(); ++it)
  {
    new KListViewItem(_groupsFolder, *it, "-", QString::number(getGroupGID(*it)));
  }

  for (QStringList::Iterator it = invalidUsers.begin(); it != invalidUsers.end(); ++it)
  {
    QString uid;
    QString gid;

    if ((*it).left(1)=="+")
    {
      uid = "-";
      QString name = (*it).right( (*it).length()-1 );
      gid = QString::number(getGroupGID(name));
    }
    else
    {
      uid = QString::number(getUserUID(*it));
      gid = QString::number(getUserGID(*it));
    }

    new KListViewItem(rejectedUserListView, *it,uid,gid);
  }
}

void UserTabImpl::setAllowedUser(int i, const QString & name)
{
    QStringList accessRights;
    accessRights << i18n("Share") << i18n("Read only") << i18n("Writeable") << i18n("Admin");

    QTableItem* item = new QTableItem( userTable,QTableItem::Never, name );
    userTable->setItem(i,0,item);

    QString uid;
    QString gid;

    if (name.left(1)=="+")
    {
      QString name2 = name.right( name.length()-1 );
      uid = "-";
      gid = QString::number(getGroupGID(name2));
    }
    else
    {
      uid = QString::number(getUserUID(name));
      gid = QString::number(getUserGID(name));
    }

    item = new QTableItem( userTable,QTableItem::Never, uid );
    userTable->setItem(i,1,item);

    item = new QTableItem( userTable,QTableItem::Never, gid );
    userTable->setItem(i,2,item);

    QComboTableItem* comboItem = new QComboTableItem( userTable,accessRights);
    userTable->setItem(i,3,comboItem);
}


void UserTabImpl::addAllowedUserBtnClicked()
{
  QPtrList<QListViewItem> list = possibleUserListView->selectedItems();

  QListViewItem* item;
  for ( item = list.first(); item; item = list.first() )
  {
    if (item == _usersFolder ||
        item == _groupsFolder)
       continue;

    int row = userTable->numRows();
    userTable->setNumRows(row+1);

    QString name = item->text(0);
    if (item->parent() == _groupsFolder)
        name = "+"+name;

    setAllowedUser(row,name);

    list.remove(item);
    delete item;
  }
}

void UserTabImpl::removeAllowedUserBtnClicked()
{
  QMemArray<int>rows;

  int j=0;

  for (int i=0; i<userTable->numRows(); i++)
  {
    if (userTable->isRowSelected(i))
    {
      QTableItem* item = userTable->item(i,0);
      QString name = item->text();
      QString uid = userTable->item(i,1)->text();
      QString gid = userTable->item(i,2)->text();

      if (name.left(1)=="+")
      {
          name = name.right(name.length()-1);
          new KListViewItem(_groupsFolder, name,uid,gid);
      } else
          new KListViewItem(_usersFolder, name,uid,gid);

      rows.resize(j+1);
      rows[j] = i;
      j++;
    }
  }

  userTable->removeRows(rows);
}


void UserTabImpl::addRejectedUserBtnClicked()
{
  QPtrList<QListViewItem> list = possibleUserListView->selectedItems();
  QStringList accessRights;
  accessRights << i18n("Share") << i18n("Read only") << i18n("Writeable") << i18n("Admin");

  QListViewItem* item;
  for ( item = list.first(); item; item = list.first() )
  {
    if (item == _usersFolder ||
        item == _groupsFolder)
       continue;

    QString name = item->text(0);

    if (item->parent() == _groupsFolder)
        name = "+"+name;

    new KListViewItem(rejectedUserListView, name, item->text(1), item->text(2));

    list.remove(item);
    delete item;
  }
}

void UserTabImpl::removeRejectedUserBtnClicked()
{
  QPtrList<QListViewItem> list = rejectedUserListView->selectedItems();

  QListViewItem* item;
  for ( item = list.first(); item; item = list.first() )
  {
    QString name = item->text(0);
    if (name.left(1)=="+")
    {
      name = name.right(name.length()-1);
      new KListViewItem(_groupsFolder, name, item->text(1), item->text(2));
    }
    else
      new KListViewItem(_usersFolder, name, item->text(1), item->text(2));

    list.remove(item);
    delete item;
  }
}

void UserTabImpl::save()
{
  kdDebug() << "saving ... " << endl;
  QStringList validUsers;
  QStringList writeList;
  QStringList readList;
  QStringList adminUsers;

  for (int i=0; i<userTable->numRows(); i++)
  {
    QTableItem* item = userTable->item(i,0);
    QComboTableItem* comboItem = static_cast<QComboTableItem*>(userTable->item(i,3));

    if (!allowAllUsersChk->isChecked())
       validUsers.append(item->text());

    kdDebug() << "adding user : " << item->text() << " access : " << comboItem->currentItem() << endl;

    switch (comboItem->currentItem())
    {
      case 0 : //if (allowAllUsersChk->isChecked())
                 //  validUsers.append(item->text());
                 break;
      case 1 : readList.append(item->text());break;
      case 2 : writeList.append(item->text());break;
      case 3 : adminUsers.append(item->text());break;
    }
  }

  QStringList invalidUsers;

  QListViewItemIterator it( rejectedUserListView );
  for ( ; it.current(); ++it )
  {
    kdDebug() << "invalid user : " << it.current()->text(0) << endl;
    invalidUsers.append(it.current()->text(0));
  }

  _share->setValue("valid users", validUsers.join(","));
  _share->setValue("read list", readList.join(","));
  _share->setValue("write list", writeList.join(","));
  _share->setValue("admin users", adminUsers.join(","));
  _share->setValue("invalid users", invalidUsers.join(","));

  kdDebug() << _share->getValue("invalid users") << endl;


  _share->setValue("force user",forceUserCombo->currentText( ) );
  _share->setValue("force group",forceGroupCombo->currentText( ) );


}


#include "usertabimpl.moc"
