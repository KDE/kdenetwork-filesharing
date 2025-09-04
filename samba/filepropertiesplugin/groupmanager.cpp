/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
*/

#include "groupmanager.h"

#include <QFileInfo>
#include <QProcess>

#include <KAuth/Action>
#include <KAuth/ExecuteJob>
#include <KLocalizedString>
#include <KUser>

GroupManager::GroupManager(QObject *parent)
    : QObject(parent)
{
    QMetaObject::invokeMethod(this, [this] {
        auto proc = new QProcess;
        proc->setProgram(QStringLiteral("testparm"));
        proc->setArguments({QStringLiteral("--debuglevel=0"),
                            QStringLiteral("--suppress-prompt"),
                            QStringLiteral("--verbose"),
                            QStringLiteral("--parameter-name"),
                            QStringLiteral("usershare path")});
        connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, proc] {
            proc->deleteLater();
            const QString path = QString::fromUtf8(proc->readAllStandardOutput().simplified());
            QFileInfo info(path);
            m_targetGroup = info.group();
            m_user = KUser().loginName();
            const QStringList groups = KUser(m_user).groupNames();

            if (path.isEmpty()) {
                m_errorText = xi18nc("@info:status", "Your Samba installation appears to be broken.");
                Q_EMIT errorTextChanged();
                const auto err = proc->readAllStandardError().trimmed();
                if (err.isEmpty()) {
                    m_errorExplanation = xi18nc("@info:status",
                                                "This error is caused by your distribution not setting up Samba sharing properly. Please file a bug with your "
                                                "distribution or check your distribution's documentation on setting up Samba sharing.");
                } else {
                    m_errorExplanation =
                        xi18nc("@info:status",
                               "This error is caused by your distribution not setting up Samba sharing properly. Please file a bug with your distribution or "
                               "check your distribution's documentation on setting up Samba sharing. Error:<nl/><message>%1</message>",
                               QString::fromUtf8(err));
                }
                Q_EMIT errorExplanationChanged();
            }

            // Check to see if the path where user shares will be exported exists
            else if (!info.exists()) {
                m_errorText = xi18nc("@info:status", "This folder can't be shared because <filename>%1</filename> does not exist.", path);
                Q_EMIT errorTextChanged();
                m_errorExplanation = xi18nc("@info:status",
                                            "This error is caused by your distribution not setting up Samba sharing properly. You can fix it yourself by "
                                            "creating that folder manually, then close and re-open this window.");
                Q_EMIT errorExplanationChanged();
                // TODO: define a helpfulAction that creates the folder
            }

            // Now see if it's writable by the user
            // In case the directory has an ACL the user may have access() even though they are not member of the
            // relevant group, so we check writability first and if that fails check what may be wrong.
            else if (!info.isWritable()) {
                // If not: check that the user is in the group
                if (!groups.contains(m_targetGroup)) {
                    m_errorText = xi18nc("@info:status",
                                         "This folder can't be shared because your user account isn't a member of the <resource>%1</resource> group.",
                                         m_targetGroup);
                    Q_EMIT errorTextChanged();
                    m_errorExplanation = xi18nc("@info:status", "You can fix this by making your user a member of that group, then restart the system.");
                    Q_EMIT errorExplanationChanged();
                    m_helpfulActionIcon = QStringLiteral("resource-group-new");
                    Q_EMIT helpfulActionIconChanged();
                    m_helpfulActionText = i18nc("action@button makes user a member of the samba share group", "Make Me a Group Member");
                    Q_EMIT helpfulActionTextChanged();
                    m_helpfulAction = HelpfulAction::AddUserToGroup;
                    m_hasHelpfulAction = true;
                    Q_EMIT hasHelpfulActionChanged();
                } else {
                    m_errorText = xi18nc("@info:status",
                                         "This folder can't be shared because your user account doesn't have permission to write into <filename>%1</filename>.",
                                         path);
                    Q_EMIT errorTextChanged();
                    m_errorExplanation = xi18nc("@info:status",
                                                "You can fix this by ensuring that the <resource>%1</resource> group has write permission for "
                                                "<filename>%2</filename>, then close and re-open this window.",
                                                m_targetGroup,
                                                path);
                    Q_EMIT errorExplanationChanged();
                    // TODO: define a helpfulAction that adds group write permission to the folder
                }
            }

            m_ready = true;
            Q_EMIT isReadyChanged();

            // If we got here without hitting any errors, everything should work
        });
        proc->start();
    });
}

void GroupManager::performHelpfulAction()
{
    switch (m_helpfulAction) {
    case HelpfulAction::AddUserToGroup: {
        const QString user = m_user;
        const QString group = m_targetGroup;
        Q_ASSERT(!user.isEmpty());
        Q_ASSERT(!group.isEmpty());
        auto action = KAuth::Action(QStringLiteral("org.kde.filesharing.samba.addtogroup"));
        action.setHelperId(QStringLiteral("org.kde.filesharing.samba"));
        action.addArgument(QStringLiteral("group"), group);
        action.setDetailsV2({{KAuth::Action::AuthDetail::DetailMessage,
                              xi18nc("@label kauth action description %1 is a username %2 a group name",
                                     "Adding user <resource>%1</resource> to group <resource>%2</resource> so they can configure Samba user shares",
                                     user,
                                     group)}});
        KAuth::ExecuteJob *job = action.execute();
        connect(job, &KAuth::ExecuteJob::result, this, [this, job, user, group] {
            job->deleteLater();
            if (job->error() != KAuth::ExecuteJob::NoError) {
                QString helpfulActionErrorText = job->errorString();
                if (helpfulActionErrorText.isEmpty()) {
                    // unknown error :(
                    helpfulActionErrorText =
                        xi18nc("@info", "Failed to make user <resource>%1</resource> a member of group <resource>%2</resource>", user, group);
                }
                Q_EMIT helpfulActionError(helpfulActionErrorText);
                return;
            }
            Q_EMIT needsReboot();
        });
        job->start();
        break;
    }
    case HelpfulAction::None:
        // Do nothing
        break;
        // no Default so we have to explicitly handle new enums in the future
    }
}
