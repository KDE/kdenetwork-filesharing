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
#include <qbuttongroup.h>
#include <qtabwidget.h>
#include <qtabbar.h>
#include <qvbox.h>
#include <qlayout.h>


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
#include <kjanuswidget.h>

#include "sambashare.h"
#include "sambafile.h"
#include "sharedlgimpl.h"
#include "printerdlgimpl.h"

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
	: KCModule(parent,name),
	lineEditDict(40,false),
	checkBoxDict(40,false),
	urlRequesterDict(40,false),
	spinBoxDict(40,false)
	
{
  load();
	
	QVBoxLayout *l = new QVBoxLayout(_interface->advancedFrame);
	l->setAutoAdd(true);
	l->setMargin(0);
	_janus = new KJanusWidget(_interface->advancedFrame,0,KJanusWidget::TreeList);
	_janus->setRootIsDecorated(false);
	_janus->setShowIconsInTreeList(true);
	
	QWidget *w;
	QFrame *f;
	QString label;
 	QPixmap icon;
	
	for (int i=0;i<_interface->advancedTab->count();)
	{
		w = _interface->advancedTab->page(i);
		label = _interface->advancedTab->label(i);

		if (label.lower() == "security")
			 icon = SmallIcon("password");
		else
		if (label.lower() == "logging")
			 icon = SmallIcon("history");
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
//			 icon = SmallIcon("connect_creating");
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
		
		_interface->advancedTab->removePage(w);
		
		w->reparent(f,QPoint(1,1),TRUE);
		
	}
	
	w = _interface->mainTab->page(6);
	_interface->mainTab->removePage(w);
	delete w;
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

  _interface->configUrlRq->setURL( _smbconf );
	
	loadBaseSettings( share );	
	loadSecurity( share );
	loadTuning( share );
	loadLogging( share );
	loadPrinting( share );
	loadFilenames( share );
	loadDomain( share );
	loadProtocol( share );
	loadSocket( share );
	loadSSL( share );
	loadLogon( share );
	loadCoding( share );
	loadWinbind( share );
	loadNetbios( share );
	loadVFS( share );
	loadBrowsing( share );
	loadMisc( share );
	
	loadDicts( share );
	
  loadUserTab();

  connect( _interface, SIGNAL(changed()), this, SLOT(configChanged()));
}

void KcmSambaConf::loadDicts(SambaShare* share) 
{
	QDictIterator<QCheckBox> checkBoxIt( checkBoxDict ); 
	 
	for( ; checkBoxIt.current(); ++checkBoxIt )	{
		checkBoxIt.current()->setChecked(share->getBoolValue(checkBoxIt.currentKey(),false,true));
		
	}
		
	QDictIterator<QLineEdit> lineEditIt( lineEditDict ); 
	 
	for( ; lineEditIt.current(); ++lineEditIt )	{
		lineEditIt.current()->setText(share->getValue(lineEditIt.currentKey(),false,true));
	}

	QDictIterator<KURLRequester> urlRequesterIt( urlRequesterDict ); 
 
	for( ; urlRequesterIt.current(); ++urlRequesterIt )	{
		urlRequesterIt.current()->setURL(share->getValue(urlRequesterIt.currentKey(),false,true));
	}

	QDictIterator<QSpinBox> spinBoxIt( spinBoxDict ); 
	 
	for( ; spinBoxIt.current(); ++spinBoxIt )	{
		spinBoxIt.current()->setValue(share->getValue(spinBoxIt.currentKey(),false,true).toInt());
	}
		
}



void KcmSambaConf::loadBaseSettings(SambaShare* share) 
{

	lineEditDict.insert("workgroup", _interface->workgroupEdit);
	lineEditDict.insert("server string", _interface->serverStringEdit);
	lineEditDict.insert("netbios name", _interface->netbiosNameEdit);
	lineEditDict.insert("netbios aliases", _interface->netbiosAliasesEdit);
	lineEditDict.insert("netbios scope", _interface->netbiosScopeEdit);
	lineEditDict.insert("interfaces", _interface->interfacesEdit);

  _interface->guestAccountCombo->insertStringList( getUnixUsers() );
	setComboIndexToValue(_interface->guestAccountCombo,"guest account",share);
	
	QString value = share->getValue("map to guest",false,true);
	
	_interface->allowGuestLoginsChk->setChecked( value.lower()!="never" );
	
	checkBoxDict.insert("guest ok",_interface->allowGuestLoginsChk);

	checkBoxDict.insert("bind interfaces only",_interface->bindInterfacesOnlyChk);

	QString s = share->getValue("security",false,true).lower();
	int i = 0;
	
	if ( s == "share" ) i = 0; else
	if ( s == "user" ) i = 1; else
	if ( s == "server" ) i = 2; else
	if ( s == "domain" ) i = 3;
		 
	_interface->securityLevelBtnGrp->setButton(i);  

}

void KcmSambaConf::loadDomain(SambaShare* share) 
{
	checkBoxDict.insert("wins proxy",_interface->winsProxyChk);
	checkBoxDict.insert("dns proxy",_interface->dnsProxyChk);
	checkBoxDict.insert("preferred master",_interface->preferredMasterChk);
	checkBoxDict.insert("local master",_interface->localMasterChk);
	checkBoxDict.insert("domain master",_interface->domainMasterChk);
	checkBoxDict.insert("domain logons",_interface->domainLogonsChk);
	
	spinBoxDict.insert("machine password timeout", _interface->machinePasswordTimeoutSpin);

	
	lineEditDict.insert("wins server", _interface->winsServerEdit);
	lineEditDict.insert("wins hook", _interface->winsHookEdit);
	
	_interface->winsSupportRadio->setChecked( share->getBoolValue("wins support",false,true));
  _interface->otherWinsRadio->setChecked( share->getValue("wins server",false,true) != "" );

	spinBoxDict.insert("os level", _interface->osLevelSpin);

}

void KcmSambaConf::loadSecurity(SambaShare* share) 
{
  int i = _interface->mapToGuestCombo->listBox()->index(_interface->mapToGuestCombo->listBox()->findItem(share->getValue("map to guest",false,true),Qt::ExactMatch));
  _interface->mapToGuestCombo->setCurrentItem(i);

	lineEditDict.insert("password server", _interface->passwordServerEdit);
	lineEditDict.insert("passwd chat", _interface->passwdChatEdit);
	lineEditDict.insert("root directory", _interface->rootDirectoryEdit);

	spinBoxDict.insert("password level", _interface->passwordLevelSpin);
	spinBoxDict.insert("min passwd length", _interface->minPasswdLengthSpin);
	spinBoxDict.insert("username level", _interface->usernameLevelSpin);
	
	checkBoxDict.insert("encrypt passwords",_interface->encryptPasswordsChk);
	checkBoxDict.insert("update encrypted",_interface->updateEncryptedChk);
	checkBoxDict.insert("passwd chat debug",_interface->passwdChatDebugChk);
	checkBoxDict.insert("unix password sync",_interface->unixPasswordSyncChk);
	checkBoxDict.insert("use rhosts",_interface->useRhostsChk);
	checkBoxDict.insert("lanman auth",_interface->lanmanAuthChk);
	checkBoxDict.insert("allow trusted domains",_interface->allowTrustedDomainsChk);
	checkBoxDict.insert("obey pam restrictions",_interface->obeyPamRestrictionsChk);
	checkBoxDict.insert("pam password change",_interface->pamPasswordChangeChk);
	checkBoxDict.insert("restrict anonymous",_interface->restrictAnonymousChk);
	checkBoxDict.insert("alternate permissions",_interface->alternatePermissionsChk);
	checkBoxDict.insert("null passwords",_interface->nullPasswordsChk);
		
	urlRequesterDict.insert("smb passwd file",_interface->smbPasswdFileUrlRq);
	urlRequesterDict.insert("passwd program",_interface->passwdProgramUrlRq);
	urlRequesterDict.insert("username map",_interface->usernameMapUrlRq);
	urlRequesterDict.insert("hosts equiv",_interface->hostsEquivUrlRq);
	
}

void KcmSambaConf::loadLogging(SambaShare* share) 
{
	urlRequesterDict.insert("log file",_interface->logFileUrlRq);

	spinBoxDict.insert("max log size", _interface->maxLogSizeSpin);
	spinBoxDict.insert("syslog", _interface->syslogSpin);
	spinBoxDict.insert("log level", _interface->logLevelSpin);

	checkBoxDict.insert("status",_interface->statusChk);
	checkBoxDict.insert("debug uid",_interface->debugUidChk);
	checkBoxDict.insert("debug pid",_interface->debugPidChk);
	checkBoxDict.insert("debug hires timestamp",_interface->microsecondsChk);
	checkBoxDict.insert("syslog only",_interface->syslogOnlyChk);
	checkBoxDict.insert("debug timestamp",_interface->timestampChk);
	checkBoxDict.insert("use mmap",_interface->useMmapChk);
  

}

void KcmSambaConf::loadTuning(SambaShare* share) 
{
	spinBoxDict.insert("change notify timeout", _interface->changeNotifyTimeoutSpin);
	spinBoxDict.insert("deadtime", _interface->deadtimeSpin);
	spinBoxDict.insert("keepalive", _interface->keepaliveSpin);
	spinBoxDict.insert("lpq cache time", _interface->lpqCacheTimeSpin);
	spinBoxDict.insert("max open files", _interface->maxOpenFilesSpin);
	spinBoxDict.insert("read size", _interface->readSizeSpin);
	spinBoxDict.insert("max disk size", _interface->maxDiskSizeSpin);
	spinBoxDict.insert("stat cache size", _interface->statCacheSizeSpin);
  
	checkBoxDict.insert("getwd cache",_interface->getwdCacheChk);
	checkBoxDict.insert("use mmap",_interface->useMmapChk);
  
}

void KcmSambaConf::loadPrinting(SambaShare* share) 
{
	checkBoxDict.insert("load printers",_interface->loadPrintersChk);
	checkBoxDict.insert("disable spoolss",_interface->disableSpoolssChk);
	checkBoxDict.insert("show add printer wizard",_interface->showAddPrinterWizardChk);
	
	lineEditDict.insert("addprinter command", _interface->addprinterCommandEdit);
	lineEditDict.insert("deleteprinter command", _interface->deleteprinterCommandEdit);
	lineEditDict.insert("enumports command", _interface->enumportsCommandEdit);
	
	urlRequesterDict.insert("printcap name",_interface->printcapNameUrlRq);
	urlRequesterDict.insert("os2 driver map",_interface->os2DriverMapUrlRq);
	
	spinBoxDict.insert("total print jobs", _interface->totalPrintJobsSpin);
}

void KcmSambaConf::loadFilenames(SambaShare* share) 
{
	checkBoxDict.insert("strip dot",_interface->stripDotChk);
	checkBoxDict.insert("stat cache",_interface->statCacheChk);
  
	lineEditDict.insert("character set", _interface->characterSetEdit);
	
	spinBoxDict.insert("mangled stack", _interface->mangledStackSpin);

}

void KcmSambaConf::loadProtocol(SambaShare* share) 
{
  // Protocol

	checkBoxDict.insert("write raw",_interface->writeRawChk);
	checkBoxDict.insert("read raw",_interface->readRawChk);
	checkBoxDict.insert("read bmpx",_interface->readBmpxChk);
	checkBoxDict.insert("large readwrite",_interface->largeReadWriteChk);
	checkBoxDict.insert("nt acl support",_interface->ntAclSupportChk);
	checkBoxDict.insert("nt smb support",_interface->ntSmbSupportChk);
	checkBoxDict.insert("nt pipe support",_interface->ntPipeSupportChk);
	checkBoxDict.insert("time server",_interface->timeServerChk);
  
	spinBoxDict.insert("max mux", _interface->maxMuxSpin);
	spinBoxDict.insert("max xmit", _interface->maxXmitSpin);
	spinBoxDict.insert("max packet", _interface->maxPacketSpin);
	spinBoxDict.insert("max ttl", _interface->maxTtlSpin);
	spinBoxDict.insert("max wins ttl", _interface->maxWinsTtlSpin);
	spinBoxDict.insert("min wins ttl", _interface->minWinsTtlSpin);

	lineEditDict.insert("announce version", _interface->announceVersionEdit);
	lineEditDict.insert("name resolve order", _interface->nameResolveOrderEdit);
		  
	setComboIndexToValue(_interface->announceAsCombo,"announce as",share);
	setComboIndexToValue(_interface->protocolCombo,"protocol",share);
	setComboIndexToValue(_interface->maxProtocolCombo,"max protocol",share);
	setComboIndexToValue(_interface->minProtocolCombo,"min protocol",share);
  
}

void KcmSambaConf::loadSocket(SambaShare* share) 
{
	// SOCKET options
	
	lineEditDict.insert("socket address", _interface->socketAddressEdit);
  
	QString s = share->getValue("socket options");
  s = s.simplifyWhiteSpace();
    
	// The string s has now the form :
	// "OPTION1=1 OPTION2=0 OPTION3=2234 OPTION4"
    
	_interface->SO_KEEPALIVEChk->setChecked(getSocketBoolValue( s, "SO_KEEPALIVE") );
	_interface->SO_REUSEADDRChk->setChecked( getSocketBoolValue( s, "SO_REUSEADDR") );
	_interface->SO_BROADCASTChk->setChecked( getSocketBoolValue( s, "SO_BROADCAST") );
	_interface->TCP_NODELAYChk->setChecked( getSocketBoolValue( s, "TCP_NODELAY") );
	_interface->IPTOS_LOWDELAYChk->setChecked( getSocketBoolValue( s, "IPTOS_LOWDELAY") );
	_interface->IPTOS_THROUGHPUTChk->setChecked( getSocketBoolValue( s, "IPTOS_THROUGHPUT") );
	
	_interface->SO_SNDBUFChk->setChecked( getSocketBoolValue( s, "SO_SNDBUF") );
	_interface->SO_RCVBUFChk->setChecked( getSocketBoolValue( s, "SO_RCVBUF") );
	_interface->SO_SNDLOWATChk->setChecked( getSocketBoolValue( s, "SO_SNDLOWAT") );
	_interface->SO_RCVLOWATChk->setChecked( getSocketBoolValue( s, "SO_RCVLOWAT") );

	_interface->SO_SNDBUFSpin->setValue( getSocketIntValue( s, "SO_SNDBUF") );
	_interface->SO_RCVBUFSpin->setValue( getSocketIntValue( s, "SO_RCVBUF") );
	_interface->SO_SNDLOWATSpin->setValue( getSocketIntValue( s, "SO_SNDLOWAT") );
	_interface->SO_RCVLOWATSpin->setValue( getSocketIntValue( s, "SO_RCVLOWAT") );

}

void KcmSambaConf::loadSSL(SambaShare* share) 
{
	// SSL
	
  int i = _interface->sslVersionCombo->listBox()->index(_interface->sslVersionCombo->listBox()->findItem(share->getValue("ssl version",false,true),Qt::ExactMatch));
  _interface->sslVersionCombo->setCurrentItem(i);
	
	checkBoxDict.insert("ssl",_interface->sslChk);
	checkBoxDict.insert("ssl require server cert",_interface->sslRequireServercertChk);
	checkBoxDict.insert("ssl compatibility",_interface->sslCompatibilityChk);
	checkBoxDict.insert("ssl require clientcert",_interface->sslRequireClientcertChk);
	
	lineEditDict.insert("ssl hosts edit", _interface->sslHostsEdit);
	lineEditDict.insert("ssl hosts resign", _interface->sslHostsResignEdit);
	lineEditDict.insert("ssl egd socket", _interface->sslEgdSocketEdit);
	lineEditDict.insert("ssl ciphers edit", _interface->sslCiphersEdit);
	
	urlRequesterDict.insert("ssl CA cert dir",_interface->sslCACertDirUrlRq);
	urlRequesterDict.insert("ssl CA cert file",_interface->sslCACertFileUrlRq);
	urlRequesterDict.insert("ssl entropy file",_interface->sslEntropyFileUrlRq);
	urlRequesterDict.insert("ssl client cert",_interface->sslClientCertUrlRq);
	urlRequesterDict.insert("ssl client key",_interface->sslClientKeyUrlRq);
	urlRequesterDict.insert("ssl server cert",_interface->sslServerCertUrlRq);
	urlRequesterDict.insert("ssl server key",_interface->sslServerKeyUrlRq);
	
	spinBoxDict.insert("ssl entropy bytes", _interface->sslEntropyBytesSpin);
	
}

void KcmSambaConf::loadLogon(SambaShare* share) 
{
	// Logon
	
	lineEditDict.insert("add user script", _interface->addUserScriptEdit);
	lineEditDict.insert("delete user script", _interface->deleteUserScriptEdit);
	lineEditDict.insert("logon script", _interface->logonScriptEdit);
	lineEditDict.insert("logon drive", _interface->logonDriveEdit);
	urlRequesterDict.insert("logon path",_interface->logonPathUrlRq);
	urlRequesterDict.insert("logon home",_interface->logonHomeUrlRq);
	
}


void KcmSambaConf::loadCoding(SambaShare* share) 
{
	lineEditDict.insert("coding system", _interface->codingSystemEdit);
	lineEditDict.insert("client code page", _interface->clientCodePageEdit);
	urlRequesterDict.insert("code page directory",_interface->codePageDirUrlRq);
}

void KcmSambaConf::loadWinbind(SambaShare* share) 
{
	lineEditDict.insert("winbind uid", _interface->winbindUidEdit);
	lineEditDict.insert("winbind gid", _interface->winbindGidEdit);
	lineEditDict.insert("template homedir", _interface->templateHomedirEdit);
	lineEditDict.insert("template shell", _interface->templateShellEdit);
	lineEditDict.insert("winbind separator", _interface->winbindSeparatorEdit);

	spinBoxDict.insert("winbind cache time", _interface->winbindCacheTimeSpin);
	
	checkBoxDict.insert("winbind enum users",_interface->winbindEnumUsersChk);
	checkBoxDict.insert("winbind enum groups",_interface->winbindEnumGroupsChk);
	

}

void KcmSambaConf::loadNetbios(SambaShare* share) 
{
	lineEditDict.insert("netbios aliases", _interface->netbiosAliasesEdit);
	lineEditDict.insert("netbios scope", _interface->netbiosScopeEdit);
}

void KcmSambaConf::loadVFS(SambaShare* share) 
{
	checkBoxDict.insert("host msdfs",_interface->hostMsdfsChk);

}

void KcmSambaConf::loadBrowsing(SambaShare* share) 
{
	checkBoxDict.insert("enhanced browsing",_interface->enhancedBrowsingChk);
	checkBoxDict.insert("browse list",_interface->browseListChk);
	spinBoxDict.insert("lm interval", _interface->lmIntervalSpin);
	
	setComboFromAutoValue(_interface->lmAnnounceCombo,"lm announce",share);
	
}

void KcmSambaConf::setComboIndexToValue(QComboBox* box, const QString & value, SambaShare* share) 
{
	int i = box->listBox()->index(box->listBox()->findItem(share->getValue(value,false,true),Qt::ExactMatch));
  box->setCurrentItem(i);
}


/**
 * Sets a combo box with the three entries yes, no, auto in that order
 * to the corresponding index.
 */
void KcmSambaConf::setComboFromAutoValue(QComboBox* box, const QString & key, SambaShare* share) 
{
	QString value = share->getValue(key,false,true);
	value = value.lower();
	int i=2;
	
	if (value == "yes" ||
		  value == "true" ||
			value == "1")
			i = 0;
	else
	if (value == "no" ||
		  value == "false" ||
			value == "0")
			i = 1;
			
  box->setCurrentItem(i);
}


void KcmSambaConf::loadMisc(SambaShare* share) 
{
	lineEditDict.insert("addShare command", _interface->addShareCommandEdit);
	lineEditDict.insert("change share command", _interface->changeShareCommandEdit);
	lineEditDict.insert("delete share command", _interface->deleteShareCommandEdit);
	lineEditDict.insert("panic action", _interface->panicActionEdit);
	lineEditDict.insert("preload", _interface->preloadEdit);
	lineEditDict.insert("default service", _interface->defaultServiceEdit);
	lineEditDict.insert("message command", _interface->messageCommandEdit);
	lineEditDict.insert("dfree command", _interface->dfreeCommandEdit);
	lineEditDict.insert("valid chars", _interface->validCharsEdit);
	lineEditDict.insert("remote announce", _interface->remoteAnnounceEdit);
	lineEditDict.insert("remote browse sync", _interface->remoteBrowseSyncEdit);
	lineEditDict.insert("homedir map", _interface->homedirMapEdit);
	lineEditDict.insert("source environment", _interface->sourceEnvironmentEdit);

	urlRequesterDict.insert("lock directory",_interface->lockDirectoryUrlRq);
	urlRequesterDict.insert("pid directory",_interface->pidDirectoryUrlRq);
	urlRequesterDict.insert("utmp directory",_interface->utmpDirectoryUrlRq);
	urlRequesterDict.insert("wtmp directory",_interface->wtmpDirectoryUrlRq);

	spinBoxDict.insert("time offset", _interface->timeOffsetSpin);
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

void KcmSambaConf::saveDicts(SambaShare* share) 
{
	QDictIterator<QCheckBox> checkBoxIt( checkBoxDict ); 
	 
	for( ; checkBoxIt.current(); ++checkBoxIt )	{
		share->setValue(checkBoxIt.currentKey(),checkBoxIt.current()->isChecked(), false, true );
	}
		
	QDictIterator<QLineEdit> lineEditIt( lineEditDict ); 
	 
	for( ; lineEditIt.current(); ++lineEditIt )	{
		share->setValue(lineEditIt.currentKey(),lineEditIt.current()->text(), false, true );
	}

	QDictIterator<KURLRequester> urlRequesterIt( urlRequesterDict ); 
 
	for( ; urlRequesterIt.current(); ++urlRequesterIt )	{
		share->setValue(urlRequesterIt.currentKey(),urlRequesterIt.current()->url(), false, true );
	}

	QDictIterator<QSpinBox> spinBoxIt( spinBoxDict ); 
	 
	for( ; spinBoxIt.current(); ++spinBoxIt )	{
		share->setValue(spinBoxIt.currentKey(),spinBoxIt.current()->value(), false, true );
	}
		
}

void KcmSambaConf::save() {
	// insert your saving code here...

  SambaShare *share = _sambaFile->getShare("global");
  assert(share);

  kdDebug() << "saving ... " << endl;
  
  // Base settings

  _smbconf = _interface->configUrlRq->url();
  
	// Security

	QString s;
	
	switch (_interface->securityLevelBtnGrp->id(_interface->securityLevelBtnGrp->selected())) {
		case 0 : s = "share";break;
		case 1 : s = "user";break;
		case 2 : s = "server";break;
		case 3 : s = "domain";break;
	}
	
	share->setValue("security",s);
	
	
//  share->setValue("security",_interface->securityLevelCombo->currentText());
  share->setValue("map to guest",_interface->mapToGuestCombo->currentText());

  share->setValue("guest account",_interface->guestAccountCombo->currentText());

  if (_interface->otherWinsRadio->isChecked())
     share->setValue("wins server",_interface->winsServerEdit->text(), false,true);
  else
     share->setValue("wins server",QString(""), false,true);  
  
	// socket options
	
	s = socketOptions();
	share->setValue("socket options",s,false,true);

	switch( _interface->lmAnnounceCombo->currentItem() ) {
		case 0 : s = "Yes";break;
		case 1 : s = "No";break;
		case 2 : s = "Auto";break;
	}
	
	share->setValue("lm announce",s,false,true);
	
	saveDicts( share );
		
  _sambaFile->slotApply();

}

bool KcmSambaConf::getSocketBoolValue( const QString & str, const QString & name )
{
  QString s = str;
  int i = s.find(name ,0,false);
		
  if (i > -1)
  {
		s = s.remove(0,i+1+QString(name).length());
		
		if ( s.startsWith("=") )
		{
	    s = s.remove(0,1);
	    if ( s.startsWith("0"))
				return false;
	    else
				return true;
		}
		else
	    return true;
	}
    
	return false;
}

int KcmSambaConf::getSocketIntValue( const QString & str, const QString & name )
{
    QString s = str;
    int i = s.find(name ,0,false);
		
    if (i > -1)
    {
			s = s.remove(0,i+name.length());
			if ( s.startsWith("=") )
			{
	    	s = s.remove(0,1);
	    
	    	i = s.find(" ");
	    	if (i < 0)
	        i = s.length();
	    	else
	        i++;
	    
	    	s = s.left( i );
	    
	    	return s.toInt();
			}
			else
	    	return 0;
    }
    
    return 0;
}

QString KcmSambaConf::socketOptions()
{
	QString s = "";
	
	if ( _interface->SO_KEEPALIVEChk->isChecked() )
		s+="SO_KEEPALIVE ";
	
	if ( _interface->SO_REUSEADDRChk->isChecked() )
		s+= "SO_REUSEADDR ";
		
	if ( _interface->SO_BROADCASTChk->isChecked() )
		s+= "SO_BROADCAST ";
		
	if ( _interface->TCP_NODELAYChk->isChecked() )
		s+= "TCP_NODELAY ";
		
	if ( _interface->IPTOS_LOWDELAYChk->isChecked() )
		s+= "IPTOS_LOWDELAY ";
		
	if ( _interface->IPTOS_THROUGHPUTChk->isChecked() )
		s+= "IPTOS_THROUGHPUT ";
		
	if ( _interface->SO_SNDBUFChk->isChecked() ) {
		s+= "SO_SNDBUF=";
		s+= QString::number( _interface->SO_SNDBUFSpin->value() );
		s+= " ";
	}
	
	if ( _interface->SO_RCVBUFChk->isChecked() ) {
		s+= "SO_RCVBUF=";
		s+= QString::number( _interface->SO_RCVBUFSpin->value() );
		s+= " ";
	}

	if ( _interface->SO_SNDLOWATChk->isChecked() ) {
		s+= "SO_SNDLOWAT=";
		s+= QString::number( _interface->SO_SNDLOWATSpin->value() );
		s+= " ";
	}

	if ( _interface->SO_RCVLOWATChk->isChecked() ) {
		s+= "SO_RCVLOWAT=";
		s+= QString::number( _interface->SO_RCVLOWATSpin->value() );
		s+= " ";
		
	}
			
	return s;
	
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
