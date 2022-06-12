/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020-2021 Harald Sitter <sitter@kde.org>
*/

#include "authhelper.h"
#include <kauth/helpersupport.h>
#include <QProcess>
#include <QRegularExpression>

static bool isValidUserName(const QString &name)
{
    // https://systemd.io/USER_NAMES/
    static QRegularExpression expr(QStringLiteral("^[a-z_][a-z0-9_-]*$"));
    return expr.match(name).hasMatch();
}

ActionReply AuthHelper::isuserknown(const QVariantMap &args)
{
    const auto username = args.value(QStringLiteral("username")).toString();
    if (!isValidUserName(username)) {
        return ActionReply::HelperErrorReply();
    }

    QProcess p;
    p.setProgram(QStringLiteral("pdbedit"));
    p.setArguments({ QStringLiteral("--debuglevel=0"), QStringLiteral("--user"), username });
    p.start();
    // Should be fairly quick: short timeout.
    const int pdbeditTimeout = 4000; // milliseconds
    p.waitForFinished(pdbeditTimeout);

    if (p.exitStatus() != QProcess::NormalExit) {
        return ActionReply::HelperErrorReply();
    }

    ActionReply reply;
    reply.addData(QStringLiteral("exists"), p.exitCode() == 0);
    return reply;
}

ActionReply AuthHelper::createuser(const QVariantMap &args)
{
    const auto username = args.value(QStringLiteral("username")).toString();
    const auto password = args.value(QStringLiteral("password")).toString();
    if (!isValidUserName(username) || password.isEmpty()) {
        return ActionReply::HelperErrorReply();
    }

    QProcess p;
    p.setProgram(QStringLiteral("smbpasswd"));
    p.setArguments({
        QStringLiteral("-L"), /* local mode */
        QStringLiteral("-s"), /* read from stdin */
        QStringLiteral("-D"), QStringLiteral("0"), /* force-disable debug */
        QStringLiteral("-a"), /* add user */
        username });
    p.start();
    // despite being in silent mode we still need to write the password twice!
    p.write((password + QStringLiteral("\n")).toUtf8());
    p.write((password + QStringLiteral("\n")).toUtf8());
    p.waitForBytesWritten();
    p.closeWriteChannel();
    p.waitForFinished();

    if (p.exitStatus() != QProcess::NormalExit) {
        auto reply = ActionReply::HelperErrorReply();
        reply.setErrorDescription(QString::fromUtf8(p.readAllStandardError()));
        return reply;
    }

    ActionReply reply;
    reply.addData(QStringLiteral("created"), p.exitCode() == 0);
    // stderr will generally contain info on what went wrong so forward it
    // so the UI may display it
    reply.addData(QStringLiteral("stderr"), p.readAllStandardError());
    return reply;
}

ActionReply AuthHelper::addtogroup(const QVariantMap &args)
{
    const auto user = args.value(QStringLiteral("user")).toString();
    const auto group = args.value(QStringLiteral("group")).toString();
    if (!isValidUserName(user) || !isValidUserName(group)) {
        return ActionReply::HelperErrorReply();
    }
    // Harden against some input abuse.
    // TODO: add ability to resolve remote UID via KAuth and verify the request (or even reduce the arguments down to
    //    only the group and resolve the UID)
    if (!group.contains(QLatin1String("samba")) || group.contains(QLatin1String("admin")) ||
        group.contains(QLatin1String("root"))) {
        return ActionReply::HelperErrorReply();
    }

    QProcess p;
#if defined(Q_OS_FREEBSD)
    p.setProgram(QStringLiteral("pw"));
    p.setArguments({
        QStringLiteral("group"),
        QStringLiteral("mod"),
        QStringLiteral("{%1}").arg(group),
        QStringLiteral("-m"),
        QStringLiteral("{%1}").arg(user) });
#elif defined(Q_OS_LINUX)
    p.setProgram(QStringLiteral("/usr/sbin/usermod"));
    p.setArguments({
        QStringLiteral("--append"),
        QStringLiteral("--groups"),
        group,
        user });
#else
#   error "Platform lacks group management support. Please add support."
#endif

    p.start();
    p.waitForFinished(1000);

    if (p.exitCode() != 0 || p.exitStatus() != QProcess::NormalExit) {
        auto reply = ActionReply::HelperErrorReply();
        reply.setErrorDescription(QString::fromUtf8(p.readAll()));
        return reply;
    }

    return ActionReply::SuccessReply();
}

KAUTH_HELPER_MAIN("org.kde.filesharing.samba", AuthHelper)
