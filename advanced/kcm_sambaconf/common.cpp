/***************************************************************************
                          common.cpp  -  description
                             -------------------
    begin                : Tue June 6 2002
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
 *  along with KSambaPlugin; if not, write to the Free Software               *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA  *
 *                                                                            *
 ******************************************************************************/

#include <qstring.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <qfileinfo.h>

#include <kmessagebox.h>
#include <klocale.h>

#include "passwd.h"
#include "sambashare.h"
#include "common.h"

void setComboToString(QComboBox* combo,const QString & s)
{
  int i = combo->listBox()->index(combo->listBox()->findItem(s,Qt::ExactMatch));
  combo->setCurrentItem(i);
}

/** 
 * Checks wether the UNIX permissions fit to the 
 * permissions specified in the SambaShare.
 * If there are less permissions on the UNIX side
 * a message box is shown to inform the user.
 * @return true if all is ok, false if the user choosed to correct something
 */
bool checkIfUnixPermissions(SambaShare* share) {
  if (!share) 
     return true;
     
  QString path = share->getValue("path");
  QFileInfo fi(path);
  
  if ( ! fi.exists()) 
     return true;
  
  if (! checkPublicPermissions(fi,share))
     return false;     
       
  return true;     
}

bool checkPublicPermissions(const QFileInfo & fi, SambaShare* share) {
  bool isPublic = share->getBoolValue("public");
  if (!isPublic)
     return true;
  
  bool readOnly = share->getBoolValue("read only");
  QString guestAccount = share->getValue("guest account");
  
  if (!readOnly) {
  
    if (! ((fi.permission(QFileInfo::WriteOther)) ||
           (fi.permission(QFileInfo::WriteUser) && guestAccount == fi.owner()) ||
           (fi.permission(QFileInfo::WriteGroup) && isUserInGroup(guestAccount, fi.group())))
       )
    {       
       if (KMessageBox::Cancel == KMessageBox::warningContinueCancel(
         0L,i18n(
           "<qt>You have specified public write access for this directory, but "
           "the guest account <i>%1</i> doesn't have UNIX write permissions !<br>" 
           "Do you want to continue nevertheless ?</qt>").arg(guestAccount)
           ,i18n("Warning")
           ,KStdGuiItem::cont()
           ,"KSambaPlugin_guestAccountHasNoWritePermissionsWarning"))
         return false;
     }   
  }
  
  return true;
}


