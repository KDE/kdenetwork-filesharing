/***************************************************************************
                          kcmsambaconf.cpp  -  description
                             -------------------
    begin                : Mon Apr  8 13:35:56 CEST 2002
    copyright            : (C) 2002 by Christian Nitschkowski
    email                : segfault_ii@web.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <qlayout.h>

#include <klocale.h>
#include <kglobal.h>
#include <klineedit.h>
#include <kurlrequester.h>
#include <kdeprint/kmmanager.h>
#include <kdeprint/kmprinter.h>
#include <kcombobox.h>

#include "kcmsambaconf.h"

KcmSambaConf::KcmSambaConf(QWidget *parent, const char *name):KCModule(parent,name)
{
	QPtrList<KMPrinter> *printerList = KMManager::self()->printerList();
	QBoxLayout * l = new QHBoxLayout( this );
	l->setAutoAdd( TRUE );
	interface = new KcmInterface(this);
	shareDialog = new KcmShareDlg(this);
	interface->setShareDlg(shareDialog);
	
	shareDialog->path->setMode(2+8+16);
	
	for (QPtrListIterator<KMPrinter> it(*printerList); it.current(); ++it){
		if (!it.current()->isSpecial()){
			shareDialog->queue->insertItem(it.current()->printerName());
		}
	}
	
	connect ( interface, SIGNAL(editShare(shareData*)), shareDialog, SLOT(edit(shareData*)));
	
	load();
};


KcmSambaConf::~KcmSambaConf() {
}


void KcmSambaConf::load() {
	// insert your loading code here...
}

void KcmSambaConf::defaults() {
	// insert your default settings code here...
	emit changed(true);
}

void KcmSambaConf::save() {
	// insert your saving code here...
	emit changed(true);
}

int KcmSambaConf::buttons () {
	return KCModule::Default|KCModule::Apply|KCModule::Help;
}

void KcmSambaConf::configChanged() {
	// insert your saving code here...
	emit changed(true);
}

QString KcmSambaConf::quickHelp() const
{
	return i18n("Helpful information about the kcmsambaconf module.");
}

// ------------------------------------------------------------------------

extern "C"
{
	KCModule *create_KcmSambaConf(QWidget *parent, const char *name)
	{
		KGlobal::locale()->insertCatalogue("KcmSambaConf");
		return new KcmSambaConf(parent, name);
	}
}

#include "kcmsambaconf.moc"
