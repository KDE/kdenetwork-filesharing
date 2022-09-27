/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
*/

#include "groupmanager.h"

#include <QFileInfo>
#include <QProcess>

#include <KUser>
#include <KAuth/Action>
#include <KAuth/ExecuteJob>
#include <KLocalizedString>

GroupManager::GroupManager(QObject *parent)
    : QObject(parent)
{
    metaObject()->invokeMethod(this, [this] {
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

            // First check to see if the path where user shares will be exported exists
            if (path.isEmpty() || !info.exists()) {
                m_errorText = xi18nc("@info:status", "This folder can't be shared because <filename>%1</filename> does not exist.", path);
                Q_EMIT errorTextChanged();
                m_errorExplanation = xi18nc("@info:status", "This error is caused by your distro not setting up Samba sharing properly. You can fix it yourself by creating that folder manually. Then close and re-open this window.");
                Q_EMIT errorExplanationChanged();
                // TODO: define a helpfulAction that creates the folder
            }

            // Now see if the group is set to something valid
            else if (m_targetGroup == QLatin1String("root")) {
                m_errorText = xi18nc("@info:status", "This folder can't be shared because <filename>%1</filename> has its group owner inappropriately set to <resource>%2</resource>.", path, m_targetGroup);
                Q_EMIT errorTextChanged();
                m_errorExplanation = xi18nc("@info:status", "This error is caused by your distro not setting up Samba sharing properly. You can fix it yourself by changing that folder's group owner to <resource>usershares</resource> and making yourself a member of that group. Then restart the system.");
                Q_EMIT errorExplanationChanged();
                // TODO: define a helpfulAction that creates the group and applies it to the folder
            }

            // Now see if the user is a member of the group
            else if (!groups.contains(m_targetGroup)) {
                m_errorText = xi18nc("@info:status", "This folder can't be shared because your user account isn't a member of the <resource>%1</resource> group.", m_targetGroup);
                Q_EMIT errorTextChanged();
                m_errorExplanation = xi18nc("@info:status", "You can fix this by making your user a member of that group. Then restart the system.");
                Q_EMIT errorExplanationChanged();
                m_helpfulActionIcon = QStringLiteral("resource-group-new");
                Q_EMIT helpfulActionIconChanged();
                m_helpfulActionText = i18nc("action@button makes user a member of the samba share group", "Make me a Group Member");
                Q_EMIT helpfulActionTextChanged();
                m_helpfulAction = HelpfulAction::AddUserToGroup;
                m_hasHelpfulAction = true;
                Q_EMIT hasHelpfulActionChanged();
            }

            // Now see if it's writable by the user
            else if (!info.isWritable()) {
                m_errorText = xi18nc("@info:status", "This folder can't be shared because your user account doesn't have permission to write into <filename>%1</filename>.", path);
                Q_EMIT errorTextChanged();
                m_errorExplanation = xi18nc("@info:status", "You can fix this by ensuring that the <resource>%1</resource> group has write permission for <filename>%2</filename>. Then close and re-open this window.",  m_targetGroup, path);
                Q_EMIT errorExplanationChanged();
                // TODO: define a helpfulAction that adds group write permission to the folder
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
            action.addArgument(QStringLiteral("user"), user);
            action.addArgument(QStringLiteral("group"), group);
            action.setDetailsV2({{KAuth::Action::AuthDetail::DetailMessage,
                                i18nc("@label kauth action description %1 is a username %2 a group name",
                                        "Adding user '%1' to group '%2' so they may configure Samba user shares",
                                        user,
                                        group) }
            });
            KAuth::ExecuteJob *job = action.execute();
            connect(job, &KAuth::ExecuteJob::result, this, [this, job, user, group] {
                job->deleteLater();
                if (job->error() != KAuth::ExecuteJob::NoError) {
                    QString helpfulActionErrorText = job->errorString();
                    if (helpfulActionErrorText.isEmpty()) {
                        // unknown error :(
                        helpfulActionErrorText = i18nc("@info", "Failed to make user <resource>%1<resource> a member of group <resource>%2<resource>", user, group);
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
