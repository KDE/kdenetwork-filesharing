// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2020-2022 Harald Sitter <sitter@kde.org>

#pragma once

#include <memory>

#include <QAbstractListModel>

#include "ace.h"

class Model : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool empty READ isEmpty NOTIFY emptyChanged)
public:
    enum class Role {
        Sid = Qt::UserRole,
        Type,
        Flags,
        Mask,
        ACEObject,
    };
    Q_ENUM(Role)

    using QAbstractListModel::QAbstractListModel;

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int intRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    void resetData(const QList<std::shared_ptr<ACE>> &acl);
    QList<std::shared_ptr<ACE>> acl() const;

    bool isEmpty()
    {
        return m_acl.isEmpty();
    }

Q_SIGNALS:
    void emptyChanged();

private:
    mutable QList<std::shared_ptr<ACE>> m_acl;
};
