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
 *  along with KSambaPlugin; if not, write to the Free Software                     *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA  *
 *                                                                            *
 ******************************************************************************/

void KcmShareDlg::checkValues()
{
/*
	bool state = true;
	// Check if the ok-button should be enabled 
	if (directory->isChecked()){
	    if (homes->isChecked()){
		state = true;
	    }else{
		if (shareName->text() == "")
		    state = false;
		if (path->url() == "")
		    state = false;
	    }
	}else{
	    if (printers->isChecked()){
		state = true;
	    }else{
		if (shareName->text() == ""){
		    state = false;
		}
	    }
	}
	buttonOk->setEnabled(state);
*/
}






void KcmShareDlg::init()
{
    directoryPixLbl->setPixmap(DesktopIcon("folder"));
    PixmapLabel1->setPixmap(SmallIcon("messagebox_warning"));
/*
    QListBoxItem* item = 0;
    QPtrList<QListBoxItem>* selectedList = new QPtrList<QListBoxItem>;
    selectedList->setAutoDelete(false);
    for (int i=0; i < possible->count(); i++){
	item = possible->item(i);
	if (item->isSelected()){
	    selectedList->append(item);
	}
    }
    selected->clearSelection();
    for (QPtrListIterator<QListBoxItem> it(*selectedList); it.current(); ++it){
	possible->takeItem(it.current());
	selected->insertItem(it.current());
    }
    delete selectedList;
*/
}




void KcmShareDlg::trytoAccept()
{
/*
    bool error = false;
    if (directory->isChecked()){
	if (!homes->isChecked()){
	    if (shareName->text() == "[homes]"){
		KMessageBox::sorry(this, i18n("Sorry, but you can't create a share named \"[homes]\".\nIf you want to share your home-directorys, please click on \"Share homes\" on the \"Base settings\"-tab."));
		error = true;
	    }
	}
    }else{
	if (!printers->isChecked()){
	    if (shareName->text() == "[printers]"){
		KMessageBox::sorry(this, i18n("Sorry, but you can't create a share named \"[printers]\".\nIf you want to share all your printers, please click on \"Share all printers\" on the \"Base settings\"-tab."));
		error = true;
	    }
	}
    }
    if (!error){
	updateShareData();
	accept();
    }
*/
}





void KcmShareDlg::homeChkToggled( bool )
{

}

void KcmShareDlg::addAllowedUserBtnClicked()
{

}

void KcmShareDlg::removeAllowedUserBtnClicked()
{

}

void KcmShareDlg::guestOnlyChk_toggled( bool b)
{
  if (b)
  {
    onlyUserChk->setChecked(false);
    publicBaseChk->setChecked(true);
  }
    
  onlyUserChk->setDisabled(b);
  publicBaseChk->setDisabled(b);
}


void KcmShareDlg::userOnlyChk_toggled( bool b)
{
  if (b)
  {
    guestOnlyChk->setChecked(false);
    publicBaseChk->setChecked(false);
  }
    
  guestOnlyChk->setDisabled(b);
  publicBaseChk->setDisabled(b);
}

void KcmShareDlg::accessModifierBtnClicked()
{
  
    
}


void KcmShareDlg::changedSlot()
{

}


void KcmShareDlg::publicBaseChk_toggled( bool b)
{
    guestOnlyChk->setEnabled(b);
    if (!b) {
       guestOnlyChk->setChecked(false);
   }
    guestAccountCombo->setEnabled(b);
    guestAccountLbl->setEnabled(b);
    
}


void KcmShareDlg::pathUrlRq_textChanged( const QString & )
{

}


void KcmShareDlg::oplocksChk_toggled( bool b)
{
    if (b) 
	fakeOplocksChk->setChecked(false);
}


void KcmShareDlg::lockingChk_toggled( bool b)
{
  // Its Dangerous to disable locking !
/*	
    if (!b)  {
	enableLockingWarnPix->setPixmap(SmallIcon("messagebox_warning"));    
	enableLockingWarnPix->show();
    } else {
	enableLockingWarnPix->hide();
    }
*/    

}


void KcmShareDlg::fakeOplocksChk_toggled( bool b)
{
/*
    if (b)  {
	fakeOplocksWarnPix->setPixmap(SmallIcon("messagebox_info"));    
	fakeOplocksWarnPix->setText(i18n("Better use the real oplocks support than this parameter"));
	fakeOplocksWarnPix->showMaximized();
	fakeOplocksWarnPix->show();
    } else {
	fakeOplocksWarnPix->hide();
    }
*/
}


void KcmShareDlg::oplockContentionLimitSpin_valueChanged( int i)
{
/*    
    oplockContentionLimitWarnPix->setMaximumWidth(32767);
    oplockContentionLimitWarnPix->setPixmap(SmallIcon("messagebox_critical"));
*/   
    //oplockContentionLimitWarnPix->show();
    
}


void KcmShareDlg::storeDosAttributesChk_toggled( bool b)
{
    mapArchiveChk->setDisabled(b);
    mapSystemChk->setDisabled(b);
    mapHiddenChk->setDisabled(b);
  
    if (b) {
	mapArchiveChk->setChecked(false);
	mapSystemChk->setChecked(false);
	mapHiddenChk->setChecked(false);
    }  
}


void KcmShareDlg::buttonHelp_clicked()
{
    KProcess* p = new KProcess();
    *p << "konqueror";
    *p << "man:smb.conf";
    p->start();
}


