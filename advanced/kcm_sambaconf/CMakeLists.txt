set(kcm_kcmsambaconf_PART_SRCS kcmsambaconf.cpp ${libfilesharesamba_SRCS})

add_definitions(-D_KDEPRINT_COMPILE )

kde4_add_ui3_files(kcm_kcmsambaconf_PART_SRCS share.ui socketoptionsdlg.ui userselectdlg.ui
    groupselectdlg.ui usertab.ui filemodedlg.ui expertuserdlg.ui
    kcminterface.ui joindomaindlg.ui
)


kde4_add_plugin(kcm_kcmsambaconf  ${kcm_kcmsambaconf_PART_SRCS})

target_link_libraries(kcm_kcmsambaconf ${KDE4_KDEUI_LIBS} ${KDE4_KDEPRINT_LIBS} ${KDE4_KDE3SUPPORT_LIBS})

install(TARGETS kcm_kcmsambaconf DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install(FILES kcmsambaconf.desktop DESTINATION  ${SERVICES_INSTALL_DIR} )

kde4_install_icons(${ICON_INSTALL_DIR})
