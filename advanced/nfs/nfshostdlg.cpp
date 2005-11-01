/***************************************************************************
                          nfshostdlg.cpp  -  description
                             -------------------
    begin                : Mon Apr 29 2002
    copyright            : (C) 2002 by Jan Sch�er
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

#include <qcheckbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qwhatsthis.h>
#include <qgroupbox.h>

#include <klocale.h>
#include <kdebug.h>
#include <kmessagebox.h>

#include <assert.h>

#include "nfshostdlg.h"
#include "hostprops.h"
#include "nfsentry.h"


NFSHostDlg::NFSHostDlg(QWidget* parent, HostList* hosts, NFSEntry* entry)
  : KDialogBase(Plain, i18n("Host Properties"), Ok|Cancel, Ok, parent),
  m_hosts(hosts), m_nfsEntry(entry), m_modified(false)
{

  QWidget* page = plainPage();

  m_gui = new HostProps(page);

  QVBoxLayout *layout = new QVBoxLayout( page, 0, 6 );
  layout->addWidget( m_gui );

  connect( m_gui, SIGNAL(modified()), this, SLOT(setModified()));

  init();

}

NFSHostDlg::~NFSHostDlg()
{
}

void NFSHostDlg::init()
{  
  if (m_hosts->count()==1) {
    NFSHost* host = m_hosts->first();
    assert(host);
    m_gui->nameEdit->setText(host->name);
    m_gui->publicChk->setChecked(host->isPublic());

    setHostValues(host);  
    
    m_gui->nameEdit->setFocus();
  } else {
    m_gui->nameEdit->setDisabled(true);
    m_gui->publicChk->setDisabled(true);
    
    m_gui->readOnlyChk->setTristate();
    m_gui->allSquashChk->setTristate();
    m_gui->rootSquashChk->setTristate();
    m_gui->hideChk->setTristate();
    m_gui->secureChk->setTristate();
    m_gui->secureLocksChk->setTristate();
    m_gui->subtreeChk->setTristate();
    m_gui->syncChk->setTristate();
    m_gui->wdelayChk->setTristate();
    
    for (NFSHost* host = m_hosts->first(); host; host = m_hosts->next()) {
         setHostValues(host);
    }
  }   
}

void NFSHostDlg::setHostValues(NFSHost* host) {
    setCheckBoxValue(m_gui->readOnlyChk, ! host->readonly);
    setCheckBoxValue(m_gui->allSquashChk, host->allSquash);
    setCheckBoxValue(m_gui->rootSquashChk, ! host->rootSquash);
    setCheckBoxValue(m_gui->hideChk, ! host->hide);
    setCheckBoxValue(m_gui->secureChk, ! host->secure);
    setCheckBoxValue(m_gui->secureLocksChk, ! host->secureLocks);
    setCheckBoxValue(m_gui->subtreeChk, ! host->subtreeCheck);
    setCheckBoxValue(m_gui->syncChk, host->sync);
    setCheckBoxValue(m_gui->wdelayChk, ! host->wdelay);

    setEditValue(m_gui->anonuidEdit,QString::number(host->anonuid));
    setEditValue(m_gui->anongidEdit,QString::number(host->anongid));
}

void NFSHostDlg::setEditValue(QLineEdit* edit, const QString & value) {
  if (edit->text().isEmpty()) 
      return;
      
  if (edit->text() == "FF")      
      edit->setText(value);
  else 
  if (edit->text() != value)
      edit->setText(QString::null);
}

void NFSHostDlg::setCheckBoxValue(QCheckBox* chk, bool value) {
  if (chk->state() == QButton::NoChange)
      return;
      
  if (chk->isChecked()) {
    if (! value)
      chk->setNoChange();       
  } else {
    if (value)
      chk->setChecked(true);
  }
}


void NFSHostDlg::slotOk()
{
  if (m_hosts->count()==1) {
    NFSHost* host = m_hosts->first();
    if (! saveName(host))
      return;

    saveValues(host);            
  } else {
    for (NFSHost* host = m_hosts->first(); host; host = m_hosts->next()) {
        saveValues(host);
    }
  }    
  
  KDialogBase::slotOk();
}

bool NFSHostDlg::saveName(NFSHost* host) {
  if (m_gui->publicChk->isChecked()) {
     NFSHost* publicHost = m_nfsEntry->getPublicHost();
     if (publicHost && publicHost != host) {
       KMessageBox::sorry(this,i18n("There already exists a public entry."),
                          i18n("Host Already Exists"));
       m_gui->publicChk->setChecked(false);
       return false;
     }
     host->name="*";
  } else {   
    QString name = m_gui->nameEdit->text().stripWhiteSpace();
    if (name.isEmpty()) {
      KMessageBox::sorry(this,
              i18n("Please enter a hostname or an IP address.").arg(name),
              i18n("No Hostname/IP-Address"));
      m_gui->nameEdit->setFocus();
      return false;
    } else {
      NFSHost* host2 = m_nfsEntry->getHostByName(name);
      if (host2 && host2 != host) {
        KMessageBox::sorry(this,i18n("The host '%1' already exists.").arg(name),
                          i18n("Host Already Exists"));
        m_gui->nameEdit->setFocus();
        return false;
      }
    }      
    host->name=name;
  }

  return true;
}


void NFSHostDlg::saveValues(NFSHost* host) {
  
  saveCheckBoxValue(host->readonly,m_gui->readOnlyChk,true);
  saveCheckBoxValue(host->allSquash,m_gui->allSquashChk,false);
  saveCheckBoxValue(host->rootSquash,m_gui->rootSquashChk,true);
  saveCheckBoxValue(host->hide,m_gui->hideChk,true);
  saveCheckBoxValue(host->secure,m_gui->secureChk,true);
  saveCheckBoxValue(host->secureLocks,m_gui->secureLocksChk,true);
  saveCheckBoxValue(host->subtreeCheck,m_gui->subtreeChk,true);
  saveCheckBoxValue(host->sync,m_gui->syncChk,false);
  saveCheckBoxValue(host->wdelay,m_gui->wdelayChk,true);

  saveEditValue(host->anonuid,m_gui->anonuidEdit);
  saveEditValue(host->anongid,m_gui->anongidEdit);
}

void NFSHostDlg::saveEditValue(int & value, QLineEdit* edit) {
  if ( edit->text().isEmpty())
      return;
  
  value = edit->text().toInt();
}

void NFSHostDlg::saveCheckBoxValue(bool & value, QCheckBox* chk, bool neg) {
  if (chk->state() == QButton::NoChange)
    return;
  
  if (neg)      
    value = ! chk->isChecked();    
  else
    value = chk->isChecked();            
}
 


bool NFSHostDlg::isModified() {
  return m_modified;
}


void NFSHostDlg::setModified()
{
  m_modified = true;
}

#include "nfshostdlg.moc"


