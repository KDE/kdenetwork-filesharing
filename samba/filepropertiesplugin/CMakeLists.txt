set(sambausershareplugin_SRCS
    sambausershareplugin.cpp
    model.cpp
    usermanager.cpp
    groupmanager.cpp
    permissionshelper.cpp
    qml/qml.qrc
)

qt_add_dbus_interface(sambausershareplugin_SRCS org.freedesktop.Avahi.Server.xml org.freedesktop.Avahi.Server)

add_library(sambausershareplugin MODULE ${sambausershareplugin_SRCS})

target_link_libraries(sambausershareplugin
    KF6::CoreAddons
    KF6::I18n
    KF6::KIOCore
    KF6::KIOWidgets
    Qt::Qml
    Qt::QuickWidgets
    Qt::Network
    Qt::DBus
    QCoro6::Coro
    KF6::AuthCore
)

if(SAMBA_INSTALL)
    target_sources(sambausershareplugin PRIVATE sambainstaller.cpp)
    target_link_libraries(sambausershareplugin PK::packagekitqt6)
endif()

if(USE_SYSTEMD)
    target_sources(sambausershareplugin PRIVATE servicehelper.cpp)
    target_link_libraries(sambausershareplugin QCoro6::DBus)

    qt_add_dbus_interface(
        SYSTEMD_MANAGER_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/org.freedesktop.systemd1.Manager.xml
        systemd_manager
    )

    add_library(systemdmanagerlib STATIC ${SYSTEMD_MANAGER_SRCS} servicedefinitions.h)

    set_property(TARGET systemdmanagerlib PROPERTY POSITION_INDEPENDENT_CODE ON)

    target_include_directories(systemdmanagerlib PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
    target_link_libraries(systemdmanagerlib PUBLIC Qt::DBus)
    target_link_libraries(sambausershareplugin systemdmanagerlib)
endif()

install(TARGETS sambausershareplugin DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf6/propertiesdialog)

add_subdirectory(kauth)
