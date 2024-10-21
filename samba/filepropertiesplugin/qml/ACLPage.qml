/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2021 Slava Aseev <nullptrnine@basealt.ru>
    SPDX-FileCopyrightText: 2023 ivan tkachenko <me@ratijas.tk>
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.filesharing.samba 1.0 as Samba

// NOTE: sambaPlugin.shareContext is a singleton its properties cannot be bound and need manual syncing back.

Item {
    // NOTE: we cannot use a Kirigami.Page for this because it adds excessive padding that we can't disable
    //   so it'd very awkwardly space within the properties dialog reducing the available space a lot and looking
    //   silly. Alas, Column is also not grand because it gets collapsed by the sheet, so we use a fixed size
    //   Item in place of a Page that gets covered by the Sheet and then that Item is filled by a Column.
    id: page

    anchors {
        fill: parent
        margins: Kirigami.Units.smallSpacing
    }

    Kirigami.OverlaySheet {
        id: denialSheet

        property bool shownOnce: false // maybeShow() this sheet only once per run not annoy users too much

        function maybeOpen() {
            if (shownOnce) {
                return;
            }
            shownOnce = true;
            open();
        }

        parent: page // there's a bug where the sheet doesn't manage to find its parent, explicitly set it

        header: Kirigami.Heading {
            text: i18nc("@title", "Denying Access")
        }

        QQC2.Label {
            Layout.fillWidth: true
            text: xi18nc("@info", `
Denying access prevents using this share even when another access rule might grant access. A denial rule always
takes precedence. In particular denying access to <resource>Everyone</resource> actually disables access for everyone.
It is generally not necessary to deny anyone because the regular directory and file permissions still apply to shared
directories. A user who does not have access to the directory locally will still not be able to access it remotely even
when the Share access rules would allow it.`)
            textFormat: Text.RichText // for xi18n markup; this also means newlines are ignored!
            wrapMode: Text.Wrap
        }
    }
    ColumnLayout {
        anchors.fill: parent

        Kirigami.InlineMessage {
            id: changePermissionsWarning

            Layout.fillWidth: true
            showCloseButton: true
            text: i18nc("@label", "This folder needs extra permissions for sharing to work")
            type: Kirigami.MessageType.Warning
            visible: sambaPlugin.permissionsHelper.permissionsChangeRequired

            actions: [
                Kirigami.Action {
                    text: i18nc("@action:button opens the change permissions page", "Fix Permissions")

                    onTriggered: stack.push("ChangePermissionsPage.qml")
                }
            ]
        }
        Kirigami.InlineMessage {
            id: posixACLWarning

            Layout.fillWidth: true
            showCloseButton: true
            text: xi18nc("@label", "The share might not work properly because share folder or its paths has Advanced Permissions: %1", sambaPlugin.permissionsHelper.pathsWithPosixACL.join(", "))
            type: Kirigami.MessageType.Warning
            visible: sambaPlugin.permissionsHelper.hasPosixACL
        }
        QQC2.CheckBox {
            id: shareEnabled

            Layout.fillWidth: true
            checked: sambaPlugin.shareContext.enabled
            text: i18nc("@option:check", "Share this folder with other computers on the local network")

            onToggled: {
                sambaPlugin.shareContext.enabled = checked;
                sambaPlugin.dirty = true;
            }
        }
        Kirigami.Separator {
            Layout.fillWidth: true
        }
        ColumnLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            enabled: shareEnabled.checked

            RowLayout {
                Layout.fillWidth: true

                QQC2.Label {
                    Layout.maximumWidth: Math.round(page.width / 2.0) // Don't let the label use more than half the space, elide the rest.
                    elide: Text.ElideRight
                    text: i18nc("@label", "Name:")
                }
                QQC2.TextField {
                    id: nameField

                    Layout.fillWidth: true
                    text: sambaPlugin.shareContext.name

                    onTextEdited: {
                        if (text.length > sambaPlugin.shareContext.maximumNameLength) {
                            tooLongMessage.visible = true;
                            // This is a soft limit, do not return.
                        } else {
                            tooLongMessage.visible = false;
                        }
                        if (!sambaPlugin.shareContext.isNameFree(text)) {
                            alreadyUsedError.visible = true;
                            return;
                        }
                        alreadyUsedError.visible = false;
                        sambaPlugin.shareContext.name = text;
                        sambaPlugin.dirty = true;
                    }
                }
            }
            Kirigami.InlineMessage {
                id: alreadyUsedError

                Layout.fillWidth: true
                text: i18nc("@label", "This name cannot be used. Share names must not be user names and there must not be two shares with the same name on the entire system.")
                type: Kirigami.MessageType.Error
                visible: false
            }
            Kirigami.InlineMessage {
                id: tooLongMessage

                Layout.fillWidth: true
                text: i18nc("@label", "This name may be too long. It can cause interoperability problems or get rejected by Samba.")
                type: Kirigami.MessageType.Warning
                visible: false
            }
            QQC2.CheckBox {
                id: allowGuestBox

                Layout.fillWidth: true
                checked: sambaPlugin.shareContext.guestEnabled
                enabled: sambaPlugin.shareContext.canEnableGuest
                text: i18nc("@option:check", "Allow guests")

                onToggled: {
                    sambaPlugin.shareContext.guestEnabled = checked;
                    sambaPlugin.dirty = true;
                }
            }
            QQC2.Label {
                Layout.fillWidth: true
                enabled: false // looks more visually connected if both are disabled
                font: Kirigami.Theme.smallFont
                text: i18nc("@label", "Guest access is disabled by the system's Samba configuration.")
                visible: !allowGuestBox.enabled
                wrapMode: Text.Wrap
            }

            // TODO: this could benefit form some splitting. This is half the file.
            QQC2.ScrollView {
                id: scroll

                Kirigami.Theme.colorSet: Kirigami.Theme.View
                Kirigami.Theme.inherit: false
                Layout.fillHeight: true
                Layout.fillWidth: true
                activeFocusOnTab: false

                Component.onCompleted: {
                    if (background) {
                        background.visible = true;
                    }
                }

                TableView {
                    id: view

                    property bool itemComplete: false

                    // This wouldn't work with horizontal scrollbar, but here
                    // it doesn't matter, because it is never visible.
                    bottomMargin: Kirigami.Units.smallSpacing
                    columnSpacing: Kirigami.Units.smallSpacing
                    columnWidthProvider: column => {
                        // Give 2/3 of the width to the access column for better looks.
                        const availableWidth = width - columnSpacing;
                        var accessWidth = Math.round(availableWidth / 1.5);
                        if (column === Samba.UserPermissionModel.ColumnAccess) {
                            return accessWidth;
                        }
                        return availableWidth - accessWidth;
                    }
                    model: sambaPlugin.userPermissionModel
                    rowSpacing: Kirigami.Units.smallSpacing
                    topMargin: Kirigami.Units.smallSpacing

                    delegate: RowLayout {
                        // This is only a layout to conveniently forward the child size regardless of which child is in
                        // use.
                        Layout.fillWidth: true

                        TableView.onReused: combo.loadCurrentIndex()

                        QQC2.Label {
                            Layout.fillWidth: true
                            Layout.leftMargin: view.columnSpacing
                            elide: Text.ElideMiddle
                            text: display === undefined ? "" : display
                            visible: column !== Samba.UserPermissionModel.ColumnAccess
                        }
                        QQC2.ComboBox {
                            id: combo

                            function loadCurrentIndex() {
                                currentIndex = indexOfValue(edit);
                            }

                            Layout.fillWidth: true
                            Layout.rightMargin: view.columnSpacing
                            model: [
                                {
                                    value: undefined,
                                    text: "---"
                                },
                                {
                                    value: "F",
                                    text: i18nc("@option:radio user can read&write", "Full Control")
                                },
                                {
                                    value: "R",
                                    text: i18nc("@option:radio user can read", "Read Only")
                                },
                                {
                                    value: "D",
                                    text: i18nc("@option:radio user not allowed to access share", "No Access")
                                }
                            ]
                            textRole: "text"
                            valueRole: "value"
                            visible: column === Samba.UserPermissionModel.ColumnAccess

                            Component.onCompleted: loadCurrentIndex()
                            onActivated: {
                                edit = currentValue; // setData on model with edit role
                                if (currentValue === 'D') {
                                    denialSheet.maybeOpen();
                                }
                                sambaPlugin.dirty = true;
                                sambaPlugin.permissionsHelper.reload();
                            }
                        }
                    }

                    Component.onCompleted: itemComplete = true
                    onWidthChanged: forceLayoutTimer.start() // make sure columns get recalculated

                    Timer {
                        // Helper timer to delay force layouting through the event loop.
                        // We want width changes to recalculate the column widths so we need to force a layout run,
                        // when doing that directly in onWidthChanged that has a chance to produce errors so instead
                        // we'll queue a timeout for the next event loop to force the layout run.
                        //   TableView::forceLayout(): Cannot do an immediate re-layout during an ongoing layout!
                        id: forceLayoutTimer

                        interval: 0
                        repeat: false
                        running: false

                        onTriggered: {
                            // forceLayout docs say it must only be called after component completion, so make sure of that.
                            if (view.itemComplete) {
                                view.forceLayout();
                            }
                        }
                    }
                }
            }
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

            onAddToSambaError: function (error) {
                changePassword.errorMessage = error;
            }
            onInSambaChanged: changePassword.userCreated(target.inSamba)
        }

        QQC2.Button {
            Layout.fillWidth: true
            icon.name: "help-about"
            text: i18nc("@button", "Show Samba status monitor")

            onClicked: sambaPlugin.showSambaStatus()
        }

        QQC2.Button {
            Layout.fillWidth: true
            icon.name: "lock"
            text: i18nc("@button", "Change Samba password")

            onClicked: changePassword.openAndClear()
        }

    }
}
