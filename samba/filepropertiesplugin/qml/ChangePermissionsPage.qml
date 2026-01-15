/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2021 Slava Aseev <nullptrnine@basealt.ru>
    SPDX-FileCopyrightText: 2022 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2026 Thomas Duckworth <tduck@filotimoproject.org>
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Kirigami.Page {
    id: page

    padding: Kirigami.Units.smallSpacing

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
<para>Add these permissions now?</para><nl/>
`, sambaPlugin.shareContext.path)
            }

            QQC2.ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true

                contentItem: TableView {
                    id: view

                    clip: true
                    interactive: false
                    model: sambaPlugin.permissionsHelper.model

                    property int maxColumn: model.columnCount() - 1

                    delegate: QQC2.Label {
                        font.bold: row == 0 /* header */ ? true : false
                        Layout.fillWidth: true
                        text: display
                        font.family: "monospace"
                        rightPadding: column >= view.maxColumn ? 0 : Kirigami.Units.largeSpacing
                    }
                }
            }
        }

        Kirigami.ActionToolBar {
            alignment: Qt.AlignRight
            flat: false

            actions: [
                Kirigami.Action {
                    icon.name: "dialog-ok-apply"
                    text: i18nc("@action:button changes permissions", "Change Permissions")
                    onTriggered: {
                        var failedPaths = sambaPlugin.permissionsHelper.changePermissions()
                        if (failedPaths.length > 0) {
                            changePermissionsError.text =
                                i18nc("@label",
                                      "Could not change permissions for: %1. All permissions changes have been reverted.",
                                      failedPaths.join(", "))
                        } else {
                            stack.popPage()
                        }
                    }
                },
                Kirigami.Action {
                    icon.name: "dialog-cancel"
                    text: i18nc("@action:button cancels permissions change", "Cancel")
                    onTriggered: stack.popPage()
                }
            ]
        }
    }
}
