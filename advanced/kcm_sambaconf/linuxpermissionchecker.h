/***************************************************************************
    begin                : Tue May 17 2003
    copyright            : (C) 2003 by Jan Schäfer
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
#ifndef LINUXPERMISSIONCHECKER_H
#define LINUXPERMISSIONCHECKER_H

#include <qfileinfo.h>

class SambaShare;
class QWidget;

/**
 * A class to check if the Samba permissions specified in the SambaShare fit 
 * to the un?x permissions of the directory. E.g. a user specifies a SambaShare as writeable
 * for the public, but the guest account has no write permissions for the directory of the
 * SambaShare. This will cause a dialog (which can be turned of)  to popup and inform the
 * user about the missing rights and ask him to continue nevertheless or to cancel.
 * This class works for all un?x systems not only for Linux, but I couldn't call the
 * class Un?xPermissionChecker ;-)
 * @author Jan Schaefer
 */
class LinuxPermissionChecker{
public:
    LinuxPermissionChecker(SambaShare*,QWidget* parent );
    ~LinuxPermissionChecker();

    /**
     * Checks all possible errors that the user could made
     * @returns <ul>
     * <li><code>false</code> if the un?x permissions of the directory doesn't fit
     * to the specified permissions in the Samba share and the user has pushed
     * the cancel button of one of the dialogs, that informed her.
     * <li><code>true</code> if either there haven't been any problems with the un?x
     * permissions, or there have been problems, but the user pressed always the continue button.
     */
    bool checkAllPermissions();

    /** 
     * Checks only the public permissions of the directory. First checks
     * if the Samba share is specified public and then checks if the guest account
     * has the appropriate rights.
     * @return @see #check
     */
    bool checkPublicPermissions();
    
    /**
     * Checks all permissions of all users specified in 
     * the Samba share
     * @return @see #check
     */
    bool checkAllUserPermissions();
        
protected:
    
    
    /**
     * Checks all permissions of the given un?x user
     * @return @see #check
     */
    bool checkUserPermissions(const QString & user);

    /** 
     * Checks the write permissions of the given un?x user
     * Does not show a dialog if showMessageBox is false
     * @return  @see #check
     */
    bool checkUserWritePermissions(const QString & user,bool showMessageBox = true);
    
    /**
     * Checks the read permissions of the given un?x user
     * Does not show a dialog if showMessageBox is false
     * @return  @see #check
     */
    bool checkUserReadPermissions(const QString & user,bool showMessageBox = true);
        
    QWidget* m_parent;
    SambaShare* m_sambaShare;
    QFileInfo m_fi;    
};

#endif
