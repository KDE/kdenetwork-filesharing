/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.filesharing.samba 1.0 as Samba

Item {
    anchors.fill: parent
    anchors.margins: Kirigami.Units.largeSpacing
    // This page may be after the InstallPage, so we need to create a GroupManager when the page loads
    // rather than relying on the global instance that runs from the get-go so we can evaluate the status AFTER
    // samba installation.
    Samba.GroupManager {
        id: manager

        onHelpfulActionError: error => {
            actionErrorMessage.text = error;
        }
        onNeedsReboot: () => {
            stackReplace("RebootPage.qml");
        }
        onReadyChanged: {
            if (ready && manager.errorText.length === 0) {
                // no error; nothing for us to do
                stackReplace(pendingStack.pop());
            }
        }
    }
    QQC2.BusyIndicator {
        anchors.centerIn: parent
        running: visible
        visible: !manager.ready
    }
    ColumnLayout {
        anchors.fill: parent
        visible: manager.ready

        Kirigami.InlineMessage {
            id: actionErrorMessage

            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true
            type: Kirigami.MessageType.Error
            visible: text != ""
        }
        Kirigami.PlaceholderMessage {
            explanation: manager.errorExplanation
            icon.name: "emblem-error"
            text: manager.errorText

            helpfulAction: Kirigami.Action {
                enabled: manager.hasHelpfulAction
                icon.name: manager.helpfulActionIcon
                text: manager.helpfulActionText

                onTriggered: manager.performHelpfulAction()
            }
        }
    }
}
