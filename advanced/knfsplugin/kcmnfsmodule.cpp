/***************************************************************************
                          kcmnfsmodule.h  -  description
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

#include <qlayout.h>
#include <qfileinfo.h>
#include <qstring.h>

#include <kurl.h>
#include <kapplication.h>
#include <kglobal.h>
#include <kconfig.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kdebug.h>

#include "nfsfile.h"
#include "cmdialogimpl.h"
#include "nfsconfigwidget.h"

#include "kcmnfsmodule.h"

#include <kgenericfactory.h>


typedef KGenericFactory<KCmNfsModule, QWidget> NFSModuleFactory;
 K_EXPORT_COMPONENT_FACTORY( kcm_kcmnfsmodule, NFSModuleFactory("kcmnfsmodule"));


KCmNfsModule::KCmNfsModule(QWidget *parent,const char * name, const QStringList &)
	:KCModule(NFSModuleFactory::instance(),parent,name)
{
	load();
}

void KCmNfsModule::exportsSpecifiedSlot(QString newExports)
{
	if ( QFileInfo(newExports).exists() )
  {
  	kapp->config()->setGroup("KNFSPlugin");
    kapp->config()->writeEntry("exports",newExports);
		kapp->config()->sync();

    if (_cmDlg)
			delete _cmDlg;

		if (_configWidget)
    {
  		delete _configWidget;
      _configWidget = 0L;
    }

    _exports = newExports;
		_file = new NFSFile(KURL(_exports), true);
		_cmDlg = new CMDialogImpl(this,_file);

    connect(_cmDlg, SIGNAL(changed(bool)), this, SLOT(setModified()));
  }
}


KCmNfsModule::~KCmNfsModule()
{
}

void KCmNfsModule::setModified()
{
	emit changed(true);
}

QString KCmNfsModule::findExports()
{
	kapp->config()->setGroup("KNFSPlugin");
  QString tmp = kapp->config()->readEntry("exports");

  if (QFileInfo(tmp).exists())
  	 return tmp;

  if (NFSFile::guessPath() != "")
  	  return NFSFile::guessPath();

  return "";
}


void KCmNfsModule::load()
{
	QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setAutoAdd(true);

  _exports = findExports();
//	_exports = "";

  if (_exports == "")
  {
		_cmDlg = 0L;
    _configWidget = new NFSConfigWidget(this);
    connect(_configWidget, SIGNAL(exportsSpecified(QString)),
    			  this, SLOT(exportsSpecifiedSlot(QString)));
  }
  else
  {
  	_file = new NFSFile(KURL(_exports), true);
	  _file->load();

  	_configWidget = 0L;
	  _cmDlg = new CMDialogImpl(this,_file);
    connect(_cmDlg, SIGNAL(changed(bool)), this, SLOT(setModified()));

    //_cmDlg->show();

  }

}

void KCmNfsModule::save()
{
	_file->save();
}

void KCmNfsModule::defaults()
{
}

QString KCmNfsModule::quickHelp() const
{
	return i18n("Helpful information about the kcmnfs module.");
}

const KAboutData* KCmNfsModule::aboutData() const
{
	KAboutData * about = new KAboutData("kcmnfs",i18n("KNFSPlugin"),"0.1a");
  
  return about;
}


/*
extern "C"
{
	KCModule *create_KCmNfsModule(QWidget *parent, const char *name, const QStringList & list)
	{
		KGlobal::locale()->insertCatalogue("KCmNfsModule");
		return new KCmNfsModule(parent, name,list);
	}
}
*/


#include "kcmnfsmodule.moc"
