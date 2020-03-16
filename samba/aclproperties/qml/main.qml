// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2021-2022 Harald Sitter <sitter@kde.org>

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.19 as Kirigami

import org.kde.filesharing.samba.acl 1.0 as Samba

QQC2.Frame {
    padding: 1 // without any padding there'd be no frame

    Kirigami.ApplicationItem {
        id: root

        anchors.fill: parent
        contextDrawer: Kirigami.ContextDrawer {
            id: contextDrawer
        }

        pageStack.globalToolBar.style: Kirigami.ApplicationHeaderStyle.Breadcrumb
        pageStack.globalToolBar.separatorVisible: bottomSeparator.visible
        pageStack.globalToolBar.colorSet: Kirigami.Theme.Window

        Component {
            id: loadingPage
            LoadingPage {}
        }

        Component {
            id: mainPage
            MainPage {}
        }

        Component {
            id: noDataPage
            NoDataPage {}
        }

        states: [
            State {
                name: "loading"
                when: !plugin.ready
                PropertyChanges { target: root.pageStack; initialPage: loadingPage }
            },
            State {
                name: "noData"
                when: Samba.Context.aceModel.empty
                PropertyChanges { target: root.pageStack; initialPage: noDataPage }
            },
            State {
                name: "" // default state
                PropertyChanges { target: root.pageStack; initialPage: mainPage }
            }
        ]
    }
}
