/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
*/

#include <QObject>

#pragma once

class UserManager;

class User : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(bool inSamba READ inSamba NOTIFY inSambaChanged)
public:
    explicit User(const QString &name, UserManager *parent);

    void resolve();
    QString name() const;
    bool inSamba() const;

    Q_INVOKABLE void addToSamba(const QString &password);

Q_SIGNALS:
    void resolved();
    void inSambaChanged();
    // stderr of smbpasswd if it came back with a failure. This may be multiple lines but usually is not.
    void addToSambaError(const QString &error);

private:
    const QString m_name;
    bool m_inSamba = false; // tdbsam knows about this user already
};

class UserManager : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    bool canManageSamba() const;
    QList<User *> users() const;
    Q_INVOKABLE User *currentUser() const;

public Q_SLOTS:
    void load();

Q_SIGNALS:
    void loaded();

private:
    QList<User *> m_users;
    User *m_currentUser = nullptr;
    bool m_canManageSamba = false;
    int m_waitingForResolution = 0;
};
