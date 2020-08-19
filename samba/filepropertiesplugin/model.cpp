/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2011 Rodrigo Belem <rclbelem@gmail.com>
*/

#include <kuser.h>

#include <QFile>
#include <QRegularExpression>
#include <QMetaEnum>

#include <sys/stat.h>

#include "model.h"

UserPermissionModel::UserPermissionModel(const KSambaShareData &shareData, QObject *parent)
    : QAbstractTableModel(parent)
    , m_userList(getUsersList())
    , m_shareData(shareData)
    , m_usersAcl()
{
    QMetaObject::invokeMethod(this, &UserPermissionModel::setupData);
}

void UserPermissionModel::setupData()
{
    const QStringList acl = m_shareData.acl().split(QLatin1Char(','),
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
                                            QString::SkipEmptyParts);
#else
                                            Qt::SkipEmptyParts);
#endif

    QList<QString>::const_iterator itr;
    for (itr = acl.constBegin(); itr != acl.constEnd(); ++itr) {
        const QStringList userInfo = (*itr).trimmed().split(QLatin1Char(':'));
        m_usersAcl.insert(userInfo.at(0), userInfo.at(1));
    }
    if (m_usersAcl.isEmpty()) {
        m_usersAcl.insert(QStringLiteral("Everyone"), QStringLiteral("R"));
    }
}

QStringList UserPermissionModel::getUsersList()
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

int UserPermissionModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_userList.count();
}

int UserPermissionModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return QMetaEnum::fromType<Column>().keyCount();
}

QVariant UserPermissionModel::data(const QModelIndex &index, int role) const
{
    if ((role == Qt::DisplayRole) && (index.column() == ColumnUsername)) {
        return QVariant(m_userList.at(index.row()));
    }

    if ((role == Qt::DisplayRole || role == Qt::EditRole) && (index.column() == ColumnAccess)) {
        QMap<QString, QVariant>::ConstIterator itr;
        for (itr = m_usersAcl.constBegin(); itr != m_usersAcl.constEnd(); ++itr) {
            if (itr.key().endsWith(m_userList.at(index.row()))) {
                return itr.value();
            }
        }
    }

    return QVariant();
}

Qt::ItemFlags UserPermissionModel::flags(const QModelIndex &index) const
{
    if (index.column() == ColumnUsername) {
        return Qt::ItemIsSelectable;
    }

    if (index.column() == ColumnAccess) {
        return (Qt::ItemIsEnabled | Qt::ItemIsEditable);
    }

    return Qt::NoItemFlags;
}

bool UserPermissionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if ((role != Qt::EditRole) || (index.column() != ColumnAccess)) {
        return false;
    }

    QString key;
    QMap<QString, QVariant>::ConstIterator itr;
    for (itr = m_usersAcl.constBegin(); itr != m_usersAcl.constEnd(); ++itr) {
        if (itr.key().endsWith(m_userList.at(index.row()))) {
            key = itr.key();
            break;
        }
    }

    if (key.isEmpty()) {
        key = m_userList.at(index.row());
    }

    if (value.isNull()) {
        m_usersAcl.take(key);
    } else {
        m_usersAcl.insert(key, value);
    }

    Q_EMIT dataChanged(index, index);
    return true;
}

QString UserPermissionModel::getAcl() const
{
    QString result;

    QMap<QString, QVariant>::ConstIterator itr;
    for (itr = m_usersAcl.constBegin(); itr != m_usersAcl.constEnd(); ++itr) {
        if (!itr.value().toString().isEmpty()) {
            result.append(itr.key() + QStringLiteral(":") + itr.value().toString().toLower());
            if (itr != (m_usersAcl.constEnd() - 1)) {
                result.append(QLatin1Char(','));
            }
        }
    }

    return result;
}
