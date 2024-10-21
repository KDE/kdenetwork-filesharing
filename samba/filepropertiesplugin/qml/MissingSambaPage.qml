/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.filesharing.samba 1.0 as Samba

// When built without packagekit we cannot do auto-installation.
ColumnLayout {
    QQC2.Label {
        Layout.alignment: Qt.AlignHCenter
        Layout.fillWidth: true
        text: xi18nc("@info", "The <application>Samba</application> file sharing service must be installed before folders can be shared.")
        explanation: i18n("Use the system's package manager to install the <command>samba</command> server package manually.")
        wrapMode: Text.Wrap
    }
    Item {
        Layout.alignment: Qt.AlignHCenter
        Layout.fillHeight: true // space everything up
    }
}
