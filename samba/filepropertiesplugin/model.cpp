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

#include <QFile>
#include <QRegularExpression>

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
    QStringList acl = shareData.acl().split(QStringLiteral(","),
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
                                            QString::SkipEmptyParts);
#else
                                            Qt::SkipEmptyParts);
#endif

    QList<QString>::const_iterator itr;
    for (itr = acl.constBegin(); itr != acl.constEnd(); ++itr) {
        QStringList userInfo = (*itr).trimmed().split(QLatin1Char(':'));
        usersAcl.insert(userInfo.at(0), QVariant(userInfo.at(1)));
    }
    if (usersAcl.isEmpty()) {
        usersAcl.insert(QStringLiteral("Everyone"), QStringLiteral("R"));
    }
}

QStringList UserPermissionModel::getUsersList() const
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
    userList.append(QStringLiteral("Everyone"));
    foreach (const QString &username, KUser::allUserNames()) {
        if (username == QStringLiteral("nobody")) {
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

int UserPermissionModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return userList.count();
}

int UserPermissionModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
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

    QString key;
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
    QString result;

    QMap<QString, QVariant>::ConstIterator itr;
    for (itr = usersAcl.constBegin(); itr != usersAcl.constEnd(); ++itr) {
        if (!itr.value().toString().isEmpty()) {
            result.append(itr.key() + QStringLiteral(":") + itr.value().toString().toLower());
            if (itr != (usersAcl.constEnd() - 1)) {
                result.append(QStringLiteral(","));
            }
        }
    }

    return result;
}
