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


typedef KGenericFactory<KSambaPropertiesDialogPlugin, KPropertiesDialog> KSambaPropertiesDialogPluginFactory;

K_EXPORT_COMPONENT_FACTORY( ksambakonqiplugin,
                            KSambaPropertiesDialogPluginFactory("ksambakonqiplugin") );




KSambaPropertiesDialogPlugin::KSambaPropertiesDialogPlugin( KPropertiesDialog *dlg,
  							    const char *,
                                                            const QStringList & )
  : KPropsDlgPlugin(dlg)
{

  propDialog = dlg;

  smbconf = findSambaConf();
	sharePath = propDialog->kurl().path();

  // Only accept local files
  if (!propDialog->kurl().isLocalFile())
  {
  	// Except smb files
  	if (propDialog->kurl().protocol().lower()=="smb")
    {

    	SambaFile sambaFile(smbconf);
      SambaShare globals = sambaFile.getShare("global");

      QString *s = globals.find("netbios name");
      if (!s)
				 return;
         
      // If we are on a smb directory of our host get it.
     	if (propDialog->kurl().host().lower()!=s->lower())
      	 return;
         
      SambaShare share = sambaFile.getShare( propDialog->kurl().fileName() );
      
      QString *path = share.find("path");
      
      if (!path)
      	 return;

      sharePath = QString(*path);
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

QString KSambaPropertiesDialogPlugin::findSambaConf() const
{
	// Perhaps the user has defined the path
	kapp->config()->setGroup("KSambaKonqiPlugin");
  QString sambaConf = kapp->config()->readEntry("smb.conf");

  if ( QFileInfo(sambaConf).exists() )
     return sambaConf;

  if ( QFileInfo("/etc/samba/smb.conf").exists() )
     return "/etc/samba/smb.conf";

  if ( QFileInfo("/etc/smb.conf").exists() )
     return "/etc/smb.conf";
  
  if ( QFileInfo("/usr/local/samba/lib/smb.conf").exists() )
     return "/usr/local/samba/lib/smb.conf";

  if ( QFileInfo("/usr/samba/lib/smb.conf").exists() )
     return "/usr/samba/lib/smb.conf";


  return "";
}


void KSambaPropertiesDialogPlugin::initValues(const QString & share, SambaFile & sambaFile)
{
  shareWidget->nameEdit->setText(share);
  shareWidget->commentEdit->setText(sambaFile.readValue(share,"comment"));
  shareWidget->readOnlyChk->setChecked(sambaFile.readBoolValue(share,"read only"));
  shareWidget->guestOkChk->setChecked(sambaFile.readBoolValue(share,"guest ok"));
  shareWidget->guestEdit->setText(addGlobalText(sambaFile.readValue(share,"guest account"),"guest account", sambaFile));;
  shareWidget->allowEdit->setText(addGlobalText(sambaFile.readValue(share,"hosts allow"),"hosts allow", sambaFile));
  shareWidget->denyEdit->setText(addGlobalText(sambaFile.readValue(share,"hosts deny"),"hosts deny", sambaFile));
  shareWidget->browseableChk->setChecked(sambaFile.readBoolValue(share,"browseable"));
  shareWidget->availableChk->setChecked(sambaFile.readBoolValue(share,"available"));
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
    sambaFile.removeShare(share);
  } 
  if (!share.isEmpty() && share != shareWidget->nameEdit->text())
  {
  	sambaFile.renameShare(share, shareWidget->nameEdit->text());
    share = shareWidget->nameEdit->text();
  }

  if (!share.isEmpty() && shareWidget->sharedRadio->isChecked());
  {
    sambaFile.writeValue(share, "comment", shareWidget->commentEdit->text());
    sambaFile.writeValue(share, "read only", shareWidget->readOnlyChk->isChecked());
    sambaFile.writeValue(share, "guest ok", shareWidget->guestOkChk->isChecked());
    sambaFile.writeValue(share, "guest account", shareWidget->guestEdit->text());
    sambaFile.writeValue(share, "hosts allow", shareWidget->allowEdit->text());
    sambaFile.writeValue(share, "hosts deny", shareWidget->denyEdit->text());
    sambaFile.writeValue(share, "browseable", shareWidget->browseableChk->isChecked());
    sambaFile.writeValue(share, "availabe", shareWidget->availableChk->isChecked());
  }


  sambaFile.slotApply();

  return;

}



/** No descriptions */
QString KSambaPropertiesDialogPlugin::addGlobalText(const QString & value, const QString & option, const SambaFile & sambaFile)
{
  if (!value.isEmpty())
      return value;

  SambaShare globals = sambaFile.getShare("global");

  QString *s = globals.find(option);

  if (s)
  {
    return QString(*s);
  }


  return QString("");


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
