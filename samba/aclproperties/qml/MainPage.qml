// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2021-2022 Harald Sitter <sitter@kde.org>

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.19 as Kirigami

import org.kde.filesharing.samba.acl 1.0 as Samba

Kirigami.ScrollablePage {
    id: page
    title: i18nc("@title", "Access Control Entries")

    ListView {
        Layout.fillHeight: true
        Layout.fillWidth: true
        model: Samba.Context.aceModel
        delegate: Kirigami.BasicListItem {
            icon: ROLE_ACEObject.inherited ? "emblem-locked" : undefined
            text: ROLE_Sid
            onClicked: root.pageStack.push("qrc:/org.kde.filesharing.samba.acl/qml/ACEPage.qml", {title: ROLE_Sid, aceObject: ROLE_ACEObject})
        }
    }

    footer: QQC2.Control {
        contentItem: Kirigami.FormLayout {
            Kirigami.Separator {
                Kirigami.FormData.label: i18nc("@title file/folder owner info", "Ownership")
                Kirigami.FormData.isSection: true
            }
            QQC2.Label {
                Kirigami.FormData.label: i18nc("@label", "Owner:")
                text: Samba.Context.owner
            }
            QQC2.Label {
                Kirigami.FormData.label: i18nc("@label", "Group:")
                text: Samba.Context.group
            }
        }
    }
}
