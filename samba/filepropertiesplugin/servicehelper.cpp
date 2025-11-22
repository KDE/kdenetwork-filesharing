/*
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 * SPDX-FileCopyrightText: 2025 Thomas Duckworth <tduck@filotimoproject.org>
 * SPDX-FileCopyrightText: 2025 Nate Graham <nate@kde.org>
 */

#include "servicehelper.h"

#include <KAuth/Action>
#include <KAuth/ExecuteJob>
#include <KLocalizedString>
#include <KOSRelease>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QDBusVariant>
#include <QDebug>
#include <QTimer>

#include <QCoro/QCoroCore>
#include <QCoro/QCoroDBus>

#include "servicedefinitions.h"
#include "systemd_manager.h"

using namespace std::chrono_literals;

namespace
{
QCoro::Task<QString> findAvailableService()
{
    // The smb service can be named "smbd.service" on some systems, e.g. Debian.
    // Ensure the correct service name is used.
    OrgFreedesktopSystemd1ManagerInterface manager(DBUS_SYSTEMD_SERVICE, DBUS_SYSTEMD_PATH, QDBusConnection::systemBus());
    auto candidates = {QStringLiteral("smb.service"), QStringLiteral("smbd.service")};

    for (const QString &candidate : candidates) {
        // If this call doesn't return an error, it means the unit file exists, even if it's disabled.
        auto reply = co_await manager.GetUnitFileState(candidate);

        if (!reply.isError()) {
            co_return candidate;
        }
    }
    co_return QString();
}
}

ServiceHelper::ServiceHelper(QObject *parent)
    : QObject(parent)
{
}

bool ServiceHelper::isEnabling() const
{
    return m_enabling;
}

bool ServiceHelper::hasFailed() const
{
    return m_failed;
}

QString ServiceHelper::errorMessage() const
{
    return m_errorMessage;
}

void ServiceHelper::setEnabling(bool enabling)
{
    if (m_enabling == enabling) {
        return;
    }
    m_enabling = enabling;
    Q_EMIT enablingChanged();
}

void ServiceHelper::setFailed(bool failed)
{
    if (m_failed == failed) {
        return;
    }
    m_failed = failed;
    Q_EMIT failedChanged();
}

void ServiceHelper::setErrorMessage(const QString &message)
{
    if (m_errorMessage == message) {
        return;
    }
    m_errorMessage = message;
    Q_EMIT errorMessageChanged();
}

QCoro::Task<bool> ServiceHelper::isServiceReady()
{
    QString serviceName = co_await findAvailableService();
    if (serviceName.isEmpty()) {
        co_return false;
    }

    OrgFreedesktopSystemd1ManagerInterface manager(DBUS_SYSTEMD_SERVICE, DBUS_SYSTEMD_PATH, QDBusConnection::systemBus());

    // Check if it's enabled.
    auto stateReply = co_await manager.GetUnitFileState(serviceName);

    if (stateReply.isError()) {
        co_return false;
    }

    bool isEnabled = (stateReply.value() == QStringLiteral("enabled"));
    if (!isEnabled) {
        co_return false;
    }

    // Now check if it's running.
    auto runningReply = co_await manager.LoadUnit(serviceName);

    if (runningReply.isError()) {
        co_return false;
    }

    QString unitPath = runningReply.value().path();
    if (unitPath.isEmpty()) {
        co_return false;
    }

    auto propMsg = QDBusMessage::createMethodCall(DBUS_SYSTEMD_SERVICE, unitPath, QStringLiteral("org.freedesktop.DBus.Properties"), QStringLiteral("Get"));
    propMsg << QStringLiteral("org.freedesktop.systemd1.Unit") << QStringLiteral("ActiveState");
    auto propReply = co_await QDBusConnection::systemBus().asyncCall(propMsg);

    if (propReply.type() == QDBusMessage::ErrorMessage || propReply.arguments().isEmpty()) {
        co_return false;
    }

    QString state = propReply.arguments().at(0).value<QDBusVariant>().variant().toString();
    co_return (state == QStringLiteral("active"));
}

void ServiceHelper::setup()
{
    setupCoroutine();
}
QCoro::Task<bool> ServiceHelper::checkSystemdEnabledState(const QString &serviceName)
{
    OrgFreedesktopSystemd1ManagerInterface manager(DBUS_SYSTEMD_SERVICE, DBUS_SYSTEMD_PATH, QDBusConnection::systemBus());
    auto stateReply = co_await manager.GetUnitFileState(serviceName);

    if (!stateReply.isError()) {
        co_return (stateReply.value() == QStringLiteral("enabled"));
    }
    co_return false;
}

QCoro::Task<bool> ServiceHelper::checkSystemdRunningState(const QString &serviceName)
{
    OrgFreedesktopSystemd1ManagerInterface manager(DBUS_SYSTEMD_SERVICE, DBUS_SYSTEMD_PATH, QDBusConnection::systemBus());
    auto loadReply = co_await manager.LoadUnit(serviceName);

    if (loadReply.isError()) {
        co_return false;
    }

    m_currentUnitPath = loadReply.value().path();
    if (m_currentUnitPath.isEmpty()) {
        co_return false;
    }

    // Check ActiveState property
    QDBusMessage propMsg =
        QDBusMessage::createMethodCall(DBUS_SYSTEMD_SERVICE, m_currentUnitPath, QStringLiteral("org.freedesktop.DBus.Properties"), QStringLiteral("Get"));
    propMsg << QStringLiteral("org.freedesktop.systemd1.Unit") << QStringLiteral("ActiveState");

    auto propReply = co_await QDBusConnection::systemBus().asyncCall(propMsg);

    if (propReply.type() != QDBusMessage::ErrorMessage && !propReply.arguments().isEmpty()) {
        QString state = propReply.arguments().at(0).value<QDBusVariant>().variant().toString();
        co_return (state == QStringLiteral("active"));
    }

    co_return false;
}

QCoro::Task<bool> ServiceHelper::performEnableAction()
{
    KAuth::Action enableserviceAction(QStringLiteral("org.kde.filesharing.samba.enableservice"));
    enableserviceAction.setHelperId(QStringLiteral("org.kde.filesharing.samba"));
    KAuth::ExecuteJob *job = enableserviceAction.execute();

    job->start();
    co_await qCoro(job, &KJob::result);

    if (job->error()) {
        setErrorMessage(xi18nc("@info",
                               "Could not enable the Samba service:<nl/>%1<nl/><nl/>Ensure the Samba file sharing service is installed and enabled.",
                               job->errorString()));
        co_return false;
    }
    co_return true;
}

QCoro::Task<bool> ServiceHelper::performStartAction()
{
    // Connect the signal before starting to avoid a race where we are waiting for a signal that has already arrived.
    QDBusConnection::systemBus().connect(DBUS_SYSTEMD_SERVICE,
                                         m_currentUnitPath,
                                         QStringLiteral("org.freedesktop.DBus.Properties"),
                                         QStringLiteral("PropertiesChanged"),
                                         this,
                                         SLOT(onSystemdUnitPropertiesChanged(QString, QVariantMap, QStringList)));

    KAuth::Action startserviceAction(QStringLiteral("org.kde.filesharing.samba.startservice"));
    startserviceAction.setHelperId(QStringLiteral("org.kde.filesharing.samba"));
    KAuth::ExecuteJob *job = startserviceAction.execute();

    job->start();
    co_await qCoro(job, &KJob::result);

    if (job->error()) {
        setErrorMessage(xi18nc("@info",
                               "Could not start the Samba service:<nl/>%1<nl/><nl/>Ensure the Samba file sharing service is installed and enabled.",
                               job->errorString()));
        co_return false;
    }
    co_return true;
}

QCoro::Task<void> ServiceHelper::setupCoroutine()
{
    if (m_enabling) {
        co_return;
    }
    setEnabling(true);
    setFailed(false);
    setErrorMessage(QString());

    // First, find the service.
    m_serviceName = co_await findAvailableService();

    if (m_serviceName.isEmpty()) {
        setErrorMessage(
            xi18nc("@info",
                   "No Samba file sharing service was found. This is a packaging issue, and should be reported to %1 on <link url='%2'>their bug reporting "
                   "website.</link>",
                   KOSRelease().name(),
                   KOSRelease().bugReportUrl()));
        setFailed(true);
        setEnabling(false);
        co_return;
    }

    // Then, check its status.
    bool currentEnabled = co_await checkSystemdEnabledState(m_serviceName);
    bool currentRunning = co_await checkSystemdRunningState(m_serviceName);

    // Enable it, if necessary.
    if (!currentEnabled) {
        bool enableSuccess = co_await performEnableAction();
        if (!enableSuccess) {
            setFailed(true);
            setEnabling(false);
            co_return;
        }
    }

    // Start it, if necessary.
    if (!currentRunning) {
        bool startSuccess = co_await performStartAction();
        if (!startSuccess) {
            setFailed(true);
            setEnabling(false);
            co_return;
        }
    }

    // Wait for active state if we had to start it.
    if (!currentRunning) {
        m_timeoutTimer = std::make_unique<QTimer>(new QTimer(this));
        m_timeoutTimer->setSingleShot(true);
        connect(m_timeoutTimer.get(), &QTimer::timeout, this, &ServiceHelper::onSystemdTimeout);
        m_timeoutTimer->start(10000);
    } else {
        setEnabling(false);
    }
}

void ServiceHelper::cleanupConnections()
{
    if (m_timeoutTimer) {
        m_timeoutTimer->stop();
        m_timeoutTimer->deleteLater();
        m_timeoutTimer = nullptr;
    }

    QDBusConnection::systemBus().disconnect(DBUS_SYSTEMD_SERVICE,
                                            m_currentUnitPath,
                                            QStringLiteral("org.freedesktop.DBus.Properties"),
                                            QStringLiteral("PropertiesChanged"),
                                            this,
                                            SLOT(onSystemdUnitPropertiesChanged(QString, QVariantMap, QStringList)));
}

void ServiceHelper::onSystemdUnitPropertiesChanged(const QString &interface, const QVariantMap &changed, const QStringList &invalidated)
{
    Q_UNUSED(invalidated);

    if (interface != QStringLiteral("org.freedesktop.systemd1.Unit")) {
        return;
    }

    if (changed.contains(QStringLiteral("ActiveState"))) {
        const QString state = changed.value(QStringLiteral("ActiveState")).toString();
        if (state == QStringLiteral("active")) {
            cleanupConnections();
            setEnabling(false);
        }
    }
}

void ServiceHelper::onSystemdTimeout()
{
    cleanupConnections();

    setErrorMessage(xi18nc("@info",
                           "Timed out waiting for <command>%1</command> to start.<nl/><nl/>Ensure the Samba file sharing service is installed and enabled.",
                           m_serviceName));

    setFailed(true);
    setEnabling(false);
}
