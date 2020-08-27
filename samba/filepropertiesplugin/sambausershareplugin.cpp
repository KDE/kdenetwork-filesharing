/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2004 Jan Schaefer <j_schaef@informatik.uni-kl.de>
    SPDX-FileCopyrightText: 2011 Rodrigo Belem <rclbelem@gmail.com>
    SPDX-FileCopyrightText: 2015-2020 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2019 Nate Graham <nate@kde.org>
*/

#include "sambausershareplugin.h"

#include <QFileInfo>
#include <QDebug>
#include <QQmlApplicationEngine>
#include <QQuickWidget>
#include <QQuickItem>
#include <KDeclarative/KDeclarative>
#include <QMetaMethod>
#include <QVBoxLayout>
#include <KLocalizedString>
#include <QTimer>
#include <QQmlContext>
#include <QPushButton>
#include <QDBusInterface>
#include <QDBusConnection>

#include <KMessageBox>
#include <KPluginFactory>
#include <KPluginLoader>
#include <KSambaShare>
#include <KSambaShareData>
#include <KService>
#include <KIO/ApplicationLauncherJob>

#include "model.h"
#include "usermanager.h"
#include "groupmanager.h"

#ifdef SAMBA_INSTALL
#include "sambainstaller.h"
#endif

K_PLUGIN_FACTORY(SambaUserSharePluginFactory, registerPlugin<SambaUserSharePlugin>();)

class ShareContext : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool canEnableGuest READ canEnableGuest CONSTANT)
    Q_PROPERTY(bool guestEnabled READ guestEnabled WRITE setGuestEnabled NOTIFY guestEnabledChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int maximumNameLength READ maximumNameLength CONSTANT)
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

SambaUserSharePlugin::SambaUserSharePlugin(QObject *parent, const QList<QVariant> &args)
    : KPropertiesDialogPlugin(qobject_cast<KPropertiesDialog *>(parent))
    , m_url(properties->item().mostLocalUrl().toLocalFile())
    , m_userManager(new UserManager(this))
{
    Q_UNUSED(args)

    if (m_url.isEmpty()) {
        return;
    }

    const QFileInfo pathInfo(m_url);
    if (!pathInfo.permission(QFile::ReadUser | QFile::WriteUser)) {
        return;
    }

    // TODO: this could be made to load delayed via invokemethod. we technically don't need to fully load
    //   the backing data in the ctor, only the qml view with busyindicator
    // TODO: relatedly if we make ShareContext and the Model more async vis a vis construction we can init them from
    //   QML and stop holding them as members in the plugin
    m_context = new ShareContext(properties->item().mostLocalUrl(), this);
    // FIXME maybe the manager ought to be owned by the model
    qmlRegisterAnonymousType<UserManager>("org.kde.filesharing.samba", 1);
    qmlRegisterAnonymousType<User>("org.kde.filesharing.samba", 1);
    m_model = new UserPermissionModel(m_context->m_shareData, m_userManager, this);

#ifdef SAMBA_INSTALL
    qmlRegisterType<SambaInstaller>("org.kde.filesharing.samba", 1, 0, "Installer");
#endif
    qmlRegisterType<GroupManager>("org.kde.filesharing.samba", 1, 0, "GroupManager");
    // Need access to the column enum, so register this as uncreatable.
    qmlRegisterUncreatableType<UserPermissionModel>("org.kde.filesharing.samba", 1, 0, "UserPermissionModel",
                                                    QStringLiteral("Access through sambaPlugin.userPermissionModel"));
    qmlRegisterAnonymousType<ShareContext>("org.kde.filesharing.samba", 1);
    qmlRegisterAnonymousType<SambaUserSharePlugin>("org.kde.filesharing.samba", 1);

    m_page.reset(new QWidget(qobject_cast<KPropertiesDialog *>(parent)));
    m_page->setAttribute(Qt::WA_TranslucentBackground);
    auto widget = new QQuickWidget(m_page.get());
    // Load kdeclarative and set translation domain before setting the source so strings gets translated.
    KDeclarative::KDeclarative kdeclarative;
    kdeclarative.setDeclarativeEngine(widget->engine());
    kdeclarative.setTranslationDomain(QStringLiteral(TRANSLATION_DOMAIN));
    kdeclarative.setupEngine(widget->engine());
    kdeclarative.setupContext();
    widget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    widget->setFocusPolicy(Qt::StrongFocus);
    widget->setAttribute(Qt::WA_AlwaysStackOnTop, true);
    widget->quickWindow()->setColor(Qt::transparent);
    auto layout = new QVBoxLayout(m_page.get());
    layout->addWidget(widget);

    widget->rootContext()->setContextProperty(QStringLiteral("sambaPlugin"), this);

    const QUrl url(QStringLiteral("qrc:/org.kde.filesharing.samba/qml/main.qml"));
    widget->setSource(url);

    auto item = properties->addPage(m_page.get(), i18nc("@title:tab", "Share"));
    if (qEnvironmentVariableIsSet("TEST_FOCUS_SHARE")) {
        QTimer::singleShot(100, [item, this] {
            properties->setCurrentPage(item);
        });
    }

    QTimer::singleShot(0, [this] {
        connect(m_userManager, &UserManager::loaded, this, [this] {
            setReady(true);
        });
        m_userManager->load();
    });
}

bool SambaUserSharePlugin::isSambaInstalled()
{
    return QFile::exists(QStringLiteral("/usr/sbin/smbd"))
        || QFile::exists(QStringLiteral("/usr/local/sbin/smbd"));
}

void SambaUserSharePlugin::showSambaStatus()
{
    KService::Ptr kcm = KService::serviceByStorageId(QStringLiteral("smbstatus"));
    if (!kcm) {
        // TODO: meh - we have no availability handling. I may have a handy class in plasma-disks
        return;
    }
    KIO::ApplicationLauncherJob(kcm).start();
}

void SambaUserSharePlugin::applyChanges()
{
    qDebug() << "!!! applying changes !!!" << m_context->enabled() << m_context->name() << m_context->guestEnabled() << m_model->getAcl() << m_context->m_shareData.path();
    if (!m_context->enabled()) {
        reportRemove(m_context->m_shareData.remove());
        return;
    }

    // TODO: should run this through reportAdd() as well, ACLs may be invalid and then we shouldn't try to save
    m_context->m_shareData.setAcl(m_model->getAcl());
    reportAdd(m_context->m_shareData.save());
}

static QString errorToString(KSambaShareData::UserShareError error)
{
    // KSambaShare is a right mess. Every function with an error returns the same enum but can only return a subset of
    // possible values. Even so, because it returns the enum we had best mapped all values to semi-suitable string
    // representations even when those are utter garbage when they require specific (e.g. an invalid ACL) that
    // we do not have here.
    switch (error) {
    case KSambaShareData::UserShareNameOk: Q_FALLTHROUGH();
    case KSambaShareData::UserSharePathOk: Q_FALLTHROUGH();
    case KSambaShareData::UserShareAclOk: Q_FALLTHROUGH();
    case KSambaShareData::UserShareCommentOk: Q_FALLTHROUGH();
    case KSambaShareData::UserShareGuestsOk: Q_FALLTHROUGH();
    case KSambaShareData::UserShareOk:
        // Technically anything but UserShareOk cannot happen, but best handle everything regardless.
        return QString();
    case KSambaShareData::UserShareExceedMaxShares:
        return i18nc("@info detailed error messsage",
                     "You have exhausted the maximum amount of shared directories you may have active at the same time.");
    case KSambaShareData::UserShareNameInvalid:
        return i18nc("@info detailed error messsage", "The share name is invalid.");
    case KSambaShareData::UserShareNameInUse:
        return i18nc("@info detailed error messsage", "The share name is already in use for a different directory.");
    case KSambaShareData::UserSharePathInvalid:
        return i18nc("@info detailed error messsage", "The path is invalid.");
    case KSambaShareData::UserSharePathNotExists:
        return i18nc("@info detailed error messsage", "The path does not exist.");
    case KSambaShareData::UserSharePathNotDirectory:
        return i18nc("@info detailed error messsage", "The path is not a directory.");
    case KSambaShareData::UserSharePathNotAbsolute:
        return i18nc("@info detailed error messsage", "The path is relative.");
    case KSambaShareData::UserSharePathNotAllowed:
        return i18nc("@info detailed error messsage", "This path may not be shared.");
    case KSambaShareData::UserShareAclInvalid:
        return i18nc("@info detailed error messsage", "The access rule is invalid.");
    case KSambaShareData::UserShareAclUserNotValid:
        return i18nc("@info detailed error messsage", "An access rule's user is not valid.");
    case KSambaShareData::UserShareGuestsInvalid:
        return i18nc("@info detailed error messsage", "The 'Guest' access rule is invalid.");
    case KSambaShareData::UserShareGuestsNotAllowed:
        return i18nc("@info detailed error messsage", "Enabling guest access is not allowed.");
    case KSambaShareData::UserShareSystemError:
        return KSambaShare::instance()->lastSystemErrorString().simplified();
    }
    Q_UNREACHABLE();
    return QString();
}

void SambaUserSharePlugin::reportAdd(KSambaShareData::UserShareError error)
{
    if (error == KSambaShareData::UserShareOk) {
        return;
    }

    QString errorMessage = errorToString(error);
    if (error == KSambaShareData::UserShareSystemError) {
        // System errors are (untranslated) CLI output. Give them localized context.
        errorMessage = xi18nc("@info error in the underlying binaries. %1 is CLI output",
                              "<para>An error occurred while trying to share the directory."
                              " The share has not been created.</para>"
                              "<para>Samba internals report:</para><message>%1</message>",
                              errorMessage);
    }
    KMessageBox::error(qobject_cast<QWidget *>(parent()),
                       errorMessage,
                       i18nc("@info/title", "Failed to Create Network Share"));
}

void SambaUserSharePlugin::reportRemove(KSambaShareData::UserShareError error)
{
    if (error == KSambaShareData::UserShareOk) {
        return;
    }

    QString errorMessage = errorToString(error);
    if (error == KSambaShareData::UserShareSystemError) {
        // System errors are (untranslated) CLI output. Give them localized context.
        errorMessage = xi18nc("@info error in the underlying binaries. %1 is CLI output",
                              "<para>An error occurred while trying to un-share the directory."
                              " The share has not been removed.</para>"
                              "<para>Samba internals report:</para><message>%1</message>",
                              errorMessage);
    }
    KMessageBox::error(qobject_cast<QWidget *>(parent()),
                       errorMessage,
                       i18nc("@info/title", "Failed to Remove Network Share"));
}

bool SambaUserSharePlugin::isReady() const
{
    return m_ready;
}

void SambaUserSharePlugin::setReady(bool ready)
{
    m_ready = ready;
    Q_EMIT readyChanged();
}

void SambaUserSharePlugin::reboot()
{
    QDBusInterface interface(QStringLiteral("org.kde.ksmserver"), QStringLiteral("/KSMServer"),
                                QStringLiteral("org.kde.KSMServerInterface"), QDBusConnection::sessionBus());
    interface.asyncCall(QStringLiteral("logout"), 0, 1, 2); // Options: do not ask again | reboot | force
}

#include "sambausershareplugin.moc"
