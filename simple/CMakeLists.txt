



########### next target ###############

set(kcm_fileshare_PART_SRCS fileshare.cpp groupconfigdlg.cpp krichtextlabel.cpp ${libfilesharesamba_SRCS} ${libfilesharenfs_SRCS} ${fileshare_propsdlgplugin_SRCS} ${libpropsdlgplugin_common_SRCS})



kde4_automoc(kcm_fileshare ${kcm_fileshare_PART_SRCS})

kde4_add_ui3_files(kcm_fileshare_PART_SRCS 
		controlcenter.ui 
		groupconfiggui.ui 
		${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/share.ui
		${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/socketoptionsdlg.ui
		${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/userselectdlg.ui
		${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/groupselectdlg.ui
		${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/usertab.ui
		${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/filemodedlg.ui
		${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/expertuserdlg.ui
		${CMAKE_SOURCE_DIR}/filesharing/advanced/propsdlgplugin/propertiespagegui.ui
		${CMAKE_SOURCE_DIR}/filesharing/advanced/nfs/nfsdialoggui.ui
		${CMAKE_SOURCE_DIR}/filesharing/advanced/nfs/hostprops.ui
		)

kde4_add_plugin(kcm_fileshare ${kcm_fileshare_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kcm_fileshare )

target_link_libraries(kcm_fileshare  ${KDE4_KIO_LIBS} )

install(TARGETS kcm_fileshare  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES fileshare.desktop  DESTINATION ${XDG_APPS_DIR})




#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#kde_module_LTLIBRARIES = kcm_fileshare.la
#
#kcm_fileshare_la_SOURCES = fileshare.cpp \
#    controlcenter.ui \
#    groupconfiggui.ui \
#    groupconfigdlg.cpp \
#    krichtextlabel.cpp
#
#kcm_fileshare_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
#kcm_fileshare_la_LIBADD = ../advanced/propsdlgplugin/libpropsdlgplugin_common.la \
#    ../advanced/nfs/libfilesharenfs.la \
#    ../advanced/kcm_sambaconf/libfilesharesamba.la \
#    $(LIB_KIO)
#
#AM_CPPFLAGS = -I$(srcdir)/../advanced/propsdlgplugin -I../advanced/propsdlgplugin $(all_includes)
#
#xdg_apps_DATA = fileshare.desktop
