/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
*/

import QtQuick 2.12
import QtQuick.Controls 2.5 as QQC2
import QtQuick.Layouts 1.14
import org.kde.kirigami 2.12 as Kirigami
import org.kde.filesharing.samba 1.0 as Samba

Kirigami.ScrollablePage {
    background: Item {} /* this page is inside a tabbox, we want its background, not a window/page background */

    Keys.onPressed: {
        // We need to explicitly handle some keys inside the sheet. Since the sheet is no FocusScope we will catch
        // them here and feed them to the sheet instead.
        if (!changePassword.sheetOpen) {
            return
        }
        changePassword.handleKeyEvent(event)
    }

    ChangePassword {
        // This is an overlay sheet, it requires a scrollable page to anchor on.
        id: changePassword

        function userCreated(userCreated)
        {
            enabled = true
            changePassword.busy = false
            if (userCreated) {
                close()
                stack.push(pendingStack.pop())
            }
        }

        onAccepted: {
            enabled = false
            busy = true
            sambaPlugin.userManager.currentUser().addToSamba(password)
        }
    }

    Connections {
        // ChangePassword being a sheet it's being crap to use and can't even connect to nothing.
        target: sambaPlugin.userManager.currentUser()
        onInSambaChanged: changePassword.userCreated(target.inSamba)
        onAddToSambaError: changePassword.errorMessage = error
    }

    ColumnLayout {
        QQC2.Label {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            textFormat: Text.RichText // for xi18n markup
            text: xi18nc("@info", `
<para>
Samba uses a separate user database from the system one.
This requires you to set a separate Samba password for every user that you want to
be able to authenticate with.
</para>
<para>
Before you can access shares with your current user account you need to set a Samba password.
</para>`)
            wrapMode: Text.Wrap
        }

        QQC2.Button {
            text: i18nc("@action:button opens dialog to create new user", "Create My Samba User")
            onClicked: changePassword.openAndClear()
        }

        QQC2.Label {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            textFormat: Text.RichText // for xi18n markup
            text: xi18nc("@info", `
Additional user management and password management can be done using Samba's <command>smbpasswd</command>
command line utility.`)
            wrapMode: Text.Wrap
        }
    }
}
