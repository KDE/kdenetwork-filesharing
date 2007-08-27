/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

#ifndef KCM_SAMBACONF_GROUPSELECTDLG_UI_H
#define KCM_SAMBACONF_GROUPSELECTDLG_UI_H

void GroupSelectDlg::init(const QStringList & specifiedGroups)
{
  QStringList unixGroups = getUnixGroups();

  for (QStringList::Iterator it = unixGroups.begin(); it != unixGroups.end(); ++it)
  {
      if ( ! specifiedGroups.contains(*it))
	  new Q3ListViewItem(groupListView, *it, QString::number(getGroupGID(*it)));
  }
}

void GroupSelectDlg::accept()
{
  Q3ListViewItemIterator it( groupListView);

  for ( ; it.current(); ++it ) {
      if ( it.current()->isSelected() )
	  selectedGroups << it.current()->text(0);
  }	
  
  access = accessBtnGrp->id(accessBtnGrp->selected());
  
  if (unixRadio->isChecked())
      groupKind = "+";
  else
      if (nisRadio->isChecked())
	  groupKind = "&";
  else
      if (bothRadio->isChecked())
	  groupKind = "@";
  
  QDialog::accept();
}




QStringList GroupSelectDlg::getSelectedGroups()
{
    return selectedGroups;
}


int GroupSelectDlg::getAccess()
{
    return access;
}


QString GroupSelectDlg::getGroupKind()
{
    return groupKind;
}

#endif // KCM_SAMBACONF_GROUPSELECTDLG_UI_H
