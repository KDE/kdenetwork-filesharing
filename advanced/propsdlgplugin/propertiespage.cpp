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

#include <kfileshare.h>
#include <knfsshare.h>
#include <ksambashare.h>
#include <klocale.h>
#include <kurl.h>
#include <kdebug.h>
#include <kfile.h>
#include <kurlrequester.h>

#include "../nfs/nfsfile.h"
#include "../nfs/nfsentry.h"
#include "../nfs/nfsdialog.h"

#include "propertiespage.h"

PropertiesPage::PropertiesPage(QWidget* parent, KFileItemList items,bool enterUrl) 
  : PropertiesPageGUI(parent), m_items(items), m_nfsFile(0), m_nfsEntry(0),
  m_enterUrl(enterUrl)
{
  if (m_enterUrl) {
    shareChk->hide();
    urlRq->setMode(KFile::Directory |
                   KFile::ExistingOnly |
                   KFile::LocalOnly );
  } else {
    urlRq->hide();
    folderLbl->hide();
  }
  
  if (m_items.isEmpty()) {
    shareFrame->setDisabled(true);
  } else {
    shareFrame->setEnabled(true);
    // currently only one dir is allowed
    m_path = m_items.first()->url().path(1);
  }
  
  
  bool nfsShared = KNFSShare::instance()->isDirectoryShared(m_path);
  bool sambaShared = KSambaShare::instance()->isDirectoryShared(m_path);

  nfsChk->setChecked(nfsShared);
  sambaChk->setChecked(sambaShared);
    
  disableSamba(i18n("Not implemented yet"));
  
//TODO  
//+  if (!KFileShare::sambaEnabled()) {
//+      disableSamba(i18n("The administrator does not allow sharing with Samba"));
//+  }      
  
  if (!KFileShare::nfsEnabled()) 
      disableNFS(i18n("The administrator does not allow sharing with Samba"));  
 
  if (!m_enterUrl)      
      shareChk->setChecked(nfsShared || sambaShared);
  
  m_nfsFile = new NFSFile(KNFSShare::instance()->exportsPath());
  
  load();
}  

PropertiesPage::~PropertiesPage() 
{
  delete m_nfsFile;
}

void PropertiesPage::disableNFS(const QString & message) {
   nfsChk->setDisabled(true);
   nfsGrp->setDisabled(true);
   QToolTip::add(nfsChk,message);
   QToolTip::add(nfsGrp,message);
}

void PropertiesPage::disableSamba(const QString & message) {
   sambaChk->setDisabled(true);
   sambaGrp->setDisabled(true);
   QToolTip::add(sambaChk,message);
   QToolTip::add(sambaGrp,message);
}


bool PropertiesPage::save() {
  bool nfsResult = saveNFS();
  return nfsResult;
}

bool PropertiesPage::load() {
  bool nfsResult = loadNFS();
  return nfsResult;
}

bool PropertiesPage::loadNFS() {
  if (!m_nfsFile->load()) {
      return false;
  }      
      
  m_nfsEntry = m_nfsFile->getEntryByPath(m_path);
  
  if (!m_nfsEntry) {
    nfsChk->setChecked(false);
    return true;
  }
  
  loadNFSEntry();
  return true; 
}  

void PropertiesPage::loadNFSEntry() {
  NFSHost* publicHost = m_nfsEntry->getPublicHost();

  if (publicHost) {
    publicNFSChk->setChecked(true);
    writableNFSChk->setChecked(!publicHost->readonly);
  } else
    publicNFSChk->setChecked(false);
}

bool PropertiesPage::saveNFS() {
  updateNFSEntry();
  return m_nfsFile->save();    
}

bool PropertiesPage::updateNFSEntry() {
  bool changed = false;
  if (shareChk->isChecked() && 
      nfsChk->isChecked()) 
  {
    if (!m_nfsEntry) {
        m_nfsEntry = new NFSEntry(m_path);
        m_nfsFile->addEntry(m_nfsEntry);
        changed = true;
    }
  
    NFSHost* publicHost = m_nfsEntry->getPublicHost();        
    
    if (publicNFSChk->isChecked()) {
      if (!publicHost) {
          publicHost = new NFSHost("*");
          publicHost->allSquash=true;
          m_nfsEntry->addHost(publicHost);
          changed = true;
      } 
      
      if (publicHost->readonly != !writableNFSChk->isChecked()) {
          publicHost->readonly  = !writableNFSChk->isChecked();
          changed = true;
      }          
    } else {
      if (publicHost) {
          m_nfsEntry->removeHost(publicHost);
          changed = true;
      }
    }
  } else { // unshare
    if (m_nfsEntry) {
      m_nfsFile->removeEntry(m_nfsEntry);
      m_nfsEntry = 0;
      changed = true;
    } 
  }

  return changed;      
}

void PropertiesPage::moreNFSBtn_clicked() {
  updateNFSEntry();
  NFSDialog* dlg = new NFSDialog(this,m_nfsEntry);
  if (dlg->exec()==QDialog::Accepted &&
      dlg->modified()) 
  {
    kdDebug() << "NFSDialog::ok" << endl;
    loadNFSEntry();
    emit changed();    
  }
}

void PropertiesPage::urlRqTextChanged( const QString & s) {
  QFileInfo info(s);
  if (info.exists() && info.isDir()) {
    shareFrame->setEnabled(true);
    m_path = s;
  }    
}

#include "propertiespage.moc"

