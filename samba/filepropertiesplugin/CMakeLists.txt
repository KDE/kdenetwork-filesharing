########### next target ###############

set(sambausershareplugin_PART_SRCS
    sambausershareplugin.cpp
    model.cpp
    usermanager.cpp
    groupmanager.cpp

    qml/qml.qrc
)

if(SAMBA_INSTALL)
    list(APPEND sambausershareplugin_PART_SRCS sambainstaller.cpp)
endif()


add_library(sambausershareplugin MODULE ${sambausershareplugin_PART_SRCS})

kcoreaddons_desktop_to_json(sambausershareplugin sambausershareplugin.desktop)

target_link_libraries(sambausershareplugin
    KF5::CoreAddons
    KF5::I18n
    KF5::KIOCore
    KF5::KIOWidgets
    Qt5::Qml
    Qt5::QuickWidgets
    KF5::Auth
    KF5::Declarative
)

if(SAMBA_INSTALL)
    target_link_libraries(sambausershareplugin PK::packagekitqt5)
endif()

install(TARGETS sambausershareplugin DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/propertiesdialog)

# kauth

kauth_install_actions(org.kde.filesharing.samba org.kde.filesharing.samba.actions)
add_executable(authhelper authhelper.cpp)
target_link_libraries(authhelper KF5::AuthCore KF5::ConfigCore KF5::I18n)

kauth_install_helper_files(authhelper org.kde.filesharing.samba root)
install(TARGETS authhelper DESTINATION ${KAUTH_HELPER_INSTALL_DIR})
