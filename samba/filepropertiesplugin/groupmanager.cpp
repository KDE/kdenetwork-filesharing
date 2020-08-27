/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
*/

#include "groupmanager.h"

#include <QProcess>
#include <QFileInfo>
#include <QDebug>

#include <KUser>
#include <KAuth/KAuthAction>
#include <KAuth/KAuthExecuteJob>
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
        connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, proc](int exitCode) {
            proc->deleteLater();
            const QString path = QString::fromUtf8(proc->readAllStandardOutput().simplified());

            m_ready = true;
            Q_EMIT isReadyChanged();

            QFileInfo info(path);
            if (exitCode != 0 || path.isEmpty() || !info.exists()) {
                return; // usershares may be disabled or path is invalid :|
            }

            if (info.isWritable()) {
                m_isMember = true;
                Q_EMIT isMemberChanged();
            }

            m_targetGroup = info.group();
            Q_EMIT targetGroupChanged();

            if (m_targetGroup != QLatin1String("root") && m_targetGroup.contains(QLatin1String("samba"))) {
                m_canMakeMember = true;
                Q_EMIT canMakeMemberChanged();
            }
        });
        proc->start();
    });
}

bool GroupManager::canMakeMember() const
{
    return m_canMakeMember;
}

bool GroupManager::isReady() const
{
    return m_ready;
}

QString GroupManager::targetGroup() const
{
    return m_targetGroup;
}

bool GroupManager::isMember() const
{
    return m_isMember;
}

void GroupManager::makeMember()
{
    Q_ASSERT(m_canMakeMember);
    const QString user = KUser().loginName();
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
    connect(job, &KAuth::ExecuteJob::result, this, [this, job] {
        job->deleteLater();
        if (job->error() != KAuth::ExecuteJob::NoError) {
            Q_EMIT makeMemberError(job->errorString());
            return;
        }
        Q_EMIT madeMember();
    });
    job->start();
}
