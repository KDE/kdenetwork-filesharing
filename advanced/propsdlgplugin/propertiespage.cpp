/*
  Copyright (c) 2004 Jan Schaefer <j_schaef@informatik.uni-kl.de>

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

#include <qcheckbox.h>
#include <qtooltip.h>
#include <qbuttongroup.h>
#include <qfileinfo.h>
#include <qlabel.h>
#include <kpushbutton.h>
#include <qtimer.h>

#include <kfileshare.h>
#include <knfsshare.h>
#include <ksambashare.h>
#include <klocale.h>
#include <kurl.h>
#include <kdebug.h>
#include <kfile.h>
#include <kurlrequester.h>
#include <kmessagebox.h>
#include <klineedit.h>
#include <kprocio.h>
#include <ktempfile.h>

// NFS related
#include "../nfs/nfsfile.h"
#include "../nfs/nfsentry.h"
#include "../nfs/nfsdialog.h"

// Samba related
#include "../kcm_sambaconf/sambafile.h"
#include "../kcm_sambaconf/sambashare.h"
#include "../kcm_sambaconf/sharedlgimpl.h"

#include "propertiespage.h"

#define FILESHARE_DEBUG 5009

PropertiesPage::PropertiesPage(QWidget* parent, KFileItemList items,bool enterUrl) 
  : PropertiesPageGUI(parent), 
  m_enterUrl(enterUrl), 
  m_items(items), 
  m_nfsFile(0), 
  m_nfsEntry(0),
  m_sambaFile(0), 
  m_sambaShare(0),
  m_sambaChanged(false),
  m_nfsChanged(false),
  m_loaded(false)
{
  if (m_items.isEmpty()) {
    shareFrame->setDisabled(true);
  } else {
    shareFrame->setEnabled(true);
    // currently only one dir is allowed
    m_path = m_items.first()->url().path(1);
  }
  
  if (m_enterUrl) {
    shareChk->hide();
    urlRq->setMode(KFile::Directory |
                   KFile::ExistingOnly |
                   KFile::LocalOnly );
    urlRq->setURL(m_path);            
    connect( urlRq, SIGNAL(textChanged(const QString&)),
             this, SLOT(urlRqTextChanged(const QString&)));       
  } else {
    urlRq->hide();
    folderLbl->hide();
  }
  
  
  enableSamba(false,i18n("Reading Samba configuration file ..."));
  enableNFS(false,i18n("Reading NFS configuration file ..."));
  
      
  //QTimer::singleShot(1, this, SLOT(load));
  load();
}  

PropertiesPage::~PropertiesPage() 
{
  delete m_nfsFile;
  delete m_sambaFile;
}

void PropertiesPage::urlRqTextChanged(const QString&) {
  if (!m_enterUrl)
      return;
      
  KURL url(urlRq->url());
  if (url.isLocalFile()) {
      QFileInfo info(url.path(1));
      if (info.exists() &&
          info.isDir()) 
      {
         shareFrame->setEnabled(true);   
         return;
      }
  }      
  
  shareFrame->setDisabled(true);
}

void PropertiesPage::load() {
  loadNFS();
  loadSamba();

  bool nfsShared = KNFSShare::instance()->isDirectoryShared(m_path);
  bool sambaShared = KSambaShare::instance()->isDirectoryShared(m_path);

  nfsChk->setChecked(nfsShared);
  sambaChk->setChecked(sambaShared);
    
  if (!m_enterUrl)      
      shareChk->setChecked(nfsShared || sambaShared);
  
  m_loaded = true;
}

void PropertiesPage::enableNFS(bool b, const QString & message) {
   nfsChk->setEnabled(b);
   nfsGrp->setEnabled(b);
   QToolTip::add(nfsChk,message);
   QToolTip::add(nfsGrp,message);
}

void PropertiesPage::enableSamba(bool b, const QString & message) {
   sambaChk->setEnabled(b);
   sambaGrp->setEnabled(b);
   QToolTip::add(sambaChk,message);
   QToolTip::add(sambaGrp,message);
}


bool PropertiesPage::save() {
  if (!hasChanged()) {
      kdDebug(FILESHARE_DEBUG) << "PropertiesPage::save: nothing changed." << endl;
      return true;
  }
  
  if (!checkURL()) {
      kdDebug(FILESHARE_DEBUG) << "PropertiesPage::save: url check failed." << endl;
      return false;
  }      
  
  updateNFSEntry();
  if (!updateSambaShare()) {
      kdDebug(FILESHARE_DEBUG) << "PropertiesPage::save: updateSambaShare failed!" << endl;
      return false;
  }      
 
  return save(m_nfsFile, m_sambaFile, m_nfsChanged, m_sambaChanged); 
}  
  
bool PropertiesPage::save(NFSFile* nfsFile, SambaFile* sambaFile, bool nfs, bool samba) 
{  
  QString nfsFileName = KNFSShare::instance()->exportsPath();
  bool nfsNeedsKDEsu = false;
  
  if (nfs) {
      if (QFileInfo(nfsFileName).isWritable()) {
          nfsFile->saveTo(nfsFileName);
      } else {
          nfsNeedsKDEsu = true;
          kdDebug(FILESHARE_DEBUG) << "PropertiesPage::save: nfs needs kdesu." << endl;
      }          
  } else
    kdDebug(FILESHARE_DEBUG) << "PropertiesPage::save: nfs has not changed." << endl;

  
  QString sambaFileName = KSambaShare::instance()->smbConfPath();
  bool sambaNeedsKDEsu = false;
  if (samba) {
      if (QFileInfo(sambaFileName).isWritable()) {
          sambaFile->saveTo(sambaFileName);
      } else {
          sambaNeedsKDEsu = true;
          kdDebug(FILESHARE_DEBUG) << "PropertiesPage::save: samba needs kdesu." << endl;
      }          
  } else
    kdDebug(FILESHARE_DEBUG) << "PropertiesPage::save: samba has not changed." << endl;

  
  if (nfsNeedsKDEsu || sambaNeedsKDEsu) {
     KTempFile nfsTempFile;
     nfsTempFile.setAutoDelete(true);
     KTempFile sambaTempFile;
     sambaTempFile.setAutoDelete(true);
     
     KProcIO proc;

     QString command;
     
     if (nfsNeedsKDEsu) {
         nfsFile->saveTo(nfsTempFile.name());
         command += QString("cp %1 %2;exportfs -ra;")
        .arg(KProcess::quote( nfsTempFile.name() ))
        .arg(KProcess::quote( nfsFileName ));
     }         
     
     if (sambaNeedsKDEsu) {
         sambaFile->saveTo(sambaTempFile.name());
         command += QString("cp %1 %2;")
        .arg(KProcess::quote( sambaTempFile.name() ))
        .arg(KProcess::quote( sambaFileName ));
     }       
         
     proc<<"kdesu" << "-d" << "-c"<<command;

     if (!proc.start(KProcess::Block, true)) {
       kdDebug(FILESHARE_DEBUG) << "PropertiesPage::save: kdesu command failed" << endl;
       return false;
     }      
  }
  
  kdDebug(FILESHARE_DEBUG) << "PropertiesPage::save: Saving successfull." << endl;
  return true;
}

bool PropertiesPage::saveSamba() {
  if (!updateSambaShare()) {
    return false;
  }
      
  if (m_sambaChanged) {
      kdDebug(FILESHARE_DEBUG) << "PropertiesPage::saveSamba: saving..." << endl;
      return m_sambaFile->save();
  }
  
  kdDebug(FILESHARE_DEBUG) << "PropertiesPage::saveSamba: samba has not changed." << endl;
  return true;      
}

bool PropertiesPage::saveNFS() {
  updateNFSEntry();
  if (!m_nfsChanged) {
      kdDebug(FILESHARE_DEBUG) << "PropertiesPage::saveNFS: NFS did not change." << endl;
      return true;
  }      
      
  kdDebug(FILESHARE_DEBUG) << "PropertiesPage::saveNFS: saving..." << endl;
  return m_nfsFile->save();    
}




bool PropertiesPage::checkURL() {

  kdDebug(FILESHARE_DEBUG) << "PropertiesPage::checkURL" << endl;
  
  if (!m_enterUrl)
      return true;
  
  kdDebug(FILESHARE_DEBUG) << "PropertiesPage::checkURL: enterUrl=true" << endl;
  
  KURL url(urlRq->url());      
  QString path = url.path(1);        
  
  kdDebug(FILESHARE_DEBUG) << "PropertiesPage::checkURL: m_path='" 
        << m_path << "'" << endl;
  
  kdDebug(FILESHARE_DEBUG) << "PropertiesPage::checkURL: path='" 
        << path << "'" << endl;
  
  if (m_path == path) {
    kdDebug(FILESHARE_DEBUG) << "PropertiesPage::checkURL: paths are equal" << endl;
    return true;
  }    
    
  kdDebug(FILESHARE_DEBUG) << "PropertiesPage::checkURL: different path" << endl;
  
  
  if (!url.isValid()) {
      KMessageBox::sorry(this,i18n("Please enter a valid path."));
      urlRq->setFocus();
      urlRq->lineEdit()->selectAll();
      return false;
  }
  
  kdDebug(FILESHARE_DEBUG) << "PropertiesPage::checkURL: url is valid" << endl;
  
  if (!url.isLocalFile()) {
      KMessageBox::sorry(this,i18n("Only local folders can be shared."));
      urlRq->setFocus();
      urlRq->lineEdit()->selectAll();
      return false;
  }
  
  kdDebug(FILESHARE_DEBUG) << "PropertiesPage::checkURL: url is local file" << endl;
  
  QFileInfo info(path);
  
  if (!info.exists()) 
  {
      KMessageBox::sorry(this,i18n("The folder does not exists."));
      urlRq->setFocus();
      urlRq->lineEdit()->selectAll();
      return false;
  }

  kdDebug(FILESHARE_DEBUG) << "PropertiesPage::checkURL: folder exits" << endl;
  
  
  if (!info.isDir()) 
  {
      KMessageBox::sorry(this,i18n("Only folders can be shared."));
      urlRq->setFocus();
      urlRq->lineEdit()->selectAll();
      return false;
  }
  
  kdDebug(FILESHARE_DEBUG) << "PropertiesPage::checkURL: path is dir" << endl;
  
  if (KSambaShare::instance()->isDirectoryShared(path) ||
      KNFSShare::instance()->isDirectoryShared(path))
  {
      KMessageBox::sorry(this,i18n("The folder is already shared."));
      urlRq->setFocus();
      urlRq->lineEdit()->selectAll();
      return false;
  }      
  
  kdDebug(FILESHARE_DEBUG) << "PropertiesPage::checkURL: folder not shared yet" << endl;
  m_path = path;
  
  return true;
}

bool PropertiesPage::loadNFS() {
  if (!KFileShare::nfsEnabled()) {
      enableNFS(false,i18n("The administrator does not allow sharing with NFS."));  
      return false;
  }      

  delete m_nfsFile;
  m_nfsFile = new NFSFile(KNFSShare::instance()->exportsPath());

  if (!m_nfsFile->load()) {
      enableNFS(false,i18n("Error: could not read NFS configuration file."));
      return false;
  }      

  enableNFS(true,"");
        
  loadNFSEntry();
  return true; 
}  

void PropertiesPage::loadNFSEntry() {
  m_nfsEntry = m_nfsFile->getEntryByPath(m_path);
  m_nfsChanged = false;
  
  if (!m_nfsEntry) {
    nfsChk->setChecked(false);
    return;
  }
  
        
  NFSHost* publicHost = m_nfsEntry->getPublicHost();

  if (publicHost) {
    publicNFSChk->setChecked(true);
    writableNFSChk->setChecked(!publicHost->readonly);
  } else
    publicNFSChk->setChecked(false);
}

void PropertiesPage::updateNFSEntry() {
  if (shareChk->isChecked() && 
      nfsChk->isChecked()) 
  {
    if (!m_nfsEntry) {
        m_nfsEntry = new NFSEntry(m_path);
        m_nfsFile->addEntry(m_nfsEntry);
        m_nfsChanged = true;
    }
  
    NFSHost* publicHost = m_nfsEntry->getPublicHost();        
    
    if (publicNFSChk->isChecked()) {
      if (!publicHost) {
          publicHost = new NFSHost("*");
          publicHost->allSquash=true;
          m_nfsEntry->addHost(publicHost);
          m_nfsChanged = true;
      } 
      
      if (publicHost->readonly != !writableNFSChk->isChecked()) {
          publicHost->readonly  = !writableNFSChk->isChecked();
          m_nfsChanged = true;
      }          
    } else {
      if (publicHost) {
          m_nfsEntry->removeHost(publicHost);
          m_nfsChanged = true;
      }
    }
  } else { // unshare
    if (m_nfsEntry) {
      m_nfsFile->removeEntry(m_nfsEntry);
      m_nfsEntry = 0;
      m_nfsChanged = true;
    } 
  }
}

void PropertiesPage::moreNFSBtn_clicked() {
  updateNFSEntry();
  NFSDialog* dlg = new NFSDialog(this,m_nfsEntry);
  if (dlg->exec()==QDialog::Accepted &&
      dlg->modified()) 
  {
    kdDebug(FILESHARE_DEBUG) << "NFSDialog::ok" << endl;
    loadNFSEntry();
    m_nfsChanged = true;
    emit changed();    
  }
  delete dlg;
}

bool PropertiesPage::loadSamba() {
  if (!KFileShare::sambaEnabled()) {
      enableSamba(false,i18n("The administrator does not allow sharing with Samba."));
      return false;
  }      
  
  delete m_sambaFile;
  m_sambaFile = new SambaFile(KSambaShare::instance()->smbConfPath(), false);
  if (! m_sambaFile->load()) {
      enableSamba(false,i18n("Error: could not read Samba configuration file."));
      return false;
  }
  
  enableSamba(true,"");
  QString shareName = m_sambaFile->findShareByPath(m_path);
  if (shareName.isNull()) {
      sambaChk->setChecked(false);
      kdDebug(FILESHARE_DEBUG) << "PropertiesPage::loadSamba: shareName is null!"
          << endl;
      return false;
  }

  kdDebug(FILESHARE_DEBUG) << "PropertiesPage::loadSamba: shareName="
        << shareName << endl;    
  
  m_sambaShare = m_sambaFile->getShare(shareName);

  loadSambaShare();
  return true;
}


void PropertiesPage::loadSambaShare() {
  if (! m_sambaShare) {
      sambaChk->setChecked(false);
      kdDebug(FILESHARE_DEBUG) << "PropertiesPage::loadSambaShare: no share with name "
          << m_sambaShare->getName() << endl;    
      return;
  }
  
  if (m_sambaShare->getBoolValue("public")) {
    publicSambaChk->setChecked(true);
    writableSambaChk->setChecked(m_sambaShare->getBoolValue("writable"));
  } else
    publicSambaChk->setChecked(false);

    
  sambaNameEdit->setText(m_sambaShare->getName() );    
}

void PropertiesPage::sambaChkToggled( bool b ) {
  if (!m_loaded)
      return;
      
  if (sambaNameEdit->text().isEmpty())      
      sambaNameEdit->setText(getNewSambaName());
}

bool PropertiesPage::updateSambaShare() {
  kdDebug(FILESHARE_DEBUG) << "PropertiesPage::updateSambaShare" << endl;
  
  if (shareChk->isChecked() && 
      sambaChk->isChecked()) 
  {
    if (m_enterUrl) {
      if (m_path != urlRq->url()) {
        m_path = urlRq->url();
      }
    }
  
    kdDebug(FILESHARE_DEBUG) << "PropertiesPage::updateSambaShare: m_path" 
          << m_path << endl;
    
    if (!m_sambaShare) {
      createNewSambaShare();
      m_sambaChanged = true;
    }      

    setSambaShareBoolValue("public", publicSambaChk);
    setSambaShareBoolValue("writable", writableSambaChk);

    if (sambaNameEdit->text().isEmpty()) {
        KMessageBox::sorry(this, i18n("You have to enter a name for the Samba share."));
        sambaNameEdit->setFocus();
        return false;
    }
    
    if (sambaNameEdit->text() != m_sambaShare->getName()) {
      SambaShare* otherShare = m_sambaFile->getShare(sambaNameEdit->text());
      if (otherShare && otherShare != m_sambaShare) {
        // There is another Share with the same name
        KMessageBox::sorry(this, i18n("<qt>There is already a share with the name <strong>%1</strong>.<br> Please choose another name.</qt>").arg(sambaNameEdit->text()));
        sambaNameEdit->selectAll();
        sambaNameEdit->setFocus();
        return false;
      }
      m_sambaShare->setName(sambaNameEdit->text());
      m_sambaChanged = true;
    }
    
    if (m_sambaShare->getValue("path") != m_path) {
      m_sambaShare->setValue("path", m_path);
      m_sambaChanged = true;
    }
          
  } else {
    if (m_sambaShare) {
      kdDebug(FILESHARE_DEBUG) << "PropertiesPage::updateSambaShare: removing share" << endl;
      m_sambaFile->removeShare(m_sambaShare);
      m_sambaShare = 0;
      m_sambaChanged = true;
    }      
  }    
  return true;
}

void PropertiesPage::setSambaShareBoolValue(const QString & value, 
                          QCheckBox* chk) 
{
  bool v = m_sambaShare->getBoolValue(value);
  if (v == chk->isChecked())
      return;
      
  m_sambaShare->setValue(value,chk->isChecked());
  m_sambaChanged = true;      
}

QString PropertiesPage::getNewSambaName() {
  QString path = m_path;
  if (path.isNull() && m_enterUrl) {
    path = urlRq->url();
  }
  
  QString shareName = KURL(path).fileName();
  
  if (!sambaNameEdit->text().isEmpty())
      shareName = sambaNameEdit->text();
  
  // Windows could have problems with longer names
  shareName = shareName.left(12).upper();

  if ( m_sambaFile->getShare(shareName) )
      shareName = m_sambaFile->getUnusedName(shareName);
      
  return shareName;      
}

void PropertiesPage::createNewSambaShare() {

  m_sambaShare = m_sambaFile->newShare(getNewSambaName(),m_path);

  kdDebug(FILESHARE_DEBUG) << "PropertiesPage::createNewSambaShare: " 
              << m_sambaShare->getName() << endl;
  
}


void PropertiesPage::moreSambaBtnClicked() {
  kdDebug(FILESHARE_DEBUG) << "PropertiesPage::moreSambaBtnClicked()" << endl;
  updateSambaShare();
  ShareDlgImpl* dlg = new ShareDlgImpl(this,m_sambaShare);
  dlg->directoryGrp->hide();
  dlg->pixmapFrame->hide();
  if (dlg->exec() == QDialog::Accepted &&
      dlg->hasChanged()) {
      m_sambaChanged = true;
      changedSlot();
      loadSambaShare();
  }
  delete dlg;
}

#include "propertiespage.moc"

