/***************************************************************************
                          KSambaPropertiesDialogPlugin.cpp  -  description
                             -------------------
    begin                : Son Apr 14 2002
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


typedef KGenericFactory<KSambaPropertiesDialogPlugin, KPropertiesDialog> KSambaPropertiesDialogPluginFactory;

K_EXPORT_COMPONENT_FACTORY( ksambakonqiplugin,
                            KSambaPropertiesDialogPluginFactory("ksambakonqiplugin") );




KSambaPropertiesDialogPlugin::KSambaPropertiesDialogPlugin( KPropertiesDialog *dlg,
  							    const char *,
                                                            const QStringList & )
  : KPropsDlgPlugin(dlg)
{

  propDialog = dlg;

  smbconf = SambaFile::findSambaConf();
	sharePath = propDialog->kurl().path();
  

  // Only accept local files
  if (!propDialog->kurl().isLocalFile())
  {
  	// Except smb files
  	if (propDialog->kurl().protocol().lower()=="smb")
    {

    	SambaFile sambaFile(smbconf);
      SambaShare* globals = sambaFile.getShare("global");

      QString *s = globals->find("netbios name");
      if (!s)
				 return;
         
      // If we are on a smb directory of our host get it.
     	if (propDialog->kurl().host().lower()!=s->lower())
      	 return;

      _share = sambaFile.getShare( propDialog->kurl().fileName() );

      QString path = _share->getValue("path",false);

      if (path.isEmpty())
      	 return;

      sharePath = path;
    }
    else
  		 return;
  } 

  frame = propDialog->addPage(i18n("Sam&ba"));

	stack = new QWidgetStack(frame);

  QVBoxLayout *stackLayout = new QVBoxLayout(frame);
  stackLayout->addWidget(stack);
  
 	shareWidget = createShareWidget(stack);
  configWidget = createConfigWidget(stack);

  stack->addWidget(shareWidget,0);
  stack->addWidget(configWidget,1);

  if (smbconf == "")
     stack->raiseWidget(configWidget);
  else
  	 stack->raiseWidget(shareWidget);

}


QWidget* KSambaPropertiesDialogPlugin::createConfigWidget(QWidget* parent)
{

	QWidget* w = new QWidget(parent,"configWidget");

  QVBoxLayout *layout = new QVBoxLayout(w,5);

  QLabel *lbl = new QLabel(i18n("<p>The SAMBA configuration file <strong>'smb.conf'</strong>" \
  															" could not be found!</p>" \
                   						  "Make sure you have SAMBA installed.\n\n"), w);

	QHBoxLayout *hbox = new QHBoxLayout(w);
	QPushButton *btn = new QPushButton(i18n("Specify location"), w);
  connect(btn, SIGNAL(pressed()), this, SLOT( slotSpecifySmbConf()));
 	
  btn->setDefault(false);
  btn->setAutoDefault(false);

  hbox->addStretch();
  hbox->addWidget(btn);

	layout->addWidget(lbl);
	layout->addLayout(hbox);
  layout->addStretch();

  return w;

}

KonqInterface* KSambaPropertiesDialogPlugin::createShareWidget(QWidget* parent)
{

  shareWidget = new KonqInterface(parent);

  connect(shareWidget->btnGrp, SIGNAL(clicked(int)), this, SLOT(slotSharedChanged(int)));
  connect(shareWidget, SIGNAL(changed()), this, SLOT(setDirty()));

	if ( QFileInfo(smbconf).exists() )
	{
    SambaFile sambaFile(smbconf);

    QString share = sambaFile.findShareByPath(sharePath);

    if (share.isEmpty())
    {
      shareWidget->notSharedRadio->setChecked(true);
      shareWidget->baseGrp->setEnabled(false);
      shareWidget->securityGrp->setEnabled(false);
      shareWidget->otherGrp->setEnabled(false);
    }
    else
    {
      shareWidget->sharedRadio->setChecked(true);
      initValues(share,sambaFile);
    }
	}
  
  return shareWidget;
}

KSambaPropertiesDialogPlugin::~KSambaPropertiesDialogPlugin()
{
}



void KSambaPropertiesDialogPlugin::initValues(const QString & share, SambaFile & sambaFile)
{
	_share = sambaFile.getShare(share);

  shareWidget->nameEdit->setText( _share->getName() );
  shareWidget->commentEdit->setText( _share->getValue("comment") );
  shareWidget->readOnlyChk->setChecked( _share->getBoolValue("read only") );
  shareWidget->guestOkChk->setChecked( _share->getBoolValue("guest ok") );
  shareWidget->guestEdit->setText( _share->getValue("guest account") );
  shareWidget->allowEdit->setText( _share->getValue("hosts allow") );
  shareWidget->denyEdit->setText( _share->getValue("hosts deny") );
  shareWidget->browseableChk->setChecked( _share->getBoolValue("browseable") );
  shareWidget->availableChk->setChecked( _share->getBoolValue("available") );

  connect( shareWidget->moreOptionsBtn, SIGNAL(pressed()),
  				 this, SLOT(moreOptionsBtnPressed()));

}

void KSambaPropertiesDialogPlugin::slotSharedChanged(int state)
{
  SambaFile sambaFile(smbconf);

  QString share = sambaFile.findShareByPath(sharePath);

  if (state==1)
  {  // Not Shared
    shareWidget->baseGrp->setEnabled(false);
    shareWidget->securityGrp->setEnabled(false);
    shareWidget->otherGrp->setEnabled(false);
  }
  else  // shared
  {

    if (share.isEmpty())
    {
      share = propDialog->kurl().fileName();
      sambaFile.newShare(share,sharePath);
      initValues(share,sambaFile);
    }

    shareWidget->baseGrp->setEnabled(true);
    shareWidget->securityGrp->setEnabled(true);
    shareWidget->otherGrp->setEnabled(true);
  }
  
  emit changed();

}


/** No descriptions */
void KSambaPropertiesDialogPlugin::applyChanges()
{
  SambaFile sambaFile(smbconf, false);

  QString share = sambaFile.findShareByPath(sharePath);

  if (share.isEmpty() && shareWidget->sharedRadio->isChecked())
  {
    if (shareWidget->nameEdit->text().isEmpty())
    {
       KMessageBox::information(frame,i18n("Please enter a name for the shared directory."),i18n("Information"));
       shareWidget->nameEdit->setFocus();
       properties->abortApplying();
       return;
    }

    share = shareWidget->nameEdit->text();

    if (!sambaFile.newShare(share,sharePath))
    {
       KMessageBox::sorry(frame,i18n("The samba share name '%1' already exists !").arg(share),i18n("Information"));
       shareWidget->nameEdit->setFocus();
       properties->abortApplying();
       return;
    }
  }

  if (!share.isEmpty() && shareWidget->notSharedRadio->isChecked())
  {
    sambaFile.removeShare(_share);
    _share = 0L;
  } 
  if (!share.isEmpty() && share != shareWidget->nameEdit->text())
  {
  	_share->setName( shareWidget->nameEdit->text() );
    share = shareWidget->nameEdit->text();
  }

  if (!share.isEmpty() && shareWidget->sharedRadio->isChecked());
  {
  	_share->setValue("comment",shareWidget->commentEdit->text());
    _share->setValue("read only", shareWidget->readOnlyChk->isChecked());
    _share->setValue("guest ok", shareWidget->guestOkChk->isChecked());
    _share->setValue("guest account", shareWidget->guestEdit->text());
    _share->setValue("hosts allow", shareWidget->allowEdit->text());
    _share->setValue("hosts deny", shareWidget->denyEdit->text());
    _share->setValue("browseable", shareWidget->browseableChk->isChecked());
    _share->setValue("availabe", shareWidget->availableChk->isChecked());
  }


  sambaFile.slotApply();

  return;

}

void KSambaPropertiesDialogPlugin::moreOptionsBtnPressed()
{
	ShareDlgImpl *dlg = new ShareDlgImpl(shareWidget,_share);

  // We already have the base settings
  dlg->tabs->removePage(dlg->baseTab);
  dlg->exec();

}


void KSambaPropertiesDialogPlugin::slotSpecifySmbConf()
{

	smbconf = KFileDialog::getOpenFileName("/",
   					"smb.conf|Samba conf. file\n"
        		"*|All files",0,i18n("Get smb.conf location"));

	if ( QFileInfo(smbconf).exists() )
  {
  	kapp->config()->setGroup("KSambaKonqiPlugin");
    kapp->config()->writeEntry("smb.conf",smbconf);
		kapp->config()->sync();

		delete shareWidget;
    shareWidget = createShareWidget(stack);
    stack->raiseWidget(shareWidget);
  }

}



#include "ksambapropertiesdialogplugin.moc"
