/*
  Copyright (c) 2002 Laurent Montel <lmontel@mandrakesoft.com>
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include <unistd.h>

#include <qlayout.h>
#include <qvbuttongroup.h>
#include <qvgroupbox.h> 
#include <qlabel.h>
#include <qdir.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qtooltip.h>
#include <qvbox.h>

#include <kpushbutton.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kdialogbase.h>
#include <kgenericfactory.h>
#include <klistview.h>
#include <kiconloader.h>
#include <knfsshare.h>
#include <ksambashare.h>
#include <kfileshare.h>
#include <kstandarddirs.h>
#include <ksimpleconfig.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <kuser.h>
#include <kurl.h>
#include <kprocess.h>
#include <krichtextlabel.h>

#include "../advanced/propsdlgplugin/propertiespage.h"
#include "../advanced/nfs/nfsfile.h"
#include "../advanced/kcm_sambaconf/sambafile.h"

#include "controlcenter.h"
#include "fileshare.h"
#include "groupconfigdlg.h"

typedef KGenericFactory<KFileShareConfig, QWidget > ShareFactory;
K_EXPORT_COMPONENT_FACTORY (kcm_fileshare, ShareFactory("kcmfileshare") )


#define FILESHARECONF "/etc/security/fileshare.conf"
#define FILESHARE_DEBUG 5009

KFileShareConfig::KFileShareConfig(QWidget *parent, const char *name, const QStringList &):
    KCModule(ShareFactory::instance(), parent, name)
{
  KGlobal::locale()->insertCatalogue("kfileshare");                            

  QBoxLayout* layout = new QVBoxLayout(this,0,
				       KDialog::spacingHint());

/*  
  QVButtonGroup *box = new QVButtonGroup( i18n("File Sharing"), this );
  box->layout()->setSpacing( KDialog::spacingHint() );
  layout->addWidget(box);
  noSharing=new QRadioButton( i18n("Do &not allow users to share files"), box );
  sharing=new QRadioButton( i18n("&Allow users to share files from their HOME folder"),  box);
*/
  m_ccgui = new ControlCenterGUI(this);
  connect( m_ccgui, SIGNAL( changed()), this, SLOT(configChanged()));
  connect( m_ccgui->allowedUsersBtn, SIGNAL( clicked()), 
           this, SLOT(allowedUsersBtnClicked()));

  QString path = QString::fromLocal8Bit( getenv( "PATH" ) );
  path += QString::fromLatin1(":/usr/sbin");
  QString sambaExec = KStandardDirs::findExe( QString::fromLatin1("smbd"), path );
  QString nfsExec = KStandardDirs::findExe( QString::fromLatin1("rpc.nfsd"), path );

  if ( nfsExec.isEmpty() && sambaExec.isEmpty())
  {
      m_ccgui->shareGrp->setDisabled(true);
      m_ccgui->sharedFoldersGroupBox->setDisabled(true);
  }
  else
  {   
      if (nfsExec.isEmpty()) {
        m_ccgui->nfsChk->setDisabled(true);
        m_ccgui->nfsChk->setChecked(false);
        QToolTip::add(m_ccgui->nfsChk,i18n("No NFS server installed on this system"));
      } 
      
      if (sambaExec.isEmpty()) {
        m_ccgui->sambaChk->setDisabled(true);
        m_ccgui->sambaChk->setChecked(false);
        QToolTip::add(m_ccgui->sambaChk,i18n("No Samba server installed on this system"));
      }
          
      m_ccgui->infoLbl->hide();      
      layout->addWidget(m_ccgui);
      updateShareListView();
      connect( KNFSShare::instance(), SIGNAL( changed()), 
               this, SLOT(updateShareListView()));
      connect( KSambaShare::instance(), SIGNAL( changed()), 
               this, SLOT(updateShareListView()));

               
  }
  
  if((getuid() == 0) || 
     ((KFileShare::shareMode() == KFileShare::Advanced) &&
      (KFileShare::authorization() == KFileShare::Authorized)))
  {
      connect( m_ccgui->addShareBtn, SIGNAL(clicked()),
               this, SLOT(addShareBtnClicked()));
      connect( m_ccgui->changeShareBtn, SIGNAL(clicked()),
               this, SLOT(changeShareBtnClicked()));
      connect( m_ccgui->removeShareBtn, SIGNAL(clicked()),
               this, SLOT(removeShareBtnClicked()));
      m_ccgui->listView->setSelectionMode(QListView::Extended);       
      m_ccgui->shareBtnPnl->setEnabled(true);        
  }
  
  
  if (getuid()==0) {               
      setButtons(Help|Apply);
  } else {
      setButtons(Help);
      m_ccgui->shareGrp->setDisabled( true );
  }

  load();
}

void KFileShareConfig::updateShareListView() 
{
      m_ccgui->listView->clear();
      KNFSShare* nfs = KNFSShare::instance();
      KSambaShare* samba = KSambaShare::instance();
      
      QStringList dirs = nfs->sharedDirectories();
      QStringList sambaDirs = samba->sharedDirectories();
      
      for ( QStringList::ConstIterator it = sambaDirs.begin(); it != sambaDirs.end(); ++it ) {
        // Do not insert duplicates
        if (nfs->isDirectoryShared(*it))
            continue;
            
        dirs += *it;            
      }

      QPixmap folderPix = SmallIcon("folder",0,KIcon::ShareOverlay);
      QPixmap okPix = SmallIcon("button_ok");
      QPixmap cancelPix = SmallIcon("button_cancel");
      
      for ( QStringList::Iterator it = dirs.begin(); it != dirs.end(); ++it ) {
        KListViewItem* item = new KListViewItem(m_ccgui->listView);
        item->setText(0,*it);
        item->setPixmap(0, folderPix);
        
        if (samba->isDirectoryShared(*it))
          item->setPixmap(1,okPix);
        else 
          item->setPixmap(1,cancelPix);
          
        if (nfs->isDirectoryShared(*it))
          item->setPixmap(2,okPix);
        else          
          item->setPixmap(2,cancelPix);

      }
      
}

void KFileShareConfig::allowedUsersBtnClicked() {
  GroupConfigDlg dlg(this,m_fileShareGroup,m_restricted,m_rootPassNeeded,
                     m_ccgui->simpleRadio->isChecked());
  if (dlg.exec() == QDialog::Accepted) {
      m_fileShareGroup = dlg.fileShareGroup().name();
      m_restricted = dlg.restricted();
      m_rootPassNeeded = dlg.rootPassNeeded();      
      configChanged();
  }      

}


void KFileShareConfig::load()
{
    KSimpleConfig config(QString::fromLatin1(FILESHARECONF),true);

    m_ccgui->shareGrp->setChecked( config.readEntry("FILESHARING", "yes") == "yes" );
    
    m_restricted = config.readEntry("RESTRICT", "yes") == "yes";
    
    if (config.readEntry("SHARINGMODE", "simple") == "simple")
        m_ccgui->simpleRadio->setChecked(true);
    else        
        m_ccgui->advancedRadio->setChecked(true);
          
    m_fileShareGroup = config.readEntry("FILESHAREGROUP", "fileshare");

    m_ccgui->sambaChk->setChecked( 
          config.readEntry("SAMBA", "yes") == "yes");

    m_ccgui->nfsChk->setChecked( 
          config.readEntry("NFS", "yes") == "yes");

    m_rootPassNeeded = config.readEntry("ROOTPASSNEEDED", "yes") == "yes";
                    
    m_smbConf = KSambaShare::instance()->smbConfPath();
}

bool KFileShareConfig::addGroupAccessesToFile(const QString & file) {
  KProcess chgrp;
  chgrp << "chgrp" << m_fileShareGroup << file;
  KProcess chmod;
  chmod << "chmod" << "g=rw" << file;
  
  if (!chgrp.start(KProcess::Block) && chgrp.normalExit()) {
      kdDebug(FILESHARE_DEBUG) << "KFileShareConfig::addGroupAccessesToFile: chgrp failed" << endl;
      return false;
      
  }      
      
  if(!chmod.start(KProcess::Block) && chmod.normalExit()) {
      kdDebug(FILESHARE_DEBUG) << "KFileShareConfig::addGroupAccessesToFile: chmod failed" << endl;
      return false;
  }
  
  return true;
  
}

bool KFileShareConfig::removeGroupAccessesFromFile(const QString & file) {
  KProcess chgrp;
  chgrp << "chgrp" << "root" << file;
  KProcess chmod;
  chmod << "chmod" << "g=r" << file;
  
  if (!chgrp.start(KProcess::Block) && chgrp.normalExit()) {
      kdDebug(FILESHARE_DEBUG) << "KFileShareConfig::removeGroupAccessesFromFile: chgrp failed" << endl;
      return false;
      
  }      
      
  if(!chmod.start(KProcess::Block) && chmod.normalExit()) {
      kdDebug(FILESHARE_DEBUG) << "KFileShareConfig::removeGroupAccessesFromFile: chmod failed" << endl;
      return false;
  }
  
  return true;
}  


bool KFileShareConfig::setGroupAccesses() {
  if (m_rootPassNeeded || ! m_ccgui->sambaChk->isChecked()) {
      if (!removeGroupAccessesFromFile(KSambaShare::instance()->smbConfPath()))
          return false;
  }
  
  if (m_rootPassNeeded || ! m_ccgui->nfsChk->isChecked()) {          
      if (!removeGroupAccessesFromFile(KNFSShare::instance()->exportsPath()))
          return false;
  }
  
  if (! m_rootPassNeeded && m_ccgui->sambaChk->isChecked()) {
      if (!addGroupAccessesToFile(KSambaShare::instance()->smbConfPath()))
          return false;
  }

  if (! m_rootPassNeeded && m_ccgui->nfsChk->isChecked()) {
      if (!addGroupAccessesToFile(KNFSShare::instance()->exportsPath()))
          return false;
  }
  

  return true;
}

void KFileShareConfig::save()
{
    setGroupAccesses();

    QDir dir("/etc/security");
    if ( !dir.exists())
        dir.mkdir("/etc/security");

    QFile file(FILESHARECONF);
    if ( ! file.open(IO_WriteOnly)) {
        KMessageBox::detailedError(this, 
            i18n("Could not save settings."),
            i18n("Could not open file '%1' for writing: %2").arg(FILESHARECONF).arg(
             file.errorString() ),
            i18n("Saving Failed"));
        return;
    }        
        
    
    QTextStream stream(&file);
    
    stream << "FILESHARING=";
    stream << (m_ccgui->shareGrp->isChecked() ? "yes" : "no");        
    
    stream << "\nRESTRICT=";
    stream << (m_restricted ? "yes" : "no");        
        
    stream << "\nSHARINGMODE=";
    stream << (m_ccgui->simpleRadio->isChecked() ? "simple" : "advanced");        

    stream << "\nFILESHAREGROUP=";
    stream << m_fileShareGroup;    
    
    stream << "\nSAMBA=";
    stream << (m_ccgui->sambaChk->isChecked() ? "yes" : "no");        
        
    stream << "\nNFS=";
    stream << (m_ccgui->nfsChk->isChecked() ? "yes" : "no");        
                
    stream << "\nROOTPASSNEEDED=";
    stream << (m_rootPassNeeded ? "yes" : "no");        
    
    stream << "\nSMBCONF=";
    stream << m_smbConf;
    
    file.close();
}

void KFileShareConfig::defaults()
{
    m_ccgui->shareGrp->setChecked( false );
}

QString KFileShareConfig::quickHelp() const
{
    return i18n("<h1>File Sharing</h1><p>This module can be used "
    		    "to enable file sharing over the network using "
				"the \"Network File System\" (NFS) or SMB in Konqueror. "
				"The latter enables you to share your files with Windows(R) "
				"computers on your network.</p>");
}

void KFileShareConfig::addShareBtnClicked() {
  showShareDialog(KFileItemList());
}


PropertiesPageDlg::PropertiesPageDlg(QWidget*parent, KFileItemList files)
  : KDialogBase(parent, "sharedlg", true,
                i18n("Share Folder"), Ok|Cancel, Ok, true)
{
  QVBox* vbox = makeVBoxMainWidget();
  
  m_page = new PropertiesPage(vbox,files,true);
}  

bool PropertiesPageDlg::hasChanged() {
  return m_page->hasChanged();
}

void PropertiesPageDlg::slotOk() {
  if (hasChanged()) {
    if (!m_page->save())
        return;
  }

  KDialogBase::slotOk();
}



void KFileShareConfig::showShareDialog(const KFileItemList & files) {
  PropertiesPageDlg* dlg = new PropertiesPageDlg(this,files);              
  if (dlg->exec() == QDialog::Accepted) {
    if ( dlg->hasChanged() ) {
         updateShareListView();
    }
  }  
  delete dlg;
}

void KFileShareConfig::changeShareBtnClicked() {
  KFileItemList files;
  QPtrList<QListViewItem> items = m_ccgui->listView->selectedItems();
  
  QListViewItem* item;
  for ( item = items.first(); item; item = items.next() ) {
      files.append(new KFileItem(KURL::fromPathOrURL(item->text(0)),"",0));
  }
  
  showShareDialog(files);
}

void KFileShareConfig::removeShareBtnClicked() {
  
  QPtrList<QListViewItem> items = m_ccgui->listView->selectedItems();
  QListViewItem *item;
  
  bool nfs = false;
  bool samba = false;
  
  for ( item = items.first(); item; item = items.next() ) {
      
      if (KNFSShare::instance()->isDirectoryShared(item->text(0)))
          nfs = true;
  
      if (KSambaShare::instance()->isDirectoryShared(item->text(0)))
          samba = true;
  }

  NFSFile nfsFile(KNFSShare::instance()->exportsPath());
  if (nfs) {
    kdDebug(FILESHARE_DEBUG) << "KFileShareConfig::removeShareBtnClicked: nfs = true" << endl;
    nfsFile.load();
    for ( item = items.first(); item; item = items.next() ) {
        nfsFile.removeEntryByPath(item->text(0));
    }
  }
  
  SambaFile smbFile(KSambaShare::instance()->smbConfPath(),false);
  if (samba) {
    kdDebug(FILESHARE_DEBUG) << "KFileShareConfig::removeShareBtnClicked: samba = true" << endl;  
    smbFile.load();
    for ( item = items.first(); item; item = items.next() ) {
        smbFile.removeShareByPath(item->text(0));
    }
  }    

  PropertiesPage::save(&nfsFile, &smbFile, nfs,samba);
    
  updateShareListView();
}

#include "fileshare.moc"
