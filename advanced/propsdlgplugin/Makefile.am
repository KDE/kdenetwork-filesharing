METASOURCES = AUTO 
kde_module_LTLIBRARIES = fileshare_propsdlgplugin.la

fileshare_propsdlgplugin_la_SOURCES = \
    propertiespagegui.ui \
    propsdlgshareplugin.cpp \
    propertiespage.cpp 

fileshare_propsdlgplugin_la_LIBADD = \
    ../nfs/libfilesharenfs.la \
    $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) $(LIB_KIO)
    
fileshare_propsdlgplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
    
INCLUDES= $(all_includes)
    
kde_services_DATA  = fileshare_propsdlgplugin.desktop

