########### next target ###############

set(sambausershareplugin_PART_SRCS
    sambausershareplugin.cpp
    model.cpp
    usermanager.cpp
    groupmanager.cpp
    permissionshelper.cpp

    qml/qml.qrc
)

if(SAMBA_INSTALL)
    list(APPEND sambausershareplugin_PART_SRCS sambainstaller.cpp)
endif()


add_library(sambausershareplugin MODULE ${sambausershareplugin_PART_SRCS})

target_link_libraries(sambausershareplugin
    KF5::CoreAddons
    KF5::I18n
    KF5::KIOCore
    KF5::KIOWidgets
    Qt::Qml
    Qt::QuickWidgets
    KF5::AuthCore
)

if(SAMBA_INSTALL)
    target_link_libraries(sambausershareplugin PK::packagekitqt${QT_MAJOR_VERSION})
endif()

install(TARGETS sambausershareplugin DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf${QT_MAJOR_VERSION}/propertiesdialog)

# kauth

kauth_install_actions(org.kde.filesharing.samba org.kde.filesharing.samba.actions)
add_executable(authhelper authhelper.cpp)
target_link_libraries(authhelper KF5::AuthCore KF5::ConfigCore KF5::I18n)

kauth_install_helper_files(authhelper org.kde.filesharing.samba root)
install(TARGETS authhelper DESTINATION ${KAUTH_HELPER_INSTALL_DIR})
