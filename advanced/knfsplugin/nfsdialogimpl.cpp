/***************************************************************************
                          nfsdialogimpl.cpp  -  description
                             -------------------
    begin                : Mon Apr 29 2002
    copyright            : (C) 2002 by Jan Schäfer
    email                : janschaefer@users.sourceforge.net
 ***************************************************************************/

/******************************************************************************
 *                                                                            *
 *  This file is part of KNfsPlugin.                                          *
 *                                                                            *
 *  Foobar is free software; you can redistribute it and/or modify            *
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation; either version 2 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  Foobar is distributed in the hope that it will be useful,                 *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with Foobar; if not, write to the Free Software                     *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA  *
 *                                                                            *
 ******************************************************************************/

#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qlistview.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qbuttongroup.h>

#include <kdebug.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <klocale.h>


#include "nfsdialogimpl.h"
#include "nfsdialog.h"
#include "nfsentry.h"
#include "nfshostdlg.h"
#include "nfsfile.h"

NFSDialogImpl::NFSDialogImpl(QWidget * parent, NFSEntry* entry, bool showPath=false)
 : NFSDialog(parent)
{
	_showPath = showPath;
  nfsFile = entry->getNFSFile();
  initSlots();
  init(entry);
  sharedRadio->setChecked(true);
  groupBox->setEnabled(true);

}

NFSDialogImpl::NFSDialogImpl(
	QWidget * parent,
	NFSFile* file, 
  const QString & _path, 
  bool showPath=false)
{
	_showPath = showPath;
  nfsFile = file;
  path = _path;

  initSlots();
  init(0);
  notSharedRadio->setChecked(true);
  groupBox->setEnabled(false);

}

NFSDialogImpl::~NFSDialogImpl()
{
}

void NFSDialogImpl::initSlots()
{
  connect( removeHostBtn, SIGNAL(clicked()), this, SLOT( slotRemoveHost()));
  connect( addHostBtn, SIGNAL(clicked()), this, SLOT( slotAddHost()));
  connect( modifyHostBtn, SIGNAL(clicked()), this, SLOT( slotModifyHost()));
  connect( listView, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT( slotModifyHost()));

}

void NFSDialogImpl::init(NFSEntry * entry)
{
  nfsEntry = entry;

  if (!_showPath)
  	pathFrame->hide();
  else
		sharedBtnGrp->hide();




  if (nfsEntry)
  {

    pathEdit->setText(nfsEntry->getPath());
    
    HostIterator it = nfsEntry->getHosts();

    NFSHost* host;

    while ( (host = it.current()) != 0 )
    {
       ++it;
       createItemFromHost(host);
    }

  }

}

QListViewItem* NFSDialogImpl::createItemFromHost(NFSHost* host)
{
  if (!host) return 0L;

  QListViewItem* item = new QListViewItem(listView);

  updateItem(item, host);

  return item;
}

void NFSDialogImpl::updateItem(QListViewItem* item, NFSHost* host)
{
  item->setText(0,host->name());

  item->setText(1,host->paramString());

}

void NFSDialogImpl::slotAddHost()
{
  NFSHost *host = new NFSHost("*", nfsEntry);

  // Set some secure parameters
  host->allSquash=true;
  host->readonly=true;

  NFSHostDlg *dlg = new NFSHostDlg(this, host);
  dlg->exec();

  if (dlg->result()==QDialog::Accepted)
  {
    createItemFromHost(host);
    setModified();
  } else
  {
    nfsEntry->removeHost(host);
    delete host;
  }

  delete dlg;
}

void NFSDialogImpl::slotRemoveHost()
{
  QListViewItem* item = listView->selectedItem();
  if (!item) return;

  NFSHost *host = nfsEntry->getHostByName(item->text(0));

  listView->takeItem(item);
  nfsEntry->removeHost(host);

  setModified();

}

void NFSDialogImpl::slotModifyHost()
{
  QListViewItem* item = listView->selectedItem();
  if (!item) return;

  NFSHost *host = nfsEntry->getHostByName(item->text(0));

  if (!host)
  {
    kdDebug() << "Warning : selected host does not exist ! " << endl;
    return;
  }

  NFSHostDlg *dlg = new NFSHostDlg(this, host);
  connect(dlg, SIGNAL(modified()), this, SLOT(setModified()));
  dlg->exec();

  disconnect(dlg, SIGNAL(modified()), this, SLOT(setModified()));

  delete dlg;

  updateItem( item,host );
}

void NFSDialogImpl::applyChanges()
{
}

void NFSDialogImpl::shareStateChanged( int state )
{
  NFSDialog::shareStateChanged(state);

  if (state==0) // Not shared clicked
  {
    nfsFile->removeEntry(nfsEntry);
  }
  else
  {
    if (!nfsEntry)
    {
      nfsEntry = new NFSEntry(path,nfsFile);
    } else
    {
      nfsFile->addEntry(nfsEntry);
    }

  }

  setModified();
}

void NFSDialogImpl::setModified()
{
  _modified = true;
  emit modified();
}

void NFSDialogImpl::specifyPathSlot()
{
	QString newPath = KFileDialog::getExistingDirectory("/",
        		this,i18n("Choose a directory to share"));
      
	NFSEntry* entry = nfsFile->getEntryByPath(newPath);

	if (entry && entry != nfsEntry)
  {
  	KMessageBox::sorry(this,i18n("The directory you have choosen is already shared !"),
    									 i18n("Directory already shared !"));
  }
  else
  	pathEdit->setText(newPath);



}

#include "nfsdialogimpl.moc"

