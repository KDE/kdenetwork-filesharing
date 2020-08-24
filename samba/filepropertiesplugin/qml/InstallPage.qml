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
    Samba.Installer {
        id: installer

        onInstalledChanged: {
            if (!installer.installed) {
                return
            }
            // Installation is a bit special because it eventually ends in a reboot. So we push that page onto the
            // pending pages and move to the group page. The group page in turn will either explicitly
            // go to the reboot page (if group changes were made) or pop a pending page if groups are already cool.
            // In either event it'll end up on the reboot page because of our pending meddling here.
            pendingStack.push("RebootPage.qml")
            stackReplace("GroupPage.qml")
        }
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        width: parent.width

        text: i18nc("@label", "Samba must be installed before folders can be shared.")
        helpfulAction: Kirigami.Action {
            iconName: "install"
            text: i18nc("@button", "Install Samba")
            onTriggered: installer.install()
            enabled: !installer.installing && !installer.installed
        }

        QQC2.Label {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            text: i18nc("@label", "The Samba package failed to install.")
            wrapMode: Text.Wrap
            visible: installer.failed
        }
        QQC2.ProgressBar {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            Layout.margins: Kirigami.Units.largeSpacing * 2
            indeterminate: true
            visible: installer.installing
        }
    }
}
