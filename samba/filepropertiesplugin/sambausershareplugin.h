/*
  Copyright (c) 2004 Jan Schaefer <j_schaef@informatik.uni-kl.de>
  Copyright (c) 2011 Rodrigo Belem <rclbelem@gmail.com>
  Copyright (c) 2019 Nate Graham <nate@kde.org>

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
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#ifdef SAMBA_INSTALL
#include <PackageKit/Daemon>
#include <PackageKit/Transaction>
#endif // SAMBA_INSTALL

#include <kpropertiesdialog.h>
#include <klocalizedstring.h>

#include "ui_sambausershareplugin.h"

#include <KSambaShareData>

class UserPermissionModel;

class SambaUserSharePlugin : public KPropertiesDialogPlugin
{
    Q_OBJECT

public:
    SambaUserSharePlugin(QObject *parent, const QList<QVariant> &args);
    virtual ~SambaUserSharePlugin();
    void applyChanges() override;

private:
    QString m_url;
    KSambaShareData shareData;
    UserPermissionModel *model;
    Ui::PropertiesPageGUI propertiesUi;
    QWidget *m_failedSambaWidgets;
    QWidget *m_installSambaWidgets;
    QWidget *m_shareWidgets;
    QLabel  *m_sambaStatusMessage;
    QProgressBar *m_installProgress;
    QPushButton *m_installSambaButton;
#ifdef SAMBA_INSTALL
    QWidget *m_justInstalledSambaWidgets;
    QPushButton *m_restartButton;
#endif // SAMBA_INSTALL

    void setupModel();
    void setupViews();
    QString getNewShareName() const;
    void load();
    void toggleShareStatus(bool checked);
    void checkShareName(const QString &name);
    void reportAdd(KSambaShareData::UserShareError error);
    void reportRemove(KSambaShareData::UserShareError error);
#ifdef SAMBA_INSTALL
    void installSamba();
    void packageFinished(PackageKit::Transaction::Exit status, uint runtime);
    void reboot();
#endif // SAMBA_INSTALL


};

#endif // SAMBAUSERSHAREPLUGIN_H
