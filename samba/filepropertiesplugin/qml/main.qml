/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2026 Thomas Duckworth <tduck@filotimoproject.org>
*/

import QtQuick
import QtQuick.Controls as QQC2
import org.kde.filesharing.samba 1.0

QQC2.StackView {
    id: stack

    // This is used in setup pages to re-trigger the initialization chain.
    function popPageAndReinit() {
        popPage()
        if (!sambaPlugin.ready) {
            sambaPlugin.reinitialize()
        }
    }

    // This is used once the plugin is ready, i.e. in ACLPage.qml.
    function popPage() {
        if (stack.depth > 1) {
            stack.pop()
        }
    }

    Connections {
        target: sambaPlugin

        function onPagePushed(url) {
            stack.push(url)
        }

        function onReadyChanged() {
            if (sambaPlugin.ready) {
                // ACLPage.qml is the final, user-facing page for after everything is set up.
                stack.replace("qrc:/org.kde.filesharing.samba/qml/ACLPage.qml")
            }
        }
    }

    initialItem: Item {
        objectName: "loader"
        // This is always at the bottom of the stack so if a page pops itself and the backend is still
        // doing something before it can push another page, there's a nice-looking loading indicator.
        QQC2.BusyIndicator {
            anchors.centerIn: parent
            running: !sambaPlugin.ready
        }
    }
}
