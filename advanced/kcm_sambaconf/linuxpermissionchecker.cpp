/***************************************************************************
    begin                : Tue May 17 2003
    copyright            : (C) 2003 by Jan Sch�fer
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
 *  along with KSambaPlugin; if not, write to the Free Software               *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA  *
 *                                                                            *
 ******************************************************************************/
#include <qregexp.h> 
#include <qstringlist.h> 

#include <kmessagebox.h>
#include <klocale.h>
#include <kdebug.h>

#include "passwd.h"
#include "sambashare.h"  
#include "linuxpermissionchecker.h"

LinuxPermissionChecker::LinuxPermissionChecker(SambaShare* share,QWidget* parent = 0L)
{
  m_sambaShare = share;
  m_parent = parent;

  if (!share) {
    kdWarning() << "WARNING: LinuxPermissionChecker: share is null !" << endl;
    return;        
  }
  
  m_fi = QFileInfo(share->getValue("path"));
  
  if ( ! m_fi.exists()) {
    kdDebug() << "LinuxPermissionChecker: path does not exists !" << endl;
  }

}


LinuxPermissionChecker::~LinuxPermissionChecker()
{
}

bool LinuxPermissionChecker::checkAllPermissions() {
  if (! m_sambaShare )
     return true;
     
  if ( ! m_fi.exists())
     return true;   

  if (! checkPublicPermissions())
     return false;     
     
  if (! checkAllUserPermissions())
     return false;
     
  return true;          
}
    
bool LinuxPermissionChecker::checkAllUserPermissions() {
  if (! m_sambaShare )
     return true;

  if ( ! m_fi.exists())
     return true;   
       
  QStringList readList = QStringList::split(QRegExp("[,\\s]+"),m_sambaShare->getValue("read list"));
  
  for ( QStringList::Iterator it = readList.begin(); it != readList.end(); ++it )
  {
    if (!checkUserReadPermissions(*it))
       return false;
  }    
  
  QStringList writeList = QStringList::split(QRegExp("[,\\s]+"),m_sambaShare->getValue("write list"));
  
  for ( QStringList::Iterator it = writeList.begin(); it != writeList.end(); ++it )
  {
    if (!checkUserWritePermissions(*it))
       return false;
  }    
  
  return true;     
}

bool LinuxPermissionChecker::checkPublicPermissions() {
  if (! m_sambaShare )
     return true;


  bool isPublic = m_sambaShare->getBoolValue("public");
  if (!isPublic)
     return true;
  
  QString guestAccount = m_sambaShare->getValue("guest account");
  
  if ( ! checkUserReadPermissions(guestAccount,false))
  {
     if (KMessageBox::Cancel == KMessageBox::warningContinueCancel(
         0L,i18n(
           "<qt>You have specified public read access for this directory, but "
           "the guest account <i>%1</i> doesn't have the needed read permissions !<br>" 
           "Do you want to continue nevertheless ?</qt>").arg(guestAccount)
           ,i18n("Warning")
           ,KStdGuiItem::cont()
           ,"KSambaPlugin_guestAccountHasNoReadPermissionsWarning"))
         return false;
  }
  
  
  if ( ! checkUserWritePermissions(guestAccount,false))
  {
     if (KMessageBox::Cancel == KMessageBox::warningContinueCancel(
         0L,i18n(
           "<qt>You have specified public write access for this directory, but "
           "the guest account <i>%1</i> doesn't have the needed write permissions !<br>" 
           "Do you want to continue nevertheless ?</qt>").arg(guestAccount)
           ,i18n("Warning")
           ,KStdGuiItem::cont()
           ,"KSambaPlugin_guestAccountHasNoWritePermissionsWarning"))
         return false;
  }
  
  return true;
}
    
bool LinuxPermissionChecker::checkUserPermissions(const QString & user) {
  if ( ! checkUserReadPermissions(user))
     return false;  
  
  if ( ! checkUserWritePermissions(user))
     return false;    
       
  return true;     
}

bool LinuxPermissionChecker::checkUserWritePermissions(const QString & user, bool showMessageBox) {
  if (m_sambaShare->getBoolValue("read only")) {
  
    if (! ((m_fi.permission(QFileInfo::WriteOther)) ||
           (m_fi.permission(QFileInfo::WriteUser) && user == m_fi.owner()) ||
           (m_fi.permission(QFileInfo::WriteGroup) && isUserInGroup(user, m_fi.group())))
       )
    {
     if (!showMessageBox)
        return false;
     
     if (KMessageBox::Cancel == KMessageBox::warningContinueCancel(
         0L,i18n(
           "<qt>You have specified read access to the user <i>%1</i> for this directory, but "
           "the user doesn't have the needed write permissions !<br>" 
           "Do you want to continue nevertheless ?</qt>").arg(user)
           ,i18n("Warning")
           ,KStdGuiItem::cont()
           ,"KSambaPlugin_userHasNoWritePermissionsWarning"))
         return false;
    }
  }    
  
  return true;
}
    
bool LinuxPermissionChecker::checkUserReadPermissions(const QString & user, bool showMessageBox) {
  if (! ((m_fi.permission(QFileInfo::ReadOther)) ||
         (m_fi.permission(QFileInfo::ReadUser) && user == m_fi.owner()) ||
         (m_fi.permission(QFileInfo::ReadGroup) && isUserInGroup(user, m_fi.group())))
     )
  {     
     if (!showMessageBox)
        return false;
        
     if (KMessageBox::Cancel == KMessageBox::warningContinueCancel(
         0L,i18n(
           "<qt>You have specified read access to the user <i>%1</i> for this directory, but "
           "the user doesn't have the needed read permissions !<br>" 
           "Do you want to continue nevertheless ?</qt>").arg(user)
           ,i18n("Warning")
           ,KStdGuiItem::cont()
           ,"KSambaPlugin_userHasNoReadPermissionsWarning"))
         return false;
     
  }
     
  return true;     
}
