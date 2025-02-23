/*
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
    SPDX-FileCopyrightText: 2025 Thomas Duckworth <tduck@filotimoproject.org>
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.filesharing.samba 1.0 as Samba

Kirigami.Dialog {
    id: addressesRoot

    padding: Kirigami.Units.largeSpacing

    title: i18nc("@title", "Connection Addresses")

    standardButtons: Kirigami.Dialog.NoButton
    showCloseButton: true

    contentItem: ColumnLayout {
        QQC2.Label {
            text: i18nc("@info:usagetip", "Use any of the following addresses to connect to this share:")
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        ColumnLayout {
            Layout.fillWidth: true

            Repeater {
                id: addressesRepeater
                model: sambaPlugin.addressList

                ColumnLayout {
                    RowLayout {
                        spacing: Kirigami.Units.mediumSpacing
                        Layout.fillWidth: true

                        Kirigami.SelectableLabel {
                            id: addressLabel
                            Layout.fillWidth: true
                            // Samba uses UNCs natively, not URLs - Windows does not support `smb://` URLs
                            // See https://unc.us/
                            text: "\\\\%1\\%2".arg(modelData).arg(sambaPlugin.shareContext.name)
                            font.family: "monospace"
                            Layout.leftMargin: Kirigami.Units.gridUnit
                            Layout.alignment: Qt.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        QQC2.Button {
                            id: copyAddressButton
                            Layout.alignment: Qt.AlignRight
                            icon.name: "edit-copy-symbolic"
                            text: i18nc("@action:button", "Copy to Clipboard")
                            display: QQC2.AbstractButton.IconOnly
                            onClicked: {
                                sambaPlugin.copyAddressToClipboard(addressLabel.text);
                            }
                            QQC2.ToolTip {
                                text: copyAddressButton.text
                                visible: copyAddressButton.hovered || (Kirigami.Settings.tabletMode && copyAddressButton.pressed)
                            }
                        }
                    }

                    Kirigami.Separator {
                        visible: index !== addressesRepeater.count - 1
                        Layout.fillWidth: true
                    }
                }
            }
        }
    }
}