/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2025 Thomas Duckworth <tduck@filotimoproject.org>
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

// When built without packagekit we cannot do auto-installation.
Kirigami.Page {
    padding: Kirigami.Units.smallSpacing
    
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
