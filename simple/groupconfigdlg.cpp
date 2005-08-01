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


#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qvbox.h>
#include <qhbox.h>

#include <klocale.h>
#include <kuser.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kinputdialog.h>
#include <klistbox.h>
#include <kprocess.h>
#include <kpushbutton.h>
#include <kcombobox.h>

#include "groupconfiggui.h"
#include "groupconfigdlg.h"

  
static QString groupListToString(const QValueList<KUserGroup> & list);
static QString prettyString(const KUser &user);
static QString fromPrettyString(const QString & s);
static void removeList(QValueList<KUser> & from, const QValueList<KUser> & that);
static bool userMod(const QString & user, const QValueList<KUserGroup> & groups);



GroupConfigDlg::GroupConfigDlg(QWidget * parent,
      const QString & fileShareGroup, bool restricted,
      bool rootPassNeeded, bool simpleSharing) 
  : KDialogBase(parent,"groupconfigdlg", true,
                i18n("Allowed Users"), Ok|Cancel, Ok, true) ,
  m_fileShareGroup(fileShareGroup),
  m_restricted(restricted) ,
  m_rootPassNeeded(rootPassNeeded),
  m_simpleSharing(simpleSharing)              
   
{
  initGUI();

  setFileShareGroup(m_fileShareGroup);
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
  QValueList<KUser>::iterator it;
  for ( it = m_users.begin(); it != m_users.end(); ++it ) {
     m_gui->listBox->insertItem(prettyString(*it));
     kdDebug(5009) << "GroupConfigDlg::updateListBox: " << (*it).loginName() << endl;
  }
}

QString prettyString(const KUser &user) {
  return user.fullName()+" ("+user.loginName()+")";
}

QString fromPrettyString(const QString & s) {
  // Jan Schaefer (jan)
  //              i   j
  int i = s.find('(');
  int j = s.find(')');
  QString loginName = s.mid(i+1,j-i-1);
  return loginName;
}

bool GroupConfigDlg::restricted() {
  return m_restricted;
}

void GroupConfigDlg::slotAddUser() {
  QValueList<KUser> allUsers = KUser::allUsers();
  
  removeList(allUsers,m_users);
  
  if (allUsers.count()==0) {
     KMessageBox::information(this,
           i18n("All users are in the %1 group already.")
           .arg(m_fileShareGroup.name()));
     return;           
  }

  QStringList stringList;
  
  QValueList<KUser>::iterator it;
  for ( it = allUsers.begin(); it != allUsers.end(); ++it ) {
    QString s = (*it).fullName()+" ("+(*it).loginName()+")";
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

void removeList(QValueList<KUser> & from, const QValueList<KUser> & that) {
  QValueList<KUser>::ConstIterator it;
  for ( it = that.begin(); it != that.end(); ++it ) {
    from.remove(*it);
  } 

}

bool GroupConfigDlg::addUser(const KUser & user, const KUserGroup & group) {
  QValueList<KUserGroup> groups = user.groups();
  groups.append(group);
  if (!userMod(user.loginName(),groups)) {
    KMessageBox::sorry(this,i18n("Could not add user '%1' to group '%2'")
            .arg(user.loginName()).arg(group.name()));
    return false;            
  } 
  return true;
}


bool GroupConfigDlg::removeUser(const KUser & user, const KUserGroup & group) {
  QValueList<KUserGroup> groups = user.groups();
  groups.remove(group);
  if (!userMod(user.loginName(),groups)) {
    KMessageBox::sorry(this,i18n("Could not remove user '%1' from group '%2'")
            .arg(user.loginName()).arg(group.name()));
    return false;            
  } 
  return true;
}

bool GroupConfigDlg::rootPassNeeded() {
  return m_rootPassNeeded;
}

void GroupConfigDlg::slotOk() {
  m_restricted = m_gui->groupUsersRadio->isChecked();
  m_rootPassNeeded = ! m_gui->writeAccessChk->isChecked();
  if (m_restricted && !m_fileShareGroup.isValid()) {
    KMessageBox::sorry(this,i18n("You have to choose a valid group."));
    return;
  }

  QValueList<KUser> addedUsers = m_users;
  removeList(addedUsers,m_origUsers);
  QValueList<KUser> removedUsers = m_origUsers;
  removeList(removedUsers,m_users);
  
  QValueList<KUser>::ConstIterator it;
  for ( it = addedUsers.begin(); it != addedUsers.end(); ++it ) {
    addUser(*it, m_fileShareGroup);
  }
  
  for ( it = removedUsers.begin(); it != removedUsers.end(); ++it ) {
    removeUser(*it, m_fileShareGroup);
  }
              
  
  KDialogBase::slotOk();
}

bool userMod(const QString & user, const QValueList<KUserGroup> & groups) {
  KProcess proc;
  proc << "usermod" << "-G" << groupListToString(groups) << user;
  return proc.start(KProcess::Block) && proc.normalExit();
}

void GroupConfigDlg::slotRemoveUser() {
  QListBoxItem* item = m_gui->listBox->selectedItem();
  if (!item)
      return;
      
  QString loginName = fromPrettyString(item->text());
  KUser user(loginName);
  m_users.remove(KUser(loginName));
  updateListBox();  
  m_gui->removeBtn->setEnabled(false);
}

QString groupListToString(const QValueList<KUserGroup> & list) {
  QValueList<KUserGroup>::ConstIterator it;
  QString result;
  
  for ( it = list.begin(); it != list.end(); ++it ) {
    result+=(*it).name()+",";
  }
  
  // remove last ,
  result.truncate(result.length()-1);
  return result;
}

void GroupConfigDlg::slotChangeGroup() {
  QValueList<KUserGroup> allGroups = KUserGroup::allGroups();
  
  QStringList stringList;
  
  QValueList<KUserGroup>::iterator it;
  for ( it = allGroups.begin(); it != allGroups.end(); ++it ) {
    QString s = (*it).name();
    stringList.append(s);
  }
  
  stringList.sort();
  
  KDialogBase* dlg = new KDialogBase(this,"groupconfigdlg", true,
                i18n("Allowed Users"), Ok|Cancel, Ok, true);
                
  QVBox* vbox = dlg->makeVBoxMainWidget();
    
  QHBox* hbox = new QHBox(vbox);
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
          i18n("Only users of the '%1' group are allowed to share folders")
          .arg(m_fileShareGroup.name()));
    m_gui->usersGrpBx->setTitle(i18n("Users of '%1' Group")
          .arg(m_fileShareGroup.name()));
    m_gui->otherGroupBtn->setText(i18n("Change Group..."));          
    m_gui->usersGrpBx->show();
  } else {
    m_gui->groupUsersRadio->setText(i18n("Only users of a certain group are allowed to share folders"));
    m_gui->otherGroupBtn->setText(i18n("Choose Group..."));
    m_gui->usersGrpBx->hide();
  }    

  
  
}

bool GroupConfigDlg::addUsersToGroup(QValueList<KUser> users,const KUserGroup & group) {
  QValueList<KUser>::ConstIterator it;
  bool result = true;
  for ( it = users.begin(); it != users.end(); ++it ) {
    if (!addUser(*it, group))
      result  = false;
  }
  return result;
}

bool GroupConfigDlg::emptyGroup(const QString & s) {
  if (KMessageBox::No == KMessageBox::questionYesNo(this, 
        i18n("Do you really want to remove all users from group '%1'?").arg(s), QString::null, KStdGuiItem::del(), KStdGuiItem::cancel())) {
      return false;        
  }

  QValueList<KUser> allUsers = KUser::allUsers();
  bool result = true;
  KUserGroup group(s);
  QValueList<KUser>::ConstIterator it;
  for ( it = allUsers.begin(); it != allUsers.end(); ++it ) {
    if (!removeUser(*it, group)) 
      result = false;
  }
  return result;
}

bool GroupConfigDlg::deleteGroup(const QString & s) {
  if (KMessageBox::No == KMessageBox::questionYesNo(this, 
        i18n("Do you really want to delete group '%1'?").arg(s), QString::null, KStdGuiItem::del(), KStdGuiItem::cancel())) {
      return false;        
  }
        
  KProcess proc;
  proc << "groupdel" << s;
  bool result = proc.start(KProcess::Block) && proc.normalExit();
  if (!result) {
    KMessageBox::sorry(this,i18n("Deleting group '%1' failed.").arg(s));
  } 
  
  return result;
}

bool GroupConfigDlg::createFileShareGroup(const QString & s) {
  if (s.isEmpty()) {
      KMessageBox::sorry(this,i18n("Please choose a valid group."));
      return false;
  }

  if (KMessageBox::No == KMessageBox::questionYesNo(this,
      i18n("This group '%1' does not exist. Should it be created?").arg(s), QString::null, i18n("Create"), i18n("Do Not Create")))
      return false;
      
  //debug("CreateFileShareGroup: "+s);
  KProcess proc;
  proc << "groupadd" << s;
  bool result = proc.start(KProcess::Block) && proc.normalExit();
  if (!result) {
    KMessageBox::sorry(this,i18n("Creation of group '%1' failed.").arg(s));
  } else {
    setFileShareGroup(KUserGroup(s));
  }
  
  return result;
}


#include "groupconfigdlg.moc"
