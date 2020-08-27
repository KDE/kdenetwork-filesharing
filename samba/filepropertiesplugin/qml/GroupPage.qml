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
            if (ready && member) { // already member nothing to do for us
                stackReplace(pendingStack.pop())
            }
        }
        onMadeMember: {
            stackReplace("RebootPage.qml")
        }
        onMakeMemberError: {
            var text = error
            if (text == "") { // unknown error :(
                text = i18nc("@label failed to change user groups so they can manage shares",
                             "Group changes failed.")
            }
            errorMessage.text = text
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
            id: errorMessage
            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true
            type: Kirigami.MessageType.Error
            visible: text != ""
        }

        Kirigami.PlaceholderMessage {
            text: manager.targetGroup ?
                    xi18nc("@label",
                           "To manage Samba user shares you need to be member of the <resource>%1</resource> group.",
                           manager.targetGroup) :
                    i18nc("@label", "You appear to not have sufficient permissions to manage Samba user shares.")
            helpfulAction: Kirigami.Action {
                enabled: manager.canMakeMember
                iconName: "resource-group-new"
                text: i18nc("@button makes user a member of the samba share group", "Make me a Group Member")
                onTriggered: manager.makeMember()
            }
        }
    }
}
