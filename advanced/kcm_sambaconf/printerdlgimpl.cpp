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
#include <qlistbox.h>
#include <qpainter.h>
#include <qgroupbox.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qlayout.h>

#include <klineedit.h>
#include <kurlrequester.h>
#include <knuminput.h>
#include <kdebug.h>
#include <kdeprint/kmmanager.h>
#include <kdeprint/kmprinter.h>
#include <kcombobox.h>
#include <kiconloader.h>

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

	QPtrList<KMPrinter> *printerList = KMManager::self()->printerListComplete();

  for (QPtrListIterator<KMPrinter> it(*printerList); it.current(); ++it)
  {
    if (!it.current()->isSpecial()){
      queueCombo->insertItem(it.current()->printerName());
    }
  }

	int i = queueCombo->listBox()->index(queueCombo->listBox()->findItem(_share->getValue("printer name"),Qt::ExactMatch));
  queueCombo->setCurrentItem(i);

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

//  guestOkChk->setChecked( _share->getBoolValue("guest ok") );
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

void PrinterDlgImpl::accept()
{
  _share->setValue("printer name",queueCombo->currentText());

  _share->setValue("path",pathUrlRq->url());

  if (printersChk->isChecked())
  {
  	 _share->setName("printers");
  }
	else
		_share->setName(shareNameEdit->text());

	_share->setValue("comment",commentEdit->text( ) );

  _share->setValue("available",availableBaseChk->isChecked( ) );
  _share->setValue("browseable",browseableBaseChk->isChecked( ) );
  _share->setValue("printable",printableBaseChk->isChecked( ) );
  _share->setValue("public",publicBaseChk->isChecked( ) );

  // Printing

  _share->setValue("printable",printableChk->isChecked());
  _share->setValue("postscript",postscriptChk->isChecked( ) );

  _share->setValue("max print jobs", QString("%1").arg(maxPrintJobsInput->value()) );
  _share->setValue("printing",printingCombo->currentText( ) );

  _share->setValue("printer friver",printerDriverEdit->text( ) );
  _share->setValue("printer driver file",printerDriverFileEdit->text( ) );
  _share->setValue("printer driver location",printerDriverLocationEdit->text( ) );

  // Commands

  _share->setValue("print command",printCommandEdit->text( ) );
  _share->setValue("lpq command",lpqCommandEdit->text( ) );
  _share->setValue("lprm command",lprmCommandEdit->text( ) );
  _share->setValue("lppause",lppauseEdit->text( ) );
  _share->setValue("lpresume",lpresumeEdit->text( ) );
  _share->setValue("queuepause",queuepauseEdit->text( ) );
  _share->setValue("queueresume",queueresumeEdit->text( ) );

  // Security

//-  _share->setValue("guest ok",guestOkChk->isChecked( ) );
  _share->setValue("guest account",guestAccountEdit->text( ) );
  _share->setValue("printer admin",printerAdminEdit->text( ) );
  _share->setValue("hosts allow",hostsAllowEdit->text( ) );
  _share->setValue("hosts deny",hostsDenyEdit->text( ) );


  // Advanced

  _share->setValue("min print space", QString("%1").arg(minPrintSpaceInput->value()) );
  _share->setValue("status",statusChk->isChecked( ) );

  _share->setValue("preexec",preExecEdit->text( ) );
  _share->setValue("postexec",postExecEdit->text( ) );
  _share->setValue("root preexec",rootPreExecEdit->text( ) );
  _share->setValue("root postexec",rootPostExecEdit->text( ) );

	KcmPrinterDlg::accept();
}

PrinterDlgImpl::~PrinterDlgImpl()
{
}

void PrinterDlgImpl::printersChkToggled(bool b)
{
	if (b)
  {
		shareNameEdit->setText("printers");
    shareNameEdit->setEnabled(false);
    
    int dist = 10;
		int w = 64 + dist;
    int h = 64 + 2*dist;

    QPixmap pix(w,h);
    pix.fill(); // fill with white

		QPixmap pix2 = DesktopIcon("printer1");

    // Draw the printericon three times
    QPainter p(&pix);
    p.drawPixmap(dist+dist/2,0,pix2);
    p.drawPixmap(dist/2,dist,pix2);
    p.drawPixmap(dist+dist/2,2*dist,pix2);
		p.end();

  	QBitmap mask(w,h);

    mask.fill(Qt::black); // everything is transparent

    p.begin(&mask);
    
    p.setRasterOp(Qt::OrROP);
    p.drawPixmap(dist+dist/2,0,*pix2.mask());
    p.drawPixmap(dist/2,dist,*pix2.mask());
    p.drawPixmap(dist+dist/2,2*dist,*pix2.mask());
		p.end();

		pix.setMask(mask);

    printerPixLbl->setPixmap(pix);
		pixFrame->layout()->setMargin( 2 );
  }
	else
  {
  	shareNameEdit->setEnabled(true);
		shareNameEdit->setText( _share->getName() );
		printerPixLbl->setPixmap(DesktopIcon("printer1"));
		pixFrame->layout()->setMargin( 11 );
  }
}

#include "printerdlgimpl.moc"
