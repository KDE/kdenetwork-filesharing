# SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
# SPDX-FileCopyrightText: 2025 Thomas Duckworth <tduck@filotimoproject.org>

add_executable(sambausershareplugin_authhelper authhelper.cpp)
target_link_libraries(sambausershareplugin_authhelper KF6::AuthCore KF6::ConfigCore KF6::I18n)
install(TARGETS sambausershareplugin_authhelper DESTINATION ${KAUTH_HELPER_INSTALL_DIR})

kauth_install_actions(org.kde.filesharing.samba org.kde.filesharing.samba.actions)
kauth_install_helper_files(sambausershareplugin_authhelper org.kde.filesharing.samba root)
