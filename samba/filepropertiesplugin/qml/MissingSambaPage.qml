/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
*/

import QtQuick 2.12
import QtQuick.Controls 2.5 as QQC2
import QtQuick.Layouts 1.14
import org.kde.kirigami as Kirigami

// When built without packagekit we cannot do auto-installation.
Item {
    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        width: parent.width - (Kirigami.Units.largeSpacing * 4)

        icon.name: "dialog-error"

        text: xi18nc("@info", "File sharing service unavailable")
        explanation: i18n("Please ensure the Samba service is enabled and running.\nIf you haven't disabled it manually, consider reporting a bug to your distribution.")

        helpfulAction: Kirigami.Action {
            icon.name: "mail-message-new"
            text: i18n("Report a bug")
            onTriggered: Qt.openUrlExternally(sambaPlugin.bugReportUrl)
            visible: sambaPlugin.bugReportUrl != null
        }
    }
}
