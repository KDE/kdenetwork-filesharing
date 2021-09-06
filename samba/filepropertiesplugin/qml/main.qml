/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
*/

import QtQuick 2.12
import QtQuick.Controls 2.5 as QQC2
import QtQuick.Layouts 1.14
import org.kde.kirigami 2.4 as Kirigami
import org.kde.filesharing.samba 1.0 as Samba

QQC2.StackView {
    id: stack

    Samba.GroupManager {
        id: groupManager
    }

    function stackReplace(target) {
        stack.replace(stack.currentItem, target)
    }

    // The stack of pending pages. Once all backing data is ready we fill the pending stack with all
    // pages that ought to get shown eventually. This enables all pages to simply pop the next page and push
    // it into the stack once they are done with their thing.
    property var pendingStack: []

    initialItem: Item {
        QQC2.BusyIndicator {
            anchors.centerIn: parent
            running: !sambaPlugin.ready || !groupManager.ready

            onRunningChanged: {
                if (running) {
                    return
                }

                pendingStack.push("ACLPage.qml")
                if (!sambaPlugin.userManager.currentUser().inSamba) {
                    pendingStack.push("UserPage.qml")
                }
                if (!groupManager.member) {
                    pendingStack.push("GroupPage.qml")
                }
                if (!sambaPlugin.isSambaInstalled()) {
                    // NB: the plugin may be built without installer support!
                    if (Samba.Installer === undefined) {
                        pendingStack.push("MissingSambaPage.qml")
                    } else {
                        pendingStack.push("InstallPage.qml")
                    }
                }

                stack.clear()
                stack.push(pendingStack.pop())
            }
        }
    }
}
