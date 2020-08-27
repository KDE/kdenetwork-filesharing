/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
*/

#pragma once

#include <QObject>

class GroupManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool ready READ isReady NOTIFY isReadyChanged)
    Q_PROPERTY(bool member READ isMember NOTIFY isMemberChanged)
    Q_PROPERTY(bool canMakeMember READ canMakeMember NOTIFY canMakeMemberChanged)
    Q_PROPERTY(QString targetGroup READ targetGroup NOTIFY targetGroupChanged)
public:
    explicit GroupManager(QObject *parent = nullptr);

    bool canMakeMember() const;
    bool isReady() const;
    QString targetGroup() const;
    bool isMember() const;

public Q_SLOTS:
    void makeMember();

Q_SIGNALS:
    void isReadyChanged();
    void isMemberChanged();
    void canMakeMemberChanged();
    void madeMember();
    void targetGroupChanged();
    void makeMemberError(const QString &error);

private:
    bool m_canMakeMember = false;
    bool m_isMember = false;
    bool m_ready = false;
    QString m_targetGroup;
};
