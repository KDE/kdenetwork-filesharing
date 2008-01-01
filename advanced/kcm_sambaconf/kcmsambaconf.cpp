/***************************************************************************
                          kcmsambaconf.cpp  -  description
                            -------------------
    begin                : Mon Apr  8 13:35:56 CEST 2002
    copyright            : (C) 2002 by Christian Nitschkowski,
    email                : segfault_ii@web.de

    copyright            : (C) 2002-2004 by Jan Schaefer
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
#include <qlabel.h>

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
#include <klistview.h>
#include <ksimpleconfig.h>

#include "sambashare.h"
#include "sambafile.h"
#include "sharedlgimpl.h"
#include "printerdlgimpl.h"
#include "dictmanager.h"
#include "kcmsambaconf.h"
#include "smbpasswdfile.h"
#include "passwd.h"
#include "qmultichecklistitem.h"
#include "joindomaindlg.h"
#include "smbconfconfigwidget.h"


#define COL_DISABLED 2
#define COL_NOPASSWORD 3


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

  if (!_share->getBoolValue("read only"))
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
  _dictMngr = 0L;
  _sambaFile = 0L;
  m_smbConfConfigWidget = 0L;


  QBoxLayout * l = new QHBoxLayout( this );
  l->setAutoAdd( TRUE );

  QString smbFile = SambaFile::findSambaConf();
  if (smbFile.isNull()) {
     createSmbConfigWidget();
     return;
  }

  slotSpecifySmbConf(smbFile);

}


KcmSambaConf::~KcmSambaConf() {
  delete _dictMngr;
}

void KcmSambaConf::createSmbConfigWidget() {
  m_smbConfConfigWidget = new SmbConfConfigWidget(this);
  connect( m_smbConfConfigWidget, SIGNAL(smbConfChoosed(const QString &)),
           this, SLOT(slotSpecifySmbConf(const QString &)));
}

void KcmSambaConf::slotSpecifySmbConf(const QString & smbConf) {
  if (m_smbConfConfigWidget) {
     m_smbConfConfigWidget->hide();
  }

  init();
  initAdvancedTab();
  load(smbConf);


  if (getuid() != 0) {
    for (int i=0;i<_interface->mainTab->count();i++) {
      QWidget* w = _interface->mainTab->page(i);
      w->setEnabled(false);
    }
  }

  _interface->show();
}

void KcmSambaConf::init() {

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

  connect( _interface->domainRadio, SIGNAL(toggled(bool)),
          _interface->joinADomainBtn, SLOT( setEnabled(bool) ));

  connect(_interface->nullPasswordsChk,SIGNAL(toggled(bool)),
          this, SLOT(nullPasswordsEnabled(bool)));

  connect( _interface->addSambaUserBtn, SIGNAL(clicked()),
          this, SLOT( addSambaUserBtnClicked() ));

  connect( _interface->removeSambaUserBtn, SIGNAL(clicked()),
          this, SLOT( removeSambaUserBtnClicked() ));

  _interface->removeSambaUserBtn->setIconSet(SmallIconSet("1rightarrow"));
  _interface->addSambaUserBtn->setIconSet(SmallIconSet("1leftarrow"));


  connect( _interface->sambaUsersListView, SIGNAL(mouseButtonPressed(int,QListViewItem*,const QPoint &,int)),
          this, SLOT(slotMouseButtonPressed(int,QListViewItem*,const QPoint &,int)));

  connect( _interface->joinADomainBtn, SIGNAL(clicked()),
          this, SLOT( joinADomainBtnClicked() ));

  connect( _interface->loadBtn, SIGNAL(clicked()),
          this, SLOT( loadBtnClicked() ));

  connect( _interface, SIGNAL(changed()), this, SLOT(configChanged()));
}


void KcmSambaConf::initAdvancedTab()
{
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

    if (label == i18n("Security"))
      icon = SmallIcon("password");
    else
    if (label == i18n("Logging"))
      icon = SmallIcon("history");
    else
    if (label == i18n("Tuning"))
      icon = SmallIcon("launch");
    else
    if (label == i18n("Filenames"))
      icon = SmallIcon("folder");
    else
    if (label == i18n("Locking"))
      icon = SmallIcon("lock");
    else
    if (label == i18n("Printing"))
      icon = SmallIcon("fileprint");
    else
    if (label == i18n("Logon"))
      icon = SmallIcon("kdmconfig");
    else
    if (label == i18n("Protocol"))
      icon = SmallIcon("core");
    else
    if (label == i18n("Charset"))
      icon = SmallIcon("charset");
    else
    if (label == i18n("Socket"))
        icon = SmallIcon("socket");
    else
    if (label == i18n("SSL"))
      icon = SmallIcon("encrypted");
    else
    if (label == i18n("Browsing"))
      icon = SmallIcon("konqueror");
    else
    if (label == i18n("Misc"))
      icon = SmallIcon("misc");
    else
    if (label == i18n("Commands"))
      icon = SmallIcon("konsole");
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

  w = _interface->mainTab->page(5);
  _interface->mainTab->removePage(w);
  delete w;
	_interface->advancedWarningPixLbl->setPixmap(DesktopIcon("messagebox_warning"));


}

void KcmSambaConf::editShare()
{
  ShareListViewItem* item = static_cast<ShareListViewItem*>(_interface->shareListView->selectedItem());

  if (!item)
    return;

  ShareDlgImpl* dlg = new ShareDlgImpl(_interface,item->getShare());
  connect(dlg, SIGNAL(changed()), this, SLOT(configChanged()));
  dlg->exec();
  item->updateShare();

  disconnect(dlg, SIGNAL(changed()), this, SLOT(configChanged()));

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



void KcmSambaConf::loadBtnClicked() {
  load( _interface->configUrlRq->url());
}

void KcmSambaConf::load(const QString & smbFile) 
{
  kdDebug(5009) << "loading " << smbFile << endl;
  _smbconf = smbFile;

  if (_sambaFile)
    delete _sambaFile;


  _sambaFile = new SambaFile(_smbconf,false);

  connect( _sambaFile, SIGNAL(completed()), this, SLOT(fillFields()));
  connect( _sambaFile, SIGNAL(canceled(const QString &)), this, SLOT(loadCanceled(const QString &)));

  _sambaFile->load();

}

void KcmSambaConf::loadCanceled(const QString & msg) {
  KMessageBox::sorry(0L,msg,i18n("Error while opening file"));
}

void KcmSambaConf::fillFields() 
{
  // Fill the ListViews

  SambaShareList* list = _sambaFile->getSharedDirs();

  SambaShare *share = 0L;
  _interface->shareListView->clear();
  for ( share = list->first(); share; share = list->next() )
  {
    new ShareListViewItem(_interface->shareListView, share);
  }

  share = 0L;
  _interface->printerListView->clear();
  list = _sambaFile->getSharedPrinters();
  for ( share = list->first(); share; share = list->next() )
  {
    new ShareListViewItem(_interface->printerListView, share);
  }

  share = _sambaFile->getShare("global");

  if ( !share)
    share = _sambaFile->newShare("global");

  Q_ASSERT( share);
  if (_dictMngr)
    delete _dictMngr;

  _dictMngr = new DictManager(share);


  _interface->configUrlRq->setURL( _smbconf );
  _interface->configUrlRq->setMode( KFile::File | KFile::ExistingOnly);


  loadBaseSettings( share );	
  loadSecurity( share );
  loadTuning( share );
  loadLogging( share );
  loadDomain( share );
  loadWins( share );
  loadPrinting( share );
  loadFilenames( share );
  loadLocking( share );
  loadProtocol( share );
  loadSocket( share );
  loadSSL( share );
  loadLogon( share );
  loadCharset( share );
  loadWinbind( share );
  loadNetbios( share );
  loadVFS( share );
  loadLDAP( share );
  loadBrowsing( share );
  loadCommands( share );
  loadMisc( share );
  loadDebug( share );

  _dictMngr->load( share, false,true );

  loadUserTab();

  connect(_dictMngr, SIGNAL(changed()), this, SLOT(configChanged()));

}


void KcmSambaConf::loadBaseSettings(SambaShare* share) 
{

  _dictMngr->add("workgroup", _interface->workgroupEdit);
  _dictMngr->add("server string", _interface->serverStringEdit);
  _dictMngr->add("netbios name", _interface->netbiosNameEdit);
  _dictMngr->add("netbios aliases", _interface->netbiosAliasesEdit);
  _dictMngr->add("netbios scope", _interface->netbiosScopeEdit);
  _dictMngr->add("interfaces", _interface->interfacesEdit);

  _interface->guestAccountCombo->insertStringList( getUnixUsers() );
  setComboIndexToValue(_interface->guestAccountCombo,"guest account",share);

  QString value = share->getValue("map to guest",false,true);

  _interface->allowGuestLoginsChk->setChecked( value.lower()!="never" );

  _dictMngr->add("guest ok",_interface->allowGuestLoginsChk);

  _dictMngr->add("bind interfaces only",_interface->bindInterfacesOnlyChk);

  QString s = share->getValue("security",false,true).lower();
  int i = 0;

  if ( s == "share" ) i = 0; else
  if ( s == "user" ) i = 1; else
  if ( s == "server" ) i = 2; else
  if ( s == "domain" ) i = 3; else
  if ( s == "ads" ) i = 4;

  _interface->securityLevelBtnGrp->setButton(i);
  _interface->updateSecurityLevelHelpLbl();

}


void KcmSambaConf::loadSecurity(SambaShare*)
{

  _dictMngr->add("map to guest",_interface->mapToGuestCombo,
                 new QStringList(QStringList() << "Never" << "Bad User" << "Bad Password" ));


  _dictMngr->add("password server", _interface->passwordServerEdit);
  _dictMngr->add("passwd chat", _interface->passwdChatEdit);
  _dictMngr->add("root directory", _interface->rootDirectoryEdit);
  _dictMngr->add("passdb backend", _interface->passdbBackendEdit);
  _dictMngr->add("auth methods", _interface->authMethodsEdit);
  _dictMngr->add("realm", _interface->realmEdit);

  _dictMngr->add("password level", _interface->passwordLevelSpin);
  _dictMngr->add("min passwd length", _interface->minPasswdLengthSpin);
  _dictMngr->add("username level", _interface->usernameLevelSpin);
  _dictMngr->add("algorithmic rid base", _interface->algorithmicRidBaseSpin);
  _dictMngr->add("passwd chat timeout", _interface->passwdChatTimeoutSpin);

  _dictMngr->add("encrypt passwords",_interface->encryptPasswordsChk);
  _dictMngr->add("update encrypted",_interface->updateEncryptedChk);
  _dictMngr->add("passwd chat debug",_interface->passwdChatDebugChk);
  _dictMngr->add("unix password sync",_interface->unixPasswordSyncChk);
  _dictMngr->add("use rhosts",_interface->useRhostsChk);
  _dictMngr->add("hide local users",_interface->hideLocalUsersChk);


  _dictMngr->add("allow trusted domains",_interface->allowTrustedDomainsChk);
  _dictMngr->add("obey pam restrictions",_interface->obeyPamRestrictionsChk);
  _dictMngr->add("pam password change",_interface->pamPasswordChangeChk);
  _dictMngr->add("restrict anonymous",_interface->restrictAnonymousChk);
  _dictMngr->add("null passwords",_interface->nullPasswordsChk);
  _dictMngr->add("paranoid server security",_interface->paranoidServerSecurityChk);

  _dictMngr->add("smb passwd file",_interface->smbPasswdFileUrlRq);
  _dictMngr->add("passwd program",_interface->passwdProgramUrlRq);
  _dictMngr->add("username map",_interface->usernameMapUrlRq);
  _dictMngr->add("hosts equiv",_interface->hostsEquivUrlRq);
  _dictMngr->add("private dir",_interface->privateDirUrlRq);

  // Authentification
  
  _dictMngr->add("lanman auth",_interface->lanmanAuthChk);
  _dictMngr->add("ntlm auth",_interface->ntlmAuthChk);
  _dictMngr->add("use spnego",_interface->useSpnegoChk);
  _dictMngr->add("server schannel",_interface->serverSchannelCombo,
                 new QStringList(QStringList() << "Yes" << "No" << "Auto" )); 
  _dictMngr->add("server signing",_interface->serverSigningCombo,
                 new QStringList(QStringList() << "Auto" << "Mandatory" << "Disabled" )); 
  
  _dictMngr->add("client lanman auth",_interface->clientLanmanAuthChk);
  _dictMngr->add("client plaintext auth",_interface->clientPlaintextAuthChk);
  _dictMngr->add("client ntlmv2 auth",_interface->clientNTLMv2AuthChk);
  _dictMngr->add("client use spnego",_interface->clientUseSpnegoChk);
  _dictMngr->add("client schannel",_interface->clientSchannelCombo,
                 new QStringList(QStringList() << "Yes" << "No" << "Auto" )); 
  _dictMngr->add("client signing",_interface->clientSigningCombo,
                 new QStringList(QStringList() << "Auto" << "Mandatory" << "Disabled" )); 

                   
  
  
  
}

void KcmSambaConf::loadLogging(SambaShare* ) 
{
  _dictMngr->add("log file",_interface->logFileUrlRq);

  _dictMngr->add("max log size", _interface->maxLogSizeSpin);
  _dictMngr->add("syslog", _interface->syslogSpin);
  _dictMngr->add("log level", _interface->logLevelEdit);

  _dictMngr->add("status",_interface->statusChk);
  _dictMngr->add("debug uid",_interface->debugUidChk);
  _dictMngr->add("debug pid",_interface->debugPidChk);
  _dictMngr->add("debug hires timestamp",_interface->microsecondsChk);
  _dictMngr->add("syslog only",_interface->syslogOnlyChk);
  _dictMngr->add("debug timestamp",_interface->timestampChk);
  _dictMngr->add("use mmap",_interface->useMmapChk);


}

void KcmSambaConf::loadTuning(SambaShare* )
{
  _dictMngr->add("change notify timeout", _interface->changeNotifyTimeoutSpin);
  _dictMngr->add("deadtime", _interface->deadtimeSpin);
  _dictMngr->add("keepalive", _interface->keepaliveSpin);
  _dictMngr->add("lpq cache time", _interface->lpqCacheTimeSpin);
  _dictMngr->add("max open files", _interface->maxOpenFilesSpin);
  _dictMngr->add("read size", _interface->readSizeSpin);
  _dictMngr->add("max disk size", _interface->maxDiskSizeSpin);
  _dictMngr->add("stat cache size", _interface->statCacheSizeSpin);
  _dictMngr->add("max smbd processes", _interface->maxSmbdProcessesSpin);
  _dictMngr->add("name cache timeout", _interface->nameCacheTimeoutSpin);

  _dictMngr->add("getwd cache",_interface->getwdCacheChk);
  _dictMngr->add("use mmap",_interface->useMmapChk);
  _dictMngr->add("hostname lookups",_interface->hostnameLookupsChk);
  _dictMngr->add("kernel change notify",_interface->kernelChangeNotifyChk);

}

void KcmSambaConf::loadLocking(SambaShare* ) 
{
  _dictMngr->add("kernel oplocks",_interface->kernelOplocksChk);
  _dictMngr->add("lock directory",_interface->lockDirectoryUrlRq);
  _dictMngr->add("pid directory",_interface->pidDirectoryUrlRq);
  _dictMngr->add("oplock break wait time",_interface->oplockBreakWaitTimeSpin);
  _dictMngr->add("lock spin time",_interface->lockSpinTimeSpin);
  _dictMngr->add("lock spin count",_interface->lockSpinCountSpin);
  

}

void KcmSambaConf::loadDomain(SambaShare*)
{
  _dictMngr->add("preferred master",_interface->preferredMasterChk);
  _dictMngr->add("local master",_interface->localMasterChk);
  _dictMngr->add("domain master",_interface->domainMasterChk);
  _dictMngr->add("domain logons",_interface->domainLogonsChk);
  _dictMngr->add("machine password timeout", _interface->machinePasswordTimeoutSpin);
  _dictMngr->add("os level", _interface->osLevelSpin);
  _dictMngr->add("domain admin group", _interface->domainAdminGroupEdit);
  _dictMngr->add("domain guest group", _interface->domainGuestGroupEdit);

}

void KcmSambaConf::loadWins(SambaShare* share)
{
  _dictMngr->add("wins proxy",_interface->winsProxyChk);
  _dictMngr->add("dns proxy",_interface->dnsProxyChk);
  _dictMngr->add("wins server", _interface->winsServerEdit);
  _dictMngr->add("wins hook", _interface->winsHookEdit);
  _interface->winsSupportRadio->setChecked( share->getBoolValue("wins support",false,true));
  _interface->otherWinsRadio->setChecked( !share->getValue("wins server",false,true).isEmpty() );
}


void KcmSambaConf::loadPrinting(SambaShare* )
{
  _dictMngr->add("load printers",_interface->loadPrintersChk);
  _dictMngr->add("disable spoolss",_interface->disableSpoolssChk);
  _dictMngr->add("show add printer wizard",_interface->showAddPrinterWizardChk);

  _dictMngr->add("addprinter command", _interface->addprinterCommandEdit);
  _dictMngr->add("deleteprinter command", _interface->deleteprinterCommandEdit);
  _dictMngr->add("enumports command", _interface->enumportsCommandEdit);

  _dictMngr->add("printcap name",_interface->printcapNameUrlRq);
  _dictMngr->add("os2 driver map",_interface->os2DriverMapUrlRq);
  _dictMngr->add("printer driver file",_interface->printerDriverFileUrlRq);

  _dictMngr->add("total print jobs", _interface->totalPrintJobsSpin);
}

void KcmSambaConf::loadFilenames(SambaShare* ) 
{
  _dictMngr->add("strip dot",_interface->stripDotChk);
  _dictMngr->add("stat cache",_interface->statCacheChk);


  _dictMngr->add("mangled stack", _interface->mangledStackSpin);
  _dictMngr->add("mangle prefix", _interface->manglePrefixSpin);

}

void KcmSambaConf::loadProtocol(SambaShare*)
{
  // Protocol

  _dictMngr->add("write raw",_interface->writeRawChk);
  _dictMngr->add("read raw",_interface->readRawChk);
  _dictMngr->add("read bmpx",_interface->readBmpxChk);
  _dictMngr->add("large readwrite",_interface->largeReadWriteChk);
  _dictMngr->add("nt smb support",_interface->ntSmbSupportChk);
  _dictMngr->add("nt pipe support",_interface->ntPipeSupportChk);
  _dictMngr->add("time server",_interface->timeServerChk);
  _dictMngr->add("unix extensions",_interface->unixExtensionsChk);

  _dictMngr->add("max mux", _interface->maxMuxSpin);
  _dictMngr->add("max xmit", _interface->maxXmitSpin);
  _dictMngr->add("max ttl", _interface->maxTtlSpin);
  _dictMngr->add("max wins ttl", _interface->maxWinsTtlSpin);
  _dictMngr->add("min wins ttl", _interface->minWinsTtlSpin);

  _dictMngr->add("announce version", _interface->announceVersionEdit);
  _dictMngr->add("name resolve order", _interface->nameResolveOrderEdit);
  _dictMngr->add("smb ports", _interface->smbPortsEdit);

  _dictMngr->add("announce as", _interface->announceAsCombo,
                 new QStringList(QStringList() << "NT" << "NT workstation" << "win95" << "WfW")); 
                  
  _dictMngr->add("protocol", _interface->protocolCombo,
                 new QStringList(QStringList() << "NT" << "lanman2" << "lanman1" << "core" << "coreplus" )); 

  _dictMngr->add("max protocol", _interface->maxProtocolCombo,
                 new QStringList(QStringList() << "NT" << "lanman2" << "lanman1" << "core" << "coreplus")); 

  _dictMngr->add("min protocol", _interface->minProtocolCombo,
                 new QStringList(QStringList() << "NT" << "lanman2" << "lanman1" << "core" << "coreplus")); 

}

void KcmSambaConf::loadSocket(SambaShare* share) 
{
  // SOCKET options

  _dictMngr->add("socket address", _interface->socketAddressEdit);

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

void KcmSambaConf::loadSSL(SambaShare*) 
{
  // SSL

  _dictMngr->add("ssl version",_interface->sslVersionCombo,
                 new QStringList(QStringList() << "ssl2" << "ssl3" << "ssl2or3" << "tls1" ));
  
  _dictMngr->add("ssl",_interface->sslChk);
  _dictMngr->add("ssl require server cert",_interface->sslRequireServercertChk);
  _dictMngr->add("ssl compatibility",_interface->sslCompatibilityChk);
  _dictMngr->add("ssl require clientcert",_interface->sslRequireClientcertChk);

  _dictMngr->add("ssl hosts edit", _interface->sslHostsEdit);
  _dictMngr->add("ssl hosts resign", _interface->sslHostsResignEdit);
  _dictMngr->add("ssl egd socket", _interface->sslEgdSocketEdit);
  _dictMngr->add("ssl ciphers edit", _interface->sslCiphersEdit);

  _dictMngr->add("ssl CA cert dir",_interface->sslCACertDirUrlRq);
  _dictMngr->add("ssl CA cert file",_interface->sslCACertFileUrlRq);
  _dictMngr->add("ssl entropy file",_interface->sslEntropyFileUrlRq);
  _dictMngr->add("ssl client cert",_interface->sslClientCertUrlRq);
  _dictMngr->add("ssl client key",_interface->sslClientKeyUrlRq);
  _dictMngr->add("ssl server cert",_interface->sslServerCertUrlRq);
  _dictMngr->add("ssl server key",_interface->sslServerKeyUrlRq);

  _dictMngr->add("ssl entropy bytes", _interface->sslEntropyBytesSpin);

}

void KcmSambaConf::loadLogon(SambaShare* ) 
{
  // Logon

  _dictMngr->add("add user script", _interface->addUserScriptEdit);
  _dictMngr->add("add group script", _interface->addGroupScriptEdit);
  _dictMngr->add("add machine script", _interface->addMachineScriptEdit);
  _dictMngr->add("add user to group script", _interface->addUserToGroupScriptEdit);
  _dictMngr->add("delete user script", _interface->deleteUserScriptEdit);
  _dictMngr->add("delete group script", _interface->deleteGroupScriptEdit);
  _dictMngr->add("delete user from group script", _interface->deleteUserFromGroupScriptEdit);
  _dictMngr->add("set primary group script", _interface->addGroupScriptEdit);
  _dictMngr->add("shutdown script", _interface->shutdownScriptEdit);
  _dictMngr->add("abort shutdown script", _interface->abortShutdownScriptEdit);
  _dictMngr->add("logon script", _interface->logonScriptEdit);
  _dictMngr->add("logon drive", _interface->logonDriveEdit);
  _dictMngr->add("logon path",_interface->logonPathUrlRq);
  _dictMngr->add("logon home",_interface->logonHomeUrlRq);

}


void KcmSambaConf::loadCharset(SambaShare* )
{
  _dictMngr->add("coding system", _interface->codingSystemEdit);
  _dictMngr->add("client code page", _interface->clientCodePageEdit);
  _dictMngr->add("code page directory",_interface->codePageDirUrlRq);
  _dictMngr->add("display charset", _interface->displayCharsetEdit);
  _dictMngr->add("unix charset", _interface->unixCharsetEdit);
  _dictMngr->add("dos charset", _interface->dosCharsetEdit);
  _dictMngr->add("character set", _interface->characterSetEdit);
  _dictMngr->add("valid chars", _interface->validCharsEdit);
  
  _dictMngr->add("unicode",_interface->unicodeChk);
}

void KcmSambaConf::loadWinbind(SambaShare* )
{
  _dictMngr->add("winbind uid", _interface->winbindUidEdit);
  _dictMngr->add("winbind gid", _interface->winbindGidEdit);
  _dictMngr->add("template homedir", _interface->templateHomedirEdit);
  _dictMngr->add("template shell", _interface->templateShellEdit);
  _dictMngr->add("winbind separator", _interface->winbindSeparatorEdit);
  _dictMngr->add("template primary group", _interface->templatePrimaryGroupEdit);

  _dictMngr->add("winbind cache time", _interface->winbindCacheTimeSpin);
  _dictMngr->add("acl compatibility",_interface->aclCompatibilityCombo,
                 new QStringList(QStringList() << "winnt" << "win2k" << ""));

  _dictMngr->add("winbind enum users",_interface->winbindEnumUsersChk);
  _dictMngr->add("winbind enum groups",_interface->winbindEnumGroupsChk);
  _dictMngr->add("winbind use default domain",_interface->winbindUseDefaultDomainChk);
  _dictMngr->add("winbind trusted domains only",_interface->winbindTrustedDomainsOnlyChk);
  _dictMngr->add("winbind enable local accounts",_interface->winbindEnableLocalAccountsChk);
  _dictMngr->add("winbind nested groups",_interface->winbindNestedGroupsChk);


}

void KcmSambaConf::loadNetbios(SambaShare* )
{
  _dictMngr->add("disable netbios",_interface->disableNetbiosChk);
  
  _dictMngr->add("netbios aliases", _interface->netbiosAliasesEdit);
  _dictMngr->add("netbios scope", _interface->netbiosScopeEdit);
}

void KcmSambaConf::loadVFS(SambaShare*) 
{
  _dictMngr->add("host msdfs",_interface->hostMsdfsChk);

}

void KcmSambaConf::loadLDAP(SambaShare*) 
{
  _dictMngr->add("ldap suffix", _interface->ldapSuffixEdit);
  _dictMngr->add("ldap machine suffix", _interface->ldapMachineSuffixEdit);
  _dictMngr->add("ldap user suffix", _interface->ldapUserSuffixEdit);
  _dictMngr->add("ldap group suffix", _interface->ldapGroupSuffixEdit);
  _dictMngr->add("ldap idmap suffix", _interface->ldapIdmapSuffixEdit);
  _dictMngr->add("ldap filter", _interface->ldapFilterEdit);
  _dictMngr->add("ldap admin dn", _interface->ldapAdminDnEdit);
  _dictMngr->add("idmap backend", _interface->idmapBackendEdit);
  
  _dictMngr->add("ldap replication sleep",_interface->ldapReplicationSleepSpin);
  
  _dictMngr->add("ldap delete dn",_interface->ldapDeleteDnChk);
  
  _dictMngr->add("ldap ssl", _interface->ldapSslCombo,
                 new QStringList(QStringList() << "No" << "Start_tls" << "Yes"));
  
  _dictMngr->add("ldap sync", _interface->ldapSyncCombo,
                 new QStringList(QStringList() << "Yes" << "No" << "Only"));

                 
}

void KcmSambaConf::loadBrowsing(SambaShare*) 
{
  _dictMngr->add("enhanced browsing",_interface->enhancedBrowsingChk);
  _dictMngr->add("browse list",_interface->browseListChk);
  _dictMngr->add("lm interval", _interface->lmIntervalSpin);
  _dictMngr->add("remote browse sync", _interface->remoteBrowseSyncEdit);
  _dictMngr->add("preload", _interface->preloadEdit);
  
  _dictMngr->add("lm announce", _interface->lmAnnounceCombo,
                 new QStringList(QStringList() << "Yes" << "No" << "Auto"));
}

void KcmSambaConf::loadCommands(SambaShare*) 
{
  _dictMngr->add("add share command", _interface->addShareCommandEdit);
  _dictMngr->add("change share command", _interface->changeShareCommandEdit);
  _dictMngr->add("delete share command", _interface->deleteShareCommandEdit);
  _dictMngr->add("message command", _interface->messageCommandEdit);
  _dictMngr->add("dfree command", _interface->dfreeCommandEdit);
  _dictMngr->add("set quota command", _interface->setQuotaCommandEdit);
  _dictMngr->add("get quota command", _interface->getQuotaCommandEdit);
  _dictMngr->add("panic action", _interface->panicActionEdit);

}

void KcmSambaConf::setComboIndexToValue(QComboBox* box, const QString & value, SambaShare* share) 
{
  int i = box->listBox()->index(box->listBox()->findItem(share->getValue(value,false,true),Qt::ExactMatch));
  box->setCurrentItem(i);
}


void KcmSambaConf::loadMisc(SambaShare*)
{
  _dictMngr->add("preload modules", _interface->preloadModulesEdit);
  _dictMngr->add("default service", _interface->defaultServiceEdit);
  _dictMngr->add("remote announce", _interface->remoteAnnounceEdit);
  _dictMngr->add("homedir map", _interface->homedirMapEdit);
  _dictMngr->add("source environment", _interface->sourceEnvironmentEdit);

  _dictMngr->add("utmp directory",_interface->utmpDirectoryUrlRq);
  _dictMngr->add("wtmp directory",_interface->wtmpDirectoryUrlRq);

  _dictMngr->add("NIS homedir", _interface->nisHomedirChk);
  _dictMngr->add("time offset", _interface->timeOffsetSpin);
}

void KcmSambaConf::loadDebug(SambaShare*) {
  _dictMngr->add("nt status support", _interface->ntStatusSupportChk);
}



void KcmSambaConf::loadUserTab()
{
  // Remote editing of users isn't supported yet
  if ( _sambaFile->isRemoteFile()) {
    _interface->mainTab->page(3)->setEnabled(false);
    return;
  } else
    _interface->mainTab->page(3)->setEnabled(true);


  SambaShare* share = _sambaFile->getShare("global");

  QStringList added;

  SmbPasswdFile passwd( KURL(share->getValue("smb passwd file",true,true)) );
  SambaUserList sambaList = passwd.getSambaUserList();
  _interface->sambaUsersListView->clear();
  SambaUser *user;
  for ( user = sambaList.first(); user; user = sambaList.next() )
  {
    QMultiCheckListItem* item = new QMultiCheckListItem(_interface->sambaUsersListView);
    item->setText(0,user->name);
    item->setText(1,QString::number(user->uid));
    item->setOn(COL_DISABLED,user->isDisabled);
    item->setOn(COL_NOPASSWORD,user->hasNoPassword);

    if ( ! _interface->nullPasswordsChk->isOn())
      item->setDisabled(COL_NOPASSWORD, true);

    added.append(user->name);


  }

  _interface->unixUsersListView->clear();

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

}

void KcmSambaConf::joinADomainBtnClicked() {
  JoinDomainDlg *dlg = new JoinDomainDlg();
  dlg->domainEdit->setText(_interface->workgroupEdit->text());
  dlg->domainControllerEdit->setText(_interface->passwordServerEdit->text());

  int result = dlg->exec();

  if (result == QDialog::Accepted) {
    SmbPasswdFile passwd;    
    if (!passwd.joinADomain(dlg->domainEdit->text(),
                            dlg->domainControllerEdit->text(),
                            dlg->usernameEdit->text(),
                            dlg->passwordEdit->text()))
    {
      KMessageBox::sorry(0,i18n("Joining the domain %1 failed.").arg(dlg->domainEdit->text()));
    }
  }
  delete dlg;
}


void KcmSambaConf::slotMouseButtonPressed(int,QListViewItem* item,const QPoint &,int col) {
  if (col < 2)
    return;

  SambaShare* share = _sambaFile->getShare("global");
  SmbPasswdFile passwd( KURL(share->getValue("smb passwd file",true,true)) );
  QMultiCheckListItem* i = static_cast<QMultiCheckListItem*>(item);
  SambaUser user( item->text(0), item->text(1).toInt() );
  user.isDisabled = i->isOn(COL_DISABLED);
  user.hasNoPassword = i->isOn(COL_NOPASSWORD);

  if (!i->isDisabled(col))
  {

    switch(col) {
      case COL_DISABLED : 
        if (i->isOn(col)) 
          passwd.enableUser(user);
        else
          passwd.disableUser(user);
        break;
      case COL_NOPASSWORD : 
        if (i->isOn(col)) {
          sambaUserPasswordBtnClicked();                    
          return; // the item is already set off by the btnClicked method
        }
        else
          passwd.setNoPassword(user);
        break;
    }

    i->toggle(col);
  }
}

void KcmSambaConf::nullPasswordsEnabled(bool b) 
{
  QListViewItemIterator it( _interface->sambaUsersListView );
  for ( ; it.current(); ++it ) {
    QMultiCheckListItem* sambaItem = static_cast<QMultiCheckListItem*>(it.current());
    sambaItem->setDisabled(COL_NOPASSWORD,!b);

  }
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
    
    QCString password;
    int passResult = KPasswordDialog::getNewPassword(password, 
                        i18n("<qt>Please enter a password for the user <b>%1</b></qt>").arg(user.name));
    if (passResult != KPasswordDialog::Accepted) {
       list.remove(item);
       continue;
    }       
    
    if (!passwd.addUser(user,password))
    {
      KMessageBox::sorry(0,i18n("<qt>Adding the user <b>%1</b> to the Samba user database failed.</qt>").arg(user.name));
      break;
    }

    QMultiCheckListItem* sambaItem = new QMultiCheckListItem(_interface->sambaUsersListView);
    sambaItem->setText(0,user.name);
    sambaItem->setText(1,QString::number(user.uid));
    sambaItem->setOn(COL_DISABLED,false);
    sambaItem->setOn(COL_NOPASSWORD,false);
    if ( ! _interface->nullPasswordsChk->isOn())
      sambaItem->setDisabled(COL_NOPASSWORD, true);
    

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
    
    QCString password;
    int passResult = KPasswordDialog::getNewPassword(password,
                        i18n("Please enter a password for the user %1").arg(user.name));
    if (passResult != KPasswordDialog::Accepted)
       return;
    
    if (!passwd.changePassword(user,password))
    {
      KMessageBox::sorry(0,i18n("Changing the password of the user %1 failed.").arg(user.name));
    } else {
      static_cast<QMultiCheckListItem*>(item)->setOn(COL_NOPASSWORD,false);
    }

  }

}


void KcmSambaConf::defaults() {
  // insert your default settings code here...
  emit changed(true);
}

#define FILESHARECONF "/etc/security/fileshare.conf"

void KcmSambaConf::save() {
  SambaShare *share = _sambaFile->getShare("global");
  assert(share);

  kdDebug(5009) << "saving ... " << endl;

  // Base settings

  _smbconf = _interface->configUrlRq->url();
  KSimpleConfig config(QString::fromLatin1(FILESHARECONF),false);
  config.writeEntry("SMBCONF",_smbconf);
  config.sync();

  // Security

  QString s;

  switch (_interface->securityLevelBtnGrp->id(_interface->securityLevelBtnGrp->selected())) {
    case 0 : s = "share";break;
    case 1 : s = "user";break;
    case 2 : s = "server";break;
    case 3 : s = "domain";break;
    case 4 : s = "ads";break;
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


  _dictMngr->save( share,false,true );

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
  // KCModule::Default|KCModule::Apply|KCModule::Help;
  return KCModule::Apply;
}

void KcmSambaConf::configChanged() {
  // insert your saving code here...
  emit changed(true);
}

QString KcmSambaConf::quickHelp() const
{
  return i18n("<h1>Samba Configuration</h1> here you can configure your SAMBA server.");
}

// ------------------------------------------------------------------------

extern "C"
{
  KDE_EXPORT KCModule *create_KcmSambaConf(QWidget *parent, const char *name)
  {
    KGlobal::locale()->insertCatalogue("kfileshare");
    return new KcmSambaConf(parent, name);
  }
}

#include "kcmsambaconf.moc"
