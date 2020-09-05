/*
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
    SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
    SPDX-FileCopyrightText: 2020 Harld Sitter <sitter@kde.org>
*/

import QtQuick 2.6
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5 as QQC2

import org.kde.kirigami 2.8 as Kirigami

Kirigami.OverlaySheet {
    id: passwordRoot

    property string password
    property bool busy: false
    property alias errorMessage: persistentError.text
    signal accepted()

    header: Kirigami.Heading {
        // FIXME make qml user name aware so we can be more contextually accurate and label it 'set password for foo'
        text: i18nc("@title", "Set password")
    }

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

        passwordRoot.password = passwordField.text
        accepted()
    }

    function handleKeyEvent(event) {
        if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
            // 🤮 https://bugreports.qt.io/browse/QTBUG-70934
            event.accepted = true
            maybeAccept()
        } else if (event.key === Qt.Key_Escape) {
            // Handle Esc manually, within the sheet we'll want it to close the sheet rater than let the event fall
            // through to a higher level item (or worse yet QWidget).
            event.accepted = true
            close()
        }
    }

    ColumnLayout {
        id: mainColumn
        spacing: Kirigami.Units.smallSpacing
        Layout.preferredWidth: Kirigami.Units.gridUnit * 15

        // We don't use a FormLayout here because layouting breaks at small widths.
        ColumnLayout {
            id: inputLayout
            Layout.alignment: Qt.AlignHCenter
            visible: !busy.running

            Kirigami.PasswordField {
                id: passwordField
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
                placeholderText: i18nc("@label:textbox", "Password")

                // Reset external error on any password change
                onTextChanged: errorMessage = ""
                // Don't use onAccepted it's no bueno. See handleKeyEvent
            }

            Kirigami.PasswordField {
                id: verifyField
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
                placeholderText: i18nc("@label:textbox", "Confirm password")

                // Reset external error on any password change
                onTextChanged: errorMessage = ""
                // Don't use onAccepted it's no bueno. See handleKeyEvent
            }

            Kirigami.InlineMessage {
                id: passwordWarning
                Layout.fillWidth: true
                type: Kirigami.MessageType.Error
                text: i18nc("@label error message", "Passwords must match")
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

            QQC2.Button {
                id: passButton
                text: i18nc("@action:button creates a new samba user with the user-specified password", "Set Password")
                enabled: isAcceptable()
                Layout.alignment: Qt.AlignRight
                onClicked: maybeAccept()
            }
        }

        QQC2.BusyIndicator {
            id: busyIndicator
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: running
            running: passwordRoot.busy
        }
    }
}
