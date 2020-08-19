########### next target ###############

set(sambausershareplugin_PART_SRCS sambausershareplugin.cpp model.cpp)

if(SAMBA_INSTALL)
    list(APPEND sambausershareplugin_PART_SRCS sambainstaller.cpp)
endif()

qt5_add_resources(sambausershareplugin_PART_SRCS qml/qml.qrc)

add_library(sambausershareplugin MODULE ${sambausershareplugin_PART_SRCS})

target_link_libraries(sambausershareplugin
    KF5::CoreAddons
    KF5::I18n
    KF5::KIOCore
    KF5::KIOWidgets
    Qt5::Qml
    Qt5::QuickWidgets
    KF5::Declarative
)

if(SAMBA_INSTALL)
    target_link_libraries(sambausershareplugin PK::packagekitqt5)
endif()

install(TARGETS sambausershareplugin DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES sambausershareplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR})
