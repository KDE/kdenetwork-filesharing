/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2026 Thomas Duckworth <tduck@filotimoproject.org>
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.filesharing.samba as Samba

Kirigami.Page {
    padding: Kirigami.Units.smallSpacing

    Samba.Installer {
        id: installer

        onInstalledChanged: {
            if (installer.installed) {
                sambaPlugin.needsReboot = true; // Mark that we need a reboot eventually
                stack.popPageAndReinit(); // Trigger the initialization chain again, which eventually will end up at RebootPage.
            }
        }
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        width: parent.width

        text: xi18nc("@info", "The <application>Samba</application> file sharing service must be installed before folders can be shared.")
        helpfulAction: Kirigami.Action {
            icon.name: "install"
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
