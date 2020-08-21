/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
*/

import QtQuick 2.12
import QtQuick.Controls 2.5 as QQC2
import QtQuick.Layouts 1.14
import org.kde.kirigami 2.12 as Kirigami
import org.kde.filesharing.samba 1.0 as Samba

Kirigami.PlaceholderMessage {
    Samba.Installer {
        id: installer

        onInstalledChanged: {
            if (!installer.installed) {
                return
            }
            pendingStack.unshift("RebootPage.qml")
            stackReplace("GroupPage.qml")
        }
    }

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
