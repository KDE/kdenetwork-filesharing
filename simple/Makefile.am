METASOURCES = AUTO

kde_module_LTLIBRARIES = kcm_fileshare.la

kcm_fileshare_la_SOURCES = fileshare.cpp \
    controlcenter.ui \
    groupconfiggui.ui \
    groupconfigdlg.cpp \
    krichtextlabel.cpp

kcm_fileshare_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
kcm_fileshare_la_LIBADD = ../advanced/propsdlgplugin/libpropsdlgplugin_common.la \
    ../advanced/nfs/libfilesharenfs.la \
    ../advanced/kcm_sambaconf/libfilesharesamba.la \
    $(LIB_KIO)

AM_CPPFLAGS = -I$(srcdir)/../advanced/propsdlgplugin -I../advanced/propsdlgplugin $(all_includes)

xdg_apps_DATA = fileshare.desktop
