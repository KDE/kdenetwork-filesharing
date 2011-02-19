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

#ifndef model_h
#define model_h

#include <QStringList>
#include <QAbstractTableModel>
#include <ksambasharedata.h>

class KSambaShareData;

class UserPermissionModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    UserPermissionModel(KSambaShareData &shareData, QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex & index) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

    QString getAcl() const;

private:
    QStringList userList;
    KSambaShareData shareData;
    QVariantMap usersAcl;

    void setupData();
    QStringList getUsersList() const;
};

#endif
