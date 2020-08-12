/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2004 Jan Schaefer <j_schaef@informatik.uni-kl.de>
    SPDX-FileCopyrightText: 2011 Rodrigo Belem <rclbelem@gmail.com>
    SPDX-FileCopyrightText: 2019 Nate Graham <nate@kde.org>
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
    ~SambaUserSharePlugin() override = default;
    void applyChanges() override;

private:
    QString m_url;
    KSambaShareData shareData;
    UserPermissionModel *model = nullptr;
    Ui::PropertiesPageGUI propertiesUi;
    QWidget *m_failedSambaWidgets = nullptr;
    QWidget *m_installSambaWidgets = nullptr;
    QWidget *m_shareWidgets = nullptr;
    QLabel *m_sambaStatusMessage = nullptr;
    QProgressBar *m_installProgress = nullptr;
    QPushButton *m_installSambaButton = nullptr;
#ifdef SAMBA_INSTALL
    QWidget *m_justInstalledSambaWidgets = nullptr;
    QPushButton *m_restartButton = nullptr;
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
    static void reboot();
#endif // SAMBA_INSTALL
};

#endif // SAMBAUSERSHAREPLUGIN_H
