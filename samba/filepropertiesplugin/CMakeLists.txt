########### next target ###############

set(sambausershareplugin_PART_SRCS sambausershareplugin.cpp delegate.cpp model.cpp)

ki18n_wrap_ui(sambausershareplugin_PART_SRCS sambausershareplugin.ui)

add_library(sambausershareplugin MODULE ${sambausershareplugin_PART_SRCS})

target_link_libraries(sambausershareplugin
    KF5::CoreAddons
    KF5::I18n
    KF5::KIOWidgets
    KF5::WidgetsAddons
    PK::packagekitqt5
)

target_include_directories(sambausershareplugin PUBLIC ${Qt5DBus_INCLUDE_DIRS})

install(TARGETS sambausershareplugin DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install(FILES sambausershareplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR})
