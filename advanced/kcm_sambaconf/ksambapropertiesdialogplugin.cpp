/***************************************************************************
                          KSambaPropertiesDialogPlugin.cpp  -  description
                            -------------------
    begin                : Son Apr 14 2002
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

#include <qframe.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qvbuttongroup.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qvgroupbox.h>
#include <qfileinfo.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qwidgetstack.h>
#include <qwhatsthis.h>
#include <qtabwidget.h>
#include <qcombobox.h>

#include <kgenericfactory.h>
#include <klocale.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kapp.h>

#include "konqinterface.h"
#include "ksambapropertiesdialogplugin.h"
#include "sambafile.h"
#include "share.h"
#include "sharedlgimpl.h"
#include "passwd.h"
#include "common.h"
#include "smbconfconfigwidget.h"

#include <assert.h>

typedef KGenericFactory<KSambaPropertiesDialogPlugin, KPropertiesDialog> KSambaPropertiesDialogPluginFactory;

K_EXPORT_COMPONENT_FACTORY( ksambakonqiplugin,
                            KSambaPropertiesDialogPluginFactory("ksambakonqiplugin") );




KSambaPropertiesDialogPlugin::KSambaPropertiesDialogPlugin( KPropertiesDialog *dlg,
                    const char *, const QStringList & )
  : KPropsDlgPlugin(dlg)
{

  // Initialize member variables
  m_sambaFile = 0L;
  m_activeShare = 0L;
  m_sambaConf = QString::null;
  m_sharePath = QString::null;

  m_sharePath = getLocalPathFromUrl(properties->kurl());

  if (m_sharePath.isNull()) {
    kdDebug() << "KSambaPropertiesDialogPlugin: URL is no local file !" << endl;
    return;
  }

  initGUI();      
}

KSambaPropertiesDialogPlugin::~KSambaPropertiesDialogPlugin()
{
  if (m_sambaFile)
    delete m_sambaFile;
}

void KSambaPropertiesDialogPlugin::initGUI() {
  QFrame* frame = properties->addPage(i18n("Sam&ba"));
  m_stack = new QWidgetStack(frame);
  QVBoxLayout *stackLayout = new QVBoxLayout(frame);
  stackLayout->addWidget(m_stack);

  m_shareWidget = 0L;
  m_configWidget = 0L;

  if (getSambaConf().isNull()) {
    createConfigWidget(m_stack);
    m_stack->addWidget(m_configWidget,0);
    m_stack->raiseWidget(m_configWidget);
  } 
  else {
    createShareWidget(m_stack);
    m_stack->addWidget(m_shareWidget,0);
    m_stack->raiseWidget(m_shareWidget);
  }
}

QString KSambaPropertiesDialogPlugin::getSambaConf() {
  if (m_sambaConf.isNull()) {
    m_sambaConf = SambaFile::findSambaConf();
  }

  return m_sambaConf;
}

SambaFile* KSambaPropertiesDialogPlugin::getSambaFile() {
  if (m_sambaFile == 0L) {

    if (getSambaConf().isNull())
      return 0L;

    m_sambaFile = new SambaFile( getSambaConf(), false );     

    if ( ! m_sambaFile->openFile()) {
      delete m_sambaFile;
      m_sambaFile = 0L;
      return 0L;
    }   
  }

  return m_sambaFile;
}



SambaShare* KSambaPropertiesDialogPlugin::getGlobalShare() {
  if (getSambaFile() == 0L)
    return 0L;

  return getSambaFile()->getShare("global");     
}


QString KSambaPropertiesDialogPlugin::getSharePath() {
  return m_sharePath;
}

SambaShare* KSambaPropertiesDialogPlugin::getActiveShare() {
  if (m_activeShare == 0L) {

    if (getSambaFile() == 0L)
        return 0L;

    if (getSharePath().isNull())
        return 0L;

    QString shareName = getSambaFile()->findShareByPath(getSharePath());
    if (shareName.isEmpty())
        return 0L;

    m_activeShare = getSambaFile()->getShare(shareName);

  }

  return m_activeShare;
}

QString KSambaPropertiesDialogPlugin::getNetbiosName() {
  if (getGlobalShare() == 0L)
    return 0L;

  QString *s = getGlobalShare()->find("netbios name");

  if (!s) 
    return QString::null;

  return QString(*s);         
}

QString KSambaPropertiesDialogPlugin::getLocalPathFromUrl(const KURL & url) {
  if (url.isLocalFile())
    return url.path();

  if (url.protocol().lower() != "smb")
    return QString::null;

  if (url.host().lower() != "localhost") {
    if (getNetbiosName().isNull())
        return QString::null;

    if (getNetbiosName().lower() != url.host().lower())
        return QString::null;
  }          

  if (getSambaFile() == 0L)
    return QString::null;

  SambaShare* share = getSambaFile()->getShare( url.fileName() ); 

  if (share == 0L)
    return QString::null;

  return share->getValue("path",false);     
}



void KSambaPropertiesDialogPlugin::createConfigWidget(QWidget* parent)
{
  m_configWidget = new SmbConfConfigWidget(parent);
  connect( m_configWidget, SIGNAL(smbConfChoosed(const QString &)), 
           this, SLOT(slotSpecifySmbConf(const QString &)));
}

void KSambaPropertiesDialogPlugin::createShareWidget(QWidget* parent)
{

  m_shareWidget = new KonqInterface(parent);

  if ( getSambaFile() == 0L ) {
        KMessageBox::sorry(0L,i18n("<qt>Couldn't open the file <em>%1</em>.</qt>").arg(getSambaConf()),
                              i18n("Error while opening file"));
    delete m_shareWidget;
    m_shareWidget = 0L;
    return;                              
  }


  QString shareName = getSambaFile()->findShareByPath(getSharePath());


  if (shareName.isEmpty()) {
    m_shareWidget->notSharedRadio->setChecked(true);
    m_shareWidget->baseGrp->setEnabled(false);
    m_shareWidget->securityGrp->setEnabled(false);
    m_shareWidget->otherGrp->setEnabled(false);
    m_shareWidget->moreOptionsBtn->setEnabled(false);
    m_wasShared = false;
  }
  else {
    m_wasShared = true;
    m_activeShare = getSambaFile()->getShare(shareName);
    m_shareWidget->sharedRadio->setChecked(true);
    initValues();
  }

  connect(m_shareWidget->btnGrp, SIGNAL(clicked(int)), this, SLOT(slotSharedChanged(int)));
  connect(m_shareWidget, SIGNAL(changed()), this, SLOT(setDirty()));
  connect( m_shareWidget->moreOptionsBtn, SIGNAL(clicked()),
          this, SLOT(moreOptionsBtnPressed()));

}



void KSambaPropertiesDialogPlugin::initValues()
{
  if (getActiveShare() == 0L) {
    kdWarning() << "KSambaPropertiesDialogPlugin::initValues : getActiveShare == 0L !" << endl;
    return;
  }     

  SambaShare* share = getActiveShare();     

  m_shareWidget->nameEdit->setText( share->getName() );
  m_shareWidget->commentEdit->setText( share->getValue("comment") );
  m_shareWidget->readOnlyChk->setChecked( share->getBoolValue("read only") );
  m_shareWidget->guestOkChk->setChecked( share->getBoolValue("guest ok") );
  m_shareWidget->allowEdit->setText( share->getValue("hosts allow") );
  m_shareWidget->denyEdit->setText( share->getValue("hosts deny") );
  m_shareWidget->browseableChk->setChecked( share->getBoolValue("browseable") );
  m_shareWidget->availableChk->setChecked( share->getBoolValue("available") );
  m_shareWidget->guestAccountCombo->insertStringList( getUnixUsers() );
  setComboToString(m_shareWidget->guestAccountCombo,share->getValue("guest account"));

}

void KSambaPropertiesDialogPlugin::saveValuesToShare() 
{
  if (getActiveShare() == 0L) {
    kdWarning() << "KSambaPropertiesDialogPlugin::saveValuesToShare : getActiveShare == 0L !" << endl;
    return;
  }     

  SambaShare* share = getActiveShare();     

  share->setValue("comment",m_shareWidget->commentEdit->text());
  share->setValue("read only", m_shareWidget->readOnlyChk->isChecked());
  share->setValue("guest ok", m_shareWidget->guestOkChk->isChecked());
  share->setValue("guest account",m_shareWidget->guestAccountCombo->currentText());
  share->setValue("hosts allow", m_shareWidget->allowEdit->text());
  share->setValue("hosts deny", m_shareWidget->denyEdit->text());
  share->setValue("browseable", m_shareWidget->browseableChk->isChecked());
  share->setValue("available", m_shareWidget->availableChk->isChecked());
}

void KSambaPropertiesDialogPlugin::slotSharedChanged(int state)
{
  if (state==1) // Not Shared
  {  
    m_shareWidget->baseGrp->setEnabled(false);
    m_shareWidget->securityGrp->setEnabled(false);
    m_shareWidget->otherGrp->setEnabled(false);
    m_shareWidget->moreOptionsBtn->setEnabled(false);
  }
  else  // shared
  {
    if (!getActiveShare())
    {
      QString shareName = properties->kurl().fileName();

      if ( getSambaFile()->getShare(shareName) == 0L)
          shareName = getSambaFile()->getUnusedName(shareName);

      m_activeShare = getSambaFile()->newShare(shareName,getSharePath());
      initValues();
    }

    m_shareWidget->baseGrp->setEnabled(true);
    m_shareWidget->securityGrp->setEnabled(true);
    m_shareWidget->otherGrp->setEnabled(true);
    m_shareWidget->moreOptionsBtn->setEnabled(true);
  }

  emit changed();
}

/**
* Checks wether the entered values are correct
* @return false if not correct otherwise true
*/
bool KSambaPropertiesDialogPlugin::checkValues()
{
  if (m_shareWidget->nameEdit->text().isEmpty())  {
    KMessageBox::information(properties,i18n("Please enter a name for the shared directory."),i18n("Information"));
    m_shareWidget->nameEdit->setFocus();
    return false;
  } 

  if (m_shareWidget->nameEdit->text().length() > 12) {
    if (KMessageBox::Cancel == KMessageBox::warningContinueCancel(
      properties,i18n(
        "<qt>The name of the share has more than <b>12</b> characters! <br>"
        "This can lead to problems with Microsoft Windows(R) clients. <br>"
        "Do you really want to continue?</qt>")
        ,i18n("Warning")
        ,KStdGuiItem::cont()
        ,"KSambaPlugin_12CharacterWarning"))
    {
      m_shareWidget->nameEdit->setFocus();
      return false;  
    }

  } 

  if (m_shareWidget->nameEdit->text().contains(' ')) {
    if (KMessageBox::Cancel == KMessageBox::warningContinueCancel(
      properties,i18n(
        "<qt>The name of the share contains a <b>space</b> character! <br>"
        "This can lead to problems with Microsoft Windows(R) clients. <br>"
        "Do you really want to continue?</qt>"
        ),i18n("Warning")
        ,KStdGuiItem::cont()
        ,"KSambaPlugin_SpaceCharacterWarning"))
    {
      m_shareWidget->nameEdit->setFocus();
      return false;  
    }
  }

  if ( ! checkIfUnixPermissions( getActiveShare())) 
     return false;
  
  return true;
}

/** No descriptions */
void KSambaPropertiesDialogPlugin::applyChanges()
{
  saveValuesToShare();

  if (getActiveShare() && m_shareWidget->sharedRadio->isChecked())
  {
    if (!checkValues())
    {
      properties->abortApplying();
      return;
    }


    QString shareName = m_shareWidget->nameEdit->text();

    if (! getActiveShare()->setName(shareName))
    {
      KMessageBox::sorry(properties,i18n("The samba share name '%1' already exists!").arg(shareName),i18n("Information"));
      m_shareWidget->nameEdit->setText(getSambaFile()->getUnusedName());
      m_shareWidget->nameEdit->setFocus();
      properties->abortApplying();
      return;
    }

    
  }

  if (m_shareWidget->sharedRadio->isChecked())
    getSambaFile()->slotApply();
  else if (m_wasShared) {
    getSambaFile()->removeShare(getActiveShare());
    getSambaFile()->slotApply();
  }

  return;

}

void KSambaPropertiesDialogPlugin::moreOptionsBtnPressed()
{
  saveValuesToShare();

  ShareDlgImpl *dlg = new ShareDlgImpl(m_shareWidget,getActiveShare());

  // We already have the base settings
  dlg->_tabs->removePage(dlg->baseTab);

  connect( dlg, SIGNAL(changed()), this, SIGNAL(changed()));
  dlg->exec();
//  disconnect( dlg, SIGNAL(changed()), this, SLOT(changedSlot()));
  delete dlg;

  initValues();
}


void KSambaPropertiesDialogPlugin::slotSpecifySmbConf(const QString & smbConf)
{
  m_sambaConf = smbConf;
  createShareWidget(m_stack);
  m_stack->addWidget(m_shareWidget,1);
  m_stack->raiseWidget(m_shareWidget);
}

#include "ksambapropertiesdialogplugin.moc"
