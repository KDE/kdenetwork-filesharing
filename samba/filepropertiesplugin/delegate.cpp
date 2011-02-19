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

#include <QComboBox>
#include <klocale.h>

#include "delegate.h"

UserPermissionDelegate::UserPermissionDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

QWidget *UserPermissionDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem & /* option */,
        const QModelIndex &index) const
{
    if (index.column() != 1) {
        return 0;
    }

    QComboBox *comboBox = new QComboBox(parent);
    comboBox->addItem(i18n("---"));
    comboBox->addItem(i18n("Full Control"), QLatin1String("F"));
    comboBox->addItem(i18n("Read Only"), QLatin1String("R"));
    comboBox->addItem(i18n("Deny"), QLatin1String("D"));

    connect(comboBox, SIGNAL(activated(int)), this, SLOT(emitCommitData()));

    return comboBox;
}

void UserPermissionDelegate::setEditorData(QWidget *editor,
        const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
    if (!comboBox || (index.column() != 1)) {
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
    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
    if (!comboBox || (index.column() != 1)) {
        return;
    }

    model->setData(index, comboBox->itemData(comboBox->currentIndex()));
}

void UserPermissionDelegate::emitCommitData()
{
    emit commitData(qobject_cast<QWidget *>(sender()));
}
