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
 *  along with KSambaPlugin; if not, write to the Free Software                     *
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
#include <qgroupbox.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <qregexp.h>
#include <qstringlist.h>
#include <qgrid.h>
#include <qcursor.h>
#include <qtable.h>
#include <qlistbox.h>
#include <qtoolbutton.h>
#include <qpixmap.h>
#include <qframe.h>
#include <qwidget.h>
#include <qtabwidget.h>


#include <klineedit.h>
#include <kurlrequester.h>
#include <knuminput.h>
#include <kdebug.h>
#include <kcombobox.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kfiledetailview.h>
#include <kdirlister.h>
#include <kmessagebox.h>
#include <kpopupmenu.h>
#include <kaction.h>
#include <krestrictedline.h>
#include <kjanuswidget.h>

#include "smbpasswdfile.h"
#include "sambafile.h"
#include "common.h"
#include "passwd.h"
#include "usertabimpl.h"
#include "filemodedlgimpl.h"
#include "hiddenfileview.h"
#include "dictmanager.h"

#include "sharedlgimpl.h"




ShareDlgImpl::ShareDlgImpl(QWidget* parent, SambaShare* share)
	: KcmShareDlg(parent,"sharedlgimpl")
{
  if (!share) {
    kdWarning() << "ShareDlgImpl::Constructor : share parameter is null!" << endl;
    return;    
  }
  
  _dictMngr = new DictManager(share);
  _share = share;
  
  initDialog();
  initAdvancedTab();
}

void ShareDlgImpl::initDialog()
{
  if (!_share)
     return;

  // Base settings
  _fileView = 0L;

	pathUrlRq->setMode(2+8+16);

  homeChk->setChecked(_share->getName().lower() == "homes");
  shareNameEdit->setText( _share->getName() );

  _dictMngr->add("path",pathUrlRq);

  _dictMngr->add("comment",commentEdit);
  _dictMngr->add("available",availableBaseChk);
  _dictMngr->add("browseable",browseableBaseChk);
  _dictMngr->add("public",publicBaseChk);

  _dictMngr->add("read only",readOnlyBaseChk);

  // User settings

  _userTab = new UserTabImpl(this,_share);
  _tabs->insertTab(_userTab,i18n("&Users"),1);
  _userTab->load();
  connect(_userTab, SIGNAL(changed()), this, SLOT(changedSlot()));

  // Filename settings

  _dictMngr->add("case sensitive",caseSensitiveCombo,
		 new QStringList(QStringList() << "auto" << "yes" << "no"));
  _dictMngr->add("preserve case",preserveCaseChk);
  _dictMngr->add("short preserve case",shortPreserveCaseChk);
  _dictMngr->add("mangled names",mangledNamesChk);
  _dictMngr->add("mangle case",mangleCaseChk);
  _dictMngr->add("mangling char",manglingCharEdit);
  _dictMngr->add("mangled map",mangledMapEdit);


  _dictMngr->add("mangling method",manglingMethodCombo,
                 new QStringList(QStringList() << "hash" << "hash2"));

  _dictMngr->add("default case",defaultCaseCombo,
                 new QStringList(QStringList() << "Lower" << "Upper"));

  _dictMngr->add("hide dot files",hideDotFilesChk);
  _dictMngr->add("strip dot",hideTrailingDotChk);
  _dictMngr->add("hide unreadable",hideUnreadableChk);
  _dictMngr->add("hide unwriteable files",hideUnwriteableFilesChk);
  _dictMngr->add("hide special files",hideSpecialFilesChk);
  _dictMngr->add("dos filemode",dosFilemodeChk);
  _dictMngr->add("dos filetimes",dosFiletimesChk);
  _dictMngr->add("dos filetime resolution",dosFiletimeResolutionChk);

  // Security tab

  _dictMngr->add("guest only",guestOnlyChk);
  _dictMngr->add("hosts allow",hostsAllowEdit);

  _dictMngr->add("only user",onlyUserChk);
  _dictMngr->add("username",userNameEdit);


  guestAccountCombo->insertStringList( getUnixUsers() );
  setComboToString(guestAccountCombo,_share->getValue("guest account"));

  _dictMngr->add("hosts deny",hostsDenyEdit);
  _dictMngr->add("force directory security mode",forceDirectorySecurityModeEdit);
  _dictMngr->add("force directory mode",forceDirectoryModeEdit);
  _dictMngr->add("force security mode",forceSecurityModeEdit);

  _dictMngr->add("force create mode",forceCreateModeEdit);
  _dictMngr->add("directory security mask",directorySecurityMaskEdit);
  _dictMngr->add("directory mask",directoryMaskEdit);
  _dictMngr->add("security mask",securityMaskEdit);
  _dictMngr->add("create mask",createMaskEdit);
  _dictMngr->add("inherit permissions",inheritPermissionsChk);
  _dictMngr->add("inherit acls",inheritAclsChk);
  _dictMngr->add("nt acl support",ntAclSupportChk);
  _dictMngr->add("delete readonly",deleteReadonlyChk);

  _dictMngr->add("wide links",wideLinksChk);
  _dictMngr->add("follow symlinks",followSymlinksChk);

  _dictMngr->add("map hidden",mapHiddenChk);
  _dictMngr->add("map archive",mapArchiveChk);
  _dictMngr->add("map system",mapSystemChk);
  _dictMngr->add("store dos attributes",eaSupportChk);
  
  _dictMngr->add("ea support",eaSupportChk);

  
  _dictMngr->add("force unknown acl user",forceUnknownAclUserEdit);
  _dictMngr->add("profile acls",profileAclsChk);
  _dictMngr->add("map acl inherit",mapAclInheritChk);


  // Advanced

  _dictMngr->add("blocking locks",blockingLocksChk);
  _dictMngr->add("fake oplocks",fakeOplocksChk);
  _dictMngr->add("locking",lockingChk);
  _dictMngr->add("level2 oplocks",level2OplocksChk);
  _dictMngr->add("posix locking",posixLockingChk);
  _dictMngr->add("strict locking",strictLockingCombo,
   		  new QStringList(QStringList() << "Auto" << "yes" << "no"));
  _dictMngr->add("share modes",shareModesChk);
  _dictMngr->add("oplocks",oplocksChk);
  

  _dictMngr->add("oplock contention limit",oplockContentionLimitSpin);
  _dictMngr->add("strict sync",strictSyncChk);
  
  // Tuning 
  
  _dictMngr->add("strict allocate",strictAllocateChk);
  
  _dictMngr->add("max connections",maxConnectionsSpin);
  _dictMngr->add("write cache size",writeCacheSizeSpin);
  _dictMngr->add("block size",blockSizeSpin);

  
  _dictMngr->add("sync always",syncAlwaysChk);
  _dictMngr->add("use sendfile",useSendfileChk);

  _dictMngr->add("csc policy",cscPolicyCombo,
                 new QStringList(QStringList() << "manual" << "documents" << "programs" << "disable"));
  
  

  // VFS
  
  _dictMngr->add("vfs objects",vfsObjectsEdit);
  _dictMngr->add("vfs options",vfsOptionsEdit);

  // Misc
  
  _dictMngr->add("preexec",preexecEdit);
  _dictMngr->add("postexec",postexecEdit);
  _dictMngr->add("root preexec",rootPreexecEdit);
  _dictMngr->add("root postexec",rootPostexecEdit);
  
  _dictMngr->add("preexec close",preexecCloseChk);
  _dictMngr->add("root preexec close",rootPreexecCloseChk);
  
  _dictMngr->add("volume",volumeEdit);
  _dictMngr->add("fstype",fstypeEdit);
  _dictMngr->add("magic script",magicScriptEdit);
  _dictMngr->add("magic output",magicOutputEdit);
  _dictMngr->add("dont descend",dontDescendEdit);
  _dictMngr->add("set directory",setDirectoryChk);
  _dictMngr->add("fake directory create times",fakeDirectoryCreateTimesChk);
    
  _dictMngr->add("msdfs root",msdfsRootChk);
  _dictMngr->add("msdfs proxy",msdfsProxyEdit);
  
  _dictMngr->load( _share );
  

  connect( _tabs, SIGNAL(currentChanged(QWidget*)), this, SLOT(tabChangedSlot(QWidget*)));
  connect(_dictMngr, SIGNAL(changed()), this, SLOT(changedSlot()));
}

ShareDlgImpl::~ShareDlgImpl()
{
  delete _fileView;
}

void ShareDlgImpl::initAdvancedTab() 
{
	
  QVBoxLayout *l = new QVBoxLayout(advancedFrame);
	l->setAutoAdd(true);
	l->setMargin(0);
	_janus = new KJanusWidget(advancedFrame,0,KJanusWidget::TreeList);
	_janus->setRootIsDecorated(false);
	_janus->setShowIconsInTreeList(true);
	
	QWidget *w;
	QFrame *f;
	QString label;
 	QPixmap icon;
	
	for (int i=0;i<advancedDumpTab->count();)
	{
		w = advancedDumpTab->page(i);
		label = advancedDumpTab->label(i);

		if (label.lower() == "security")
			 icon = SmallIcon("password");
		else
		if (label.lower() == "tuning")
			 icon = SmallIcon("launch");
		else
		if (label.lower() == "filenames")
			 icon = SmallIcon("folder");
		else
		if (label.lower() == "printing")
			 icon = SmallIcon("fileprint");
		else
		if (label.lower() == "locking")
			 icon = SmallIcon("lock");
		else
		if (label.lower() == "logon")
			 icon = SmallIcon("kdmconfig");
		else
		if (label.lower() == "protocol")
			 icon = SmallIcon("core");
		else
		if (label.lower() == "coding")
			 icon = SmallIcon("charset");
		else
		if (label.lower() == "socket")
				icon = SmallIcon("socket");
		else
		if (label.lower() == "ssl")
			 icon = SmallIcon("encrypted");
		else
		if (label.lower() == "browsing")
			 icon = SmallIcon("konqueror");
		else
		if (label.lower() == "misc")
			 icon = SmallIcon("misc");
		else {
			 icon = QPixmap(16,16);
			 icon.fill();
		}
			 //SmallIcon("empty2");
		
		f = _janus->addPage( label,label,icon );
		l = new QVBoxLayout(f);
	  l->setAutoAdd(true);
		l->setMargin(0);
		
		advancedDumpTab->removePage(w);
		
		w->reparent(f,QPoint(1,1),TRUE);
		
	}
	
	w = _tabs->page(5);
	_tabs->removePage(w);
	delete w;


}


void ShareDlgImpl::tabChangedSlot(QWidget* w)
{
  // We are only interrested in the Hidden files tab
  if ( QString(w->name()) == "hiddenFilesTab" )
     loadHiddenFilesView();

}

void ShareDlgImpl::loadHiddenFilesView()
{

  if (_fileView)
     return;
     
  _fileView = new HiddenFileView( this, _share );

  if ( ! _share->isSpecialSection())
     _fileView->load();


}

void ShareDlgImpl::accept()
{
	// Base settings
  if (!_share)
     return;

  if (homeChk->isChecked())
  	 _share->setName("homes");
	else
    _share->setName(shareNameEdit->text());

  // User settings

  _userTab->save();

  // Security
  
  _share->setValue("guest account",guestAccountCombo->currentText( ) );

  
  // Hidden files
  if (_fileView)
      _fileView->save();

  _dictMngr->save( _share );      
      
	KcmShareDlg::accept();
}

void ShareDlgImpl::homeChkToggled(bool b)
{
  shareNameEdit->setDisabled(b);
	pathUrlRq->setDisabled(b);

	if (b)
  {
  	shareNameEdit->setText("homes");
    pathUrlRq->setURL("");
    directoryPixLbl->setPixmap(DesktopIcon("folder_home",48));
		
  }
  else
  {
  	shareNameEdit->setText( _share->getName() );
	  pathUrlRq->setURL( _share->getValue("path") );
    directoryPixLbl->setPixmap(DesktopIcon("folder"));
  }
}

void ShareDlgImpl::accessModifierBtnClicked()
{
  if (!QObject::sender()) {
    kdWarning() << "ShareDlgImpl::accessModifierBtnClicked() : QObject::sender() is null!" << endl;
    return;
  }
  
  
  QString name = QObject::sender()->name();
  
  QLineEdit *edit = 0L;
      
  if (name == "forceCreateModeBtn")
     edit = forceCreateModeEdit;
  else
  if (name == "forceSecurityModeBtn")
     edit = forceSecurityModeEdit;
  else
  if (name == "forceDirectoryModeBtn")
     edit = forceDirectoryModeEdit;
  else
  if (name == "forceDirectorySecurityModeBtn")
     edit = forceDirectorySecurityModeEdit;
  else
  if (name == "createMaskBtn")
     edit = createMaskEdit;
  else
  if (name == "securityMaskBtn")
     edit = securityMaskEdit;
  else
  if (name == "directoryMaskBtn")
     edit = directoryMaskEdit;
  else
  if (name == "directorySecurityMaskBtn")
     edit = directorySecurityMaskEdit;

  if (!edit) {
    kdWarning() << "ShareDlgImpl::accessModifierBtnClicked() : edit is null! name=" << name << endl;
    return;
  }
  
  FileModeDlgImpl dlg(this, edit);

  dlg.exec();
}

void ShareDlgImpl::changedSlot() {
  m_changed = true;
  kdDebug(5009) << "ShareDlgImpl::changedSlot()" << endl;
  emit changed();
}

void ShareDlgImpl::pathUrlRq_textChanged( const QString & s)
{
  if (_fileView && ! _share->isSpecialSection())
     _fileView->load();
}


#include "sharedlgimpl.moc"
