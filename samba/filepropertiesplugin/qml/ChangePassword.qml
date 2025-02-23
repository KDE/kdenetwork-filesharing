/*
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
    SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2025 Thomas Duckworth <tduck@filotimoproject.org>
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami

Kirigami.Dialog {
    id: passwordRoot

    padding: Kirigami.Units.largeSpacing

    property string name
    property string password
    property bool busy: false
    property bool isPasswordChange: false
    property alias errorMessage: persistentError.text

    title: isPasswordChange ? i18nc("@title", "Change password for '%1'", name) : i18nc("@title", "Set password for '%1'", name)

    standardButtons: Kirigami.Dialog.NoButton
    showCloseButton: false
    flatFooterButtons: false

    function openAndClear() {
        verifyField.text = ""
        passwordField.text = ""
        passwordField.forceActiveFocus()
        open()
    }

    function isAcceptable() {
        return !passwordWarning.visible && verifyField.text && passwordField.text;
    }

    function maybeAccept() {
        if (!isAcceptable()) {
            return
        }

        close()
        passwordRoot.password = passwordField.text
        accepted()
    }

    contentItem: ColumnLayout {
        ColumnLayout {
            id: inputLayout
            spacing: Kirigami.Units.smallSpacing
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            visible: !passwordRoot.busy

            Kirigami.PasswordField {
                id: passwordField
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
                placeholderText: i18nc("@label:textbox", "Password")

                // Reset external error on any password change
                onTextChanged: errorMessage = ""
                onAccepted: maybeAccept()
            }

            Kirigami.PasswordField {
                id: verifyField
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
                placeholderText: i18nc("@label:textbox", "Confirm password")

                // Reset external error on any password change
                onTextChanged: errorMessage = ""
                onAccepted: maybeAccept()
            }

            Kirigami.InlineMessage {
                id: passwordWarning
                Layout.fillWidth: true
                type: Kirigami.MessageType.Error
                text: i18nc("@label error message", "Passwords must match.")
                visible: passwordField.text != "" && verifyField.text != "" && passwordField.text != verifyField.text
                Layout.alignment: Qt.AlignLeft
            }

            // This is a separate, second, message because otherwise we'd have to do a whole state conversion dance
            // logic that hurts my eyes.
            // This message is for problems in the backend that we need to tell the user about. It's different in
            // that the text is mutable and not controlled by us.
            Kirigami.InlineMessage {
                id: persistentError
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
                type: Kirigami.MessageType.Error
                visible: text != ""
            }

        }

        QQC2.BusyIndicator {
            id: busyIndicator
            Layout.alignment: Qt.AlignCenter
            visible: running
            running: passwordRoot.busy
        }
    }

    customFooterActions: [
        Kirigami.Action {
            id: passButton
            icon.name: "dialog-ok"
            text: i18nc("@action:button creates a new samba user with the user-specified password", "Set Password")
            enabled: isAcceptable()
            onTriggered: maybeAccept()
        },
        Kirigami.Action {
            id: cancelButton
            icon.name: "dialog-cancel"
            text: i18nc("@action:button", "Cancel")
            onTriggered: close()
        }
    ]
}
