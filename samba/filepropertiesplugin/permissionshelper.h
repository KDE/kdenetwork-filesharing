/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2021 Slava Aseev <nullptrnine@basealt.ru>
    SPDX-FileCopyrightText: 2022 Harald Sitter <sitter@kde.org>
*/
#pragma once

#include <QAbstractTableModel>
#include <QFile>
#include <QObject>
#include <QVariant>

#include <QCoro/Task>

class QFileInfo;
class PermissionsHelper;
class UserPermissionModel;
class UserManager;

class PermissionsHelperModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Column { ColumnPath, ColumnOldPermissions, ColumnNewPermissions };
    Q_ENUM(Column)

    explicit PermissionsHelperModel(PermissionsHelper *helper);

    int rowCount(const QModelIndex &parent = {}) const override;
    Q_INVOKABLE int columnCount(const QModelIndex &parent = {}) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

private:
    const PermissionsHelper *parent;
};

class PermissionsHelper : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool permissionsChangeRequired READ permissionsChangeRequired NOTIFY permissionsChanged)
    Q_PROPERTY(bool hasPosixACL READ hasPosixACL NOTIFY permissionsChanged)
    Q_PROPERTY(QStringList pathsWithPosixACL MEMBER m_filesWithPosixACL NOTIFY permissionsChanged)
    Q_PROPERTY(PermissionsHelperModel *model MEMBER m_model CONSTANT)

public:
    struct PermissionsChangeInfo {
        QString path;
        QFile::Permissions oldPerm;
        QFile::Permissions newPerm;
    };

    explicit PermissionsHelper(const QString &path, const UserManager *userManager, const UserPermissionModel *permissionModel, QObject *parent = nullptr);

    const QList<PermissionsChangeInfo> &affectedPaths() const;
    bool permissionsChangeRequired() const;
    bool hasPosixACL() const;

    Q_INVOKABLE QStringList changePermissions();
    Q_INVOKABLE void reload();

Q_SIGNALS:
    void permissionsChanged();

private:
    void addPath(const QFileInfo &fileInfo, QFile::Permissions requiredPermissions);
    QCoro::Task<void> reloadInternal();

private:
    const QString m_path;
    const UserManager *m_userManager;
    const UserPermissionModel *m_permissionModel;

    PermissionsHelperModel *m_model = nullptr;
    QList<PermissionsChangeInfo> m_affectedPaths;
    QStringList m_filesWithPosixACL;
};
