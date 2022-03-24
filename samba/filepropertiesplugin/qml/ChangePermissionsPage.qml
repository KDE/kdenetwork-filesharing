/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2021 Slava Aseev <nullptrnine@basealt.ru>
*/

import QtQuick 2.12
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.14
import org.kde.kirigami 2.12 as Kirigami

Item {
    id: page

    ColumnLayout {
        anchors.fill: parent
        Layout.fillWidth: true
        Layout.fillHeight: true

        Kirigami.InlineMessage {
            id: changePermissionsError
            Layout.fillWidth: true
            visible: text !== ""
            type: Kirigami.MessageType.Error
        }

        ColumnLayout {
            Layout.fillWidth: true

            QQC2.Label {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                textFormat: Text.RichText
                text: xi18nc("@info", `
<para>The folder <filename>%1</filename> needs extra permissions for sharing to work.</para>
<para>Do you want to add these permissions now?</para><nl/>
`, sambaPlugin.shareContext.path)
            }

            Row {
                id: row
                Layout.fillWidth: true

                Repeater {
                    id: repeater
                    model: [
                        i18nc("@title", "File Path"),
                        i18nc("@title", "Current Permissions"),
                        i18nc("@title", "Required Permissions")
                    ]

                    QQC2.Label {
                        width: row.width / repeater.count
                        text: modelData
                    }
                }
            }

            QQC2.ScrollView {
                Layout.fillWidth: true

                contentItem: TableView {
                    id: view

                    property bool itemComplete: false

                    anchors.fill: parent
                    clip: true
                    interactive: false
                    model: sambaPlugin.permissionsHelper.model

                    columnWidthProvider: function (column) {
                        return view.model ? view.width / view.model.columnCount() : 0
                    }

                    Timer {
                        id: forceLayoutTimer
                        interval: 0
                        running: false
                        repeat: false
                        onTriggered: {
                            if (view.itemComplete) {
                                view.forceLayout()
                            }
                        }
                    }
                    onWidthChanged: forceLayoutTimer.start()

                    delegate: RowLayout {
                        Layout.fillWidth: true

                        QQC2.Label {
                            Layout.fillWidth: true
                            text: display
                            elide: Text.ElideMiddle
                        }
                    }

                    Component.onCompleted: itemComplete = true
                }
            }
        }

        Kirigami.ActionToolBar {
            alignment: Qt.AlignRight

            actions: [
                Kirigami.Action {
                    icon.name: "dialog-ok-apply"
                    text: i18nc("@action:button changes permissions", "Change Permissions")
                    onTriggered: {
                        var failedPaths = sambaPlugin.permissionsHelper.changePermissions()
                        if (failedPaths.length > 0) {
                            changePermissionsError.text =
                                i18nc("@label",
                                      "Could not change permissions for: %1. All permission changes have been reverted to initial state.",
                                      failedPaths.join(", "))
                        } else {
                            stack.pop()
                        }
                    }
                },
                Kirigami.Action {
                    icon.name: "dialog-cancel"
                    text: i18nc("@action:button cancels permissions change", "Cancel")
                    onTriggered: stack.pop()
                }
            ]
        }
    }
}
