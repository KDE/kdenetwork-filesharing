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

#include <kdebug.h>
#include <kdialog.h>
#include <kgenericfactory.h>
#include <klistview.h>
#include <kiconloader.h>
#include <knfsshare.h>
#include <ksambashare.h>
#include <kstandarddirs.h>
#include <ksimpleconfig.h>
#include <kmessagebox.h>
#include <kapp.h>

#include "controlcenter.h"
#include "fileshare.h"

typedef KGenericFactory<KFileShareConfig, QWidget > ShareFactory;
K_EXPORT_COMPONENT_FACTORY (kcm_fileshare, ShareFactory("kcmfileshare") )


#define FILESHARECONF "/etc/security/fileshare.conf"

KFileShareConfig::KFileShareConfig(QWidget *parent, const char *name, const QStringList &):
    KCModule(ShareFactory::instance(), parent, name)
{
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
   

   QString path = QString::fromLatin1("/usr/sbin");
   QString sambaExec = KStandardDirs::findExe( QString::fromLatin1("smbd"), path );
   QString nfsExec = KStandardDirs::findExe( QString::fromLatin1("rpc.nfsd"), path );

   
  if ( nfsExec.isEmpty() && sambaExec.isEmpty())
  {
      m_ccgui->setEnabled( false );
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
  
  if(getuid() == 0)
  {
      setButtons(Help|Apply);
  }
  else
  {
      setButtons(Help);
      m_ccgui->shareGrp->setEnabled( false );
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


void KFileShareConfig::load()
{
    KSimpleConfig config(QString::fromLatin1(FILESHARECONF),true);
    bool restrict = config.readEntry("RESTRICT", "yes") == "yes";
    m_ccgui->shareGrp->setChecked( ! restrict );
    
    if (config.readEntry("SHARINGMODE", "simple") == "simple")
        m_ccgui->simpleRadio->setChecked(true);
    else        
        m_ccgui->advancedRadio->setChecked(true);
          

    m_ccgui->sambaChk->setChecked( 
          config.readEntry("SAMBA", "yes") == "yes");

    m_ccgui->nfsChk->setChecked( 
          config.readEntry("NFS", "yes") == "yes");
          
}

void KFileShareConfig::save()
{
    QDir dir("/etc/security");
    if ( !dir.exists())
        dir.mkdir("/etc/security");

    QFile file(FILESHARECONF);
    if ( ! file.open(IO_WriteOnly)) {
        KMessageBox::detailedError(this, 
            i18n("Could not save settings!"),
            i18n("Could not open file '%1' for writing: %2").arg(FILESHARECONF).arg(
             file.errorString() ),
            i18n("Saving failed"));
        return;
    }        
        
    QTextStream stream(&file);
    stream << "RESTRICT=";
    stream << (m_ccgui->shareGrp->isChecked() ? "no" : "yes");        
        
    stream << "\nSHARINGMODE=";
    stream << (m_ccgui->simpleRadio->isChecked() ? "simple" : "advanced");        

    stream << "\nSAMBA=";
    stream << (m_ccgui->sambaChk->isChecked() ? "yes" : "no");        
        
    stream << "\nNFS=";
    stream << (m_ccgui->nfsChk->isChecked() ? "yes" : "no");        
                
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

#include "fileshare.moc"
