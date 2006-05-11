kde4_header()




########### next target ###############

set(kcm_fileshare_PART_SRCS fileshare.cpp groupconfigdlg.cpp krichtextlabel.cpp )

kde4_automoc(${kcm_fileshare_PART_SRCS})

kde4_add_ui3_files(kcm_fileshare_PART_SRCS controlcenter.ui groupconfiggui.ui )

kde4_add_plugin(kcm_fileshare ${kcm_fileshare_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kcm_fileshare )

target_link_libraries(kcm_fileshare  ${KDE4_KDECORE_LIBS} propsdlgplugin_common filesharenfs filesharesamba kio )

install_targets(${PLUGIN_INSTALL_DIR} kcm_fileshare )


########### install files ###############

install_files( ${XDG_APPS_DIR} FILES fileshare.desktop )

kde4_footer()



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
