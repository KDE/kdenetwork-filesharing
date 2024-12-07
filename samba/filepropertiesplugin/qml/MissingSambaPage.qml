/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
*/

import QtQuick 2.12
import QtQuick.Controls 2.5 as QQC2
import QtQuick.Layouts 1.14
import org.kde.kirigami as Kirigami

// When built without packagekit we cannot do auto-installation.
Item {
    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        width: parent.width - (Kirigami.Units.largeSpacing * 4)

        icon.name: "emblem-error"

        text: xi18nc("@info", "The <application>Samba</application> file sharing service must be installed before folders can be shared.")
        explanation: i18n("Because this distro does not include PackageKit, we cannot show you a nice \"Install it\" button, and you will have to use your package manager to install the <command>samba</command> server package manually.")
    }
}
