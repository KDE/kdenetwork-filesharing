/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

void KcmInterface::updateShareData(QListViewItem* item)
{
/*
    shareData* current, * last;
    last = shares.last();
    for (current = shares.first(); current; current = shares.next()){
	if (current->listItem->depth() != 0){
	    if (current->listItem == item){
		currentShareData = current;
		return;
	    }
	}else{
	    if (current->listItem == item->parent()){
		currentShareData = current;
		return;
	    }
	}
    }
*/
}

void KcmInterface::editShare()
{
/*
	unsigned short int page = 0;
	// Finally emit editShare.
	emit editShare(currentShareData);
	
	// Dis-/Enable the homes-/printers-selection
	shareDialog->checkHomesAndPrinters();
	// Set the focus to the selected item
	if (shareList->selectedItem()){
	    if (shareList->selectedItem()->text(0) == i18n("Base settings"))
		shareDialog->base->setFocus();
	    if (shareList->selectedItem()->text(0) == i18n("Type"))
		shareDialog->base->setFocus();
	    if (shareList->selectedItem()->text(0) == i18n("Path"))
		shareDialog->path->setFocus();
	    if (shareList->selectedItem()->text(0) == i18n("Comment"))
		shareDialog->comment->setFocus();
	    if (shareList->selectedItem()->text(0) == i18n("Browseable"))
		shareDialog->browse->setFocus();
	    if (shareList->selectedItem()->text(0) == i18n("Read Only"))
		shareDialog->read->setFocus();
	    if (shareList->selectedItem()->text(0) == i18n("Public"))
		shareDialog->publ->setFocus();
	    // Check the items adjusted on page 1
	    if (shareList->selectedItem()->text(0) == i18n("User settings")){
		shareDialog->user->setFocus();
		page = 1;
	    }
	}
	shareDialog->tabs->setCurrentPage(page);
	if (shareDialog->exec() == QDialog::Accepted){
	    rebuild();
	}else{
	    if (currentShareData->name == ""){
		shares.remove(currentShareData);
		// Temporary. shares should be set to auto-deletion
		delete currentShareData;
	    }
	}
*/
}


void KcmInterface::rebuild()
{
/*
    QListViewItem* item = currentShareData->listItem;
    if (item){
	delete item;
    }
    item = new QListViewItem(shareList);
    currentShareData->listItem = item;
    QListViewItem* item2;
    item->setText(0, currentShareData->name);
    item = new QListViewItem(item);
    item->setText(0, i18n("Base settings"));
    item2 = new QListViewItem(item);
    item2->setText(0, i18n("Type"));
    if (currentShareData->directory)
	item2->setText(1, i18n("Directory"));
    else
	item2->setText(1, i18n("Printer"));
    item2 = new QListViewItem(item);
    item2->setText(0, i18n("Comment"));
    if (currentShareData->comment == "")
	item2->setText(1, "<" + i18n("none") + ">");
    else
	item2->setText(1, currentShareData->comment);
    item2 = new QListViewItem(item);
    if (currentShareData->directory){
	item2->setText(0, i18n("Path"));
    }else{
	item2->setText(0, i18n("Queue"));
    }
    if (currentShareData->directory){
	item2->setText(1, currentShareData->path);
    }else{
	item2->setText(1, currentShareData->queue);
    }
    item2 = new QListViewItem(item);
    item2->setText(0, i18n("Browseable"));
    if (currentShareData->browse)
	item2->setText(1, i18n("yes"));
    else
	item2->setText(1, i18n("no"));
    item2 = new QListViewItem(item);
    item2->setText(0, i18n("Public"));
    if (currentShareData->publ)
	item2->setText(1, i18n("yes"));
    else
	item2->setText(1, i18n("no"));
    item2 = new QListViewItem(item);
    item2->setText(0, i18n("Read only"));
    if (currentShareData->directory){
	if (currentShareData->read)
	    item2->setText(1, i18n("yes"));
	else
	    item2->setText(1, i18n("no"));
    }else{
	item2->setText(1, "<" + i18n("not available") + ">");
    }
    item = new QListViewItem(currentShareData->listItem);
    item->setText(0, i18n("User settings"));
    item2 = new QListViewItem(item);
    if (currentShareData->users)
	item2->setText(0, i18n("Allowed users"));
    else
	item2->setText(0, i18n("Allowed groups"));
    shareList->setSelected(currentShareData->listItem, true);
    currentShareData->listItem->setVisible(true);
*/
}


void KcmInterface::addShare()
{
/*
	currentShareData = new shareData(0);
	// Fill the structure with default values
	currentShareData->name="";
	currentShareData->path="";
	currentShareData->queue="";
	currentShareData->browse=false;
	currentShareData->read=true;
	currentShareData->publ=false;
	currentShareData->directory=true;
	// Add the new share to the shares list
	shares.append(currentShareData);
	// Edit the share
	edit();
*/
}


void KcmInterface::selectedShare(QListViewItem* item)
{
/*
    bool enable = false;
    if (item){
	enable = true;
    }
    BtnEdit->setEnabled(enable);
    BtnDelete->setEnabled(enable);
*/
}


void KcmInterface::delShare()
{
/*
    shares.remove(currentShareData);
    delete currentShareData;
    currentShareData=0;
    BtnEdit->setEnabled(false);
    BtnDelete->setEnabled(false);
*/
}


void KcmInterface::selectAndEdit( QListViewItem * item )
{
/*
    updateShareData(item);
    selectedShare(item);
    edit();
*/
}




void KcmInterface::init()
{
    addShareBtn->setIconSet(SmallIcon("filenew"));
    editShareBtn->setIconSet(SmallIcon("edit"));
    removeShareBtn->setIconSet(SmallIcon("editdelete"));
    
    addPrinterBtn->setIconSet(SmallIcon("filenew"));
    editPrinterBtn->setIconSet(SmallIcon("edit"));
    removePrinterBtn->setIconSet(SmallIcon("editdelete"));
    
}