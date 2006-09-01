



########### next target ###############


########### next target ###############
set(kcm_kcmsambaconf_PART_SRCS printerdlgimpl.cpp kcmsambaconf.cpp ${libfilesharesamba_SRCS})

kde4_add_ui3_files(kcm_kcmsambaconf_PART_SRCS share.ui socketoptionsdlg.ui userselectdlg.ui
    groupselectdlg.ui usertab.ui filemodedlg.ui expertuserdlg.ui
    kcminterface.ui kcmprinterdlg.ui joindomaindlg.ui
)


kde4_automoc(${kcm_kcmsambaconf_PART_SRCS})


kde4_add_plugin(kcm_kcmsambaconf WITH_PREFIX ${kcm_kcmsambaconf_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kcm_kcmsambaconf )

target_link_libraries(kcm_kcmsambaconf  ${KDE4_KDEUI_LIBS} ${KDE4_KDEPRINT_LIBS}  )

install(TARGETS kcm_kcmsambaconf  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES kcmsambaconf.desktop  DESTINATION ${XDG_APPS_DIR})

kde4_install_icons( ${ICON_INSTALL_DIR}   )


