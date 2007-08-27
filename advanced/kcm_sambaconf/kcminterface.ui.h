/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

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
 *  along with KSambaPlugin; if not, write to the Free Software Foundation,   *
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA         *
 *                                                                            *
 ******************************************************************************/

#ifndef KCM_SAMBACONF_KCMINTERFACE_UI_H
#define KCM_SAMBACONF_KCMINTERFACE_UI_H

void KcmInterface::init()
{
    addShareBtn->setIconSet(SmallIconSet("document-new"));
    editShareBtn->setIconSet(SmallIconSet("edit"));
    removeShareBtn->setIconSet(SmallIconSet("edit-delete"));
    editDefaultShareBtn->setIconSet(SmallIconSet("queue"));
    
    addPrinterBtn->setIconSet(SmallIconSet("document-new"));
    editPrinterBtn->setIconSet(SmallIconSet("edit"));
    removePrinterBtn->setIconSet(SmallIconSet("edit-delete"));
    editDefaultPrinterBtn->setIconSet(SmallIconSet("print_class"));

    advancedWarningPixLbl->setPixmap(SmallIcon("dialog-warning"));
}


void KcmInterface::changedSlot()
{
    emit changed();
}


void KcmInterface::securityLevelCombo_activated( int i )
{
   passwordServerEdit->setDisabled(i<2);
   allowGuestLoginsChk->setDisabled(i==0);
}


void KcmInterface::shareRadio_clicked()
{
  securityLevelHelpLbl->setText(i18n("Use the <i>share</i> security level if you have a home network "
				     "or a small office network.<br /> It allows everyone to read the list "
		        	     "of all your shared directories and printers before a login is required."));
}


void KcmInterface::userRadio_clicked()
{
  securityLevelHelpLbl->setText(i18n("Use the <i>user</i> security level if you have a bigger network "
				     "and you do not want to allow everyone to read your list of shared "
				     "directories and printers without a login.<br /><br />"
				     "If you want to run your Samba server as a <b>Primary Domain controller</b> (PDC) "
				     "you also have to set this option."));
}

void KcmInterface::serverRadio_clicked()
{
  securityLevelHelpLbl->setText(i18n("Use the <i>server</i> security level if you have a big network "
			             "and the samba server should validate the username/password "
			             "by passing it to another SMB server, such as an NT box."));
}


void KcmInterface::domainRadio_clicked()
{
  securityLevelHelpLbl->setText(i18n("Use the <i>domain</i> security level if you have a big network "
				     "and the samba server should validate the username/password "
				     "by passing it to a Windows NT Primary or Backup Domain Controller."));
}


void KcmInterface::adsRadioClicked()
{
  securityLevelHelpLbl->setText(i18n("Use the <i>ADS</i> security level if you have a big network "
				     "and the samba server should act as a domain member in an ADS realm."));
}


void KcmInterface::KUrlLabel1_leftClickedURL()
{
    K3Process* p = new K3Process();

    *p << "konqueror";
    *p << "man:smb.conf";
    p->start();
}


void KcmInterface::lmAnnounceCombo_activated( int i)
{
    lmIntervalSpin->setEnabled(i==0);
}


void KcmInterface::allowGuestLoginsChk_toggled( bool b)
{
    int i = 0;
    if (b)
	i = 1;

    mapToGuestCombo->setCurrentItem(i);
}


void KcmInterface::mapToGuestCombo_activated( int i)
{
    allowGuestLoginsChk->setChecked(i>0);
}

#endif // KCM_SAMBACONF_KCMINTERFACE_UI_H
