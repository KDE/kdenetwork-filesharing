METASOURCES = AUTO 


kde_module_LTLIBRARIES = fileshare_propsdlgplugin.la

fileshare_propsdlgplugin_la_SOURCES = \
    propsdlgshareplugin.cpp 

fileshare_propsdlgplugin_la_COMPILE_FIRST = propertiespagegui.h

fileshare_propsdlgplugin_la_LIBADD = \
    libpropsdlgplugin_common.la \
    ../nfs/libfilesharenfs.la \
    ../kcm_sambaconf/libfilesharesamba.la \
	$(LIB_KIO)

fileshare_propsdlgplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

AM_CPPFLAGS = -I../kcm_sambaconf $(all_includes)

kde_services_DATA  = fileshare_propsdlgplugin.desktop

# Shared with the 'simple' kcm
noinst_LTLIBRARIES = libpropsdlgplugin_common.la
libpropsdlgplugin_common_la_SOURCES = propertiespage.cpp propertiespagegui.ui
