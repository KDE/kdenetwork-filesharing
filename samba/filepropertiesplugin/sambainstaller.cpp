/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2004 Jan Schaefer <j_schaef@informatik.uni-kl.de>
    SPDX-FileCopyrightText: 2011 Rodrigo Belem <rclbelem@gmail.com>
    SPDX-FileCopyrightText: 2019 Nate Graham <nate@kde.org>
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
*/

#include "sambainstaller.h"

#include "sambausershareplugin.h"

void SambaInstaller::install()
{
    setInstalling(true);
    const QString package = QStringLiteral(SAMBA_PACKAGE_NAME);
    QStringList distroSambaPackages = package.split(QLatin1Char(','));

    PackageKit::Transaction *resolveTransaction = PackageKit::Daemon::resolve(distroSambaPackages,
                                                                              PackageKit::Transaction::FilterArch);

    QSharedPointer<QStringList> pkgids(new QStringList);

    connect(resolveTransaction, &PackageKit::Transaction::package,
            this, [pkgids](PackageKit::Transaction::Info /*info*/, const QString &packageId) {
        pkgids->append(packageId);
    });

    connect(resolveTransaction, &PackageKit::Transaction::finished,
            this, [this, pkgids](PackageKit::Transaction::Exit exit) {
        if (exit != PackageKit::Transaction::ExitSuccess) {
            setFailed(true);
            return;
        }
        auto installTransaction = PackageKit::Daemon::installPackages(*pkgids);
        connect(installTransaction, &PackageKit::Transaction::finished,
                this, &SambaInstaller::packageFinished);
    });
}

bool SambaInstaller::isInstalling() const
{
    return m_installing;
}

bool SambaInstaller::hasFailed() const
{
    return m_failed;
}

bool SambaInstaller::isInstalled()
{
    return SambaUserSharePlugin::isSambaInstalled();
}

void SambaInstaller::packageFinished(PackageKit::Transaction::Exit status)
{
    setInstalling(false);
    switch (status) {
    case PackageKit::Transaction::ExitSuccess:
        Q_EMIT installedChanged();
        return;
    default:
        break; // failure. technically not true though, not all !success are failures
    }
    setFailed(true);
}

void SambaInstaller::setFailed(bool failed)
{
    if (m_failed == failed) {
        return;
    }

    if (m_installing && failed) {
        setInstalling(false);
    }

    m_failed = true;
    Q_EMIT failedChanged();
}

void SambaInstaller::setInstalling(bool installing)
{
    if (m_installing == installing) {
        return;
    }

    m_installing = installing;
    if (m_installing) {
        m_failed = false;
        Q_EMIT failedChanged();
    }
    Q_EMIT installingChanged();
}

