/*
     SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
     SPDX-FileCopyrightText: 2025 Thomas Duckworth <tduck@filotimoproject.org>
     SPDX-FileCopyrightText: 2025 Nate Graham <nate@kde.org>
 */

#include "servicehelper.h"

#include <KAuth/Action>
#include <KAuth/ExecuteJob>
#include <KLocalizedString>
#include <KOSRelease>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusVariant>
#include <QDebug>
#include <QTimer>

static const QString DBUS_SYSTEMD_SERVICE = QStringLiteral("org.freedesktop.systemd1");
static const QString DBUS_SYSTEMD_PATH = QStringLiteral("/org/freedesktop/systemd1");
static const QString DBUS_SYSTEMD_MANAGER_INTERFACE = QStringLiteral("org.freedesktop.systemd1.Manager");

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

QString ServiceHelper::findAvailableService()
{
    const QStringList candidates = {QStringLiteral("smb.service"), QStringLiteral("smbd.service")};
    for (const QString &candidate : candidates) {
        QDBusMessage dbusMessage =
            QDBusMessage::createMethodCall(DBUS_SYSTEMD_SERVICE, DBUS_SYSTEMD_PATH, DBUS_SYSTEMD_MANAGER_INTERFACE, QStringLiteral("GetUnitFileState"));

        dbusMessage << candidate;

        QDBusMessage dbusReply = QDBusConnection::systemBus().call(dbusMessage);

        if (dbusReply.type() != QDBusMessage::ErrorMessage) {
            return candidate;
        }
    }

    return QString();
}

bool ServiceHelper::isServiceReady()
{
    QString serviceName = findAvailableService();
    if (serviceName.isEmpty()) {
        return false;
    }

    // Check enabled
    QDBusMessage getStateMsg =
        QDBusMessage::createMethodCall(DBUS_SYSTEMD_SERVICE, DBUS_SYSTEMD_PATH, DBUS_SYSTEMD_MANAGER_INTERFACE, QStringLiteral("GetUnitFileState"));
    getStateMsg << serviceName;
    QDBusMessage getStateReply = QDBusConnection::systemBus().call(getStateMsg);

    if (getStateReply.type() == QDBusMessage::ErrorMessage || getStateReply.arguments().isEmpty()) {
        return false;
    }

    bool isEnabled = (getStateReply.arguments().at(0).toString() == QStringLiteral("enabled"));
    if (!isEnabled) {
        return false;
    }

    // Check running
    QDBusMessage runningMsg =
        QDBusMessage::createMethodCall(DBUS_SYSTEMD_SERVICE, DBUS_SYSTEMD_PATH, DBUS_SYSTEMD_MANAGER_INTERFACE, QStringLiteral("GetUnit"));
    runningMsg << serviceName;
    QDBusMessage runningReply = QDBusConnection::systemBus().call(runningMsg);

    if (runningReply.type() == QDBusMessage::ErrorMessage || runningReply.arguments().isEmpty()) {
        return false;
    }

    QString unitPath = runningReply.arguments().at(0).value<QDBusObjectPath>().path();
    if (unitPath.isEmpty()) {
        return false;
    }

    runningMsg = QDBusMessage::createMethodCall(DBUS_SYSTEMD_SERVICE, unitPath, QStringLiteral("org.freedesktop.DBus.Properties"), QStringLiteral("Get"));
    runningMsg << QStringLiteral("org.freedesktop.systemd1.Unit") << QStringLiteral("ActiveState");
    runningReply = QDBusConnection::systemBus().call(runningMsg);

    if (runningReply.type() == QDBusMessage::ErrorMessage || runningReply.arguments().isEmpty()) {
        return false;
    }

    QString state = runningReply.arguments().at(0).value<QDBusVariant>().variant().toString();
    return (state == QStringLiteral("active"));
}

void ServiceHelper::setup()
{
    if (m_enabling) {
        return;
    }
    setEnabling(true);
    setFailed(false);
    setErrorMessage(QString());

    // Re-discover service details since we are an instance now
    m_serviceName = findAvailableService();

    if (m_serviceName.isEmpty()) {
        setErrorMessage(
            xi18nc("@info",
                   "No Samba file sharing service was found. This is a packaging issue, and should be reported to %1 on <link url='%2'>their bug reporting "
                   "website.</link>",
                   KOSRelease().name(),
                   KOSRelease().bugReportUrl()));
        setFailed(true);
        setEnabling(false);
        return;
    }

    bool currentEnabled = false;
    bool currentRunning = false;

    // Check if the service is already enabled.
    QDBusMessage getStateMsg =
        QDBusMessage::createMethodCall(DBUS_SYSTEMD_SERVICE, DBUS_SYSTEMD_PATH, DBUS_SYSTEMD_MANAGER_INTERFACE, QStringLiteral("GetUnitFileState"));
    getStateMsg << m_serviceName;
    QDBusMessage getStateReply = QDBusConnection::systemBus().call(getStateMsg);

    if (getStateReply.type() != QDBusMessage::ErrorMessage && !getStateReply.arguments().isEmpty()) {
        currentEnabled = (getStateReply.arguments().at(0).toString() == QStringLiteral("enabled"));
    }

    // Now, check if the service is already running, and get its unit path.
    QDBusMessage runningMsg =
        QDBusMessage::createMethodCall(DBUS_SYSTEMD_SERVICE, DBUS_SYSTEMD_PATH, DBUS_SYSTEMD_MANAGER_INTERFACE, QStringLiteral("GetUnit"));
    runningMsg << m_serviceName;
    QDBusMessage runningReply = QDBusConnection::systemBus().call(runningMsg);

    if (runningReply.type() != QDBusMessage::ErrorMessage && !runningReply.arguments().isEmpty()) {
        m_currentUnitPath = runningReply.arguments().at(0).value<QDBusObjectPath>().path();

        if (!m_currentUnitPath.isEmpty()) {
            runningMsg = QDBusMessage::createMethodCall(DBUS_SYSTEMD_SERVICE,
                                                        m_currentUnitPath,
                                                        QStringLiteral("org.freedesktop.DBus.Properties"),
                                                        QStringLiteral("Get"));
            runningMsg << QStringLiteral("org.freedesktop.systemd1.Unit") << QStringLiteral("ActiveState");
            runningReply = QDBusConnection::systemBus().call(runningMsg);

            if (runningReply.type() != QDBusMessage::ErrorMessage && !runningReply.arguments().isEmpty()) {
                QString state = runningReply.arguments().at(0).value<QDBusVariant>().variant().toString();
                if (state == QStringLiteral("active")) {
                    currentRunning = true;
                }
            }
        }
    }

    // Enable the service if necessary.
    if (!currentEnabled) {
        KAuth::Action enableserviceAction(QStringLiteral("org.kde.filesharing.samba.enableservice"));
        enableserviceAction.setHelperId(QStringLiteral("org.kde.filesharing.samba"));
        KAuth::ExecuteJob *job = enableserviceAction.execute();

        if (!job->exec()) {
            setErrorMessage(xi18nc("@info",
                                   "Could not enable the Samba service:<nl/>%1<nl/><nl/>Ensure the Samba file sharing service is installed and enabled.",
                                   job->errorString()));
            setFailed(true);
            setEnabling(false);
            return;
        }
    }

    // Then, start the service if necessary.
    if (!currentRunning) {
        KAuth::Action startserviceAction(QStringLiteral("org.kde.filesharing.samba.startservice"));
        startserviceAction.setHelperId(QStringLiteral("org.kde.filesharing.samba"));
        KAuth::ExecuteJob *job = startserviceAction.execute();

        if (!job->exec()) {
            setErrorMessage(xi18nc("@info",
                                   "Could not start the Samba service:<nl/>%1<nl/><nl/>Ensure the Samba file sharing service is installed and enabled.",
                                   job->errorString()));
            setFailed(true);
            setEnabling(false);
            return;
        }
    }

    // Wait for the service to become active.
    if (!currentRunning) {
        // Fetch the unit path again after starting it. It may have been empty beforehand.
        if (m_currentUnitPath.isEmpty()) {
            QDBusMessage refetchMsg =
                QDBusMessage::createMethodCall(DBUS_SYSTEMD_SERVICE, DBUS_SYSTEMD_PATH, DBUS_SYSTEMD_MANAGER_INTERFACE, QStringLiteral("GetUnit"));
            refetchMsg << m_serviceName;
            QDBusMessage refetchReply = QDBusConnection::systemBus().call(refetchMsg);

            if (refetchReply.type() != QDBusMessage::ErrorMessage && !refetchReply.arguments().isEmpty()) {
                m_currentUnitPath = refetchReply.arguments().at(0).value<QDBusObjectPath>().path();
            }
        }

        if (m_currentUnitPath.isEmpty()) {
            // If it's still empty here, something is very wrong with DBus/Systemd
            setErrorMessage(xi18nc("@info", "Service started, but unit path is invalid."));
            setFailed(true);
            setEnabling(false);
            return;
        }

        QDBusConnection::systemBus().connect(DBUS_SYSTEMD_SERVICE,
                                             m_currentUnitPath,
                                             QStringLiteral("org.freedesktop.DBus.Properties"),
                                             QStringLiteral("PropertiesChanged"),
                                             this,
                                             SLOT(onSystemdUnitPropertiesChanged(QString, QVariantMap, QStringList)));

        m_timeoutTimer = new QTimer(this);
        m_timeoutTimer->setSingleShot(true);
        connect(m_timeoutTimer, &QTimer::timeout, this, &ServiceHelper::onSystemdTimeout);
        m_timeoutTimer->start(10000);
    } else {
        // We're already running and enabled.
        setEnabling(false);
    }
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
            if (m_timeoutTimer) {
                m_timeoutTimer->stop();
            }
            setEnabling(false);
            // Success is implied by enabling=false and failed=false.
        }
    }
}

void ServiceHelper::onSystemdTimeout()
{
    // Cleanup signal
    QDBusConnection::systemBus().disconnect(DBUS_SYSTEMD_SERVICE,
                                            m_currentUnitPath,
                                            QStringLiteral("org.freedesktop.DBus.Properties"),
                                            QStringLiteral("PropertiesChanged"),
                                            this,
                                            SLOT(onSystemdUnitPropertiesChanged(QString, QVariantMap, QStringList)));

    setErrorMessage(xi18nc("@info",
                           "Timed out waiting for <command>%1</command> to start.<nl/><nl/>Ensure the Samba file sharing service is installed and enabled.",
                           m_serviceName));

    setFailed(true);
    setEnabling(false);
}
