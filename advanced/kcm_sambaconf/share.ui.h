/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

void KcmShareDlg::edit( shareData * data )
{
/*
	shareName->setText(data->name);
	share = data;
	if (data->directory){
		if (data->name == "[homes]"){
			homesChk->setChecked(true);
		}else{
			pathUrlRq->setURL(data->path);
			homesChk->setChecked(false);
		}
	}else{
		if (data->name == "[printers]"){
			printersChk->setChecked(true);
		}else{
			queue->setCurrentItem(data->queue);
			printersChk->setChecked(false);
		}
	}
	directory->setChecked(data->directory);
	browse->setChecked(data->browse);
	read->setChecked(data->read);
	publ->setChecked(data->publ);
	selectUsers->setChecked(data->users);
	enableUserSelection->setChecked(data->enableUserSelection);
*/
}

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


void KcmShareDlg::updateShareData()
{
/*
    bool error = false;
    if (directory->isChecked()){
	share->special = homes->isChecked();
	if (share->special){
	    share->name = "[homes]";
	    share->path = i18n("Home directories");
	}else{
	    share->name = shareName->text();
	    share->path = path->url();
	}
	share->read = read->isChecked();
    }else{
	share->special = printers->isChecked();
	if (share->special){
	    share->name = "[printers]";
	    share->queue = i18n("All printers");
	}else{
	    share->name = shareName->text();
	    share->queue = queue->currentText();
	}
    }
    share->comment = comment->text();
    share->browse = browse->isChecked();
    share->directory = directory->isChecked();
    share->publ = publ->isChecked();
    share->users = selectUsers->isChecked();
    share->enableUserSelection = selectUsers->isChecked();
*/
}



void KcmShareDlg::checkTypes( bool directory )
{
/*
    bool temp = false;
    if (directory){	
	if (!homes->isChecked()){
	    temp = true;
	}
    }else{
	if (!printers->isChecked()){
	    temp = true;
	}
    }
    Lbl_shareName->setEnabled(temp);
    shareName->setEnabled(temp);
//    Lbl_shareName->setDisabled(homes->isChecked()|printers->isChecked());
//    shareName->setDisabled(homes->isChecked()|printers->isChecked());
*/
}


void KcmShareDlg::init()
{
    directoryPixLbl->setPixmap(DesktopIcon("folder"));
    printerPixLbl->setPixmap(DesktopIcon("printer1"));
    
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


void KcmShareDlg::delUsers()
{
/*
    QListBoxItem* item = 0;
    QPtrList<QListBoxItem>* selectedList = new QPtrList<QListBoxItem>;
    selectedList->setAutoDelete(false);
    for (int i=0; i < selected->count(); i++){
	item = selected->item(i);
	if (item->isSelected()){
	    selectedList->append(item);
	}
    }
    possible->clearSelection();
    for (QPtrListIterator<QListBoxItem> it(*selectedList); it.current(); ++it){
	selected->takeItem(it.current());
	possible->insertItem(it.current());
    }
    delete selectedList;
*/
}


void KcmShareDlg::createHomes( bool value )
{
/*
    homesExist = value;
*/
}


void KcmShareDlg::createPrinters( bool value )
{
/*
    printersExist = value;
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


void KcmShareDlg::checkHomesAndPrinters()
{
/*
    bool homesState = false;
    bool printersState = false;
    if (homesExist)
	homesState=true;
    if (printersExist)
	printersState=true;
    homesGroup->setDisabled(homesState);
    printersGroup->setDisabled(printersState);
*/
}
