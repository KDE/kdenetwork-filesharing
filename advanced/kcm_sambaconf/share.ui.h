/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

void shareDialog::helpIndex()
{

}

void shareDialog::helpContents()
{

}

void shareDialog::helpAbout()
{

}

void shareDialog::setFields( QListViewItem * item )
{
    globItem = item;
    if (item){
	sharename->setText(item->text(0));
	path->setURL(item->text(1));
	comment->setText(item->text(6));
	if (item->text(2) == ""){
	    browse->setChecked(false);
	}else{
	    browse->setChecked(true);
	}
	if (item->text(3) == ""){
	    cbpublic->setChecked(false);
	}else{
	    cbpublic->setChecked(true);
	}
	if (item->text(4) == ""){
	    read->setChecked(false);
	}else{
	    read->setChecked(true);
	}
	if (item->text(5) == ""){
	    print->setChecked(false);
	}else{
	    print->setChecked(true);
	}
    }else{
	/** Catch NULL-pointers */
	done(QDialog::Rejected);
    }
    this->exec();
}

void shareDialog::acceptData()
{
    globItem->setText(0, sharename->text());
    if (globItem->text(0) == "homes"){
	KMessageBox::error(this, i18n("You can't create a share named \"homes\" in this dialog.\nIf you want to create such a share, use the share-homes-dialog instead."));
	return;
    }
    if  (globItem->text(0) == "printers"){
	KMessageBox::error(this, i18n("You can't create a share named \"printers\" in this dialog.\nIf you want to create such a share, use the share-all-printers-dialog instead."));
	return;
    }
    if (dir->isChecked()){
	globItem->setText(1, path->url());
    }else{
	globItem->setText(1, queue->text(queue->currentItem()));
    }
    if (browse->isChecked())
	globItem->setText(2, "x");
    else
	globItem->setText(2, "");
    if (cbpublic->isChecked())
	globItem->setText(3, "x");
    else
	globItem->setText(3, "");
    if (dir->isChecked()){
	globItem->setText(5, "");
	if (read->isChecked())
	    globItem->setText(4, "x");
	else
	    globItem->setText(4, "");
    }else{
	globItem->setText(4, "");
	if (print->isChecked())
	    globItem->setText(5, "x");
	else
	    globItem->setText(5, "");
    }
    globItem->setText(6, comment->text());
    
    emit sendFields(globItem);
    emit accept();
}

void shareDialog::addToList()
{
    QListBoxItem* item = available->item(available->currentItem());
    available->takeItem(item);
    selected->insertItem(item);
    selected->setCurrentItem(item);
}

void shareDialog::removeFromList()
{
    QListBoxItem* item = selected->item(selected->currentItem());
    selected->takeItem(item);
    available->insertItem(item);
    available->setCurrentItem(item);
}



void shareDialog::cancelCreation()
{
    emit cancel();
    reject();
}