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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA  *
 *                                                                            *
 ******************************************************************************/

/**
 * @author Jan Schäfer
 **/

#include <qcheckbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <q3groupbox.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <qregexp.h>
#include <qstringlist.h>
#include <q3grid.h>
#include <qcursor.h>
#include <q3table.h>
#include <q3listbox.h>
#include <qtoolbutton.h>
#include <qpixmap.h>
#include <q3frame.h>
#include <qwidget.h>
#include <QVBoxLayout>
#include <QList>
#include <QLabel>

#include <klineedit.h>
#include <kurlrequester.h>
#include <knuminput.h>
#include <kdebug.h>
#include <kcombobox.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kdirlister.h>
#include <kmessagebox.h>
#include <kmenu.h>
#include <kaction.h>
#include <krestrictedline.h>
#include <KPageWidget>

#include "smbpasswdfile.h"
#include "sambafile.h"
#include "common.h"
#include "passwd.h"
#include "usertabimpl.h"
#include "filemodedlgimpl.h"
#include "hiddenfileview.h"
#include "dictmanager.h"

#include "sharedlgimpl.h"

#define DEBUG 5009

ShareDlgImpl::ShareDlgImpl(QWidget* parent, SambaShare* share)
  : QDialog(parent)
{
  if (!share) {
    kWarning() << "share parameter is null!";
    return;
  }

  kDebug(DEBUG) << "setting up ui ..." << endl;
  setupUi(this);

  kDebug(DEBUG) << "creating DictManager ..." << endl;
  _dictMngr = new DictManager(share);
  _share = share;

  initDialog();
  initAdvancedTab();

  kDebug(DEBUG) << "finished" << endl;

}

void ShareDlgImpl::initDialog()
{
  if (!_share)
     return;

  kDebug(DEBUG) << "started" << endl;

  // Base settings
  _fileView = 0L;

  kDebug(DEBUG) << "setting pathUrlRq mode ..." << endl;

  pathUrlRq->setMode(KFile::Directory | KFile::ExistingOnly | KFile::LocalOnly);

  kDebug(DEBUG) << "checking homes ..." << endl;

  homeChk->setChecked(_share->getName().toLower() == "homes");
  shareNameEdit->setText( _share->getName() );

  directoryPixLbl->setPixmap(DesktopIcon("folder"));
  PixmapLabel1->setPixmap(SmallIcon("dialog-warning"));


  _dictMngr->add("path",pathUrlRq);

  _dictMngr->add("comment",commentEdit);
  _dictMngr->add("available",availableBaseChk);
  _dictMngr->add("browseable",browseableBaseChk);
  _dictMngr->add("public",publicBaseChk);

  _dictMngr->add("read only",readOnlyBaseChk);

  // User settings

  kDebug(DEBUG) << "creating UserTab ..." << endl;
  _userTab = new UserTabImpl(this,_share);
  _tabs->insertTab(1,_userTab,i18n("&Users"));
  kDebug(DEBUG) << "loading UserTab ..." << endl;
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
		 new QStringList(QStringList() << "auto" << "yes" << "no"));
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
  _dictMngr->add("do not descend",dontDescendEdit);
  _dictMngr->add("set directory",setDirectoryChk);
  _dictMngr->add("fake directory create times",fakeDirectoryCreateTimesChk);

  _dictMngr->add("msdfs root",msdfsRootChk);
  _dictMngr->add("msdfs proxy",msdfsProxyEdit);

  kDebug(DEBUG) << "dictMngr: loading share ..." << endl;

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
  kDebug(DEBUG) << "started" << endl;

  QVBoxLayout *l = new QVBoxLayout(advancedFrame);
	l->setMargin(0);
	_janus = new KPageWidget(advancedFrame);
	_janus->setFaceType(KPageView::List);
	l->addWidget(_janus);
// 	_janus->setRootIsDecorated(false);
// 	_janus->setShowIconsInTreeList(true);

	KPageWidgetItem *f;
 	QPixmap icon;

	QList<QWidget*> widgets;
	QList<QString> labels;

	for (int i=0;i<advancedDumpTab->count();i++) {
	  kDebug(DEBUG) <<  advancedDumpTab->tabText(i) << endl;
	  widgets.append(advancedDumpTab->widget(i));
	  labels.append(advancedDumpTab->tabText(i));
        }

	KIconLoader* il = KIconLoader::global();

	for (int i=0; i<widgets.size(); i++)
	{
	  QWidget* w = widgets.at(i);
	  QString label = labels.at(i);

		kDebug(DEBUG) << "handling widget " << label << endl;
		if (label.toLower() == "security")
		  //		  icon = il->loadIcon("kgpg-key1-kgpg",KIconLoader::Small);
		  icon = il->loadIcon("dialog-password",KIconLoader::Small);
		else
		if (label.toLower() == "tuning")
		  icon = il->loadIcon("preferences-system-performance",KIconLoader::Small);
		else
		if (label.toLower() == "vfs")
		  icon = il->loadIcon("folder",KIconLoader::Small);
		else
		if (label.toLower() == "filenames")
		  icon = il->loadIcon("text-plain",KIconLoader::Small);//SmallIcon("folder");
		else
		if (label.toLower() == "exec")
		  icon = il->loadIcon("gear",KIconLoader::Small);
		else
		if (label.toLower() == "locking")
		  icon = il->loadIcon("object-locked",KIconLoader::Small);//SmallIcon("folder");
		else
		if (label.toLower() == "misc")
		  icon = il->loadIcon("preferences-other",KIconLoader::Small);
		else {
			 icon = QPixmap(16,16);
			 icon.fill();
		}
			 //SmallIcon("empty2");

		QWidget* frame = new QWidget();
		QVBoxLayout* l = new QVBoxLayout(frame);
		KPageWidgetItem *page = new KPageWidgetItem( frame, label );
		l->setMargin(0);
		l->addWidget(w);
		kDebug(DEBUG) << "adding page ..." << endl;
		_janus->addPage(page);
		w->show();
		page->setIcon(KIcon(icon));

		//		advancedDumpTab->removeTab(advancedDumpTab->indexOf(w));
	}

	QWidget *w = _tabs->page(5);
	_tabs->removeTab(5);
	delete w;

        storeDosAttributesChk_toggled( false );
}


void ShareDlgImpl::tabChangedSlot(QWidget* w)
{
  // We are only interested in the Hidden files tab
  if ( QString(w->objectName()) == "hiddenFilesTab" )
     loadHiddenFilesView();

}

void ShareDlgImpl::loadHiddenFilesView()
{

  if (_fileView)
     return;

  kDebug(DEBUG) << "creating HiddenFileView ..." << endl;

  _fileView = new HiddenFileView( this, _share );

  if ( ! _share->isSpecialSection()) {
    kDebug(DEBUG) << "loading HiddenFileView ..." << endl;
    _fileView->load();
  }


}

void ShareDlgImpl::accept()
{
  if (!_share)
     return;

  if (homeChk->isChecked())
  	 _share->setName("homes");
	else
    _share->setName(shareNameEdit->text());

  _userTab->save();

  _share->setValue("guest account",guestAccountCombo->currentText( ) );

  if (_fileView)
      _fileView->save();

  _dictMngr->save( _share );

  QDialog::accept();
}

void ShareDlgImpl::homeChkToggled(bool b)
{
  shareNameEdit->setDisabled(b);
	pathUrlRq->setDisabled(b);

	if (b)
  {
  	shareNameEdit->setText("homes");
    pathUrlRq->setUrl(KUrl(""));
    directoryPixLbl->setPixmap(DesktopIcon("user-home",48));

  }
  else
  {
  	shareNameEdit->setText( _share->getName() );
	  pathUrlRq->setUrl( _share->getValue("path") );
    directoryPixLbl->setPixmap(DesktopIcon("folder"));
  }
}

void ShareDlgImpl::accessModifierBtnClicked()
{
  if (!QObject::sender()) {
    kWarning() << "ShareDlgImpl::accessModifierBtnClicked() : QObject::sender() is null!";
    return;
  }


  QString name = QObject::sender()->objectName();

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
    kWarning() << "ShareDlgImpl::accessModifierBtnClicked() : edit is null! name=" << name;
    return;
  }

  FileModeDlgImpl dlg(this, edit);

  dlg.exec();
}

void ShareDlgImpl::changedSlot() {
  m_changed = true;
  kDebug(5009) << "ShareDlgImpl::changedSlot()";
  emit changed();
}

void ShareDlgImpl::pathUrlRq_textChanged( const QString &)
{
  if (_fileView && ! _share->isSpecialSection())
     _fileView->load();
}


void ShareDlgImpl::checkValues()
{
/*
	bool state = true;
	// Check if the ok-button should be enabled
	if (directory->isChecked()){
	    if (homes->isChecked()){
		state = true;
	    }else{
		if (shareName->text() == "")
		    state = false;
		if (path->url() == "")
		    state = false;
	    }
	}else{
	    if (printers->isChecked()){
		state = true;
	    }else{
		if (shareName->text() == ""){
		    state = false;
		}
	    }
	}
	buttonOk->setEnabled(state);
*/
}


void ShareDlgImpl::guestOnlyChk_toggled( bool b)
{
  if (b)
  {
    onlyUserChk->setChecked(false);
    publicBaseChk->setChecked(true);
  }

  onlyUserChk->setDisabled(b);
  publicBaseChk->setDisabled(b);
}


void ShareDlgImpl::userOnlyChk_toggled( bool b)
{
  if (b)
  {
    guestOnlyChk->setChecked(false);
    publicBaseChk->setChecked(false);
  }

  guestOnlyChk->setDisabled(b);
  publicBaseChk->setDisabled(b);
}

void ShareDlgImpl::publicBaseChk_toggled( bool b)
{
    guestOnlyChk->setEnabled(b);
    if (!b) {
       guestOnlyChk->setChecked(false);
   }
    guestAccountCombo->setEnabled(b);
    guestAccountLbl->setEnabled(b);

}


void ShareDlgImpl::oplocksChk_toggled( bool b)
{
    if (b)
	fakeOplocksChk->setChecked(false);
}


void ShareDlgImpl::lockingChk_toggled( bool b)
{
  // Its Dangerous to disable locking !
/*
    if (!b)  {
	enableLockingWarnPix->setPixmap(SmallIcon("dialog-warning"));
	enableLockingWarnPix->show();
    } else {
	enableLockingWarnPix->hide();
    }
*/

}


void ShareDlgImpl::fakeOplocksChk_toggled( bool b)
{
/*
    if (b)  {
	fakeOplocksWarnPix->setPixmap(SmallIcon("dialog-information"));
	fakeOplocksWarnPix->setText(i18n("Better use the real oplocks support than this parameter"));
	fakeOplocksWarnPix->showMaximized();
	fakeOplocksWarnPix->show();
    } else {
	fakeOplocksWarnPix->hide();
    }
*/
}


void ShareDlgImpl::oplockContentionLimitSpin_valueChanged( int i)
{
/*
    oplockContentionLimitWarnPix->setMaximumWidth(32767);
    oplockContentionLimitWarnPix->setPixmap(SmallIcon("dialog-error"));
*/
    //oplockContentionLimitWarnPix->show();

}


void ShareDlgImpl::storeDosAttributesChk_toggled( bool b)
{
    mapArchiveChk->setDisabled(b);
    mapSystemChk->setDisabled(b);
    mapHiddenChk->setDisabled(b);

    if (b) {
	mapArchiveChk->setChecked(false);
	mapSystemChk->setChecked(false);
	mapHiddenChk->setChecked(false);
    }
}


void ShareDlgImpl::buttonHelp_clicked()
{
    K3Process* p = new K3Process();
    *p << "konqueror";
    *p << "man:smb.conf";
    p->start();
}


#include "sharedlgimpl.moc"
