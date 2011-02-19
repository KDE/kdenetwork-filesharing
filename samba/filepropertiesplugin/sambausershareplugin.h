/*
  Copyright (c) 2004 Jan Schaefer <j_schaef@informatik.uni-kl.de>
  Copyright (c) 2011 Rodrigo Belem <rclbelem@gmail.com>

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

#ifndef SAMBAUSERSHAREPLUGIN_H
#define SAMBAUSERSHAREPLUGIN_H

#include <QVariant>
#include <QComboBox>

#include <kpropertiesdialog.h>

#include "ui_sambausershareplugin.h"

class UserPermissionModel;

class SambaUserSharePlugin : public KPropertiesDialogPlugin
{
    Q_OBJECT

public:
    SambaUserSharePlugin(QObject *parent, const QList<QVariant> &args);
    virtual ~SambaUserSharePlugin();
    virtual void applyChanges();

private Q_SLOTS:
    void load();
    void toggleShareStatus(bool checked);
    void installSamba();
    void checkShareName(const QString &name);

private:
    QString url;
    KSambaShareData shareData;
    UserPermissionModel *model;
    Ui::PropertiesPageGUI propertiesUi;

    void setupModel();
    void setupViews();
    QStringList getUsersList();
    QString getNewShareName();
};

#endif // SAMBAUSERSHAREPLUGIN_H
