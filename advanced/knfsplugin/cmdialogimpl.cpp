/***************************************************************************
                          cmdialogimpl.cpp  -  description
                             -------------------
    begin                : Mon Apr 29 2002
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

#include <qlistview.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qlineedit.h>

#include <kdialogbase.h>
#include <kdebug.h>
#include <klocale.h>

#include "cmdialogimpl.h"

#include "nfsdialogimpl.h"
#include "nfsfile.h"


CMDialogImpl::CMDialogImpl(QWidget* parent, NFSFile* file)
	: CMDialog(parent,"cmdialogimpl")
{
	nfsFile = file;
  
	EntryIterator it = nfsFile->getEntries();
  
	NFSEntry* entry;

  while ( (entry = it.current()) != 0 )
  {
     ++it;
     createItemFromEntry(entry);
  }


}

CMDialogImpl::~CMDialogImpl()
{
}

QListViewItem* CMDialogImpl::createItemFromEntry(NFSEntry* entry)
{
  if (!entry) return 0L;

  QListViewItem* item = new QListViewItem(listView);

  updateItem(item, entry);

  return item;
}

void CMDialogImpl::updateItem(QListViewItem* item, NFSEntry* entry)
{
	if (! nfsFile->hasEntry(entry))
  {
	  listView->takeItem(item);
  	nfsFile->removeEntry(entry);
    delete entry;
    return;
  }

  item->setText(0,entry->getPath());

  QString hoststr = "";

  HostIterator it = entry->getHosts();
  
  NFSHost* host;
  
  while ( (host = it.current()) != 0)
  {
  	++it;
    hoststr+=host->toString();
    hoststr+=" ";
  }

  item->setText(1,hoststr);

}

void CMDialogImpl::addEntrySlot()
{
	NFSEntry *entry = new NFSEntry("", nfsFile);

  KDialogBase *dlg = createEntryDialog(entry);
  dlg->exec();

  if (dlg->result()==QDialog::Accepted)
  {
  	entry->setPath( nfsDlg->pathEdit->text());
    createItemFromEntry(entry);
    setModified();
  } else
  {
    nfsFile->removeEntry(entry);
    delete entry;
  }

  delete dlg;
}

void CMDialogImpl::modifyEntrySlot()
{
  QListViewItem* item = listView->selectedItem();
  if (!item) return;

  NFSEntry *entry = nfsFile->getEntryByPath(item->text(0));

  if (!entry)
  {
    kdDebug() << "Warning : selected entry does not exist ! " << endl;
    return;
  }

  KDialogBase *dlg = createEntryDialog(entry);
  dlg->exec();

  if (dlg->result()==QDialog::Accepted)
  {
  	entry->setPath( nfsDlg->pathEdit->text());
  }

  delete dlg;

  updateItem( item,entry );
  
  setModified();
}

KDialogBase* CMDialogImpl::createEntryDialog(NFSEntry* entry)
{

	KDialogBase* dlg = new KDialogBase(KDialogBase::Plain,
  																	 i18n("Share properties"), 
                                     KDialogBase::Ok|KDialogBase::Cancel,
                                     KDialogBase::Ok, this);

  QWidget* page = dlg->plainPage();

  nfsDlg = new NFSDialogImpl(page,entry,true);
  
  connect(nfsDlg, SIGNAL(modified()), this, SLOT(setModified()));

  QVBoxLayout *layout = new QVBoxLayout( page, 0, 6 );
  layout->addWidget( nfsDlg );

  return dlg;
}

void CMDialogImpl::removeEntrySlot()
{
  QListViewItem* item = listView->selectedItem();
  if (!item) return;

  NFSEntry *entry = nfsFile->getEntryByPath(item->text(0));

  listView->takeItem(item);
  nfsFile->removeEntry(entry);
  delete entry;

  setModified();
}



void CMDialogImpl::setModified()
{
	emit changed(true);
}

#include "cmdialogimpl.moc"
