// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2020-2022 Harald Sitter <sitter@kde.org>

#include "model.h"

#include <QMetaEnum>

#include "aceobject.h"

int Model::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_acl.count();
}

QVariant Model::data(const QModelIndex &index, int intRole) const
{
    if (!index.isValid()) {
        return {};
    }

    const auto ace = m_acl.at(index.row());
    switch (static_cast<Role>(intRole)) {
    case Role::Sid:
        return ace->sid;
    case Role::Type:
        return ace->type;
    case Role::Flags:
        return ace->flags;
    case Role::Mask:
        return ace->mask;
    case Role::ACEObject:
        return QVariant::fromValue(new ACEObject(ace));
    }

    return {};
}

QHash<int, QByteArray> Model::roleNames() const
{
    static QHash<int, QByteArray> roles;
    if (!roles.isEmpty()) {
        return roles;
    }

    const QMetaEnum roleEnum = QMetaEnum::fromType<Role>();
    for (int i = 0; i < roleEnum.keyCount(); ++i) {
        const int value = roleEnum.value(i);
        Q_ASSERT(value != -1);
        roles[static_cast<int>(value)] = QByteArray("ROLE_") + roleEnum.valueToKey(value);
    }
    return roles;
}

void Model::resetData(const QList<std::shared_ptr<ACE>> &acl)
{
    beginResetModel();
    m_acl = acl;
    endResetModel();
    Q_EMIT emptyChanged();
}

QList<std::shared_ptr<ACE>> Model::acl() const
{
    return m_acl;
}
