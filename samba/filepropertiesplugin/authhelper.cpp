/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020-2021 Harald Sitter <sitter@kde.org>
*/

#include "authhelper.h"
#include <KLocalizedString>
#include <KUser>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QProcess>
#include <QRegularExpression>
#include <kauth/helpersupport.h>

static const QString DBUS_SYSTEMD_SERVICE = QStringLiteral("org.freedesktop.systemd1");
static const QString DBUS_SYSTEMD_PATH  = QStringLiteral("/org/freedesktop/systemd1");
static const QString DBUS_SYSTEMD_MANAGER_INTERFACE = QStringLiteral("org.freedesktop.systemd1.Manager");
static const QString SMB_SYSTEMD_SERVICE = QStringLiteral("smb");

namespace
{
bool isValidUserName(const QString &name)
{
    // https://systemd.io/USER_NAMES/
    static QRegularExpression expr(QStringLiteral("^[a-z_][a-z0-9_-]*$"));
    return expr.match(name).hasMatch();
}

std::optional<QString> callerUidToUserName()
{
    const auto uid = KAuth::HelperSupport::callerUid();
    if (uid < 0) {
        return {};
    }

    const KUser user(static_cast<K_UID>(uid));
    if (!user.isValid()) {
        return {};
    }

    auto name = user.loginName();
    if (name.isEmpty()) {
        return {};
    }
    return name;
}
} // namespace

ActionReply AuthHelper::isuserknown(const QVariantMap &args)
{
    const auto username = args.value(QStringLiteral("username")).toString();
    if (!isValidUserName(username)) {
        auto reply = ActionReply::HelperErrorReply();
        reply.setErrorDescription(xi18nc("@info", "User name <resource>%1</resource> is not valid as the name of a Samba user; cannot check for its existence.", username));
        return reply;
    }

    QProcess p;
    const auto program = QStringLiteral("pdbedit");
    const auto arguments = QStringList({QStringLiteral("--debuglevel=0"), QStringLiteral("--user"), username });
    p.setProgram(program);
    p.setArguments(arguments);
    p.start();
    // Should be fairly quick: short timeout.
    const int pdbeditTimeout = 4000; // milliseconds
    p.waitForFinished(pdbeditTimeout);

    if (p.exitStatus() != QProcess::NormalExit) {
        // QByteArray can't do direct conversion to QString
        const QString errorText = QString::fromUtf8(p.readAllStandardOutput());
        auto reply = ActionReply::HelperErrorReply();
        reply.setErrorDescription(xi18nc("@info '%1 %2' together make up a terminal command; %3 is the command's output",
                                         "Command <command>%1 %2</command> failed:<nl/><nl/>%3", program, arguments.join(QLatin1Char(' ')), errorText));
        return reply;
    }

    ActionReply reply;
    reply.addData(QStringLiteral("exists"), p.exitCode() == 0);
    return reply;
}

ActionReply AuthHelper::createuser(const QVariantMap &args)
{
    const auto username = callerUidToUserName();
    if (!username.has_value()) {
        auto reply = ActionReply::HelperErrorReply();
        reply.setErrorDescription(xi18nc("@info error while looking up uid from dbus", "Could not resolve calling user."));
        return reply;
    }
    const auto password = args.value(QStringLiteral("password")).toString();
    if (password.isEmpty()) {
        auto reply = ActionReply::HelperErrorReply();
        reply.setErrorDescription(i18nc("@info", "For security reasons, creating Samba users with empty passwords is not allowed."));
        return reply;
    }

    QProcess p;
    p.setProgram(QStringLiteral("smbpasswd"));
    p.setArguments({
        QStringLiteral("-L"), /* local mode */
        QStringLiteral("-s"), /* read from stdin */
        QStringLiteral("-D"), QStringLiteral("0"), /* force-disable debug */
        QStringLiteral("-a"), /* add user */
        username.value() });
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
    const auto group = args.value(QStringLiteral("group")).toString();
    const auto user = callerUidToUserName();
    if (!user.has_value()) {
        auto reply = ActionReply::HelperErrorReply();
        reply.setErrorDescription(xi18nc("@info error while looking up uid from dbus", "Could not resolve calling user."));
        return reply;
    }
    if (!isValidUserName(group)) {
        auto reply = ActionReply::HelperErrorReply();
        reply.setErrorDescription(xi18nc("@info", "<resource>%1</resource> is not a valid group name; cannot make user <resource>%2</resource> a member of it.", group, user.value()));
        return reply;
    }
    // Harden against some input abuse.
    // Keep this condition in sync with the one in groupmanager.cpp
    if (group.contains(QLatin1String("admin")) ||
        group.contains(QLatin1String("root"))) {
        auto reply = ActionReply::HelperErrorReply();
        reply.setErrorDescription(xi18nc("@info", "For security reasons, cannot make user <resource>%1</resource> a member of group <resource>%2</resource>. \
                                                   The group name is insecure; valid group names do not \
                                                   include the text <resource>admin</resource> or <resource>root</resource>."));
        return reply;
    }

    QProcess p;
#if defined(Q_OS_FREEBSD)
    p.setProgram(QStringLiteral("pw"));
    p.setArguments({
        QStringLiteral("group"),
        QStringLiteral("mod"),
        QStringLiteral("{%1}").arg(group),
        QStringLiteral("-m"),
        QStringLiteral("{%1}").arg(user.value()) });
#elif defined(Q_OS_LINUX) || defined(Q_OS_HURD)
    p.setProgram(QStringLiteral("/usr/sbin/usermod"));
    p.setArguments({
        QStringLiteral("--append"),
        QStringLiteral("--groups"),
        group,
        user.value() });
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

ActionReply AuthHelper::enablesmb()
{
    QDBusMessage dbusMessage = QDBusMessage::createMethodCall(DBUS_SYSTEMD_SERVICE,
                                                              DBUS_SYSTEMD_PATH,
                                                              DBUS_SYSTEMD_MANAGER_INTERFACE,
                                                              QStringLiteral("EnableUnitFiles"));

    dbusMessage << QStringList{SMB_SYSTEMD_SERVICE} << false << true;

    QDBusMessage dbusReply = QDBusConnection::systemBus().call(dbusMessage);

    if (dbusReply.type() == QDBusMessage::ErrorMessage) {
        auto kauthReply = ActionReply::HelperErrorReply();
        kauthReply.setErrorDescription(xi18nc("@info", "Could not enable the <command>smb</command> Systemd unit: %1", dbusReply.errorMessage()));
        return kauthReply;
    }

    return ActionReply::SuccessReply();
}

ActionReply AuthHelper::runsmb()
{
    QDBusMessage dbusMessage = QDBusMessage::createMethodCall(DBUS_SYSTEMD_SERVICE,
                                                      DBUS_SYSTEMD_PATH,
                                                      DBUS_SYSTEMD_MANAGER_INTERFACE,
                                                      QStringLiteral("StartUnit"));

    dbusMessage << SMB_SYSTEMD_SERVICE << QStringLiteral("replace");

    QDBusMessage dbusReply = QDBusConnection::systemBus().call(dbusMessage);

    if (dbusReply.type() == QDBusMessage::ErrorMessage) {
        auto kauthReply = ActionReply::HelperErrorReply();
        kauthReply.setErrorDescription(xi18nc("@info", "Could not start the <command>smb</command> Systemd unit: %1", dbusReply.errorMessage()));
        return kauthReply;
    }

    return ActionReply::SuccessReply();
}

KAUTH_HELPER_MAIN("org.kde.filesharing.samba", AuthHelper)
