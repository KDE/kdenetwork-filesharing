/***************************************************************************
                          nfspropsdlgplugin.cpp  -  description
                             -------------------
    begin                : Don Apr 25 2002
    copyright            : (C) 2002 by Jan Schäfer
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

#include <qstringlist.h>
#include <qwidgetstack.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qgroupbox.h>

#include <klocale.h>
#include <kgenericfactory.h>
#include <kconfig.h>
#include <kfiledialog.h>
#include <kapplication.h>

#include "nfspropsdlgplugin.h"
#include "nfsdialogimpl.h"
#include "nfsfile.h"
#include "nfsconfigwidget.h"

typedef KGenericFactory<NFSPropsDlgPlugin, KPropertiesDialog> NFSPropsDlgPluginFactory;

K_EXPORT_COMPONENT_FACTORY( knfskonqiplugin,
                            NFSPropsDlgPluginFactory("knfskonqiplugin") );

NFSPropsDlgPlugin::NFSPropsDlgPlugin(KPropertiesDialog *dlg,const char *, const QStringList &)
: KPropsDlgPlugin(dlg)
{
  _dialog = dlg;

  QWidget *w = _dialog->addPage(i18n("&NFS"));

	_stack = new QWidgetStack(w);

  QVBoxLayout *stackLayout = new QVBoxLayout(w);
  stackLayout->addWidget(_stack);

  exports  = findExports();

  if (exports == "")
  {
     nfsWidget = new QWidget(_stack);

	   configWidget = new NFSConfigWidget(_stack);
     connect(configWidget, SIGNAL(exportsSpecified(QString)),
     				 this, SLOT( exportsSpecifiedSlot(QString)));

     _stack->addWidget(nfsWidget,0);
     _stack->addWidget(configWidget,1);
     _stack->raiseWidget(configWidget);
  }
  else
  {
	   nfsWidget = createNFSWidget(_stack);
     _stack->addWidget(nfsWidget,0);
  	 _stack->raiseWidget(nfsWidget);
  }



}

NFSPropsDlgPlugin::~NFSPropsDlgPlugin()
{
}

QString NFSPropsDlgPlugin::findExports()
{
	kapp->config()->setGroup("KNFSPlugin");
  QString tmp = kapp->config()->readEntry("exports");

  if (QFileInfo(tmp).exists())
  	 return tmp;

  if (QFileInfo("/etc/exports").exists())
     return "/etc/exports";

  return "";
}


void NFSPropsDlgPlugin::exportsSpecifiedSlot(QString newExports)
{
	if ( QFileInfo(exports).exists() )
  {
  	kapp->config()->setGroup("KNFSPlugin");
    kapp->config()->writeEntry("exports",exports);
		kapp->config()->sync();

		delete nfsWidget;
    nfsWidget = createNFSWidget(_stack);
     _stack->addWidget(nfsWidget,0);
    _stack->raiseWidget(nfsWidget);
  }
}

void NFSPropsDlgPlugin::applyChanges()
{
  nfsFile->save();
}

QWidget* NFSPropsDlgPlugin::createNFSWidget(QWidget* parent)
{
  nfsFile = new NFSFile(KURL(exports));
  nfsFile->load();

  QString path = _dialog->kurl().path();

  NFSEntry* nfsEntry = nfsFile->getEntryByPath(path);

  NFSDialogImpl *nfsTab;

  if (nfsEntry)
     nfsTab = new NFSDialogImpl(parent,nfsEntry);
  else
     nfsTab = new NFSDialogImpl(parent,nfsFile,path);

  connect(nfsTab, SIGNAL(modified()), this, SLOT(setDirty()));

  return nfsTab;
}





#include "nfspropsdlgplugin.moc"
