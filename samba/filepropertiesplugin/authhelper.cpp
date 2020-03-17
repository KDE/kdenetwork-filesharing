/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
*/

#include "authhelper.h"

#include <QProcess>

ActionReply AuthHelper::isuserknown(const QVariantMap &args)
{
    const auto username = args.value(QStringLiteral("username")).toString();
    if (username.isEmpty()) {
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
    if (username.isEmpty() || password.isEmpty()) {
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

KAUTH_HELPER_MAIN("org.kde.filesharing.samba", AuthHelper)
