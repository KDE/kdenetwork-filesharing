/*
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
    SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2024 Thomas Duckworth <tduck973564@gmail.com>
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami

Kirigami.Dialog {
    id: passwordRoot
    padding: Kirigami.Units.largeSpacing

    property bool busy: false
    property alias errorMessage: persistentError.text
    property string password

    signal accepted()

    function handleKeyEvent(event) {
        if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
            // 🤮 https://bugreports.qt.io/browse/QTBUG-70934
            event.accepted = true;
            maybeAccept();
        } else if (event.key === Qt.Key_Escape) {
            // Handle Esc manually, within the sheet we'll want it to close the sheet rater than let the event fall
            // through to a higher level item (or worse yet QWidget).
            event.accepted = true;
            close();
        }
    }
    function isAcceptable() {
        return !passwordWarning.visible && verifyField.text && passwordField.text;
    }
    function maybeAccept() {
        if (!isAcceptable()) {
            return;
        }
        close();
        passwordRoot.password = passwordField.text;
        accepted();
    }
    function openAndClear() {
        verifyField.text = "";
        passwordField.text = "";
        passwordField.forceActiveFocus();
        open();
    }

    title: i18nc("@title", "Set password")

    ColumnLayout {
        id: mainColumn

        Layout.preferredWidth: Kirigami.Units.gridUnit * 15
        spacing: Kirigami.Units.smallSpacing

        // We don't use a FormLayout here because layouting breaks at small widths.
        ColumnLayout {
            id: inputLayout

            Layout.alignment: Qt.AlignHCenter
            visible: !passwordRoot.busy.running

            Kirigami.PasswordField {
                id: passwordField

                Layout.alignment: Qt.AlignLeft
                Layout.fillWidth: true
                placeholderText: i18nc("@label:textbox", "Password")

                // Reset external error on any password change
                onTextChanged: errorMessage = ""
                // Don't use onAccepted it's no bueno. See handleKeyEvent
            }
            Kirigami.PasswordField {
                id: verifyField

                Layout.alignment: Qt.AlignLeft
                Layout.fillWidth: true
                placeholderText: i18nc("@label:textbox", "Confirm password")

                // Reset external error on any password change
                onTextChanged: errorMessage = ""
                // Don't use onAccepted it's no bueno. See handleKeyEvent
            }
            Kirigami.InlineMessage {
                id: passwordWarning

                Layout.alignment: Qt.AlignLeft
                Layout.fillWidth: true
                text: i18nc("@label error message", "Passwords must match")
                type: Kirigami.MessageType.Error
                visible: passwordField.text !== "" && verifyField.text !== "" && passwordField.text !== verifyField.text
            }

            // This is a separate, second, message because otherwise we'd have to do a whole state conversion dance
            // logic that hurts my eyes.
            // This message is for problems in the backend that we need to tell the user about. It's different in
            // that the text is mutable and not controlled by us.
            Kirigami.InlineMessage {
                id: persistentError

                Layout.alignment: Qt.AlignLeft
                Layout.fillWidth: true
                type: Kirigami.MessageType.Error
                visible: text !== ""
            }
        }
        QQC2.BusyIndicator {
            id: busyIndicator

            Layout.alignment: Qt.AlignCenter
            running: passwordRoot.busy
            visible: running
        }
    }

    customFooterActions: Kirigami.Action {
        id: passButton

        enabled: isAcceptable()
        icon.name: "dialog-ok"
        text: i18nc("@action:button creates a new samba user with the user-specified password", "Set Password")
        onTriggered: maybeAccept()
    }
}
