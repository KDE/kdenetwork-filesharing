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
#include <qlistview.h>

class SambaShare;
class SambaFile;
class QPixmap;
/**
 * A QListViewItem which holds a SambaShare object
 **/
class ShareListViewItem : public QListViewItem
{
public:
 	ShareListViewItem(QListView * parent, SambaShare* share);

	SambaShare* getShare() const;
  void setShare(SambaShare* share);

protected:
 	SambaShare* _share;
  QPixmap createPropertyPixmap();
};


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
		const KAboutData* aboutData() {return _myAboutData; };

	public slots:
		void configChanged();

		void editShare();
    void addShare();
    void removeShare();

    void editPrinter();
    void addPrinter();
    void removePrinter();
    
    void editShareDefaults();
    void editPrinterDefaults();

	protected:
  	/**
     * The path of the smb.conf file
     **/
  	QString _smbconf;
		SambaFile* _sambaFile;

  private:
		KAboutData* _myAboutData;
		KcmInterface* _interface;
};

#endif
