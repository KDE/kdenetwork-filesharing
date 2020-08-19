/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
*/

import QtQuick 2.12
import QtQuick.Controls 2.5 as QQC2
import QtQuick.Layouts 1.14
import org.kde.kirigami 2.4 as Kirigami
import org.kde.filesharing.samba 1.0 as Samba

// NOTE: Samba.ShareContext is a singleton its properties cannot be bound and need manual syncing back.

ColumnLayout {
    id: page

    QQC2.CheckBox {
        id: shareEnabled
        text: i18nc("@option:check", "Share this folder with other computers on the local network")
        checked: Samba.ShareContext.enabled
        onToggled: {
            Samba.ShareContext.enabled = checked
            Samba.Plugin.dirty = true
        }
    }

    ColumnLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        enabled: shareEnabled.checked

        RowLayout {
            Layout.fillWidth: true
            QQC2.Label {
                Layout.maximumWidth: Math.round(page.width / 2.0) // Don't let the label use more than half the space, elide the rest.
                text: i18nc("@label", "Name:")
                elide: Text.ElideRight
            }
            QQC2.TextField {
                id: nameField
                Layout.fillWidth: true
                text: Samba.ShareContext.name
                onTextEdited: {
                    if (text.length > Samba.ShareContext.maximumNameLength) {
                        tooLongMessage.visible = true;
                        // This is a soft limit, do not return.
                    } else {
                        tooLongMessage.visible = false
                    }

                    if (!Samba.ShareContext.isNameFree(text)) {
                        alreadyUsedError.visible = true;
                        return
                    }
                    alreadyUsedError.visible = false;

                    Samba.ShareContext.name = text
                    Samba.Plugin.dirty = true
                }
            }
        }

        Kirigami.InlineMessage {
            id: alreadyUsedError
            Layout.fillWidth: true
            type: Kirigami.MessageType.Error
            text: i18nc("@label",
                        "This name cannot be used. Share names must not be user names and there must not be two shares with the same name on the entire system.")
            visible: false
        }

        Kirigami.InlineMessage {
            id: tooLongMessage
            Layout.fillWidth: true
            type: Kirigami.MessageType.Warning
            text: i18nc("@label",
                        "This name may be too long. It can cause interoperability problems or get rejected by Samba.")
            visible: false
        }

        QQC2.CheckBox {
            text: i18nc("@option:check", "Allow guests")
            checked: Samba.ShareContext.guestEnabled
            onToggled: {
                Samba.ShareContext.guestEnabled = checked
                Samba.Plugin.dirty = true
            }
        }

        // TODO: this could benefit form some splitting. This is half the file.
        QQC2.ScrollView {
            id: scroll

            Layout.fillHeight: true
            Layout.fillWidth: true

            activeFocusOnTab: false
            Kirigami.Theme.colorSet: Kirigami.Theme.View
            Kirigami.Theme.inherit: false

            Component.onCompleted: background.visible = true // crashes when initialized with this. god knows why

            contentItem: TableView {
                id: view

                property bool itemComplete: false

                anchors.fill: parent
                anchors.margins: Kirigami.Units.smallSpacing
                clip: true
                interactive: false
                model: Samba.UserPermissionModel

                columnWidthProvider: function (column) {
                    // Give 2/3 of the width to the access column for better looks.
                    var accessWidth = Math.round(width / 1.5)
                    if (column == Samba.UserPermissionModel.ColumnAccess) {
                        return accessWidth
                    }
                    return width - accessWidth
                }

                Timer {
                    // Helper timer to delay force layouting through the event loop.
                    // We want width changes to recalculate the column widths so we need to force a layout run,
                    // when doing that directly in onWidthChanged that has a chance to produce errors so instead
                    // we'll queue a timeout for the next event loop to force the layout run.
                    //   TableView::forceLayout(): Cannot do an immediate re-layout during an ongoing layout!
                    id: forceLayoutTimer
                    interval: 0
                    running: false
                    repeat: false
                    onTriggered: {
                        // forceLayout docs say it must only be called after component completion, so make sure of that.
                        if (view.itemComplete) {
                            view.forceLayout()
                        }
                    }
                }

                onWidthChanged: forceLayoutTimer.start() // make sure columns get recalculated

                delegate: ColumnLayout {
                    // This is only a layout to conveniently forward the child size regardless of which child is in
                    // use.
                    Layout.fillWidth: true

                    QQC2.Label {
                        Layout.fillWidth: true
                        visible: !combo.visible
                        text: display === undefined ? "" : display
                        elide: Text.ElideMiddle
                    }

                    QQC2.ComboBox {
                        Layout.fillWidth: true
                        id: combo
                        textRole: "text"
                        valueRole: "value"
                        visible: column == Samba.UserPermissionModel.ColumnAccess
                        model: [
                            { value: undefined, text: "---" },
                            { value: "F", text: i18nc("@option:radio user can read&write", "Full Control") },
                            { value: "R", text: i18nc("@option:radio user can read", "Read Only") },
                            { value: "D", text: i18nc("@option:radio user not allowed to access share", "No Access") }
                        ]
                        onActivated: {
                            edit = currentValue // setData on model with edit role
                            Samba.Plugin.dirty = true
                        }
                        Component.onCompleted: currentIndex = indexOfValue(edit)
                    }
                }

                Component.onCompleted: itemComplete = true
            }
        }
    }

    QQC2.Button {
        Layout.fillWidth: true
        text: i18nc("@button", "Show Samba status monitor")
        onClicked: Samba.Plugin.showSambaStatus()
    }
}