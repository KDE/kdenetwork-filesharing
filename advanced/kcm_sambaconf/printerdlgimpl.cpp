/***************************************************************************
                          printerdlgimpl.cpp  -  description
                             -------------------
    begin                : Tue June 6 2002
    copyright            : (C) 2002 by Jan Schäfer
    email                : janschaefer@users.sourceforge.net
 ***************************************************************************/

/******************************************************************************
 *                                                                            *
 *  This file is part of KSambaPlugin.                                        *
 *                                                                            *
 *  KSambaPlugin is free software; you can redistribute it and/or modify      *
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation; either version 2 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  KSambaPlugin is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with KSambaPlugin; if not, write to the Free Software                     *
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
#include <qtabwidget.h>

#include <klineedit.h>
#include <kurlrequester.h>
#include <knuminput.h>
#include <kdebug.h>
#include <kdeprint/kmmanager.h>
#include <kdeprint/kmprinter.h>
#include <kcombobox.h>
#include <kiconloader.h>
#include <klocale.h>

#include <assert.h>

#include "sambafile.h"
#include "printerdlgimpl.h"
#include "usertabimpl.h"
#include "passwd.h"
#include "dictmanager.h"


PrinterDlgImpl::PrinterDlgImpl(QWidget* parent, SambaShare* share)
	: KcmPrinterDlg(parent,"sharedlgimpl")
{
  if (!share) {
    kdWarning() << "PrinterDlgImpl::Constructor : share parameter is null!" << endl;
    return;    
  }

  _share = share;
  _dictMngr = new DictManager(share);
  
  initDialog();
}

void PrinterDlgImpl::initDialog()
{
	// Base settings
  if (!_share)
     return;

	QPtrList<KMPrinter> *printerList = KMManager::self()->printerListComplete();

  for (QPtrListIterator<KMPrinter> it(*printerList); it.current(); ++it)
  {
    if (!it.current()->isSpecial()){
      queueCombo->insertItem(it.current()->printerName());
    }
  }

  setComboToString(queueCombo,_share->getValue("printer name"));

 
  _dictMngr->add("path",pathUrlRq);
  printersChk->setChecked( _share->getName() == "printers" );

  shareNameEdit->setText( _share->getName() );
  _dictMngr->add("comment",commentEdit);

  _dictMngr->add("available",availableBaseChk);
  _dictMngr->add("browseable",browseableBaseChk);
  _dictMngr->add("public",publicBaseChk);
  
  // Users

  _userTab = new UserTabImpl(this,_share);
  _tabs->insertTab(_userTab,i18n("&Users"),1);
  _userTab->load();
  connect(_userTab, SIGNAL(changed()), this, SLOT(changedSlot()));
  

  // Printing

  _dictMngr->add("postscript",postscriptChk);
  _dictMngr->add("use client driver",useClientDriverChk);
  _dictMngr->add("default devmode",defaultDevmodeChk);

  _dictMngr->add("max print jobs",maxPrintJobsSpin);
  _dictMngr->add("max reported print jobs",maxReportedPrintJobsSpin);
  _dictMngr->add("printing",printingCombo,
                  new QStringList(QStringList() 
                  << "sysv" << "aix" << "hpux" << "bsd" << "qnx"
                  << "plp" << "lprng" << "softq" << "cups" << "nt" << "os2"));
                  
  _dictMngr->add("printer driver",printerDriverEdit);
  _dictMngr->add("printer driver location",printerDriverLocationEdit);

  // Commands

  _dictMngr->add("print command",printCommandEdit);
  _dictMngr->add("lpq command",lpqCommandEdit);
  _dictMngr->add("lprm command",lprmCommandEdit);
  _dictMngr->add("lppause",lppauseEdit);
  _dictMngr->add("lpresume",lpresumeEdit);
  _dictMngr->add("queuepause",queuepauseEdit);
  _dictMngr->add("queueresume",queueresumeEdit);
  
  // Security

  _dictMngr->add("printer admin",printerAdminEdit);
  _dictMngr->add("hosts allow",hostsAllowEdit);
  _dictMngr->add("hosts deny",hostsDenyEdit);
  
  guestAccountCombo->insertStringList( getUnixUsers() );
  setComboToString(guestAccountCombo,_share->getValue("guest account"));

  // Advanced

  _dictMngr->add("min print space",minPrintSpaceSpin);
  _dictMngr->add("status",statusChk);
  _dictMngr->add("preexec",preExecEdit);
  _dictMngr->add("postexec",postExecEdit);
  _dictMngr->add("root preexec",rootPreExecEdit);
  _dictMngr->add("root postexec",rootPostExecEdit);

  _dictMngr->load( _share );
  connect(_dictMngr, SIGNAL(changed()), this, SLOT(changedSlot()));
    
}

void PrinterDlgImpl::accept()
{
  if (!_share)
     return;

  // Security
  
  _share->setValue("guest account",guestAccountCombo->currentText( ) );
  _share->setValue("printer name",queueCombo->currentText());

  if (printersChk->isChecked())
  {
  	 _share->setName("printers");
  }
	else
		_share->setName(shareNameEdit->text());


  _userTab->save();
  _dictMngr->save( _share );      
      

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
