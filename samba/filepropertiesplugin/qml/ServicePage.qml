/*
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 * SPDX-FileCopyrightText: 2025 Thomas Duckworth <tduck@filotimoproject.org>
 */

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.filesharing.samba as Samba

Kirigami.Page {
    padding: Kirigami.Units.smallSpacing

    Samba.ServiceHelper {
        id: serviceHelper
        onEnablingChanged: {
            if (!enabling && !failed) {
                stackReplace(pendingStack.pop())
            }
        }
    }

    QQC2.BusyIndicator {
        anchors.centerIn: parent
        running: serviceHelper.enabling
        visible: running
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        width: parent.width - (Kirigami.Units.largeSpacing * 4)
        visible: serviceHelper.failed && !serviceHelper.enabling

        type: Kirigami.PlaceholderMessage.Type.Error
        icon.name: "dialog-error"

        text: i18nc("@title", "Failed to Enable Samba")
        explanation: serviceHelper.errorMessage
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        width: parent.width - (Kirigami.Units.largeSpacing * 4)
        visible: !serviceHelper.failed && !serviceHelper.enabling

        text: xi18nc("@info", "The <application>Samba</application> file sharing service must be enabled and started before folders can be shared.")

        helpfulAction: Kirigami.Action {
            icon.name: "dialog-ok-apply"
            text: i18nc("@button", "Enable the Samba Service")
            onTriggered: serviceHelper.setup()
        }
    }
}
