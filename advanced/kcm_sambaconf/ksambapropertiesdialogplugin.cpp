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
  
 	initWidget = createInitWidget(stack);
  configWidget = createConfigWidget(stack);

  stack->addWidget(initWidget,0);
  stack->addWidget(configWidget,1);

  if (smbconf == "")
     stack->raiseWidget(configWidget);
  else
  	 stack->raiseWidget(initWidget);

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

QWidget* KSambaPropertiesDialogPlugin::createInitWidget(QWidget* parent)
{
	QWidget* w = new QWidget(parent);


  QVBoxLayout *layout = new QVBoxLayout(w,5);


  QVButtonGroup *btnGrp = new QVButtonGroup(w);
  btnGrp->setLineWidth(0);
  btnGrp->setMargin(0);

  notSharedRadio = new QRadioButton(i18n("Not shared"),btnGrp);
  sharedRadio = new QRadioButton(i18n("Shared"),btnGrp);

  btnGrp->insert(notSharedRadio,1);
  btnGrp->insert(sharedRadio,2);

  connect(btnGrp, SIGNAL(clicked(int)), this, SLOT(slotSharedChanged(int)));

  layout->addWidget(btnGrp);

  // Base Options

  QString help;

  baseGrpBox = new QVGroupBox(i18n("Base Options"),w);
  QFrame* innerBox = new QFrame(baseGrpBox);
  QGridLayout *gridLayout = new QGridLayout(innerBox,2,2,5,5);

  nameEdit = new QLineEdit(innerBox);
  QLabel* lbl = new QLabel(i18n("&Name"),innerBox);
  lbl->setBuddy(nameEdit);
  gridLayout->addWidget(lbl,0,0);
  gridLayout->addWidget(nameEdit,0,1);
  connect(nameEdit, SIGNAL(textChanged(const QString &)), this, SLOT(setDirty()));

  help = i18n("This is the name of the share");
  
  QWhatsThis::add(nameEdit,help);
  QWhatsThis::add(lbl,help);

  commentEdit = new QLineEdit(innerBox);
  lbl = new QLabel(i18n("&Comment"),innerBox);
  lbl->setBuddy(commentEdit);
  gridLayout->addWidget(lbl,1,0);
  gridLayout->addWidget(commentEdit,1,1);
  connect(commentEdit, SIGNAL(textChanged(const QString &)), this, SLOT(setDirty()));

  help = i18n("This is a text field that is seen next"
  					  " to a share when a client does a queries"
  					  " the server, either via the network"
  					  " neighborhood or via net view to list"
  					  " what shares are available.");
              
	QWhatsThis::add(commentEdit,help);
  QWhatsThis::add(lbl,help);

  layout->addWidget(baseGrpBox);


  // Security Options

  secureGrpBox = new QVGroupBox(i18n("Security Options"),w);
  innerBox = new QFrame(secureGrpBox);
  gridLayout = new QGridLayout(innerBox,5,2,5,5);

  guestAccountEdit = new QLineEdit(innerBox);
  lbl = new QLabel(i18n("&Guest account"),innerBox);
  lbl->setBuddy(guestAccountEdit);
  gridLayout->addWidget(lbl,0,0);
  gridLayout->addWidget(guestAccountEdit,0,1);

  help = i18n("This is a username which will be used for access"
  						" to services which are specified as guest ok."
              " Whatever privileges this user has will be available"
              " to any client connecting to the guest service."
              " Typically this user will exist in the password file,"
              " but will not have a valid login. The user account \"ftp\""
              " is often a good choice for this parameter."
              " If a username is specified in a given service,"
              " the specified username overrides this one.");

	QWhatsThis::add(lbl,help);
  QWhatsThis::add(guestAccountEdit,help);

  readOnlyChk = new QCheckBox(innerBox);
  lbl = new QLabel(i18n("&Read only"),innerBox);
  lbl->setBuddy(readOnlyChk);
  gridLayout->addWidget(lbl,1,0);
  gridLayout->addWidget(readOnlyChk,1,1);
  connect(readOnlyChk, SIGNAL(clicked()), this, SLOT(setDirty()));

	help = i18n("If this parameter is yes, then users"
  					  " of a service may not create or modify"
              " files in the service's directory.");

	QWhatsThis::add(readOnlyChk,help);
  QWhatsThis::add(lbl,help);

  guestOkChk = new QCheckBox(innerBox);
  lbl = new QLabel(i18n("Gue&st ok"),innerBox);
  lbl->setBuddy(guestOkChk);
  gridLayout->addWidget(lbl,2,0);
  gridLayout->addWidget(guestOkChk,2,1);
  connect(guestOkChk, SIGNAL(clicked()), this, SLOT(setDirty()));

	help = i18n("If this parameter is yes for a service,"
 		 		"then no password is required to connect to the service. "
        "Privileges will be those of the guest account.");

  QWhatsThis::add(lbl,help);
  QWhatsThis::add(guestOkChk,help);

  hostsAllowLineEdit = new QLineEdit(innerBox);
  lbl = new QLabel(i18n("&Hosts allow"),innerBox);
  lbl->setBuddy(hostsAllowLineEdit);
  gridLayout->addWidget(lbl,3,0);
  gridLayout->addWidget(hostsAllowLineEdit,3,1);
  connect(hostsAllowLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(setDirty()));

  help = i18n("This parameter is a comma, space, or tab delimited"
  					  " set of hosts which are permitted to access a service.");

	QWhatsThis::add(lbl,help);
  QWhatsThis::add(hostsAllowLineEdit,help);

  hostsDenyLineEdit = new QLineEdit(innerBox);
  lbl = new QLabel(i18n("Hosts &deny"),innerBox);
  lbl->setBuddy(hostsDenyLineEdit);
  gridLayout->addWidget(lbl,4,0);
  gridLayout->addWidget(hostsDenyLineEdit,4,1);
  connect(hostsDenyLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(setDirty()));

  help = i18n("The opposite of hosts allow - hosts listed"
  					  " here are NOT permitted access to services"
  					  " unless the specific services have their own"
  					  " lists to override this one. Where the lists"
  					  " conflict, the allow list takes precedence.");
              
	QWhatsThis::add(lbl,help);
	QWhatsThis::add(hostsDenyLineEdit,help);


  layout->addWidget(secureGrpBox);

  // Browse options

  browseGrpBox = new QVGroupBox(i18n("Other Options"),w);
  innerBox = new QFrame(browseGrpBox);
  gridLayout = new QGridLayout(innerBox,2,2,5,5);

  browseableChk = new QCheckBox(innerBox);
  lbl = new QLabel(i18n("&Browseable"),innerBox);
  lbl->setBuddy(browseableChk);
  gridLayout->addWidget(lbl,0,0);
  gridLayout->addWidget(browseableChk,0,1);
  connect(browseableChk, SIGNAL(clicked()), this, SLOT(setDirty()));

  help = i18n("This controls whether this share is seen in the"
  					  " list of available shares in a net view and in the browse list.");
              
	QWhatsThis::add(lbl,help);
  QWhatsThis::add(browseableChk,help);

  // Browse options

//  miscGrpBox = new QVGroupBox(i18n("Miscellaneous Options"),frame);
//  innerBox = new QFrame(miscGrpBox);
//  gridLayout = new QGridLayout(innerBox,1,2,5,5);

  availableChk = new QCheckBox(innerBox);
  lbl = new QLabel(i18n("A&vailable"),innerBox);
  lbl->setBuddy(availableChk);
  gridLayout->addWidget(lbl,1,0);
  gridLayout->addWidget(availableChk,1,1);
  connect(availableChk, SIGNAL(clicked()), this, SLOT(setDirty()));

  help = i18n("This parameter lets you \"turn off\" a service."
  					  " If available = no, then ALL attempts to connect"
              " to the service will fail. Such failures are logged.");
              
	QWhatsThis::add(lbl,help);
  QWhatsThis::add(availableChk,help);

  layout->addWidget(browseGrpBox);

//  layout->addWidget(miscGrpBox);

	if ( QFileInfo(smbconf).exists() )
	{
    SambaFile sambaFile(smbconf);

    QString share = sambaFile.findShareByPath(sharePath);

    if (share.isEmpty())
    {
      notSharedRadio->setChecked(true);
      baseGrpBox->setEnabled(false);
      secureGrpBox->setEnabled(false);
      browseGrpBox->setEnabled(false);
  //    miscGrpBox->setEnabled(false);
    }
    else
    {
      sharedRadio->setChecked(true);
      initValues(share,sambaFile);
    }
	}
  
  return w;
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

  nameEdit->setText(share);
  commentEdit->setText(sambaFile.readValue(share,"comment"));
  guestAccountEdit->setText(addGlobalText(sambaFile.readValue(share,"guest account"),"guest account", sambaFile));;
  readOnlyChk->setChecked(sambaFile.readBoolValue(share,"read only"));
  guestOkChk->setChecked(sambaFile.readBoolValue(share,"guest ok"));
  hostsAllowLineEdit->setText(addGlobalText(sambaFile.readValue(share,"hosts allow"),"hosts allow", sambaFile));
  hostsDenyLineEdit->setText(addGlobalText(sambaFile.readValue(share,"hosts deny"),"hosts deny", sambaFile));
  browseableChk->setChecked(sambaFile.readBoolValue(share,"browseable"));
  availableChk->setChecked(sambaFile.readBoolValue(share,"available"));
}

void KSambaPropertiesDialogPlugin::slotSharedChanged(int state)
{
  SambaFile sambaFile(smbconf);

  QString share = sambaFile.findShareByPath(sharePath);

  if (state==1)
  {  // Not Shared
    baseGrpBox->setEnabled(false);
    secureGrpBox->setEnabled(false);
    browseGrpBox->setEnabled(false);
//    miscGrpBox->setEnabled(false);
  }
  else  // shared
  {

    if (share.isEmpty())
    {
      share = propDialog->kurl().fileName();
      sambaFile.newShare(share,sharePath);
      initValues(share,sambaFile);
    }

    baseGrpBox->setEnabled(true);
    secureGrpBox->setEnabled(true);
    browseGrpBox->setEnabled(true);
//    miscGrpBox->setEnabled(true);
  }
  
  emit changed();

}


/** No descriptions */
void KSambaPropertiesDialogPlugin::applyChanges()
{
  SambaFile sambaFile(smbconf, false);

  QString share = sambaFile.findShareByPath(sharePath);

  if (share.isEmpty() && sharedRadio->isChecked())
  {
    if (nameEdit->text().isEmpty())
    {
       KMessageBox::information(frame,i18n("Please enter a name for the shared directory."),i18n("Information"));
       nameEdit->setFocus();
       properties->abortApplying();
       return;
    }

    share = nameEdit->text();

    if (!sambaFile.newShare(share,sharePath))
    {
       KMessageBox::sorry(frame,i18n("The samba share name '%1' already exists !").arg(share),i18n("Information"));
       nameEdit->setFocus();
       properties->abortApplying();
       return;
    }
  }

  if (!share.isEmpty() && notSharedRadio->isChecked())
  {
    sambaFile.removeShare(share);
  } 
  if (!share.isEmpty() && share != nameEdit->text())
  {
  	sambaFile.renameShare(share, nameEdit->text());
    share = nameEdit->text();
  }

  if (!share.isEmpty() && sharedRadio->isChecked());
  {
    sambaFile.writeValue(share, "comment", commentEdit->text());
    sambaFile.writeValue(share, "guest account", guestAccountEdit->text());
    sambaFile.writeValue(share, "read only", readOnlyChk->isChecked());
    sambaFile.writeValue(share, "guest ok", guestOkChk->isChecked());
    sambaFile.writeValue(share, "hosts allow", hostsAllowLineEdit->text());
    sambaFile.writeValue(share, "hosts deny", hostsDenyLineEdit->text());
    sambaFile.writeValue(share, "browseable", browseableChk->isChecked());
    sambaFile.writeValue(share, "availabe", availableChk->isChecked());
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

		delete initWidget;
    initWidget = createInitWidget(stack);
    stack->raiseWidget(initWidget);
  }

}



#include "ksambapropertiesdialogplugin.moc"
