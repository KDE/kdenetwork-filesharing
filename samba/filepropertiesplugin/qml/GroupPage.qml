/*
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 *   SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
 *   SPDX-FileCopyrightText: 2026 Thomas Duckworth <tduck@filotimoproject.org>
 */

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.filesharing.samba as Samba

Kirigami.Page {
    padding: Kirigami.Units.smallSpacing

    Connections {
        target: sambaPlugin.groupManager

        function onReadyChanged() {
            if (sambaPlugin.groupManager.ready && sambaPlugin.groupManager.errorText.length === 0) {
                stack.popAndRetry()
            }
        }

        function onNeedsReboot() {
            sambaPlugin.needsReboot = true
        }

        function onHelpfulActionError(error) {
            actionErrorMessage.text = error
        }
    }

    QQC2.BusyIndicator {
        anchors.centerIn: parent
        visible: !sambaPlugin.groupManager.ready
        running: visible
    }

    ColumnLayout {
        anchors.fill: parent
        visible: sambaPlugin.groupManager.ready

        Kirigami.InlineMessage {
            id: actionErrorMessage
            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true
            type: Kirigami.MessageType.Error
            visible: text !== ""
            onVisibleChanged: {
                if (!visible) text = ""
            }
        }

        Kirigami.PlaceholderMessage {
            Layout.fillWidth: true
            Layout.fillHeight: true

            text: sambaPlugin.groupManager.errorText
            explanation: sambaPlugin.groupManager.errorExplanation

            helpfulAction: Kirigami.Action {
                enabled: sambaPlugin.groupManager.hasHelpfulAction
                icon.name: sambaPlugin.groupManager.helpfulActionIcon
                text: sambaPlugin.groupManager.helpfulActionText
                onTriggered: sambaPlugin.groupManager.performHelpfulAction()
            }
        }
    }
}
