/***************************************************************************
                          kcmsambaconf.cpp  -  description
                             -------------------
    begin                : Mon Apr  8 13:35:56 CEST 2002
    copyright            : (C) 2002 by Christian Nitschkowski,
    email                : segfault_ii@web.de

    copyright            : (C) 2002 by Jan Schäfer
    email                : janschaefer@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <assert.h>

#include <qlayout.h>
#include <qgroupbox.h>
#include <qpainter.h>
#include <qcheckbox.h>
#include <qlistbox.h>


#include <klocale.h>
#include <kglobal.h>
#include <klineedit.h>
#include <kurlrequester.h>
#include <kcombobox.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <knuminput.h>


#include "sambashare.h"
#include "sambafile.h"
#include "sharedlgimpl.h"
#include "printerdlgimpl.h"
#include "socketoptionsdlg.h"

#include "kcmsambaconf.h"


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

	setText(0, _share->getName());
	setText(2, _share->getValue("comment"));

	if (_share->isPrinter()){
		if ( _share->getName() == "printers" ){
			setPixmap(0, SmallIcon("print_class"));
		}else{
			setPixmap(0, SmallIcon("print_printer"));
		}
		setText(1, _share->getValue("printer name"));
	}else{
		if ( _share->getName() == "homes" ){
			setPixmap(0, SmallIcon("folder_home"));
		}else{
			setPixmap(0, SmallIcon("folder"));
		}
		setText(1, _share->getValue("path"));
	}

	setPixmap(3, createPropertyPixmap());
}

QPixmap ShareListViewItem::createPropertyPixmap()
{
	// Create a big pixmap wich holds the
	// icons which are needed

	const int numberOfPix = 4; // the max number of pixmaps to join. Won't be changed

	int w = 22; // Standard size of one pixmap
	int margin = 4; // Margin between pixmaps
	int h = 22; // Standard height of the pixmaps

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

	if (!_share->getBoolValue("available")){
		p.drawPixmap(x,0,SmallIcon("no"));
	}

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
  
	if (!item) return;

	ShareDlgImpl* dlg = new ShareDlgImpl(_interface, item->getShare());
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

	if (dlg->result() == QDialog::Rejected ){
		removeShare();
	}else{
		item->updateShare();
		emit changed(true);
	}

	delete dlg;
}

void KcmSambaConf::removeShare()
{
	ShareListViewItem* item = static_cast<ShareListViewItem*>(_interface->shareListView->selectedItem());

	if (!item) return;

	SambaShare *share = item->getShare();
	delete item;
	_sambaFile->removeShare(share);

	emit changed(true);
}


void KcmSambaConf::editPrinter()
{
	ShareListViewItem* item = static_cast<ShareListViewItem*>(_interface->printerListView->selectedItem());

	if (!item) return;

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

	if (dlg->result() == QDialog::Rejected ){
		removePrinter();
	}else{
		item->updateShare();
		emit changed(true);
	}

	delete dlg;
}

void KcmSambaConf::removePrinter() 
{
	ShareListViewItem* item = static_cast<ShareListViewItem*>(_interface->printerListView->selectedItem());

	if (!item) return;

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
	QBoxLayout * l = new QHBoxLayout( this );
	l->setAutoAdd( TRUE );

	_interface = new KcmInterface(this);

	connect ( _interface->editShareBtn, SIGNAL(pressed()), this, SLOT(editShare()));
	connect ( _interface->addShareBtn, SIGNAL(pressed()), this, SLOT(addShare()));
	connect ( _interface->removeShareBtn, SIGNAL(pressed()), this, SLOT(removeShare()));

	connect ( _interface->editPrinterBtn, SIGNAL(pressed()), this, SLOT(editPrinter()));
	connect ( _interface->addPrinterBtn, SIGNAL(pressed()), this, SLOT(addPrinter()));
	connect ( _interface->removePrinterBtn, SIGNAL(pressed()), this, SLOT(removePrinter()));

	connect ( _interface->editDefaultPrinterBtn, SIGNAL(pressed()), this, SLOT(editPrinterDefaults()));
	connect ( _interface->editDefaultShareBtn, SIGNAL(pressed()), this, SLOT(editShareDefaults()));

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

	if ( !share){
		share = _sambaFile->newShare("global");
	}

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

	int i = _interface->securityLevelCombo->listBox()->index(_interface->securityLevelCombo->listBox()->findItem(share->getValue("security level"), Qt::ExactMatch));
	_interface->securityLevelCombo->setCurrentItem(i);

	i = _interface->mapToGuestCombo->listBox()->index(_interface->mapToGuestCombo->listBox()->findItem(share->getValue("map to guest"), Qt::ExactMatch));
	_interface->mapToGuestCombo->setCurrentItem(i);


	_interface->passwordServerEdit->setText( share->getValue("password server", false, true) );
	_interface->passwdChatEdit->setText( share->getValue("passwd chat", false, true) );
	_interface->passwordLevelSpin->setValue( share->getValue("password level", false, true).toInt());
	_interface->minPasswdLengthSpin->setValue( share->getValue("min passwd length", false, true).toInt());
	_interface->encryptPasswordChk->setChecked( share->getBoolValue("encrypt passwords", false, true));
	_interface->updateEncryptedChk->setChecked( share->getBoolValue("update encrypted", false, true));

	_interface->smbPasswdFileUrlRq->setURL( share->getValue("smb passwd file", false, true) );
	_interface->passwdProgramUrlRq->setURL( share->getValue("passwd program", false, true) );

	_interface->passwdChatDebugChk->setChecked( share->getBoolValue("passwd chat debug", false, true));
	_interface->unixPasswordSyncChk->setChecked( share->getBoolValue("unix password sync", false, true));

	_interface->usernameMapUrlRq->setURL( share->getValue("username map", false, true) );
	_interface->usernameLevelSpin->setValue( share->getValue("username level", false, true).toInt());

	_interface->useRhostsChk->setChecked( share->getBoolValue("use rhosts", false, true));
	_interface->lanmanAuthChk->setChecked( share->getBoolValue("lanman auth", false, true));
	_interface->allowTrustedDomainsChk->setChecked( share->getBoolValue("allow trusted domains", false, true));
	_interface->obeyPamRestrictionsChk->setChecked( share->getBoolValue("obey pam restrictions", false, true));
	_interface->pamPasswordChangeChk->setChecked( share->getBoolValue("pam password change", false, true));
	_interface->restrictAnonymousChk->setChecked( share->getBoolValue("restrict anonymous", false, true));
	_interface->alternatePermissionsChk->setChecked( share->getBoolValue("alternate permissions", false, true));

	_interface->rootDirectoryEdit->setText( share->getValue("root directory", false, true) );
	_interface->hostsEquivUrlRq->setURL( share->getValue("hosts equiv", false, true) );
}

void KcmSambaConf::defaults() {
	// insert your default settings code here...
	emit changed(true);
}

void KcmSambaConf::save() {
	// insert your saving code here...
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
