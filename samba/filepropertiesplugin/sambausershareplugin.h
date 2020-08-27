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

#include <memory>

class UserPermissionModel;
class ShareContext;
class UserManager;

class SambaUserSharePlugin : public KPropertiesDialogPlugin
{
    Q_OBJECT
    Q_PROPERTY(bool dirty READ isDirty WRITE setDirty NOTIFY changed) // So qml can mark dirty
    Q_PROPERTY(bool ready READ isReady NOTIFY readyChanged) // intentionally not writable from qml
    // Expose instance-singleton members so QML may access them.
    // They aren't application-wide singletons and also cannot easily be ctor'd from QML.
    Q_PROPERTY(UserManager *userManager MEMBER m_userManager CONSTANT)
    Q_PROPERTY(UserPermissionModel *userPermissionModel MEMBER m_model CONSTANT)
    Q_PROPERTY(ShareContext *shareContext MEMBER m_context CONSTANT)
public:
    SambaUserSharePlugin(QObject *parent, const QList<QVariant> &args);
    ~SambaUserSharePlugin() override = default;
    void applyChanges() override;

    Q_INVOKABLE static bool isSambaInstalled();
    Q_INVOKABLE static void reboot();
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
    // Hold the qquickwidget so it gets destroyed with us. Otherwise we'd have bogus reference errors
    // as the Plugin instance is exposed as contextProperty to qml but the widget is parented to the PropertiesDialog
    // (i.e. our parent). So the lifetime of the widget would usually exceed ours.
    std::unique_ptr<QWidget> m_page = nullptr;
};

#endif // SAMBAUSERSHAREPLUGIN_H
