// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2022 Harald Sitter <sitter@kde.org>

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.19 as Kirigami

import org.kde.filesharing.samba.acl 1.0 as Samba

Kirigami.Page {
    id: page

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        width: parent.width - (Kirigami.Units.largeSpacing * 4)
        text: i18nc("@info", "No Permissions Found")
        explanation: i18nc("@info", "There are probably no SMB/Windows/Advanced permissions set on this file.")
        icon.name: "data-warning"
    }
}
