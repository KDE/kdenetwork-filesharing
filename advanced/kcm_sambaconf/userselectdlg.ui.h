/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/


void UserSelectDlg::init(const QStringList & specifiedUsers, SambaShare* share)
{
  SmbPasswdFile passwd( KURL(share->getValue("smb passwd file",true,true)) );
  SambaUserList sambaList = passwd.getSambaUserList();

  for (SambaUser * user = sambaList.first(); user; user = sambaList.next() )
  {
    if (! specifiedUsers.contains(user->name))
        new QListViewItem(userListView, user->name, QString::number(user->uid), QString::number(user->gid));	
  }

}


void UserSelectDlg::accept()
{
  QListViewItemIterator it( userListView);

  for ( ; it.current(); ++it ) {
      if ( it.current()->isSelected() )
	  selectedUsers << it.current()->text(0);
  }	
  
  access = accessBtnGrp->id(accessBtnGrp->selected());
  
  QDialog::accept();

}


QStringList UserSelectDlg::getSelectedUsers()
{
    return selectedUsers;
}


int UserSelectDlg::getAccess()
{
    return access;
}
