// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2021-2022 Harald Sitter <sitter@kde.org>

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.19 as Kirigami

import org.kde.filesharing.samba.acl 1.0 as Samba

Kirigami.ScrollablePage {
    id: page
    required property var aceObject

    component ACEObjectCheckBox: QQC2.CheckBox {
        property var objectProperty
        onToggled: page.aceObject[objectProperty] = checked
        // This is not bound, the user changes the checked state!
        Component.onCompleted: checked = aceObject[objectProperty]
        Layout.fillWidth: true
    }

    ColumnLayout {
        Kirigami.InlineMessage {
            Layout.fillWidth: true
            visible: aceObject.inherited
            type: Kirigami.MessageType.Information
            text: i18nc("@info",
                        "This permission entry was inherited from a parent container and can only be modified on that parent (e.g. a higher level directory).")
        }

        ColumnLayout {
            enabled: !aceObject.inherited
            width: parent.width
            RowLayout {
                QQC2.Label { text: i18nc("@label", "Type:") }
                QQC2.ComboBox {
                    model: Samba.Context.types
                    textRole: "text"
                    valueRole: "value"
                    onActivated: aceObject.type = currentValue
                    Component.onCompleted: currentIndex = indexOfValue(aceObject.type)
                }
            }
            RowLayout {
                QQC2.Label { text: i18nc("@label", "Applies to:") }
                QQC2.ComboBox {
                    model: Samba.Context.inheritances
                    textRole: "text"
                    valueRole: "value"
                    onActivated: aceObject.inheritance = currentValue
                    Component.onCompleted: currentIndex = indexOfValue(aceObject.inheritance)
                }
            }
            ColumnLayout {
                Layout.leftMargin: Kirigami.Units.gridUnit
                ACEObjectCheckBox {
                    objectProperty: "execute"
                    text: i18nc("@option:check", "Traverse folder / execute file")
                }
                ACEObjectCheckBox {
                    objectProperty: "readData"
                    text: i18nc("@option:check", "List folder / read data")
                }
                ACEObjectCheckBox {
                    objectProperty: "readAttributes"
                    text: i18nc("@option:check", "Read attributes")
                }
                ACEObjectCheckBox {
                    objectProperty: "readExtendedAttributes"
                    text: i18nc("@option:check", "Read extended attributes")
                }
                ACEObjectCheckBox {
                    objectProperty: "writeData"
                    text: i18nc("@option:check", "Create files / write data")
                }
                ACEObjectCheckBox {
                    objectProperty: "appendData"
                    text: i18nc("@option:check", "Create folders / append data")
                }
                ACEObjectCheckBox {
                    objectProperty: "writeAttributes"
                    text: i18nc("@option:check", "Write attributes")
                }
                ACEObjectCheckBox {
                    objectProperty: "writeExtendedAttributes"
                    text: i18nc("@option:check", "Write extended attributes")
                }
                ACEObjectCheckBox {
                    objectProperty: "canDeleteData"
                    text: i18nc("@option:check", "Delete subfolders and files")
                }
                ACEObjectCheckBox{
                    objectProperty: "canDelete"
                    text: i18nc("@option:check", "Delete")
                }
                ACEObjectCheckBox {
                    objectProperty: "readPermissions"
                    text: i18nc("@option:check", "Read permissions")
                }
                ACEObjectCheckBox {
                    objectProperty: "changePermissions"
                    text: i18nc("@option:check", "Change permissions")
                }
                ACEObjectCheckBox {
                    objectProperty: "takeOwnership"
                    text: i18nc("@option:check", "Take ownership")
                }
            }
            ACEObjectCheckBox {
                objectProperty: "noPropagate"
                text: i18nc("@option:check", "Only apply these permissions to objects and/or containers within this container")
            }
        }
    }
}
