########### next target ###############

set(sambausershareplugin_PART_SRCS sambausershareplugin.cpp delegate.cpp model.cpp)

kde4_add_ui_files(sambausershareplugin_PART_SRCS sambausershareplugin.ui)


kde4_add_plugin(sambausershareplugin ${sambausershareplugin_PART_SRCS})

target_link_libraries(sambausershareplugin ${KDE4_KIO_LIBS})

install(TARGETS sambausershareplugin DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install(FILES sambausershareplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR})
