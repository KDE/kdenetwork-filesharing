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

#include <assert.h>

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
  _sambaFile = 0L;
  _share = 0L;


  // Only accept local files
  if (smbconf != "" && !propDialog->kurl().isLocalFile())
  {
  	// Except smb files
  	if (propDialog->kurl().protocol().lower()=="smb")
    {

      _sambaFile = new SambaFile(smbconf,false);
      SambaShare* globals = _sambaFile->getShare("global");

      QString *s = globals->find("netbios name");
      if (!s)
				 return;
         
      // If we are on a smb directory of our host get it.
     	if (propDialog->kurl().host().lower()!=s->lower())
      	 return;

      _share = _sambaFile->getShare( propDialog->kurl().fileName() );

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
    if (!_sambaFile)
       _sambaFile = new SambaFile(smbconf,false);

    QString shareName = _sambaFile->findShareByPath(sharePath);


    if (shareName.isEmpty())
    {
      shareWidget->notSharedRadio->setChecked(true);
      shareWidget->baseGrp->setEnabled(false);
      shareWidget->securityGrp->setEnabled(false);
      shareWidget->otherGrp->setEnabled(false);
    }
    else
    {
      _share = _sambaFile->getShare(shareName);
      shareWidget->sharedRadio->setChecked(true);
      initValues();
    }
	}
  
  return shareWidget;
}

KSambaPropertiesDialogPlugin::~KSambaPropertiesDialogPlugin()
{
  if (_sambaFile)
     delete _sambaFile;
}



void KSambaPropertiesDialogPlugin::initValues()
{
  assert(_share);

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
  if (state==1)
  {  // Not Shared
    shareWidget->baseGrp->setEnabled(false);
    shareWidget->securityGrp->setEnabled(false);
    shareWidget->otherGrp->setEnabled(false);
		
		if (_share)
			_sambaFile->removeShare(_share);
			
		_share = 0L;
  }
  else  // shared
  {
    if (!_share)
    {
      QString shareName = propDialog->kurl().fileName();

      if (_sambaFile->getShare(shareName))
          shareName = _sambaFile->getUnusedName();

      _share = _sambaFile->newShare(shareName,sharePath);
      initValues();
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
  if (_share && shareWidget->sharedRadio->isChecked())
  {
    if (shareWidget->nameEdit->text().isEmpty())
    {
       KMessageBox::information(frame,i18n("Please enter a name for the shared directory."),i18n("Information"));
       shareWidget->nameEdit->setFocus();
       properties->abortApplying();
       return;
    }

    QString shareName = shareWidget->nameEdit->text();

    if (!_share->setName(shareName))
    {
       KMessageBox::sorry(frame,i18n("The samba share name '%1' already exists !").arg(shareName),i18n("Information"));
       shareWidget->nameEdit->setText(_sambaFile->getUnusedName());
       shareWidget->nameEdit->setFocus();
       properties->abortApplying();
       return;
    }

  	_share->setValue("comment",shareWidget->commentEdit->text());
    _share->setValue("read only", shareWidget->readOnlyChk->isChecked());
    _share->setValue("guest ok", shareWidget->guestOkChk->isChecked());
    _share->setValue("guest account", shareWidget->guestEdit->text());
    _share->setValue("hosts allow", shareWidget->allowEdit->text());
    _share->setValue("hosts deny", shareWidget->denyEdit->text());
    _share->setValue("browseable", shareWidget->browseableChk->isChecked());
    _share->setValue("available", shareWidget->availableChk->isChecked());

  }

  _sambaFile->slotApply();

  return;

}

void KSambaPropertiesDialogPlugin::moreOptionsBtnPressed()
{
	ShareDlgImpl *dlg = new ShareDlgImpl(shareWidget,_share);

  // We already have the base settings
  dlg->_tabs->removePage(dlg->baseTab);
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
