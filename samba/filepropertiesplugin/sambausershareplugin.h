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
#include <QProgressBar>
#include <QPushButton>
#ifdef SAMBA_INSTALL
#include <PackageKit/Daemon>
#include <PackageKit/Transaction>
#endif // SAMBA_INSTALL

#include <kpropertiesdialog.h>
#include <klocalizedstring.h>

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
    void checkShareName(const QString &name);
#ifdef SAMBA_INSTALL
    void installSamba();
    void packageInstall(PackageKit::Transaction::Info info,
                        const QString &packageId,
                        const QString &summary);
    void packageFinished(PackageKit::Transaction::Exit status, uint runtime);
#endif // SAMBA_INSTALL

private:
    QString m_url;
    KSambaShareData shareData;
    UserPermissionModel *model;
    Ui::PropertiesPageGUI propertiesUi;
    QWidget *m_installSambaWidgets;
    QWidget *m_shareWidgets;
    QProgressBar *m_installProgress;
    QPushButton *m_installSambaButton;

    void setupModel();
    void setupViews();
    QStringList getUsersList();
    QString getNewShareName();

};

#endif // SAMBAUSERSHAREPLUGIN_H
