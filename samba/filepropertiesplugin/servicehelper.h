/*
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 *  SPDX-FileCopyrightText: 2025 Thomas Duckworth <tduck@filotimoproject.org>
 *  SPDX-FileCopyrightText: 2025 Nate Graham <nate@kde.org>
 */

#pragma once

#include <QObject>
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

    /**
     * Determines if the Samba service is fully operational.
     * Use this to decide whether to instantiate this class.
     */
    static bool isServiceReady();

    bool isEnabling() const;
    bool hasFailed() const;
    QString errorMessage() const;

    /**
     * Attempts to set up the service.
     * Sets 'enabling' to true, then clears it upon success or failure.
     */
    Q_INVOKABLE void setup();

    static QString findAvailableService();

Q_SIGNALS:
    void enablingChanged();
    void failedChanged();
    void errorMessageChanged();

private Q_SLOTS:
    void onSystemdUnitPropertiesChanged(const QString &interface, const QVariantMap &changed, const QStringList &invalidated);
    void onSystemdTimeout();

private:
    void setEnabling(bool enabling);
    void setFailed(bool failed);
    void setErrorMessage(const QString &message);

    bool m_enabling = false;
    bool m_failed = false;
    QString m_errorMessage;

    QString m_serviceName;
    QString m_currentUnitPath;
    QTimer *m_timeoutTimer = nullptr;
};
