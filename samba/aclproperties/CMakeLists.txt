# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2020-2021 Harald Sitter <sitter@kde.org>

add_library(SambaAcl MODULE plugin.cpp qml/qml.qrc debug.cpp model.cpp aceobject.cpp)
target_link_libraries(SambaAcl
    KF6::CoreAddons
    KF6::I18n
    KF6::KIOWidgets
    KF6::KIOCore
    Qt::Qml
    Qt::QuickWidgets
)

install(TARGETS SambaAcl DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf6/propertiesdialog/)
