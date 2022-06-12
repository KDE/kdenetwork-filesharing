/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
*/

#ifndef AUTHHELPER_H
#define AUTHHELPER_H

#include <KAuth/ActionReply>

using namespace KAuth;

class AuthHelper: public QObject
{
    Q_OBJECT
public Q_SLOTS:
    ActionReply isuserknown(const QVariantMap &args);
    ActionReply createuser(const QVariantMap &args);
    ActionReply addtogroup(const QVariantMap &args);
};

#endif // AUTHHELPER_H
