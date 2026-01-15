/*
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 * SPDX-FileCopyrightText: 2026 Thomas Duckworth <tduck@filotimoproject.org>
 */

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.filesharing.samba as Samba

Kirigami.Page {
    padding: Kirigami.Units.smallSpacing

    Connections {
        target: sambaPlugin.serviceHelper
        onEnablingChanged: {
            if (!sambaPlugin.serviceHelper.enabling && !sambaPlugin.serviceHelper.failed) {
                // Lock the UI state immediately so the placeholder doesn't reappear
                // when we transition to the next page, which looks weird.
                message.visible = false

                stack.popPageAndReinit()
            }
        }
    }

    QQC2.BusyIndicator {
        anchors.centerIn: parent
        running: sambaPlugin.serviceHelper.enabling
        visible: running
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        width: parent.width - (Kirigami.Units.largeSpacing * 4)
        visible: sambaPlugin.serviceHelper.failed && !sambaPlugin.serviceHelper.enabling

        type: Kirigami.PlaceholderMessage.Type.Actionable
        icon.name: "dialog-error"

        text: i18nc("@title", "Failed to Enable Samba")
        explanation: sambaPlugin.serviceHelper.errorMessage
    }

    Kirigami.PlaceholderMessage {
        id: message

        anchors.centerIn: parent
        width: parent.width - (Kirigami.Units.largeSpacing * 4)

        visible: !sambaPlugin.serviceHelper.failed
        && !sambaPlugin.serviceHelper.enabling

        text: xi18nc("@info", "The <application>Samba</application> file sharing service must be enabled and started before folders can be shared.")

        helpfulAction: Kirigami.Action {
            icon.name: "dialog-ok-apply"
            text: i18nc("@button", "Enable the Samba Service")
            onTriggered: sambaPlugin.serviceHelper.setup()
        }
    }
}
