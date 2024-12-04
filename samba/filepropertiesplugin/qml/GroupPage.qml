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
    // This page may be after the InstallPage and so we need to create a GroupManager when the page loads
    // rather than relying on the global instance that runs from the get go so we can evaluate the status AFTER
    // samba installation.
    Samba.GroupManager {
        id: manager
        onReadyChanged: {
            if (ready && manager.errorText.length === 0) { // no error; nothing for us to do
                stackReplace(pendingStack.pop())
            }
        }
        onNeedsReboot: () => {
            stackReplace("RebootPage.qml")
        }
        onHelpfulActionError: (error) => {
            actionErrorMessage.text = error
        }
    }

    QQC2.BusyIndicator {
        anchors.centerIn: parent
        visible: !manager.ready
        running: visible
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
            icon.name: "dialog-error"
            text: manager.errorText
            explanation: manager.errorExplanation
            helpfulAction: Kirigami.Action {
                enabled: manager.hasHelpfulAction
                icon.name: manager.helpfulActionIcon
                text: manager.helpfulActionText
                onTriggered: manager.performHelpfulAction()
            }
        }
    }
}
