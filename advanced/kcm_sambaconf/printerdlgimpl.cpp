/***************************************************************************
                          printerdlgimpl.cpp  -  description
                             -------------------
    begin                : Tue June 6 2002
    copyright            : (C) 2002 by Jan Schäfer
    email                : janschaefer@users.sourceforge.net
 ***************************************************************************/

/******************************************************************************
 *                                                                            *
 *  This file is part of KSambaPlugin.                                          *
 *                                                                            *
 *  KSambaPlugin is free software; you can redistribute it and/or modify            *
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation; either version 2 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  KSambaPlugin is distributed in the hope that it will be useful,                 *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with Foobar; if not, write to the Free Software                     *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA  *
 *                                                                            *
 ******************************************************************************/


/**
 * @author Jan Schäfer
 **/
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlabel.h>

#include <klineedit.h>
#include <kurlrequester.h>
#include <knuminput.h>
#include <kdebug.h>
#include <kdeprint/kmmanager.h>
#include <kdeprint/kmprinter.h>
#include <kcombobox.h>
#include <qgroupbox.h>

#include <assert.h>

#include "sambafile.h"
#include "printerdlgimpl.h"

PrinterDlgImpl::PrinterDlgImpl(QWidget* parent, SambaShare* share)
	: KcmPrinterDlg(parent,"sharedlgimpl")
{

  _share = share;
	assert(_share);
  initDialog();
}

void PrinterDlgImpl::initDialog()
{
	// Base settings

	assert(_share);

  if (!_share)
     return;

	QPtrList<KMPrinter> *printerList = KMManager::self()->printerList();

  for (QPtrListIterator<KMPrinter> it(*printerList); it.current(); ++it)
  {
    if (!it.current()->isSpecial()){
      queueCombo->insertItem(it.current()->printerName());
    }
  }

  queueCombo->setCurrentText(_share->getValue("printer name"));

  pathUrlRq->setURL( _share->getValue("path") );
  printersChk->setChecked( _share->getName() == "printers" );

  shareNameEdit->setText( _share->getName() );
	commentEdit->setText( _share->getValue("comment") );

  availableBaseChk->setChecked( _share->getBoolValue("available") );
  browseableBaseChk->setChecked( _share->getBoolValue("browseable") );
  printableBaseChk->setChecked( _share->getBoolValue("printable") );
  publicBaseChk->setChecked( _share->getBoolValue("public") );

  // Printing

  printableChk->setChecked( _share->getBoolValue("printable") );
  postscriptChk->setChecked( _share->getBoolValue("postscript") );

  maxPrintJobsInput->setValue( _share->getValue("max print jobs").toInt() );
  printingCombo->setCurrentText( _share->getValue("printing") );

  printerDriverEdit->setText( _share->getValue("printer friver") );
  printerDriverFileEdit->setText( _share->getValue("printer driver file") );
  printerDriverLocationEdit->setText( _share->getValue("printer driver location") );

  // Commands

  printCommandEdit->setText( _share->getValue("print command") );
  lpqCommandEdit->setText( _share->getValue("lpq command") );
  lprmCommandEdit->setText( _share->getValue("lprm command") );
  lppauseEdit->setText( _share->getValue("lppause") );
  lpresumeEdit->setText( _share->getValue("lpresume") );
  queuepauseEdit->setText( _share->getValue("queuepause") );
  queueresumeEdit->setText( _share->getValue("queueresume") );

  // Security

  guestOkChk->setChecked( _share->getBoolValue("guest ok") );
  guestAccountEdit->setText( _share->getValue("guest account") );
  printerAdminEdit->setText( _share->getValue("printer admin") );
  hostsAllowEdit->setText( _share->getValue("hosts allow") );
  hostsDenyEdit->setText( _share->getValue("hosts deny") );


  // Advanced

  minPrintSpaceInput->setValue( _share->getValue("min print space").toInt() );
  statusChk->setChecked( _share->getBoolValue("status") );

  preExecEdit->setText( _share->getValue("preexec") );
  postExecEdit->setText( _share->getValue("postexec") );
  rootPreExecEdit->setText( _share->getValue("root preexec") );
  rootPostExecEdit->setText( _share->getValue("root postexec") );

  // Hidden files
}

PrinterDlgImpl::~PrinterDlgImpl()
{
}

#include "printerdlgimpl.moc"
