/***************************************************************************
                          kcmsambaconf.h  -  description
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
#ifndef KCMSAMBACONF_H_
#define KCMSAMBACONF_H_

#include <kcmodule.h>
#include <kaboutdata.h>
#include <kcminterface.h>
#include <share.h>
#include <qptrlist.h>

class KcmSambaConf: public KCModule
{
	Q_OBJECT
	public:
		KcmSambaConf(QWidget *parent = 0L, const char *name = 0L);
		virtual ~KcmSambaConf();

		void load();
		void save();
		void defaults();
		int buttons();
		QString quickHelp() const;
		const KAboutData* aboutData() {return myAboutData; };

	public slots:
		void configChanged();

	private:
		/** The add/edit-share dialog. */
		KcmShareDlg* shareDialog;
		KAboutData *myAboutData;
		KcmInterface* interface;
};

#endif
