/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
*/

import QtQuick 2.12
import QtQuick.Controls 2.5 as QQC2
import QtQuick.Layouts 1.14
import org.kde.kirigami 2.4 as Kirigami
import org.kde.filesharing.samba 1.0 as Samba

// When built without packagekit we cannot do auto-installation.
ColumnLayout {
    QQC2.Label {
        Layout.alignment: Qt.AlignHCenter
        Layout.fillWidth: true
        // FIXME could put this in a component shared with crappy page
        text: i18nc("@label", "Samba must be installed before folders can be shared.")
        wrapMode: Text.Wrap
    }
    Item {
        Layout.alignment = Qt.AlignHCenter
        Layout.fillHeight: true // space everything up
    }
}
