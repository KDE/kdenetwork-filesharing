/***************************************************************************
                          nfshostdlg.cpp  -  description
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

#include <qcheckbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qwhatsthis.h>
#include <qgroupbox.h>

#include <klocale.h>
#include <kdebug.h>
#include <kmessagebox.h>

#include "nfshostdlg.h"
#include "hostprops.h"
#include "nfsentry.h"


NFSHostDlg::NFSHostDlg(QWidget* parent, NFSHost* host)
  : KDialogBase(Plain, i18n("Host properties"), Ok|Cancel, Ok, parent)
{

  QWidget* page = plainPage();

  _hostProps = new HostProps(page);

  QVBoxLayout *layout = new QVBoxLayout( page, 0, 6 );
  layout->addWidget( _hostProps );

  connect( _hostProps, SIGNAL(modified()), this, SLOT(setModified()));

//  _hostProps->show();
  init(host);

}

NFSHostDlg::~NFSHostDlg()
{
}

void NFSHostDlg::init(NFSHost* host)
{
  _host = host;

  _hostProps->nameEdit->setText(host->name());

  if (host->name()=="*")
  {
    _hostProps->publicChk->setChecked(true);
  }

  _hostProps->readOnlyChk->setChecked(! host->readonly);
  _hostProps->allSquashChk->setChecked( host->allSquash);
  _hostProps->rootSquashChk->setChecked(! host->rootSquash);
  _hostProps->hideChk->setChecked(! host->hide);
  _hostProps->secureChk->setChecked(! host->secure);
  _hostProps->secureLocksChk->setChecked(! host->secureLocks);
  _hostProps->subtreeChk->setChecked(! host->subtreeCheck);
  _hostProps->syncChk->setChecked(host->sync);
  _hostProps->wdelayChk->setChecked(! host->wdelay);

  _hostProps->anonuidEdit->setText( QString().setNum(host->anonuid) );
  _hostProps->anongidEdit->setText( QString().setNum(host->anongid) );

}


void NFSHostDlg::slotOk()
{
  _host->readonly = !_hostProps->readOnlyChk->isChecked();
  _host->allSquash = _hostProps->allSquashChk->isChecked();
  _host->rootSquash = !_hostProps->rootSquashChk->isChecked();
  _host->hide = !_hostProps->hideChk->isChecked();
  _host->secure = !_hostProps->secureChk->isChecked();
  _host->secureLocks = !_hostProps->secureLocksChk->isChecked();
  _host->subtreeCheck = !_hostProps->subtreeChk->isChecked();
  _host->sync = _hostProps->syncChk->isChecked();
  _host->wdelay = !_hostProps->wdelayChk->isChecked();

  _host->anonuid = _hostProps->anonuidEdit->text().toInt();
  _host->anongid = _hostProps->anongidEdit->text().toInt();

  QString test = _hostProps->nameEdit->text();

  if (_hostProps->publicChk->isChecked())
    test = "*";

  if (!_host->setName(test))
  {
    if (test=="*")
    {
      KMessageBox::sorry(this,i18n("There already exists a public entry !"),i18n("Host already exists !"));
      _hostProps->publicChk->setChecked(false);
    }
    else
      KMessageBox::sorry(this,i18n("The host '%1' already exists !").arg(test),i18n("Host already exists !"));

    _hostProps->nameEdit->setFocus();

    return;
  }

  KDialogBase::slotOk();
}


void NFSHostDlg::slotCancel()
{
  KDialogBase::slotCancel();
}

void NFSHostDlg::setModified()
{
  emit modified();
}

#include "nfshostdlg.moc"


