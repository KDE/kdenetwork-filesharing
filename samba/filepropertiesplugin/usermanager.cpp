/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
*/

#include "usermanager.h"

#include <KUser>
#include <QFile>
#include <QRegularExpression>
#include <QProcess>

#include <KAuthAction>
#include <KAuthExecuteJob>
#include <KLocalizedString>

const auto everyoneUserName = QStringLiteral("Everyone");

static QStringList getUsersList()
{
    uid_t defminuid = 1000;
    uid_t defmaxuid = 65000;

    QFile loginDefs(QStringLiteral("/etc/login.defs"));
    if (loginDefs.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!loginDefs.atEnd()) {
            const QString line = QString::fromLatin1(loginDefs.readLine());
            {
                const QRegularExpression expression(QStringLiteral("^\\s*UID_MIN\\s+(?<UID_MIN>\\d+)"));
                const auto match = expression.match(line);
                if (match.hasMatch()) {
                    defminuid = match.captured(u"UID_MIN").toUInt();
                }
            }
            {
                const QRegularExpression expression(QStringLiteral("^\\s*UID_MAX\\s+(?<UID_MAX>\\d+)"));
                const auto match = expression.match(line);
                if (match.hasMatch()) {
                    defmaxuid = match.captured(u"UID_MAX").toUInt();
                }
            }
        }
    }

    QStringList userList;
    userList.append(everyoneUserName);
    const QStringList userNames = KUser::allUserNames();
    for (const QString &username : userNames) {
        if (username == QLatin1String("nobody")) {
            continue;
        }
        KUser user(username);
        const uid_t nativeId = user.userId().nativeId();
        if (nativeId >= defminuid && nativeId <= defmaxuid) {
            userList << username;
        }
    }

    return userList;
}

User::User(const QString &name, UserManager *parent)
    : QObject(parent)
    , m_name(name)
{
}

QString User::name() const
{
    return m_name;
}

bool User::inSamba() const
{
    return m_inSamba;
}

void User::resolve()
{
    if (!qobject_cast<UserManager *>(parent())->canManageSamba() || m_name == everyoneUserName) {
        // Assume the user is cool. If the auth backend isn't tbdsam it's likely delegated to a domain controller
        // which in turn suggests that the local system is delegating all users to the controller. If not we
        // can't do anything about it anyway the options are disable everything or pretend it'll work.
        m_inSamba = true;
        Q_EMIT resolved();
        return;
    }

    auto action = KAuth::Action(QStringLiteral("org.kde.filesharing.samba.isuserknown"));
    action.setHelperId(QStringLiteral("org.kde.filesharing.samba"));
    action.addArgument(QStringLiteral("username"), m_name);
    // Detail message won't really show up unless the system admin forces authentication for this. Which would
    // be very awkward
    action.setDetailsV2({{
        KAuth::Action::AuthDetail::DetailMessage,
        i18nc("@label kauth action description %1 is a username", "Checking if Samba user '%1' exists", m_name) }
    });
    KAuth::ExecuteJob *job = action.execute();
    connect(job, &KAuth::ExecuteJob::result, this, [this, job] {
        job->deleteLater();
        m_inSamba = job->data().value(QStringLiteral("exists"), false).toBool();
        Q_EMIT inSambaChanged();
        Q_EMIT resolved();
    });
    job->start();
}

void User::addToSamba(const QString &password)
{
    Q_ASSERT(qobject_cast<UserManager *>(parent())->canManageSamba());

    auto action = KAuth::Action(QStringLiteral("org.kde.filesharing.samba.createuser"));
    action.setHelperId(QStringLiteral("org.kde.filesharing.samba"));
    action.addArgument(QStringLiteral("username"), m_name);
    action.addArgument(QStringLiteral("password"), password);
    action.setDetailsV2({{
        KAuth::Action::AuthDetail::DetailMessage,
        i18nc("@label kauth action description %1 is a username", "Creating new Samba user '%1'", m_name) }
    });
    KAuth::ExecuteJob *job = action.execute();
    connect(job, &KAuth::ExecuteJob::result, this, [this, job] {
        job->deleteLater();
        m_inSamba = job->data().value(QStringLiteral("created"), false).toBool();
        if (!m_inSamba) {
            Q_EMIT addToSambaError(job->data().value(QStringLiteral("stderr"), QString()).toString());
        }
        Q_EMIT inSambaChanged();
    });
    job->start();
}

bool UserManager::canManageSamba() const
{
    return m_canManageSamba;
}

void UserManager::load()
{
    auto proc = new QProcess(this);
    proc->setProgram(QStringLiteral("testparm"));
    proc->setArguments({
        QStringLiteral("--debuglevel=0"),
        QStringLiteral("--suppress-prompt"),
        QStringLiteral("--verbose"),
        QStringLiteral("--parameter-name"),
        QStringLiteral("passdb backend")
    });
    connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, proc](int exitCode) {
        proc->deleteLater();
        const QByteArray output = proc->readAllStandardOutput().simplified();

        if (exitCode == 0 && output == QByteArrayLiteral("tdbsam")) {
            m_canManageSamba = true;
        }

        const QString currentUserName = KUser().loginName();
        const QStringList nameList = getUsersList();
        for (const auto &name : nameList) {
            ++m_waitingForResolution;
            auto user = new User(name, this);
            connect(user, &User::resolved, this, [this] {
                if (--m_waitingForResolution <= 0) {
                    Q_EMIT loaded();
                }
            }, Qt::QueuedConnection /* queue to ensure even shortcut resolution goes through the loop */);
            m_users.append(user);
            if (user->name() == currentUserName) {
                m_currentUser = user;
            }
            user->resolve();
        }
    });
    proc->start();
}

QList<User *> UserManager::users() const
{
    return m_users;
}

Q_INVOKABLE User *UserManager::currentUser() const
{
    return m_currentUser;
}
