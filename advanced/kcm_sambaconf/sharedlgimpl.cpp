/***************************************************************************
                          sharedlgimpl.cpp  -  description
                             -------------------
    begin                : Tue June 6 2002
    copyright            : (C) 2002 by Jan Schäfer
    email                : janschaefer@users.sourceforge.net
 ***************************************************************************/

/******************************************************************************
 *                                                                            *
 *  This file is part of KSambaPlugin.                                          *
 *                                                                            *
 *  KSambaPlugin is free software; you can redistribute it and/or modify            *
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation; either version 2 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  KSambaPlugin is distributed in the hope that it will be useful,                 *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with Foobar; if not, write to the Free Software                     *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA  *
 *                                                                            *
 ******************************************************************************/


/**
 * @author Jan Schäfer
 **/
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlabel.h>

#include <klineedit.h>
#include <kurlrequester.h>
#include <knuminput.h>
#include <kdebug.h>
#include <kcombobox.h>
#include <qgroupbox.h>

#include <assert.h>

#include "sambafile.h"
#include "sharedlgimpl.h"

ShareDlgImpl::ShareDlgImpl(QWidget* parent, SambaShare* share)
	: KcmShareDlg(parent,"sharedlgimpl")
{

  _share = share;
	assert(_share);
  initDialog();
}

void ShareDlgImpl::initDialog()
{
	// Base settings

	assert(_share);
  
  if (!_share)
     return;

	pathUrlRq->setMode(2+8+16);

  homeChk->setChecked( _share->getName() == "homes" );
  pathUrlRq->setURL( _share->getValue("path") );

  shareNameEdit->setText( _share->getName() );

	commentEdit->setText( _share->getValue("comment") );

  availableBaseChk->setChecked( _share->getBoolValue("available") );
  browseableBaseChk->setChecked( _share->getBoolValue("browseable") );
  readOnlyBaseChk->setChecked( ! _share->getBoolValue("writeable") );
  publicBaseChk->setChecked( _share->getBoolValue("public") );

  // User settings

  invalidUsersEdit->setText( _share->getValue("invalid users") );
  forceUserEdit->setText( _share->getValue("force user") );
  forceGroupEdit->setText( _share->getValue("force group") );

  // Filename settings
  
  defaultCaseCombo->setCurrentText( _share->getValue("default case") );
  caseSensitiveChk->setChecked( _share->getBoolValue("case sensitive") );
  preserveCaseChk->setChecked( _share->getBoolValue("preserve case") );
  shortPreserveCaseChk->setChecked( _share->getBoolValue("short preserve case") );
  mangledNamesChk->setChecked( _share->getBoolValue("mangled names") );
  mangeCaseChk->setChecked( _share->getBoolValue("mange case") );
  manglingCharEdit->setText( _share->getValue("mangling char") );

  hideDotFilesChk->setChecked( _share->getBoolValue("hide dot files") );
  hideTrailingDotChk->setChecked( _share->getBoolValue("hide trailing dot") );
  hideUnreadableChk->setChecked( _share->getBoolValue("hide unreadable") );
  dosFilemodeChk->setChecked( _share->getBoolValue("dos filemode") );
  dosFiletimesChk->setChecked( _share->getBoolValue("dos filetimes") );
  dosFiletimeResolutionChk->setChecked( _share->getBoolValue("dos filetime resolution") );
  deleteReadonlyChk->setChecked( _share->getBoolValue("delete readonly") );


  readOnlyChk->setChecked( _share->getBoolValue("read only") );
  guestOkChk->setChecked( _share->getBoolValue("guest ok") );
  guestOnlyChk->setChecked( _share->getBoolValue("guest only") );
  userOnlyChk->setChecked( _share->getBoolValue("user only") );
  hostsAllowEdit->setText( _share->getValue("hosts allow") );
  guestAccountEdit->setText( _share->getValue("guest account") );
  hostsDenyEdit->setText( _share->getValue("hosts deny") );
  forceDirectorySecurityModeEdit->setText( _share->getValue("force directory security mode") );
  forceDirectoryModeEdit->setText( _share->getValue("force directory mode") );
  forceSecurityModeEdit->setText( _share->getValue("force security mode") );
  
  forceCreateModeEdit->setText( _share->getValue("force create mode") );
  directorySecurityMaskEdit->setText( _share->getValue("directory security mask") );
  directoryMaskEdit->setText( _share->getValue("directory mask") );
  securityMaskEdit->setText( _share->getValue("security mask") );
  createMaskEdit->setText( _share->getValue("create mask") );
  inheritPermissionsChk->setChecked( _share->getBoolValue("inherit permissions") );
  
  // Advanced

  blockingLocksChk->setChecked( _share->getBoolValue("blocking locks") );
  fakeOplocksChk->setChecked( _share->getBoolValue("fake oplocks") );
  lockingChk->setChecked( _share->getBoolValue("locking") );
  level2OplocksChk->setChecked( _share->getBoolValue("level2 oplocks") );
  posixLockingChk->setChecked( _share->getBoolValue("posix locking") );
  strictLockingChk->setChecked( _share->getBoolValue("strict locking") );
  shareModesChk->setChecked( _share->getBoolValue("share modes") );
  oplocksChk->setChecked( _share->getBoolValue("oplocks") );
  
  oplockContentionLimitInput->setValue( _share->getValue("oplock contention limit").toInt() );
  strictSyncChk->setChecked( _share->getBoolValue("strict sync") );

  maxConnectionsInput->setValue( _share->getValue("max connections").toInt() );
  writeCacheSizeInput->setValue( _share->getValue("write cache size").toInt() );

  syncAlwaysChk->setChecked( _share->getBoolValue("sync always") );
  statusChk->setChecked( _share->getBoolValue("status") );

  // Hidden files
}

ShareDlgImpl::~ShareDlgImpl()
{
}

#include "sharedlgimpl.moc"
