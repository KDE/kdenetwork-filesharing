METASOURCES = AUTO

kde_module_LTLIBRARIES = kcm_fileshare.la

kcm_fileshare_la_SOURCES = fileshare.cpp \
    controlcenter.ui \
    groupconfiggui.ui \
    groupconfigdlg.cpp \
    ../advanced/propsdlgplugin/propertiespage.cpp \
    ../advanced/propsdlgplugin/propertiespagegui.cpp 
    

kcm_fileshare_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
kcm_fileshare_la_LIBADD = $(LIB_KIO) \
    ../advanced/nfs/libfilesharenfs.la \
    ../advanced/kcm_sambaconf/libfilesharesamba.la 

AM_CPPFLAGS= $(all_includes)

xdg_apps_DATA = fileshare.desktop
