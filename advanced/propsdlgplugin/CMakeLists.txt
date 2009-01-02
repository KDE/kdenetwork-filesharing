include_directories(${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/ ${CMAKE_SOURCE_DIR}/filesharing/advanced/nfs/)

########### next target ###############

set(fileshare_propsdlgplugin_PART_SRCS propsdlgshareplugin.cpp ${libpropsdlgplugin_common_SRCS} ${libfilesharesamba_SRCS} ${libfilesharenfs_SRCS})

kde4_add_ui3_files(fileshare_propsdlgplugin_PART_SRCS
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/nfs/hostprops.ui
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/nfs/nfsdialoggui.ui
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/socketoptionsdlg.ui
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/userselectdlg.ui
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/groupselectdlg.ui
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/usertab.ui
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/filemodedlg.ui
    propertiespagegui.ui
)

kde4_add_ui_files(fileshare_propsdlgplugin_PART_SRCS
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/share.ui
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/expertuserdlg.ui
)


kde4_add_plugin(fileshare_propsdlgplugin ${fileshare_propsdlgplugin_PART_SRCS})

target_link_libraries(fileshare_propsdlgplugin ${KDE4_KIO_LIBS} ${KDE4_KDE3SUPPORT_LIBS})

install(TARGETS fileshare_propsdlgplugin DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install(FILES fileshare_propsdlgplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR})
