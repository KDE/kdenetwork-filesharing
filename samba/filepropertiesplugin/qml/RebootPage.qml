/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2025 Thomas Duckworth <tduck@filotimoproject.org>
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.filesharing.samba 1.0 as Samba

Kirigami.Page {
    padding: Kirigami.Units.smallSpacing

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        width: parent.width

        text: i18nc("@label", "Restart the computer to apply changes.")
        helpfulAction: Kirigami.Action {
            icon.name: "system-restart"
            text: i18nc("@button restart the system", "Restart")
            onTriggered: sambaPlugin.reboot()
        }
    }
}
