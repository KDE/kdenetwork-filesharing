add_library(sambausershareplugin MODULE
    sambausershareplugin.cpp
    model.cpp
    usermanager.cpp
    groupmanager.cpp
    permissionshelper.cpp
    qml/qml.qrc
)

target_link_libraries(sambausershareplugin
    KF6::CoreAddons
    KF6::I18n
    KF6::KIOCore
    KF6::KIOWidgets
    Qt::Qml
    Qt::QuickWidgets
    QCoro6::Coro
    KF6::AuthCore
)

if(SAMBA_INSTALL)
    target_sources(sambausershareplugin PRIVATE sambainstaller.cpp)
    target_link_libraries(sambausershareplugin PK::packagekitqt6)
endif()

install(TARGETS sambausershareplugin DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf6/propertiesdialog)

# kauth

kauth_install_actions(org.kde.filesharing.samba org.kde.filesharing.samba.actions)
add_executable(authhelper authhelper.cpp)
target_link_libraries(authhelper KF6::AuthCore KF6::ConfigCore KF6::I18n)

kauth_install_helper_files(authhelper org.kde.filesharing.samba root)
install(TARGETS authhelper DESTINATION ${KAUTH_HELPER_INSTALL_DIR})
