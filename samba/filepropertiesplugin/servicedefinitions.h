/*
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 *   SPDX-FileCopyrightText: 2025 Thomas Duckworth <tduck@filotimoproject.org>
 *   SPDX-FileCopyrightText: 2025 Nate Graham <nate@kde.org>
 */

#include <QString>

using namespace Qt::StringLiterals;

constexpr auto DBUS_SYSTEMD_SERVICE = "org.freedesktop.systemd1"_L1;
constexpr auto DBUS_SYSTEMD_PATH = "/org/freedesktop/systemd1"_L1;
constexpr auto DBUS_SYSTEMD_MANAGER_INTERFACE = "org.freedesktop.systemd1.Manager"_L1;
