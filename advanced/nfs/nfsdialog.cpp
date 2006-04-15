/*
  Copyright (c) 2004 Jan Schaefer <j_schaef@informatik.uni-kl.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/


#include <qradiobutton.h>
#include <qpushbutton.h>
#include <q3groupbox.h>
#include <qlineedit.h>
#include <q3buttongroup.h>
#include <qlayout.h>
//Added by qt3to4:
#include <QVBoxLayout>
#include <Q3PtrList>

#include <kdebug.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <k3listview.h>


#include "nfsdialog.h"
#include "nfsentry.h"
#include "nfshostdlg.h"
#include "nfsfile.h"
#include "nfsdialoggui.h"

NFSDialog::NFSDialog(QWidget * parent, NFSEntry* entry)
 : KDialogBase(Plain, i18n("NFS Options"), Ok|Cancel, Ok, parent), 
   m_nfsEntry(entry),
   m_modified(false)
{
  if (m_nfsEntry) 
      m_workEntry = m_nfsEntry->copy();
  else
      kWarning() << "NFSDialog::NFSDialog: entry is null!" << endl;
            
  initGUI();
  initSlots();
  initListView();
}

NFSDialog::~NFSDialog()
{
  delete m_workEntry;
}

void NFSDialog::initGUI() {
  QWidget* page = plainPage();
  m_gui = new NFSDialogGUI(page);

  QVBoxLayout *layout = new QVBoxLayout( page );
  layout->addWidget( m_gui );
#warning "kde4 port it"
  //KAccel* accel = new KAccel( m_gui->listView );
  //accel->insert( "Delete", Qt::Key_Delete, this, SLOT(slotRemoveHost()));
}

void NFSDialog::initSlots()
{
  connect( m_gui->removeHostBtn, SIGNAL(clicked()), this, SLOT( slotRemoveHost()));
  connect( m_gui->addHostBtn, SIGNAL(clicked()), this, SLOT( slotAddHost()));
  connect( m_gui->modifyHostBtn, SIGNAL(clicked()), this, SLOT( slotModifyHost()));
  connect( m_gui->listView, SIGNAL(doubleClicked(Q3ListViewItem*)), 
           this, SLOT( slotModifyHost()));

}

void NFSDialog::initListView()
{
  if (m_workEntry) {
    HostIterator it = m_workEntry->getHosts();

    NFSHost* host;
    while ( (host = it.current()) != 0 ) {
       ++it;
       createItemFromHost(host);
    }
  }
}

Q3ListViewItem* NFSDialog::createItemFromHost(NFSHost* host)
{
  if (!host) 
    return 0;

  Q3ListViewItem* item = new Q3ListViewItem(m_gui->listView);
  updateItem(item, host);
  return item;
}

void NFSDialog::updateItem(Q3ListViewItem* item, NFSHost* host)
{
  item->setText(0,host->name);
  item->setText(1,host->paramString());
}

void NFSDialog::slotAddHost()
{
  NFSHost *host = new NFSHost();

  // Set some secure parameters
  //host->allSquash=true;
  host->readonly=true;

  HostList hostList;
  hostList.append(host);
  
  NFSHostDlg *dlg = new NFSHostDlg(this, &hostList, m_workEntry);
  dlg->exec();

  
  if (dlg->result()==QDialog::Accepted) {
    m_workEntry->addHost(host);
    createItemFromHost(host);
    setModified();
  } else {
    delete host;
  }

  delete dlg;
}

void NFSDialog::slotOk() {
  if (m_modified) {
    m_nfsEntry->copyFrom(m_workEntry);
  }    
  KDialogBase::slotOk();
}

void NFSDialog::slotRemoveHost()
{
  QList<Q3ListViewItem *> items = m_gui->listView->selectedItems();
  if (items.count()==0)
      return;
  foreach ( Q3ListViewItem* item, items ) {
    QString name = item->text(0);
    m_gui->listView->takeItem(item);
    
    NFSHost* host = m_workEntry->getHostByName(name);
    if (host) {
      m_workEntry->removeHost(host);
    } else {
      kWarning() << "NFSDialog::slotRemoveHost: no host " 
                  << name << " << found!" << endl;
    }
      
  }
    
  m_gui->modifyHostBtn->setDisabled(true);
  m_gui->removeHostBtn->setDisabled(true);
  setModified();
}

void NFSDialog::slotModifyHost()
{
  QList<Q3ListViewItem*> items = m_gui->listView->selectedItems();
  if (items.count()==0)
      return;
  
  HostList hostList;
  foreach ( Q3ListViewItem* item, items ) {
    NFSHost* host = m_workEntry->getHostByName(item->text(0));
    if (host)
      hostList.append(host);
    else
      kWarning() << "NFSDialog::slogModifyHost: host " 
                  << item->text(0) << " is null!" << endl;     
  }
  
  NFSHostDlg *dlg = new NFSHostDlg(this, &hostList, m_workEntry);
  if (dlg->exec() == QDialog::Accepted &&
      dlg->isModified()) 
  {      
      setModified();
  }
        
  delete dlg;

  NFSHost* host = hostList.first();
  foreach ( Q3ListViewItem* item, items ) {
      if (item && host)
          updateItem( item,host);
      host = hostList.next();
  }
}

void NFSDialog::setModified()
{
  m_modified = true;
}

bool NFSDialog::modified() {
  return m_modified;
}

#include "nfsdialog.moc"

