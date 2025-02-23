/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2004 Jan Schaefer <j_schaef@informatik.uni-kl.de>
    SPDX-FileCopyrightText: 2011 Rodrigo Belem <rclbelem@gmail.com>
    SPDX-FileCopyrightText: 2019 Nate Graham <nate@kde.org>
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2021 Slava Aseev <nullptrnine@basealt.ru>
    SPDX-FileCopyrightText: 2025 Thomas Duckworth <tduck@filotimoproject.org>
*/

#ifndef SAMBAUSERSHAREPLUGIN_H
#define SAMBAUSERSHAREPLUGIN_H

#include <KPropertiesDialogPlugin>
#include <KPropertiesDialog>
#include <KSambaShareData>
#include <KSambaShare>
#include <QFileInfo>

#include <memory>
#include "usermanager.h"
#include "model.h"
#include "permissionshelper.h"

class ShareContext : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool canEnableGuest READ canEnableGuest CONSTANT)
    Q_PROPERTY(bool guestEnabled READ guestEnabled WRITE setGuestEnabled NOTIFY guestEnabledChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int maximumNameLength READ maximumNameLength CONSTANT)
    Q_PROPERTY(QString path READ path CONSTANT)
public:
    explicit ShareContext(const QUrl &url, QObject *parent = nullptr)
        : QObject(parent)
        , m_shareData(resolveShare(url))
        , m_enabled(KSambaShare::instance()->isDirectoryShared(m_shareData.path()))
        // url isn't a member. always use .path()!
    {
    }

    bool enabled() const
    {
        return m_enabled;
    }

    void setEnabled(bool enabled)
    {
        m_enabled = enabled;
        Q_EMIT enabledChanged();
    }

    bool canEnableGuest()
    {
        return KSambaShare::instance()->areGuestsAllowed();
    }

    bool guestEnabled() const
    {
        // WTF is that enum even...
        switch (m_shareData.guestPermission()) {
        case KSambaShareData::GuestsNotAllowed:
            return false;
        case KSambaShareData::GuestsAllowed:
            return true;
        }
        Q_UNREACHABLE();
        return false;
    }

    void setGuestEnabled(bool enabled)
    {
        m_shareData.setGuestPermission(enabled ? KSambaShareData::GuestsAllowed : KSambaShareData::GuestsNotAllowed);
        Q_EMIT guestEnabledChanged();
    }

    QString name() const
    {
        return m_shareData.name();
    }

    QString path() const {
        return m_shareData.path();
    }

    void setName(const QString &name)
    {
        m_shareData.setName(name);
        Q_EMIT nameChanged();
    }

    static constexpr int maximumNameLength()
    {
        // Windows 10 allows creating shares with a maximum of 60 characters when measured on 2020-08-13.
        // We consider this kind of a soft limit as there appears to be no actual limit specified anywhere.
        return 60;
    }


    Q_INVOKABLE static bool isNameFree(const QString &name)
    {
        return KSambaShare::instance()->isShareNameAvailable(name);
    }

public Q_SLOTS:
    QString newShareName(const QUrl &url)
    {
        Q_ASSERT(url.isValid());
        Q_ASSERT(!url.isEmpty());
        // TODO pretty sure this is buggy for urls with trailing slash where filename would be ""
        return url.fileName().left(maximumNameLength());
    }

Q_SIGNALS:
    void enabledChanged();
    void guestEnabledChanged();
    void nameChanged();

private:
    KSambaShareData resolveShare(const QUrl &url)
    {
        QFileInfo info(url.toLocalFile());
        const QString path = info.canonicalFilePath();
        Q_ASSERT(!path.isEmpty());
        const QList<KSambaShareData> shareList = KSambaShare::instance()->getSharesByPath(path);
        if (!shareList.isEmpty()) {
            return shareList.first(); // FIXME: using just the first in the list for a while
        }
        KSambaShareData newShare;
        newShare.setName(newShareName(url));
        newShare.setGuestPermission(KSambaShareData::GuestsNotAllowed);
        newShare.setPath(path);
        return newShare;
    }

public:
    // TODO shouldn't be public may need refactoring though because the ACL model needs an immutable copy
    KSambaShareData m_shareData;
private:
    bool m_enabled = false; // this gets cached so we can manipulate its state from qml
};


class SambaUserSharePlugin : public KPropertiesDialogPlugin
{
    Q_OBJECT
    Q_PROPERTY(bool dirty READ isDirty WRITE setDirty NOTIFY changed) // So qml can mark dirty
    Q_PROPERTY(bool ready READ isReady NOTIFY readyChanged) // intentionally not writable from qml
    Q_PROPERTY(QStringList addressList MEMBER m_addressList NOTIFY addressListChanged)
    // Expose instance-singleton members so QML may access them.
    // They aren't application-wide singletons and also cannot easily be ctor'd from QML.
    Q_PROPERTY(UserManager *userManager MEMBER m_userManager CONSTANT)
    Q_PROPERTY(UserPermissionModel *userPermissionModel MEMBER m_model CONSTANT)
    Q_PROPERTY(ShareContext *shareContext MEMBER m_context CONSTANT)
    Q_PROPERTY(PermissionsHelper *permissionsHelper MEMBER m_permissionsHelper CONSTANT)
    Q_PROPERTY(QString bugReportUrl READ bugReportUrl CONSTANT)

public:
    SambaUserSharePlugin(QObject *parent);
    ~SambaUserSharePlugin() override = default;
    void applyChanges() override;

    Q_INVOKABLE static bool isSambaInstalled();
    Q_INVOKABLE static void reboot();
    Q_INVOKABLE static void showSambaStatus();
    Q_INVOKABLE static void copyAddressToClipboard(const QString &address);

    bool isReady() const;

    QString bugReportUrl() const;

Q_SIGNALS:
    void readyChanged();
    void addressListChanged();

private:
    void setReady(bool ready);
    void reportAdd(KSambaShareData::UserShareError error);
    void reportRemove(KSambaShareData::UserShareError error);

    Q_SLOT void initUserManager();
    Q_SLOT void initAddressList();

    const QString m_url;
    ShareContext *m_context= nullptr;
    UserPermissionModel *m_model = nullptr;
    UserManager *m_userManager = nullptr;
    PermissionsHelper *m_permissionsHelper = nullptr;
    QStringList m_addressList;
    bool m_ready = false;
    // Hold the qquickwidget so it gets destroyed with us. Otherwise we'd have bogus reference errors
    // as the Plugin instance is exposed as contextProperty to qml but the widget is parented to the PropertiesDialog
    // (i.e. our parent). So the lifetime of the widget would usually exceed ours.
    std::unique_ptr<QWidget> m_page = nullptr;
};

#endif // SAMBAUSERSHAREPLUGIN_H
