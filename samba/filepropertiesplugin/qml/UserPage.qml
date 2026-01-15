/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2026 Thomas Duckworth <tduck@filotimoproject.org>
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.filesharing.samba 1.0 as Samba

Kirigami.ScrollablePage {
    padding: Kirigami.Units.largeSpacing

    background: Item {} /* this page is inside a tabbox, we want its background, not a window/page background */

    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.None

    ChangePassword {
        id: changePassword

        name: sambaPlugin.userManager.currentUser().name
        isPasswordChange: false

        function userCreated(userCreated)
        {
            enabled = true
            changePassword.busy = false
            if (userCreated) {
                close()
                stack.popPageAndReinit()
            }
        }

        onAccepted: {
            enabled = false
            busy = true
            sambaPlugin.userManager.currentUser().addToSamba(password)
        }
    }

    Connections {
        target: sambaPlugin.userManager.currentUser()
        onAddToSambaError: function (error) {
            changePassword.errorMessage = error
        }
        onInSambaChanged: changePassword.userCreated(target.inSamba)
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
            Layout.alignment: Qt.AlignHCenter
            icon.name: "lock-symbolic"
            text: i18nc("@action:button opens dialog to create new user", "Set Samba password")
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
