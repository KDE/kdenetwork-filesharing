kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############


########### next target ###############
set(libfilesharesamba_SRCS
    sambafile.cpp 
    sharedlgimpl.cpp 
    sambashare.cpp 
    common.cpp 
    usertabimpl.cpp 
    filemodedlgimpl.cpp 
    smbpasswdfile.cpp 
    passwd.cpp 
    hiddenfileview.cpp 
    dictmanager.cpp 
    qmultichecklistitem.cpp 
    smbconfconfigwidget.cpp
    linuxpermissionchecker.cpp 
)
kde4_add_ui3_files(libfilesharesamba_SRCS
	share.ui
	socketoptionsdlg.ui
	userselectdlg.ui
	groupselectdlg.ui
	usertab.ui
	filemodedlg.ui
	expertuserdlg.ui
)


set(kcm_kcmsambaconf_PART_SRCS printerdlgimpl.cpp kcmsambaconf.cpp ${libfilesharesamba_SRCS})

kde4_automoc(${kcm_kcmsambaconf_PART_SRCS})

kde4_add_ui3_files(kcm_kcmsambaconf_PART_SRCS kcminterface.ui kcmprinterdlg.ui joindomaindlg.ui )

kde4_add_plugin(kcm_kcmsambaconf WITH_PREFIX ${kcm_kcmsambaconf_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kcm_kcmsambaconf )

target_link_libraries(kcm_kcmsambaconf  ${LIB_KDEUI} kdeprint  )

install_targets(${LIB_INSTALL_DIR}/kde4 kcm_kcmsambaconf )


########### install files ###############

install_files( ${XDG_APPS_DIR} FILES kcmsambaconf.desktop )

kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )

kde4_footer()



#original Makefile.am contents follow:

#KDE_ICON = AUTO
#noinst_LTLIBRARIES = libfilesharesamba.la
#kde_module_LTLIBRARIES = libkcm_kcmsambaconf.la 
#
#AM_CPPFLAGS = $(all_includes)
#METASOURCES=AUTO
#
#libkcm_kcmsambaconf_la_SOURCES = \
#      kcminterface.ui \
#      kcmprinterdlg.ui \
#      printerdlgimpl.cpp \
#      kcmsambaconf.cpp \
#      joindomaindlg.ui 
#libkcm_kcmsambaconf_la_COMPILE_FIRST = usertab.h share.h
#      
#
#libkcm_kcmsambaconf_la_LIBADD = \
#      -lkdeprint \
#      libfilesharesamba.la
#      
#libkcm_kcmsambaconf_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#
#libfilesharesamba_la_SOURCES = \
#    sambafile.cpp \
#    share.ui \
#    sharedlgimpl.cpp \
#    sambashare.cpp \
#    socketoptionsdlg.ui \
#    common.cpp \
#    userselectdlg.ui \
#    groupselectdlg.ui \
#    usertab.ui \
#    usertabimpl.cpp \
#    filemodedlg.ui \
#    filemodedlgimpl.cpp \
#    smbpasswdfile.cpp \
#    passwd.cpp \
#    hiddenfileview.cpp \
#    dictmanager.cpp \
#    qmultichecklistitem.cpp \
#    smbconfconfigwidget.cpp \
#    linuxpermissionchecker.cpp \
#    expertuserdlg.ui
#
#
#xdg_apps_DATA = kcmsambaconf.desktop
