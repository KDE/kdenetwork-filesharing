/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2011 Rodrigo Belem <rclbelem@gmail.com>
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2021 Slava Aseev <nullptrnine@basealt.ru>
*/

#include <kuser.h>

#include <QFile>
#include <QRegularExpression>
#include <QMetaEnum>

#include <sys/stat.h>

#include "model.h"
#include "usermanager.h"

UserPermissionModel::UserPermissionModel(const KSambaShareData &shareData, UserManager *userManager, QObject *parent)
    : QAbstractTableModel(parent)
    , m_userManager(userManager)
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




int UserPermissionModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_userManager->users().count();
}

int UserPermissionModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return QMetaEnum::fromType<Column>().keyCount();
}

QVariant UserPermissionModel::data(const QModelIndex &index, int role) const
{
    if ((role == Qt::DisplayRole) && (index.column() == ColumnUsername)) {
        return QVariant(m_userManager->users().at(index.row())->name());
    }

    if ((role == Qt::DisplayRole || role == Qt::EditRole) && (index.column() == ColumnAccess)) {
        QMap<QString, QVariant>::ConstIterator itr;
        for (itr = m_usersAcl.constBegin(); itr != m_usersAcl.constEnd(); ++itr) {
            if (itr.key().endsWith(m_userManager->users().at(index.row())->name())) {
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
        if (itr.key().endsWith(m_userManager->users().at(index.row())->name())) {
            key = itr.key();
            break;
        }
    }

    if (key.isEmpty()) {
        key = m_userManager->users().at(index.row())->name();
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
    // ACE order matters. Based on testing samba behaves the following way when checking if a user may do something:
    //   - rights granted stack up until the first applicable denial (r+f = f)
    //   - denials end the lookup BUT all permissions until then are applied!
    // We always put Everyone at the beginning and do not support groups so effectively we behave the same way
    // as on Windows. Everyone is to mean **everyone** and the user rules add on top but the Everyone ACE is the
    // baseline permission for everyone. By extension because of the samba resolution behavior Everyone:D will disable
    // the share pretty much.
    // This has another more important element though: Everyone:R must not be before denials because samba applies
    // any matched reads when it encounters a denial. e.g. Everyone:R,foo:D means foo can still read because D merely
    // ends the lookup, it doesn't take already granted permissions away. With that in mind we need to reshuffle
    // the ACEs so *all* D come first followed by all R and last all F.
    // https://docs.microsoft.com/en-us/windows/win32/secauthz/how-dacls-control-access-to-an-object
    // https://docs.microsoft.com/en-us/windows/win32/secauthz/order-of-aces-in-a-dacl

    // NOTE: D < R < F ordering would also be fine should we ever grow group support I think

    QStringList denials;
    QStringList readables;
    QStringList fulls;
    for (auto it = m_usersAcl.constBegin(); it != m_usersAcl.constEnd(); ++it) {
        const QString &userName = it.key();
        const QString access = it->value<QString>();
        if (access.isEmpty()) {
            continue; // --- undefined (no access)
        }
        // NB: we do not append access because samba is being inconsistent with itself. `net usershare info`
        // uses capital letters, but `net usershare add` will (for example) not accept capital D, so if you were to
        // take the output of info and feed it to add it'd error out -.- ... force everything lower case here
        // so it definitely works with add
        if (access == QLatin1String("D")) {
            denials << userName + QStringLiteral(":d");
        } else if (access == QLatin1String("R")) {
            readables << userName + QStringLiteral(":r");
        } else if (access == QLatin1String("F")) {
            fulls << userName + QStringLiteral(":f");
        } else {
            Q_UNREACHABLE(); // unmapped value WTH
        }
    }

    return (denials + readables + fulls).join(QLatin1Char(','));
}

UserPermissionModel::SambaACEHashMap UserPermissionModel::getUsersACEs() const {
    SambaACEHashMap result;
    for (auto it = m_usersAcl.constBegin(); it != m_usersAcl.constEnd(); ++it) {
        result.insert(it.key(), it->value<QString>());
    }
    return result;
}
