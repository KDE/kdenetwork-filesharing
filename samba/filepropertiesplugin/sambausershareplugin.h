/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2004 Jan Schaefer <j_schaef@informatik.uni-kl.de>
    SPDX-FileCopyrightText: 2011 Rodrigo Belem <rclbelem@gmail.com>
    SPDX-FileCopyrightText: 2019 Nate Graham <nate@kde.org>
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
*/

#ifndef SAMBAUSERSHAREPLUGIN_H
#define SAMBAUSERSHAREPLUGIN_H

#include <kpropertiesdialog.h>
#include <KSambaShareData>

class UserPermissionModel;
class ShareContext;
class UserManager;

class SambaUserSharePlugin : public KPropertiesDialogPlugin
{
    Q_OBJECT
    Q_PROPERTY(bool dirty READ isDirty WRITE setDirty NOTIFY changed) // So qml can mark dirty
    Q_PROPERTY(bool ready READ isReady NOTIFY readyChanged) // intentionally not writable from qml
public:
    SambaUserSharePlugin(QObject *parent, const QList<QVariant> &args);
    ~SambaUserSharePlugin() override = default;
    void applyChanges() override;

    Q_INVOKABLE static bool isSambaInstalled();
    Q_INVOKABLE static void showSambaStatus();

    bool isReady() const;

Q_SIGNALS:
    void readyChanged();

private:
    void setReady(bool ready);
    void reportAdd(KSambaShareData::UserShareError error);
    void reportRemove(KSambaShareData::UserShareError error);

    const QString m_url;
    ShareContext *m_context= nullptr;
    UserPermissionModel *m_model = nullptr;
    UserManager *m_userManager = nullptr;
    bool m_ready = false;
};

#endif // SAMBAUSERSHAREPLUGIN_H
