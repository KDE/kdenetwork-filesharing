/***************************************************************************
                          kcmsambaconf.h  -  description
                             -------------------
    begin                : Mon Apr  8 13:35:56 CEST 2002
    copyright            : (C) 2002 by Christian Nitschkowski
    email                : segfault_ii@web.de
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
	void updateShare();

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

    void socketOptionsBtnClicked();
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
