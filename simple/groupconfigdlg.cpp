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


#include <qradiobutton.h>
#include <qpushbutton.h>
#include <q3groupbox.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <kvbox.h>
#include <khbox.h>
//Added by qt3to4:

#include <klocale.h>
#include <kuser.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kinputdialog.h>
#include <k3listbox.h>
#include <kprocess.h>
#include <kpushbutton.h>
#include <kcombobox.h>

#include "groupconfigdlg.h"


static QString groupListToString(const QList<KUserGroup> & list);
static QString prettyString(const KUser &user);
static QString fromPrettyString(const QString & s);
static void removeList(QList<KUser> & from, const QList<KUser> & that);
static bool userMod(const QString & user, const QList<KUserGroup> & groups);


GroupConfigGUI::GroupConfigGUI( QWidget *parent )
    : QWidget( parent )
{
    setupUi( this );
    connect( listBox, SIGNAL( selectionChanged(Q3ListBoxItem*) ), this, SLOT(listBox_selectionChanged(Q3ListBoxItem*) ) );
}

void GroupConfigGUI::listBox_selectionChanged( Q3ListBoxItem * i)
{
    removeBtn->setEnabled(i);
}

GroupConfigDlg::GroupConfigDlg(QWidget * parent,
      const QString & fileShareGroup, bool restricted,
      bool rootPassNeeded, bool simpleSharing)
  : KDialog(parent),
  m_fileShareGroup(fileShareGroup),
  m_restricted(restricted) ,
  m_rootPassNeeded(rootPassNeeded),
  m_simpleSharing(simpleSharing)

{
  setCaption(i18n("Allowed Users"));
  setButtons(Ok|Cancel);
  setDefaultButton(Ok);
  setModal(true);
  showButtonSeparator(true);
  initGUI();

  setFileShareGroup(m_fileShareGroup);
  connect(this,SIGNAL(okClicked()),this,SLOT(slotOk()));
}

GroupConfigDlg::~GroupConfigDlg() {
}

void GroupConfigDlg::initUsers() {
  m_origUsers = m_fileShareGroup.users();
  m_users = m_origUsers;
}

void GroupConfigDlg::initGUI() {
  m_gui = new GroupConfigGUI(this);
  setMainWidget(m_gui);
  setFileShareGroup(m_fileShareGroup);

  m_gui->allUsersRadio->setChecked(!m_restricted);
  m_gui->groupUsersRadio->setChecked(m_restricted);
  m_gui->writeAccessChk->setChecked(!m_rootPassNeeded);

  connect( m_gui->addBtn, SIGNAL(clicked()),
           this, SLOT(slotAddUser()));
  connect( m_gui->removeBtn, SIGNAL(clicked()),
           this, SLOT(slotRemoveUser()));
  connect( m_gui->otherGroupBtn, SIGNAL(clicked()),
           this, SLOT(slotChangeGroup()));

  if (m_simpleSharing) {
      // if simple sharing users never need the root password
      m_gui->writeAccessChk->setDisabled(true);
  }
}

void GroupConfigDlg::updateListBox() {
  m_gui->listBox->clear();
  QList<KUser>::iterator it;
  for ( it = m_users.begin(); it != m_users.end(); ++it ) {
     m_gui->listBox->insertItem(prettyString(*it));
     kDebug(5009) << "GroupConfigDlg::updateListBox: " << (*it).loginName();
  }
}

QString prettyString(const KUser &user) {
  return user.fullName()+" ("+user.loginName()+')';
}

QString fromPrettyString(const QString & s) {
  // Jan Schaefer (jan)
  //              i   j
  int i = s.indexOf('(');
  int j = s.indexOf(')');
  QString loginName = s.mid(i+1,j-i-1);
  return loginName;
}

bool GroupConfigDlg::restricted() const {
  return m_restricted;
}

void GroupConfigDlg::slotAddUser() {
  QList<KUser> allUsers = KUser::allUsers();

  removeList(allUsers,m_users);

  if (allUsers.count()==0) {
     KMessageBox::information(this,
           i18n("All users are in the %1 group already.",
            m_fileShareGroup.name()));
     return;
  }

  QStringList stringList;

  QList<KUser>::iterator it;
  for ( it = allUsers.begin(); it != allUsers.end(); ++it ) {
    QString s = (*it).fullName()+" ("+(*it).loginName()+')';
    stringList.append(s);
  }

  stringList.sort();

  bool ok;
  QString userName = KInputDialog::getItem(
            i18n("Select User"),
            i18n("Select a user:"),
            stringList,
            0,
            false,
            &ok);

  if (!ok)
      return;

  QString loginName = fromPrettyString(userName);
  KUser user(loginName);
  m_users.append(KUser(loginName));
  updateListBox();
}

void removeList(QList<KUser> & from, const QList<KUser> & that) {
  QList<KUser>::ConstIterator it;
  for ( it = that.begin(); it != that.end(); ++it ) {
    from.remove(*it);
  }

}

bool GroupConfigDlg::addUser(const KUser & user, const KUserGroup & group) {
  QList<KUserGroup> groups = user.groups();
  groups.append(group);
  if (!userMod(user.loginName(),groups)) {
    KMessageBox::sorry(this,i18n("Could not add user '%1' to group '%2'",
             user.loginName(), group.name()));
    return false;
  }
  return true;
}


bool GroupConfigDlg::removeUser(const KUser & user, const KUserGroup & group) {
  QList<KUserGroup> groups = user.groups();
  groups.remove(group);
  if (!userMod(user.loginName(),groups)) {
    KMessageBox::sorry(this,i18n("Could not remove user '%1' from group '%2'",
             user.loginName(), group.name()));
    return false;
  }
  return true;
}

bool GroupConfigDlg::rootPassNeeded() const {
  return m_rootPassNeeded;
}

void GroupConfigDlg::slotOk() {
  m_restricted = m_gui->groupUsersRadio->isChecked();
  m_rootPassNeeded = ! m_gui->writeAccessChk->isChecked();
  if (m_restricted && !m_fileShareGroup.isValid()) {
    KMessageBox::sorry(this,i18n("You have to choose a valid group."));
    return;
  }

  QList<KUser> addedUsers = m_users;
  removeList(addedUsers,m_origUsers);
  QList<KUser> removedUsers = m_origUsers;
  removeList(removedUsers,m_users);

  QList<KUser>::ConstIterator it;
  for ( it = addedUsers.constBegin(); it != addedUsers.constEnd(); ++it ) {
    addUser(*it, m_fileShareGroup);
  }

  for ( it = removedUsers.constBegin(); it != removedUsers.constEnd(); ++it ) {
    removeUser(*it, m_fileShareGroup);
  }


  KDialog::accept();
}

bool userMod(const QString & user, const QList<KUserGroup> & groups) {
  KProcess proc;
  proc << "usermod" << "-G" << groupListToString(groups) << user;
  return !proc.execute();
}

void GroupConfigDlg::slotRemoveUser() {
  Q3ListBoxItem* item = m_gui->listBox->selectedItem();
  if (!item)
      return;

  QString loginName = fromPrettyString(item->text());
  KUser user(loginName);
  m_users.remove(KUser(loginName));
  updateListBox();
  m_gui->removeBtn->setEnabled(false);
}

QString groupListToString(const QList<KUserGroup> & list) {
  QList<KUserGroup>::ConstIterator it;
  QString result;

  for ( it = list.begin(); it != list.end(); ++it ) {
    result+=(*it).name()+',';
  }

  // remove last ,
  result.truncate(result.length()-1);
  return result;
}

void GroupConfigDlg::slotChangeGroup() {
  QList<KUserGroup> allGroups = KUserGroup::allGroups();

  QStringList stringList;

  QList<KUserGroup>::iterator it;
  for ( it = allGroups.begin(); it != allGroups.end(); ++it ) {
    QString s = (*it).name();
    stringList.append(s);
  }

  stringList.sort();

  KDialog* dlg = new KDialog(this);
  dlg->setCaption(i18n("Allowed Users"));
  dlg->setButtons(Ok|Cancel);
  dlg->setDefaultButton(Ok);
  dlg->setModal(true);
  dlg->showButtonSeparator(true);
  KVBox* vbox = new KVBox(this);
  dlg->setMainWidget(vbox);

  KHBox* hbox = new KHBox(vbox);
  QLabel* lbl = new QLabel(i18n("New file share group:"),hbox);
  KComboBox* combo = new KComboBox(hbox);
  combo->insertStringList(stringList);
  combo->setEditable(true);
  combo->setCurrentText(m_fileShareGroup.name());

  QCheckBox* addChk = new QCheckBox(
        i18n("Add users from the old file share group to the new one"),
        vbox);

  QCheckBox* removeUsersChk = new QCheckBox(
        i18n("Remove users from old file share group"),
        vbox);

  QCheckBox* removeGroupChk = new QCheckBox(
        i18n("Delete the old file share group"),
        vbox);

  if (dlg->exec() == QDialog::Accepted) {
    QString groupName = combo->currentText();
    if (groupName != m_fileShareGroup.name()) {
      QString oldGroup = m_fileShareGroup.name();
      if (allGroups.contains(KUserGroup(groupName)))
        setFileShareGroup(KUserGroup(groupName));
      else {
        if (!createFileShareGroup(groupName)) {
          delete dlg;
          return;
        }
      }

      if (removeGroupChk->isChecked())
        deleteGroup(oldGroup);
      else
      if (removeUsersChk->isChecked())
        emptyGroup(oldGroup);

      if (addChk->isChecked()) {
          addUsersToGroup(m_users,KUserGroup(groupName));
          // reread the users
          m_fileShareGroup = KUserGroup(groupName);
      }


      initUsers();
      updateListBox();

    }
  }

  delete dlg;

}

void GroupConfigDlg::setFileShareGroup(const KUserGroup & group) {
  m_fileShareGroup = group;

  if (m_fileShareGroup.isValid()) {
    initUsers();
    updateListBox();
    m_gui->groupUsersRadio->setText(
          i18n("Only users of the '%1' group are allowed to share folders",
           m_fileShareGroup.name()));
    m_gui->usersGrpBx->setTitle(i18n("Users of '%1' Group",
           m_fileShareGroup.name()));
    m_gui->otherGroupBtn->setText(i18n("Change Group..."));
    m_gui->usersGrpBx->show();
  } else {
    m_gui->groupUsersRadio->setText(i18n("Only users of a certain group are allowed to share folders"));
    m_gui->otherGroupBtn->setText(i18n("Choose Group..."));
    m_gui->usersGrpBx->hide();
  }



}

bool GroupConfigDlg::addUsersToGroup(QList<KUser> users,const KUserGroup & group) {
  QList<KUser>::ConstIterator it;
  bool result = true;
  for ( it = users.constBegin(); it != users.constEnd(); ++it ) {
    if (!addUser(*it, group))
      result  = false;
  }
  return result;
}

bool GroupConfigDlg::emptyGroup(const QString & s) {
  if (KMessageBox::No == KMessageBox::questionYesNo(this,
        i18n("Do you really want to remove all users from group '%1'?", s), QString(), KStandardGuiItem::del(), KStandardGuiItem::cancel())) {
      return false;
  }

  QList<KUser> allUsers = KUser::allUsers();
  bool result = true;
  KUserGroup group(s);
  QList<KUser>::ConstIterator it;
  for ( it = allUsers.constBegin(); it != allUsers.constEnd(); ++it ) {
    if (!removeUser(*it, group))
      result = false;
  }
  return result;
}

bool GroupConfigDlg::deleteGroup(const QString & s) {
  if (KMessageBox::No == KMessageBox::questionYesNo(this,
        i18n("Do you really want to delete group '%1'?", s), QString(), KStandardGuiItem::del(), KStandardGuiItem::cancel())) {
      return false;
  }

  KProcess proc;
  proc << "groupdel" << s;
  if (proc.execute()) {
    KMessageBox::sorry(this,i18n("Deleting group '%1' failed.", s));
    return false;
  }

  return true;
}

bool GroupConfigDlg::createFileShareGroup(const QString & s) {
  if (s.isEmpty()) {
      KMessageBox::sorry(this,i18n("Please choose a valid group."));
      return false;
  }

  if (KMessageBox::No == KMessageBox::questionYesNo(this,
      i18n("This group '%1' does not exist. Should it be created?", s), QString(), KGuiItem(i18n("Create")), KGuiItem(i18n("Do Not Create"))))
      return false;

  //debug("CreateFileShareGroup: "+s);
  KProcess proc;
  proc << "groupadd" << s;
  if (proc.execute()) {
    KMessageBox::sorry(this,i18n("Creation of group '%1' failed.", s));
    return false;
  } else {
    setFileShareGroup(KUserGroup(s));
  }

  return true;
}


#include "groupconfigdlg.moc"
