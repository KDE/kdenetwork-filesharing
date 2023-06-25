# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2020-2021 Harald Sitter <sitter@kde.org>

add_library(SambaAcl MODULE plugin.cpp qml/qml.qrc debug.cpp model.cpp aceobject.cpp)
target_link_libraries(SambaAcl
    KF${KF_MAJOR_VERSION}::CoreAddons
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::KIOWidgets
    KF${KF_MAJOR_VERSION}::KIOCore
    Qt::Qml
    Qt::QuickWidgets
)

install(TARGETS SambaAcl DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf${KF_MAJOR_VERSION}/propertiesdialog/)
