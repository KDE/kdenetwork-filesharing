/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.filesharing.samba 1.0 as Samba

Kirigami.ScrollablePage {
    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.None

    background: Item {
    } /* this page is inside a tabbox, we want its background, not a window/page background */

    Keys.onPressed: {
        // We need to explicitly handle some keys inside the sheet. Since the sheet is no FocusScope we will catch
        // them here and feed them to the sheet instead.
        if (!changePassword.sheetOpen) {
            return;
        }
        changePassword.handleKeyEvent(event);
    }

    ChangePassword {
        id: changePassword

        function userCreated(userCreated) {
            enabled = true;
            changePassword.busy = false;
            if (userCreated) {
                close();
                stackReplace(pendingStack.pop());
            }
        }

        onAccepted: {
            enabled = false;
            busy = true;
            sambaPlugin.userManager.currentUser().addToSamba(password);
        }
    }

    Connections {
        target: sambaPlugin.userManager.currentUser()

        onAddToSambaError: function(error) {
            changePassword.errorMessage = error;
        }
        onInSambaChanged: changePassword.userCreated(target.inSamba)
    }

    ColumnLayout {
        QQC2.Label {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            text: xi18nc("@info", `
<para>
Samba uses a separate user database from the system one.
This requires you to set a separate Samba password for every user that you want to
be able to authenticate with.
</para>
<para>
Before shares can be accessed with your current user account, a Samba password needs to be set. 
</para>`)
            textFormat: Text.RichText // for xi18n markup
            wrapMode: Text.Wrap
        }
        QQC2.Button {
            Layout.alignment: Qt.AlignHCenter
            icon.name: "lock"
            text: i18nc("@action:button opens dialog to create new user", "Create Samba password")

            onClicked: changePassword.openAndClear()
        }
        QQC2.Label {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            text: xi18nc("@info", `
Additional user management and password management can be done using Samba's <command>smbpasswd</command>
command line utility.`)
            textFormat: Text.RichText // for xi18n markup
            wrapMode: Text.Wrap
        }
    }
}
