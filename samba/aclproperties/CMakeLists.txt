# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2020-2021 Harald Sitter <sitter@kde.org>

add_library(SambaAcl MODULE plugin.cpp qml/qml.qrc debug.cpp model.cpp aceobject.cpp)
target_link_libraries(SambaAcl
    KF5::CoreAddons
    KF5::I18n
    KF5::KIOWidgets
    KF5::KIOCore
    Qt::Qml
    Qt::QuickWidgets
)

install(TARGETS SambaAcl DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/propertiesdialog/)
