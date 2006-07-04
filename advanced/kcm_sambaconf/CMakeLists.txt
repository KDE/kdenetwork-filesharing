



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

kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )




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
