/*
  Copyright (c) 2004 Jan Schaefer <j_schaef@informatik.uni-kl.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#include <qstring.h>
#include <qvbox.h>

#include <kgenericfactory.h>
#include <kdebug.h>
#include <kfileshare.h>
#include <kmessagebox.h>

#include "propertiespage.h"
#include "propsdlgshareplugin.h"

typedef KGenericFactory<PropsDlgSharePlugin, KPropertiesDialog> PropsDlgSharePluginFactory;

K_EXPORT_COMPONENT_FACTORY( fileshare_propsdlgplugin,
                            PropsDlgSharePluginFactory("fileshare_propsdlgplugin") );

class PropsDlgSharePlugin::Private                            
{
  public:
    PropertiesPage* page; 
};
                            
PropsDlgSharePlugin::PropsDlgSharePlugin( KPropertiesDialog *dlg,
                    const char *, const QStringList & )
  : KPropsDlgPlugin(dlg), d(0)
{
  if (KFileShare::shareMode() == KFileShare::Simple) {     
      kdDebug() << "PropsDlgSharePlugin: Sharing mode is simple. Aborting." << endl;
      return;
  }   
       
  d = new Private();

  QVBox* vbox = properties->addVBoxPage(i18n("&Share"));                            
  properties->setFileSharingPage(vbox);
  
  d->page = new PropertiesPage(vbox, properties->items());
  connect(d->page, SIGNAL(changed()), 
          this, SIGNAL(changed()));
  
  kdDebug() << "Fileshare properties dialog plugin loaded" << endl;
  
}                            

PropsDlgSharePlugin::~PropsDlgSharePlugin()
{
  delete d;
}

void PropsDlgSharePlugin::applyChanges() 
{
  if (!d->page->save()) {
    KMessageBox::sorry(d->page,
                i18n("Saving the changes failed"));

    properties->abortApplying();
  }
}


#include "propsdlgshareplugin.moc"

