/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2011 Rodrigo Belem <rclbelem@gmail.com>
    SPDX-FileCopyrightText: 2019 Nate Graham <nate@kde.org>
*/

#include <QComboBox>
#include <KLocalizedString>

#include "delegate.h"
#include "model.h"

UserPermissionDelegate::UserPermissionDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

QWidget *UserPermissionDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem & /* option */,
        const QModelIndex &index) const
{
    if (index.column() != UserPermissionModel::ColumnAccess) {
        return nullptr;
    }

    auto comboBox = new QComboBox(parent);
    comboBox->addItem(i18n("---"));
    comboBox->addItem(i18n("Full Control"), QLatin1String("F"));
    comboBox->addItem(i18n("Read Only"), QLatin1String("R"));
    comboBox->addItem(i18n("Deny"), QLatin1String("D"));

    return comboBox;
}

void UserPermissionDelegate::setEditorData(QWidget *editor,
        const QModelIndex &index) const
{
    auto comboBox = qobject_cast<QComboBox *>(editor);
    if (!comboBox || (index.column() != UserPermissionModel::ColumnAccess)) {
        return;
    }

    int pos = comboBox->findData(index.model()->data(index, Qt::EditRole));
    if (pos == -1) {
        pos = 0;
    }

    comboBox->setCurrentIndex(pos);
}

void UserPermissionDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
        const QModelIndex &index) const
{
    auto comboBox = qobject_cast<QComboBox *>(editor);
    if (!comboBox || (index.column() != UserPermissionModel::ColumnAccess)) {
        return;
    }

    model->setData(index, comboBox->itemData(comboBox->currentIndex()));
}
