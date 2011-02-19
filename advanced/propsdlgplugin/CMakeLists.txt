########### next target ###############

set(fileshare_propsdlgplugin_PART_SRCS propsdlgshareplugin.cpp delegate.cpp model.cpp)

kde4_add_ui_files(fileshare_propsdlgplugin_PART_SRCS propertiespagegui.ui)


kde4_add_plugin(fileshare_propsdlgplugin ${fileshare_propsdlgplugin_PART_SRCS})

target_link_libraries(fileshare_propsdlgplugin ${KDE4_KIO_LIBS})

install(TARGETS fileshare_propsdlgplugin DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install(FILES fileshare_propsdlgplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR})
