/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

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



