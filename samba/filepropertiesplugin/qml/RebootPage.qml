/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
*/

import QtQuick 2.12
import QtQuick.Controls 2.5 as QQC2
import QtQuick.Layouts 1.14
import org.kde.kirigami 2.12 as Kirigami
import org.kde.filesharing.samba 1.0 as Samba

Item {
    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        width: parent.width

        text: i18nc("@label", "Restart the computer to complete the changes.")
        helpfulAction: Kirigami.Action {
            iconName: "system-restart"
            text: i18nc("@button restart the system", "Restart")
            onTriggered: sambaPlugin.reboot()
        }
    }
}
