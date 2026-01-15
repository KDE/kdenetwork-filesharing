/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2026 Thomas Duckworth <tduck@filotimoproject.org>
*/

#pragma once

#include <QObject>

enum class HelpfulAction {
    None,
    AddUserToGroup
};

class GroupManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString errorText MEMBER m_errorText NOTIFY errorTextChanged)
    Q_PROPERTY(QString errorExplanation READ errorExplanation NOTIFY errorExplanationChanged)
    Q_PROPERTY(bool hasHelpfulAction MEMBER m_hasHelpfulAction NOTIFY hasHelpfulActionChanged)
    Q_PROPERTY(QString helpfulActionIcon MEMBER m_helpfulActionIcon NOTIFY helpfulActionIconChanged)
    Q_PROPERTY(QString helpfulActionText MEMBER m_helpfulActionText NOTIFY helpfulActionTextChanged)
    Q_PROPERTY(bool ready READ isReady NOTIFY isReadyChanged)
public:
    explicit GroupManager(QObject *parent = nullptr);

    bool isReady();
    QString errorExplanation();

public Q_SLOTS:
    void performHelpfulAction();

Q_SIGNALS:
    void isReadyChanged();
    void errorTextChanged();
    void errorExplanationChanged();
    void hasHelpfulActionChanged();
    void helpfulActionIconChanged();
    void helpfulActionTextChanged();
    void helpfulActionError(const QString &error);
    void needsReboot();

private:
    bool m_ready = false;
    QString m_targetGroup;
    QString m_user;
    QString m_errorText;
    QString m_errorExplanation;
    HelpfulAction m_helpfulAction;
    bool m_hasHelpfulAction = false;
    QString m_helpfulActionIcon;
    QString m_helpfulActionText;
};
