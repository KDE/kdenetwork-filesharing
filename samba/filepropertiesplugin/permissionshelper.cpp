/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2021 Danil Shein <dshein@altlinux.org>
    SPDX-FileCopyrightText: 2021 Slava Aseev <nullptrnine@basealt.ru>
*/

#include "permissionshelper.h"

#include <KFileItem>
#include <KIO/StatJob>
#include <KUser>
#include <QDebug>
#include <QFileInfo>
#include <QMetaEnum>

#include "model.h"
#include "usermanager.h"

static QString getUserPrimaryGroup(const QString &user)
{
    const QStringList groups = KUser(user).groupNames();
    if (!groups.isEmpty()) {
        return groups.at(0);
    }
    // if we can't fetch the user's groups then assume the group name is the same as the user name
    return user;
}

static KFileItem getCompleteFileItem(const QString &path)
{
    const QUrl url = QUrl::fromLocalFile(path);
    auto job = KIO::stat(url);
    job->exec();
    KIO::UDSEntry entry = job->statResult();
    KFileItem item(entry, url);
    return item;
}

static QString permissionsToString(QFile::Permissions perm)
{
    const char permStr[] = {(perm & QFileDevice::ReadOwner) ? 'r' : '-',
                            (perm & QFileDevice::WriteOwner) ? 'w' : '-',
                            (perm & QFileDevice::ExeOwner) ? 'x' : '-',
                            (perm & QFileDevice::ReadGroup) ? 'r' : '-',
                            (perm & QFileDevice::WriteGroup) ? 'w' : '-',
                            (perm & QFileDevice::ExeGroup) ? 'x' : '-',
                            (perm & QFileDevice::ReadOther) ? 'r' : '-',
                            (perm & QFileDevice::WriteOther) ? 'w' : '-',
                            (perm & QFileDevice::ExeOther) ? 'x' : '-'};

    const int permsAsNum = ((perm & QFileDevice::ReadOwner) ? S_IRUSR : 0)
            + ((perm & QFileDevice::WriteOwner) ? S_IWUSR : 0)
            + ((perm & QFileDevice::ExeOwner) ? S_IXUSR : 0)
            + ((perm & QFileDevice::ReadGroup) ? S_IRGRP : 0)
            + ((perm & QFileDevice::WriteGroup) ? S_IWGRP : 0)
            + ((perm & QFileDevice::ExeGroup) ? S_IXGRP : 0)
            + ((perm & QFileDevice::ReadOther) ? S_IROTH : 0)
            + ((perm & QFileDevice::WriteOther) ? S_IWOTH : 0)
            + ((perm & QFileDevice::ExeOther) ? S_IXOTH : 0);

    return QString::fromLatin1(permStr, sizeof(permStr)) + QStringLiteral(" (0%1)").arg(QString::number(permsAsNum, 8));
}

PermissionsHelperModel::PermissionsHelperModel(PermissionsHelper *helper)
    : QAbstractTableModel(helper)
    , parent(helper)
{
}

int PermissionsHelperModel::rowCount(const QModelIndex &) const
{
    return parent->affectedPaths().count();
}

int PermissionsHelperModel::columnCount(const QModelIndex &) const
{
    return QMetaEnum::fromType<Column>().keyCount();
}

QVariant PermissionsHelperModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case ColumnPath:
            return parent->affectedPaths().at(index.row()).path;
        case ColumnOldPermissions:
            return QVariant::fromValue(permissionsToString(parent->affectedPaths().at(index.row()).oldPerm));
        case ColumnNewPermissions:
            return QVariant::fromValue(permissionsToString(parent->affectedPaths().at(index.row()).newPerm));
        };
    }

    return {};
}

Qt::ItemFlags PermissionsHelperModel::flags(const QModelIndex &) const
{
    return Qt::NoItemFlags;
}

bool PermissionsHelperModel::setData(const QModelIndex &, const QVariant &, int)
{
    return false;
}

void PermissionsHelper::addPath(const QFileInfo &fileInfo, QFile::Permissions requiredPermissions)
{
    auto oldPerm = fileInfo.permissions();
    auto newPerm = oldPerm | requiredPermissions;
    m_affectedPaths.append({fileInfo.filePath(), oldPerm, newPerm});
}

PermissionsHelper::PermissionsHelper(const QString &path, const UserManager *userManager, const UserPermissionModel *permissionModel, QObject *parent)
    : QObject(parent)
    , m_path(path)
    , m_userManager(userManager)
    , m_permissionModel(permissionModel)
    , m_model(new PermissionsHelperModel(this))
{
}

Q_INVOKABLE void PermissionsHelper::reload() {
    if (!m_userManager->currentUser()) {
        qWarning() << "PermissionsHelper::reload() failed: current user is null";
        return;
    }

    m_affectedPaths.clear();
    m_filesWithPosixACL.clear();

    QString user = m_userManager->currentUser()->name();

    QFile::Permissions permsForShare;
    QFile::Permissions permsForSharePath;

    auto usersACEs = m_permissionModel->getUsersACEs();
    for (auto it = usersACEs.constBegin(); it != usersACEs.constEnd(); ++it) {
        const auto &aceUser = it.key();
        const auto &access = it.value();

        if (aceUser != user) {
            if (getUserPrimaryGroup(aceUser) == getUserPrimaryGroup(user)) {
                if (access == QLatin1String("R")) {
                    permsForShare |= (QFile::ExeGroup | QFile::ReadGroup);
                } else if (access == QLatin1String("F")) {
                    permsForShare |= (QFile::ExeGroup | QFile::ReadGroup | QFile::WriteGroup);
                }
                permsForSharePath |= QFile::ExeGroup;
            } else {
                if (access == QLatin1String("R")) {
                    permsForShare |= (QFile::ExeOther | QFile::ReadOther);
                } else if (access == QLatin1String("F")) {
                    permsForShare |= (QFile::ExeOther | QFile::ReadOther | QFile::WriteOther);
                }
                permsForSharePath |= QFile::ExeOther;
            }
        }
    }

    // store share path if permissions are insufficient
    QFileInfo fileInfo(m_path);
    if (!fileInfo.permission(permsForShare)) {
        addPath(fileInfo, permsForShare);
    }
    // check and store share POSIX ACL
    if (getCompleteFileItem(m_path).hasExtendedACL()) {
        m_filesWithPosixACL.append(m_path);
    }

    // check if share path could be resolved (has 'g+x' or 'o+x' all the way through)
    if (permsForShare) {
        QStringList pathParts = m_path.split(QStringLiteral("/"), Qt::SkipEmptyParts);
        pathParts.removeLast();
        QString currentPath;

        for (const auto &it : qAsConst(pathParts)) {
            currentPath.append(QStringLiteral("/") + it);
            fileInfo = QFileInfo(currentPath);
            if (!fileInfo.permission(permsForSharePath)) {
                addPath(fileInfo, permsForSharePath);
            }
            // check and store share path element's POSIX ACL
            if (getCompleteFileItem(currentPath).hasExtendedACL()) {
                m_filesWithPosixACL.append(currentPath);
            }
        }
    }

    Q_EMIT permissionsChanged();
}

const QList<PermissionsHelper::PermissionsChangeInfo> &PermissionsHelper::affectedPaths() const
{
    return m_affectedPaths;
}

Q_INVOKABLE QStringList PermissionsHelper::changePermissions()
{
    QStringList failedPaths;

    for (const auto &affected : m_affectedPaths) {
        // do not break the loop to collecting all possible failed paths
        if (!QFile::setPermissions(affected.path, affected.newPerm)) {
            failedPaths += affected.path;
        }
    }

    // roll back files permissions if some paths failed
    if (!failedPaths.isEmpty()) {
        for (const auto &affected : m_affectedPaths) {
            if (!QFile::setPermissions(affected.path, affected.oldPerm)) {
                qWarning() << "SharePermissionsHelper::sharePermsChange: failed to restore permissions for " << affected.path;
            }
        }
    } else {
        m_affectedPaths.clear();
        Q_EMIT permissionsChanged();
    }

    return failedPaths;
}

bool PermissionsHelper::permissionsChangeRequired() const
{
    return !m_affectedPaths.empty();
}

bool PermissionsHelper::hasPosixACL() const
{
    return !m_filesWithPosixACL.empty();
}
