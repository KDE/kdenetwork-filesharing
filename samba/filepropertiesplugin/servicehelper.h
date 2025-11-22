/*
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 *   SPDX-FileCopyrightText: 2025 Thomas Duckworth <tduck@filotimoproject.org>
 *   SPDX-FileCopyrightText: 2025 Nate Graham <nate@kde.org>
 */

#pragma once

#include <QCoro/QCoroTask>
#include <QObject>
#include <QTimer>
#include <QVariantMap>

class QTimer;

class ServiceHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabling READ isEnabling NOTIFY enablingChanged)
    Q_PROPERTY(bool failed READ hasFailed NOTIFY failedChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
    explicit ServiceHelper(QObject *parent = nullptr);

    bool isEnabling() const;
    bool hasFailed() const;
    QString errorMessage() const;

    Q_INVOKABLE void setup();

    static QCoro::Task<bool> isServiceReady();

Q_SIGNALS:
    void enablingChanged();
    void failedChanged();
    void errorMessageChanged();

private Q_SLOTS:
    void onSystemdUnitPropertiesChanged(const QString &interface, const QVariantMap &changed, const QStringList &invalidated);
    void onSystemdTimeout();

private:
    QCoro::Task<bool> checkSystemdEnabledState(const QString &serviceName);
    QCoro::Task<bool> checkSystemdRunningState(const QString &serviceName);

    QCoro::Task<bool> performEnableAction();
    QCoro::Task<bool> performStartAction();
    // Runs all necessary functions (which are above) to set up the service.
    QCoro::Task<void> setupCoroutine();

    void setEnabling(bool enabling);
    void setFailed(bool failed);
    void setErrorMessage(const QString &message);
    void cleanupConnections();

    bool m_enabling = false;
    bool m_failed = false;
    QString m_errorMessage;

    QString m_serviceName;
    QString m_currentUnitPath;
    std::unique_ptr<QTimer> m_timeoutTimer = nullptr;
};
