/*
 * Copyright 2011  Rodrigo Belem <rclbelem@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <kuser.h>

#include <sys/stat.h>

#include "model.h"

UserPermissionModel::UserPermissionModel(KSambaShareData &shareData, QObject *parent)
    : QAbstractTableModel(parent)
    , userList(getUsersList())
    , shareData(shareData)
    , usersAcl()
{
    setupData();
}

void UserPermissionModel::setupData()
{
    QStringList acl = shareData.acl().split(",", QString::SkipEmptyParts);

    QList<QString>::const_iterator itr;
    for (itr = acl.constBegin(); itr != acl.constEnd(); ++itr) {
        QStringList userInfo = (*itr).trimmed().split(":");
        usersAcl.insert(userInfo.at(0), QVariant(userInfo.at(1)));
    }
    if (usersAcl.isEmpty()) {
        usersAcl.insert("Everyone", QVariant("R"));
    }
}

QStringList UserPermissionModel::getUsersList() const
{
    unsigned int defminuid;
    unsigned int defmaxuid;

#ifdef __linux__
    struct stat st;
    if (!stat("/etc/debian_version", &st)) { /* debian */
        defminuid = 1000;
        defmaxuid = 29999;
    } else if (!stat("/usr/portage", &st)) { /* gentoo */
        defminuid = 1000;
        defmaxuid = 65000;
    } else if (!stat("/etc/mandrake-release", &st)) { /* mandrake - check before redhat! */
        defminuid = 500;
        defmaxuid = 65000;
    } else if (!stat("/etc/redhat-release", &st)) { /* redhat */
        defminuid = 100;
        defmaxuid = 65000;
    } else /* if (!stat("/etc/SuSE-release", &st)) */ { /* suse */
        defminuid = 500;
        defmaxuid = 65000;
    }
#else
    defminuid = 1000;
    defmaxuid = 65000;
#endif

    QStringList userList;
    userList.append("Everyone");
    foreach (const QString &username, KUser::allUserNames()) {
        if (username == "nobody") {
            continue;
        }
        KUser user(username);
        if (user.uid() >= defminuid) {
            userList << username;
        }
    }

    return userList;
}

int UserPermissionModel::rowCount(const QModelIndex &parent) const
{
    return userList.count();
}

int UserPermissionModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QVariant UserPermissionModel::data(const QModelIndex &index, int role) const
{
    if ((role == Qt::DisplayRole) && (index.column() == 0)) {
        return QVariant(userList.at(index.row()));
    }

    if ((role == Qt::DisplayRole || role == Qt::EditRole) && (index.column() == 1)) {
        QMap<QString, QVariant>::ConstIterator itr;
        for (itr = usersAcl.constBegin(); itr != usersAcl.constEnd(); ++itr) {
            if (itr.key().endsWith(userList.at(index.row()))) {
                return itr.value();
            }
        }
    }

    return QVariant();
}

Qt::ItemFlags UserPermissionModel::flags(const QModelIndex &index) const
{
    if (index.column() == 0) {
        return Qt::ItemIsSelectable;
    }

    if (index.column() == 1) {
        return (Qt::ItemIsEnabled | Qt::ItemIsEditable);
    }

    return Qt::NoItemFlags;
}

bool UserPermissionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if ((role != Qt::EditRole) || (index.column() != 1)) {
        return false;
    }

    QString key("");
    QMap<QString, QVariant>::ConstIterator itr;
    for (itr = usersAcl.constBegin(); itr != usersAcl.constEnd(); ++itr) {
        if (itr.key().endsWith(userList.at(index.row()))) {
            key = itr.key();
            break;
        }
    }

    if (key.isEmpty()) {
        key = userList.at(index.row());
    }

    if (value.isNull()) {
        usersAcl.take(key);
    } else {
        usersAcl.insert(key, value);
    }

    emit dataChanged(index, index);
    return true;
}

QString UserPermissionModel::getAcl() const
{
    QString result("");

    QMap<QString, QVariant>::ConstIterator itr;
    for (itr = usersAcl.constBegin(); itr != usersAcl.constEnd(); ++itr) {
        if (!itr.value().toString().isEmpty()) {
            result.append(itr.key() + ":" + itr.value().toString().toLower());
            if (itr != (usersAcl.constEnd() - 1)) {
                result.append(",");
            }
        }
    }

    return result;
}
