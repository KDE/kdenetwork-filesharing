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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

#ifndef PROPSDLGPLUGIN_PROPSDLGSHAREPLUGIN_H
#define PROPSDLGPLUGIN_PROPSDLGSHAREPLUGIN_H

#include <kpropertiesdialog.h>

class PropsDlgSharePlugin : public KPropertiesDialogPlugin
{
    Q_OBJECT

public:
    PropsDlgSharePlugin(KPropertiesDialog *dlg, const QStringList &);
    virtual ~PropsDlgSharePlugin();
    virtual void applyChanges();

protected slots:
    void slotConfigureFileSharing();

private:
    class Private;
    Private *d;
};

#endif // PROPSDLGPLUGIN_PROPSDLGSHAREPLUGIN_H
