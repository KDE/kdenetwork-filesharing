/***************************************************************************
                          kcmsambaconf.cpp  -  description
                             -------------------
    begin                : Mon Apr  8 13:35:56 CEST 2002
    copyright            : (C) 2002 by Christian Nitschkowski,
    email                : segfault_ii@web.de

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
 *  along with KSambaPlugin; if not, write to the Free Software                     *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA  *
 *                                                                            *
 ******************************************************************************/

#include <assert.h>
#include <unistd.h>

#include <qlayout.h>
#include <qgroupbox.h>
#include <qpainter.h>
#include <qcheckbox.h>
#include <qlistbox.h>
#include <qradiobutton.h>


#include <klocale.h>
#include <kglobal.h>
#include <klineedit.h>
#include <kurlrequester.h>
#include <kcombobox.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <knuminput.h>
#include <krestrictedline.h>
#include <kmessagebox.h>

#include "sambashare.h"
#include "sambafile.h"
#include "sharedlgimpl.h"
#include "printerdlgimpl.h"
#include "socketoptionsdlg.h"

#include "kcmsambaconf.h"
#include "smbpasswdfile.h"
#include "passwd.h"



ShareListViewItem::ShareListViewItem(QListView * parent, SambaShare* share)
	: QListViewItem(parent)
{
	setShare(share);
}

SambaShare* ShareListViewItem::getShare() const
{
	return _share;
}

void ShareListViewItem::setShare(SambaShare* share)
{
	assert(share);
  _share = share;
	updateShare();
}

void ShareListViewItem::updateShare()
{
	assert(_share);

  setText(0,_share->getName());
  setText(2,_share->getValue("comment"));

  if (_share->isPrinter())
  {
  	if ( _share->getName() == "printers" )
    	setPixmap(0,SmallIcon("print_class"));
  	else
    	setPixmap(0,SmallIcon("print_printer"));
    setText(1,_share->getValue("printer name"));
  }
  else
  {
  	if ( _share->getName() == "homes" )
  		setPixmap(0,SmallIcon("folder_home"));
    else
  		setPixmap(0,SmallIcon("folder"));
    setText(1,_share->getValue("path"));
	}

  setPixmap(3,createPropertyPixmap());
}

QPixmap ShareListViewItem::createPropertyPixmap()
{
  // Create a big pixmap wich holds the
  // icons which are needed

  int numberOfPix = 4; // the max number of pixmaps to join

  int w = 22; // Standard size of one pixmap
  int margin = 4; // Margin between pixmaps
  int h = 22;

  int totalWidth = (w+margin)*numberOfPix;

  QPixmap pix(totalWidth,h);

  pix.fill();  // Fill with white

  QPainter p(&pix);

  int x = 0;

  if (_share->getBoolValue("public"))
  {
    p.drawPixmap(x,0,SmallIcon("network"));
    x = x+w+margin;
  }

  if (_share->getBoolValue("writable"))
  {
    p.drawPixmap(x,0,SmallIcon("edit"));
    x = x+w+margin;
  }

  if (_share->getBoolValue("printable"))
  {
    p.drawPixmap(x,0,SmallIcon("fileprint"));
    x = x+w+margin;
  }

  if (_share->getBoolValue("browseable"))
  {
    p.drawPixmap(x,0,SmallIcon("run"));
    x = x+w+margin;
  }

  if (!_share->getBoolValue("available"))
    p.drawPixmap(x,0,SmallIcon("no"));


  p.end();

  return QPixmap(pix);
}

KcmSambaConf::KcmSambaConf(QWidget *parent, const char *name)
	: KCModule(parent,name)
{
  load();
};


KcmSambaConf::~KcmSambaConf() {
}


void KcmSambaConf::editShare() 
{
	ShareListViewItem* item = static_cast<ShareListViewItem*>(_interface->shareListView->selectedItem());
  
  if (!item)
  	 return;

  ShareDlgImpl* dlg = new ShareDlgImpl(_interface,item->getShare());
  dlg->exec();
  
  item->updateShare();

	emit changed(true);
  delete dlg;
}

void KcmSambaConf::addShare()
{
	SambaShare* share = _sambaFile->newShare(_sambaFile->getUnusedName(),"");
  ShareListViewItem* item = new ShareListViewItem( _interface->shareListView, share );
  _interface->shareListView->setSelected(item,true);

  ShareDlgImpl* dlg = new ShareDlgImpl(_interface,share);
  dlg->exec();

  if (dlg->result() == QDialog::Rejected )
  	removeShare();
  else  {
    item->updateShare();
    emit changed(true);
  }

  delete dlg;
}

void KcmSambaConf::removeShare()
{
	ShareListViewItem* item = static_cast<ShareListViewItem*>(_interface->shareListView->selectedItem());

  if (!item)
  	 return;

	SambaShare *share = item->getShare();
  delete item;
  _sambaFile->removeShare(share);

  emit changed(true);
}


void KcmSambaConf::editPrinter()
{
	ShareListViewItem* item = static_cast<ShareListViewItem*>(_interface->printerListView->selectedItem());

  if (!item)
  	 return;

  PrinterDlgImpl* dlg = new PrinterDlgImpl(_interface,item->getShare());
  dlg->exec();
  
	item->updateShare();
  delete dlg;

  emit changed(true);
}

void KcmSambaConf::addPrinter()
{
	SambaShare* share = _sambaFile->newPrinter(_sambaFile->getUnusedName(),"");
  ShareListViewItem* item = new ShareListViewItem( _interface->shareListView, share );
  _interface->printerListView->setSelected(item,true);

  PrinterDlgImpl* dlg = new PrinterDlgImpl(_interface,share);
  dlg->exec();

  if (dlg->result() == QDialog::Rejected )
  	 removePrinter();
  else
  {
    item->updateShare();
		emit changed(true);
  }

  delete dlg;
}

void KcmSambaConf::removePrinter() 
{
	ShareListViewItem* item = static_cast<ShareListViewItem*>(_interface->printerListView->selectedItem());

  if (!item)
  	 return;

	SambaShare *share = item->getShare();
  delete item;
  _sambaFile->removeShare(share);
  
	emit changed(true);
}

void KcmSambaConf::editShareDefaults()
{
	SambaShare* share = _sambaFile->getShare("global");

  ShareDlgImpl* dlg = new ShareDlgImpl(_interface,share);
  dlg->directoryGrp->setEnabled(false);
  dlg->identifierGrp->setEnabled(false);
  dlg->exec();
  delete dlg;
	
  emit changed(true);
}

void KcmSambaConf::editPrinterDefaults()
{
	SambaShare* share = _sambaFile->getShare("global");

  PrinterDlgImpl* dlg = new PrinterDlgImpl(_interface,share);
  dlg->printerGrp->setEnabled(false);
  dlg->identifierGrp->setEnabled(false);
  dlg->exec();
  delete dlg;
	
  emit changed(true);
}

void KcmSambaConf::socketOptionsBtnClicked()
{
		SambaShare* share = _sambaFile->getShare("global");

    SocketOptionsDlg *dlg = new SocketOptionsDlg(_interface);
    dlg->setShare(share);
    dlg->exec();
    
    delete dlg;
}

void KcmSambaConf::load() 
{
  kdDebug() << "loading" << endl;

	QBoxLayout * l = new QHBoxLayout( this );
	l->setAutoAdd( TRUE );

	_interface = new KcmInterface(this);


	connect ( _interface->sambaUserPasswordBtn, SIGNAL(clicked()), this, SLOT(sambaUserPasswordBtnClicked()));

	connect ( _interface->editShareBtn, SIGNAL(clicked()), this, SLOT(editShare()));
	connect ( _interface->addShareBtn, SIGNAL(clicked()), this, SLOT(addShare()));
	connect ( _interface->removeShareBtn, SIGNAL(clicked()), this, SLOT(removeShare()));

	connect ( _interface->editPrinterBtn, SIGNAL(clicked()), this, SLOT(editPrinter()));
	connect ( _interface->addPrinterBtn, SIGNAL(clicked()), this, SLOT(addPrinter()));
	connect ( _interface->removePrinterBtn, SIGNAL(clicked()), this, SLOT(removePrinter()));

	connect ( _interface->editDefaultPrinterBtn, SIGNAL(clicked()), this, SLOT(editPrinterDefaults()));
	connect ( _interface->editDefaultShareBtn, SIGNAL(clicked()), this, SLOT(editShareDefaults()));

	connect ( _interface->socketOptionsBtn, SIGNAL(clicked()), this, SLOT(socketOptionsBtnClicked()));
  
  _smbconf = SambaFile::findSambaConf();
	_sambaFile = new SambaFile(_smbconf,false);


  // Fill the ListViews

  SambaShareList* list = _sambaFile->getSharedDirs();

  SambaShare *share = 0L;
  for ( share = list->first(); share; share = list->next() )
  {
  	new ShareListViewItem(_interface->shareListView, share);
  }

  share = 0L;
  list = _sambaFile->getSharedPrinters();
  for ( share = list->first(); share; share = list->next() )
  {
  	new ShareListViewItem(_interface->printerListView, share);
  }

  share = _sambaFile->getShare("global");

  if ( !share)
     share = _sambaFile->newShare("global");

	assert( share);

  // Base settings

  _interface->configUrlRq->setURL( _smbconf );
  _interface->workgroupEdit->setText( share->getValue("workgroup",false,true) );
  _interface->serverStringEdit->setText( share->getValue("server string",false,true) );
  _interface->netbiosNameEdit->setText( share->getValue("netbios name",false,true) );
  _interface->netbiosAliasesEdit->setText( share->getValue("netbios aliases",false,true) );
  _interface->netbiosScopeEdit->setText( share->getValue("netbios scope",false,true) );

  _interface->codingSystemEdit->setText( share->getValue("coding system",false,true) );
  _interface->clientCodePageEdit->setText( share->getValue("client code page",false,true) );
  _interface->codePageDirUrlRq->setURL( share->getValue("code page directory",false,true) );

  _interface->interfacesEdit->setText( share->getValue("interfaces",false,true) );
  _interface->bindInterfacesOnlyChk->setChecked( share->getBoolValue("bind interfaces only",false,true));

  // Security

  int i = _interface->securityLevelCombo->listBox()->index(_interface->securityLevelCombo->listBox()->findItem(share->getValue("security",false,true),Qt::ExactMatch));
  _interface->securityLevelCombo->setCurrentItem(i);

  i = _interface->mapToGuestCombo->listBox()->index(_interface->mapToGuestCombo->listBox()->findItem(share->getValue("map to guest",false,true),Qt::ExactMatch));
  _interface->mapToGuestCombo->setCurrentItem(i);

  _interface->passwordServerEdit->setText( share->getValue("password server",false,true) );
  _interface->passwdChatEdit->setText( share->getValue("passwd chat",false,true) );
  _interface->passwordLevelSpin->setValue( share->getValue("password level", false, true).toInt());
  _interface->minPasswdLengthSpin->setValue( share->getValue("min passwd length", false, true).toInt());
  _interface->encryptPasswordChk->setChecked( share->getBoolValue("encrypt passwords",false,true));
  _interface->updateEncryptedChk->setChecked( share->getBoolValue("update encrypted",false,true));

  _interface->smbPasswdFileUrlRq->setURL( share->getValue("smb passwd file",false,true) );
  _interface->passwdProgramUrlRq->setURL( share->getValue("passwd program",false,true) );

  _interface->passwdChatDebugChk->setChecked( share->getBoolValue("passwd chat debug",false,true));
  _interface->unixPasswordSyncChk->setChecked( share->getBoolValue("unix password sync",false,true));

  _interface->usernameMapUrlRq->setURL( share->getValue("username map",false,true) );
  _interface->usernameLevelSpin->setValue( share->getValue("username level", false, true).toInt());

  _interface->useRhostsChk->setChecked( share->getBoolValue("use rhosts",false,true));
  _interface->lanmanAuthChk->setChecked( share->getBoolValue("lanman auth",false,true));
  _interface->allowTrustedDomainsChk->setChecked( share->getBoolValue("allow trusted domains",false,true));
  _interface->obeyPamRestrictionsChk->setChecked( share->getBoolValue("obey pam restrictions",false,true));
  _interface->pamPasswordChangeChk->setChecked( share->getBoolValue("pam password change",false,true));
  _interface->restrictAnonymousChk->setChecked( share->getBoolValue("restrict anonymous",false,true));
  _interface->alternatePermissionsChk->setChecked( share->getBoolValue("alternate permissions",false,true));

  _interface->rootDirectoryEdit->setText( share->getValue("root directory",false,true) );
  _interface->hostsEquivUrlRq->setURL( share->getValue("hosts equiv",false,true) );

  // Advanced
  _interface->changeNotifyTimeoutSpin->setValue( share->getValue("change notify timeout", false, true).toInt());
  _interface->keepaliveSpin->setValue( share->getValue("keepalive", false, true).toInt());
  _interface->lpqCacheTimeSpin->setValue( share->getValue("lpq cache time", false, true).toInt());
  _interface->maxOpenFilesSpin->setValue( share->getValue("max open files", false, true).toInt());
  _interface->readSizeSpin->setValue( share->getValue("read size", false, true).toInt());
  _interface->maxDiskSizeSpin->setValue( share->getValue("max disk size", false, true).toInt());
  _interface->statCacheSizeSpin->setValue( share->getValue("stat cache size", false, true).toInt());
  _interface->getwdCacheChk->setChecked( share->getBoolValue("getwd cache",false,true));

  _interface->maxLogSizeInput->setValue( share->getValue("max log size", false, true).toInt());
  _interface->logFileUrlRq->setURL( share->getValue("log file",false,true) );

  _interface->syslogSpin->setValue( share->getValue("syslog", false, true).toInt());
  _interface->logLevelSpin->setValue( share->getValue("log level", false, true).toInt());

  _interface->statusChk->setChecked( share->getBoolValue("status",false,true));

  _interface->debugUidChk->setChecked( share->getBoolValue("debug uid",false,true));
  _interface->debugPidChk->setChecked( share->getBoolValue("debug pid",false,true));
  _interface->microsecondsChk->setChecked( share->getBoolValue("debug hires timestamp",false,true));
  _interface->syslogOnlyChk->setChecked( share->getBoolValue("syslog only",false,true));
  _interface->timestampChk->setChecked( share->getBoolValue("debug timestamp",false,true));

  // WINS

  _interface->winsSupportRadio->setChecked( share->getBoolValue("wins support",false,true));
  _interface->winsProxyChk->setChecked( share->getBoolValue("wins proxy",false,true));
  _interface->dnsProxyChk->setChecked( share->getBoolValue("dns proxy",false,true));

  _interface->winsServerEdit->setText( share->getValue("wins server",false,true) );

  _interface->otherWinsRadio->setChecked( share->getValue("wins server",false,true) != "" );
  _interface->winsHookEdit->setText( share->getValue("wins hook",false,true) );

  _interface->preferredMasterChk->setChecked( share->getBoolValue("preferred master",false,true));
  _interface->localMasterChk->setChecked( share->getBoolValue("local master",false,true));
  _interface->domainMasterChk->setChecked( share->getBoolValue("domain master",false,true));
  _interface->domainLogonsChk->setChecked( share->getBoolValue("domain logons",false,true));

  _interface->osLevelSpin->setValue( share->getValue("os level", false, true).toInt());



  // Protocol

  _interface->writeRawChk->setChecked( share->getBoolValue("write raw",false,true));
  _interface->readRawChk->setChecked( share->getBoolValue("read raw",false,true));
  _interface->readBmpxChk->setChecked( share->getBoolValue("read bmpx",false,true));
  _interface->largeReadWriteChk->setChecked( share->getBoolValue("large readwrite",false,true));
  _interface->ntAclSupportChk->setChecked( share->getBoolValue("nt acl support",false,true));
  _interface->ntSmbSupportChk->setChecked( share->getBoolValue("nt smb support",false,true));
  _interface->ntPipeSupportChk->setChecked( share->getBoolValue("nt pipe support",false,true));
  _interface->timeServerChk->setChecked( share->getBoolValue("time server",false,true));

  _interface->maxMuxInput->setValue( share->getValue("max mux", false, true).toInt());
  _interface->maxXmitInput->setValue( share->getValue("max xmit", false, true).toInt());
  _interface->maxPacketInput->setValue( share->getValue("max packet", false, true).toInt());
  _interface->maxTtlInput->setValue( share->getValue("max ttl", false, true).toInt());
  _interface->maxWinsTtlInput->setValue( share->getValue("max wins ttl", false, true).toInt());
  _interface->minWinsTtlInput->setValue( share->getValue("min wins ttl", false, true).toInt());

  i = _interface->announceAsCombo->listBox()->index(_interface->announceAsCombo->listBox()->findItem(share->getValue("announce as",false,true),Qt::ExactMatch));
  _interface->announceAsCombo->setCurrentItem(i);

  i = _interface->protocolCombo->listBox()->index(_interface->protocolCombo->listBox()->findItem(share->getValue("protocol",false,true),Qt::ExactMatch));
  _interface->protocolCombo->setCurrentItem(i);

  i = _interface->maxProtocolCombo->listBox()->index(_interface->maxProtocolCombo->listBox()->findItem(share->getValue("max protocol",false,true),Qt::ExactMatch));
  _interface->maxProtocolCombo->setCurrentItem(i);

  i = _interface->minProtocolCombo->listBox()->index(_interface->minProtocolCombo->listBox()->findItem(share->getValue("min protocol",false,true),Qt::ExactMatch));
  _interface->minProtocolCombo->setCurrentItem(i);

  _interface->announceVersionEdit->setText( share->getValue("announce version",false,true) );
  _interface->nameResolveOrderEdit->setText( share->getValue("name resolve order",false,true) );

  loadUserTab();

  connect( _interface, SIGNAL(changed()), this, SLOT(configChanged()));
}

void KcmSambaConf::loadUserTab()
{
  SambaShare* share = _sambaFile->getShare("global");

  QStringList added;

  SmbPasswdFile passwd( KURL(share->getValue("smb passwd file",true,true)) );
  SambaUserList sambaList = passwd.getSambaUserList();

  SambaUser *user;
  for ( user = sambaList.first(); user; user = sambaList.next() )
  {
    new KListViewItem(_interface->sambaUsersListView, user->name, QString::number(user->uid));
    added.append(user->name);
  }

  UnixUserList unixList = getUnixUserList();

  UnixUser *unixUser;
  for ( unixUser = unixList.first(); unixUser; unixUser = unixList.next() )
  {
    QStringList::Iterator it;

    it=added.find(unixUser->name);
    if (it == added.end())
        new KListViewItem(_interface->unixUsersListView, unixUser->name, QString::number(unixUser->uid));
  }

  _interface->unixUsersListView->setSelectionMode(QListView::Extended);
  _interface->sambaUsersListView->setSelectionMode(QListView::Extended);

  connect( _interface->addSambaUserBtn, SIGNAL(clicked()),
           this, SLOT( addSambaUserBtnClicked() ));

  connect( _interface->removeSambaUserBtn, SIGNAL(clicked()),
           this, SLOT( removeSambaUserBtnClicked() ));



}

void KcmSambaConf::saveUserTab()
{
}

void KcmSambaConf::addSambaUserBtnClicked()
{
  QPtrList<QListViewItem> list = _interface->unixUsersListView->selectedItems();

  SambaShare* share = _sambaFile->getShare("global");
  SmbPasswdFile passwd( KURL(share->getValue("smb passwd file",true,true)) );

  QListViewItem* item;
  for ( item = list.first(); item; item = list.first() )
  {
    SambaUser user( item->text(0), item->text(1).toInt() );
    if (!passwd.addUser(user))
    {
      KMessageBox::sorry(0,i18n("Adding the user %1 to the Samba user database failed.").arg(user.name));
      break;
    }

    new KListViewItem(_interface->sambaUsersListView, item->text(0), item->text(1));

    list.remove(item);
    delete item;
  }
}

void KcmSambaConf::removeSambaUserBtnClicked()
{
  QPtrList<QListViewItem> list = _interface->sambaUsersListView->selectedItems();

  SambaShare* share = _sambaFile->getShare("global");
  SmbPasswdFile passwd( KURL(share->getValue("smb passwd file",true,true)) );

  QListViewItem* item;
  for ( item = list.first(); item; item = list.first() )
  {
    SambaUser user( item->text(0), item->text(1).toInt() );
    if (!passwd.removeUser(user))
    {
      KMessageBox::sorry(0,i18n("Removing the user %1 from the Samba user database failed.").arg(user.name));
      continue;
    }

    new KListViewItem(_interface->unixUsersListView, item->text(0), item->text(1));
    list.remove(item);
    delete item;
  }
}

void KcmSambaConf::sambaUserPasswordBtnClicked()
{
  QPtrList<QListViewItem> list = _interface->sambaUsersListView->selectedItems();

  SambaShare* share = _sambaFile->getShare("global");
  SmbPasswdFile passwd( KURL(share->getValue("smb passwd file",true,true)) );

  QListViewItem* item;
  for ( item = list.first(); item; item = list.next() )
  {
    SambaUser user( item->text(0), item->text(1).toInt() );
    if (!passwd.changePassword(user))
    {
      KMessageBox::sorry(0,i18n("Changing the password of the user %1 failed.").arg(user.name));
    }

  }

}


void KcmSambaConf::defaults() {
	// insert your default settings code here...
	emit changed(true);
}

void KcmSambaConf::save() {
	// insert your saving code here...

  SambaShare *share = _sambaFile->getShare("global");
  assert(share);

  kdDebug() << "saving ... " << endl;
  
  // Base settings

  _smbconf = _interface->configUrlRq->url();
  share->setValue("workgroup", _interface->workgroupEdit->text(), false, true );
  share->setValue("server string", _interface->serverStringEdit->text(), false, true );
  share->setValue("netbios name",_interface->netbiosNameEdit->text(), false, true );
  share->setValue("netbios aliases",_interface->netbiosAliasesEdit->text(), false, true );
  share->setValue("netbios scope",_interface->netbiosScopeEdit->text(), false, true );

  share->setValue("coding system",_interface->codingSystemEdit->text(), false, true );
  share->setValue("client code page",_interface->clientCodePageEdit->text(), false, true );
  share->setValue("code page directory",_interface->codePageDirUrlRq->url(), false, true);

  share->setValue("interfaces",_interface->interfacesEdit->text(), false, true );
  share->setValue("bind interfaces only",_interface->bindInterfacesOnlyChk->isChecked(), false, true );

  // Security

  share->setValue("security",_interface->securityLevelCombo->currentText());
  share->setValue("map to guest",_interface->mapToGuestCombo->currentText());


  share->setValue("password server",_interface->passwordServerEdit->text(), false, true );
  share->setValue("passwd chat",_interface->passwdChatEdit->text(), false, true );
  share->setValue("password level",_interface->passwordLevelSpin->value(), false, true);
  share->setValue("min passwd length",_interface->minPasswdLengthSpin->value(), false, true);
  share->setValue("encrypt passwords",_interface->encryptPasswordChk->isChecked(), false, true );
  share->setValue("update encrypted",_interface->updateEncryptedChk->isChecked(), false, true );

  share->setValue("smb passwd file",_interface->smbPasswdFileUrlRq->url(), false, true);
  share->setValue("passwd program",_interface->passwdProgramUrlRq->url(), false, true);

  share->setValue("passwd chat debug",_interface->passwdChatDebugChk->isChecked(), false, true );
  share->setValue("unix password sync",_interface->unixPasswordSyncChk->isChecked(), false, true );

  share->setValue("username map",_interface->usernameMapUrlRq->url(), false, true );
  share->setValue("username level",_interface->usernameLevelSpin->value(), false, true );

  share->setValue("use rhosts",_interface->useRhostsChk->isChecked(), false, true );
  share->setValue("lanman auth",_interface->lanmanAuthChk->isChecked(), false, true );
  share->setValue("allow trusted domains",_interface->allowTrustedDomainsChk->isChecked(), false, true );
  share->setValue("obey pam restrictions",_interface->obeyPamRestrictionsChk->isChecked(), false, true );
  share->setValue("pam password change",_interface->pamPasswordChangeChk->isChecked(), false, true );
  share->setValue("restrict anonymous",_interface->restrictAnonymousChk->isChecked(), false, true );
  share->setValue("alternate permissions",_interface->alternatePermissionsChk->isChecked(), false, true );

  share->setValue("root directory",_interface->rootDirectoryEdit->text(), false, true );
  share->setValue("hosts equiv",_interface->hostsEquivUrlRq->url(), false, true );

  // Advanced
  share->setValue("change notify timeout",_interface->changeNotifyTimeoutSpin->value(), false, true);
  share->setValue("keepalive",_interface->keepaliveSpin->value(), false, true);
  share->setValue("lpq cache time",_interface->lpqCacheTimeSpin->value(), false, true);
  share->setValue("max open files",_interface->maxOpenFilesSpin->value(), false, true);
  share->setValue("read size",_interface->readSizeSpin->value(), false, true);
  share->setValue("max disk size",_interface->maxDiskSizeSpin->value(), false, true);
  share->setValue("stat cache size",_interface->statCacheSizeSpin->value(), false, true);
  share->setValue("getwd cache",_interface->getwdCacheChk->isChecked(), false, true );

  share->setValue("max log size",_interface->maxLogSizeInput->value(), false, true);
  share->setValue("log file",_interface->logFileUrlRq->url(), false, true);

  share->setValue("syslog",_interface->syslogSpin->value(), false, true);
  share->setValue("log level",_interface->logLevelSpin->value(), false, true);

  share->setValue("status",_interface->statusChk->isChecked(), false, true );

  share->setValue("debug uid",_interface->debugUidChk->isChecked(), false, true );
  share->setValue("debug pid",_interface->debugPidChk->isChecked(), false, true );
  share->setValue("debug hires timestamp",_interface->microsecondsChk->isChecked(), false, true );
  share->setValue("syslog only",_interface->syslogOnlyChk->isChecked(), false, true );
  share->setValue("debug timestamp",_interface->timestampChk->isChecked(), false, true );


  // WINS

  share->setValue("wins support",_interface->winsSupportRadio->isChecked(), false,true);
  share->setValue("wins proxy",_interface->winsProxyChk->isChecked(), false,true);
  share->setValue("dns proxy",_interface->dnsProxyChk->isChecked(), false,true);

  if (_interface->otherWinsRadio->isChecked())
     share->setValue("wins server",_interface->winsServerEdit->text(), false,true);
  else
     share->setValue("wins server",QString(""), false,true);  
  
  share->setValue("wins hook",_interface->winsHookEdit->text(), false,true);

  share->setValue("preferred master",_interface->preferredMasterChk->isChecked(), false,true);
  share->setValue("local master",_interface->localMasterChk->isChecked(), false,true);
  share->setValue("domain master",_interface->domainMasterChk->isChecked(), false,true);
  share->setValue("domain logons",_interface->domainLogonsChk->isChecked(), false,true);

  share->setValue("os level",QString::number(_interface->osLevelSpin->value()),  false, true);

  // Protocol

  share->setValue("write raw",_interface->writeRawChk->isChecked(), false,true);
  share->setValue("read raw",_interface->readRawChk->isChecked(), false,true);
  share->setValue("read bmpx",_interface->readBmpxChk->isChecked(), false,true);
  share->setValue("large readwrite",_interface->largeReadWriteChk->isChecked(), false,true);
  share->setValue("nt acl support",_interface->ntAclSupportChk->isChecked(), false,true);
  share->setValue("nt smb support",_interface->ntSmbSupportChk->isChecked(), false,true);
  share->setValue("nt pipe support",_interface->ntPipeSupportChk->isChecked(), false,true);
  share->setValue("time server",_interface->timeServerChk->isChecked(), false,true);

  share->setValue("max mux",QString::number(_interface->maxMuxInput->value()), false, true);
  share->setValue("max xmit",QString::number(_interface->maxXmitInput->value()), false, true);
  share->setValue("max packet",QString::number(_interface->maxPacketInput->value()), false, true);
  share->setValue("max ttl",QString::number(_interface->maxTtlInput->value()), false, true);
  share->setValue("max wins ttl",QString::number(_interface->maxWinsTtlInput->value()), false, true);
  share->setValue("min wins ttl",QString::number(_interface->minWinsTtlInput->value()), false, true);

  share->setValue("announce as",_interface->announceAsCombo->currentText(),false,true);
  share->setValue("protocol",_interface->protocolCombo->currentText(),false,true);
  share->setValue("max protocol",_interface->maxProtocolCombo->currentText(),false,true);
  share->setValue("min protocol",_interface->minProtocolCombo->currentText(),false,true);

  share->setValue("announce version",_interface->announceVersionEdit->text(), false,true);
  share->setValue("name resolve order",_interface->nameResolveOrderEdit->text(), false,true);


  _sambaFile->slotApply();

}

int KcmSambaConf::buttons () {
	return KCModule::Default|KCModule::Apply|KCModule::Help;
}

void KcmSambaConf::configChanged() {
	// insert your saving code here...
	emit changed(true);
}

QString KcmSambaConf::quickHelp() const
{
	return i18n("Helpful information about the kcmsambaconf module.");
}

// ------------------------------------------------------------------------

extern "C"
{
	KCModule *create_KcmSambaConf(QWidget *parent, const char *name)
	{
		KGlobal::locale()->insertCatalogue("KcmSambaConf");
		return new KcmSambaConf(parent, name);
	}
}

#include "kcmsambaconf.moc"
