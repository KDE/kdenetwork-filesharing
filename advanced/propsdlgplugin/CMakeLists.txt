kde4_header()

include_directories(${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/ ${CMAKE_SOURCE_DIR}/filesharing/advanced/nfs/ ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############
set(libpropsdlgplugin_common_SRCS 
	propertiespage.cpp)
kde4_add_ui3_files(libpropsdlgplugin_common_SRCS propertiespagegui.ui)

set(fileshare_propsdlgplugin_PART_SRCS propsdlgshareplugin.cpp ${libpropsdlgplugin_common_SRCS} ${libfilesharesamba_SRCS} ${libfilesharenfs_SRCS})

kde4_automoc(${fileshare_propsdlgplugin_PART_SRCS})

kde4_add_plugin(fileshare_propsdlgplugin ${fileshare_propsdlgplugin_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} fileshare_propsdlgplugin )

target_link_libraries(fileshare_propsdlgplugin  ${KDE4_KIO_LIBRARIES} )

install_targets(${LIB_INSTALL_DIR}/kde4 fileshare_propsdlgplugin )


########### next target ###############


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES fileshare_propsdlgplugin.desktop )

kde4_footer()



#original Makefile.am contents follow:

#METASOURCES = AUTO 
#
#
#kde_module_LTLIBRARIES = fileshare_propsdlgplugin.la
#
#fileshare_propsdlgplugin_la_SOURCES = \
#    propsdlgshareplugin.cpp 
#
#fileshare_propsdlgplugin_la_COMPILE_FIRST = propertiespagegui.h
#
#fileshare_propsdlgplugin_la_LIBADD = \
#    libpropsdlgplugin_common.la \
#    ../nfs/libfilesharenfs.la \
#    ../kcm_sambaconf/libfilesharesamba.la \
#	$(LIB_KIO)
#
#fileshare_propsdlgplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#
#AM_CPPFLAGS = -I../kcm_sambaconf $(all_includes)
#
#kde_services_DATA  = fileshare_propsdlgplugin.desktop
#
## Shared with the 'simple' kcm
#noinst_LTLIBRARIES = libpropsdlgplugin_common.la
#libpropsdlgplugin_common_la_SOURCES = propertiespage.cpp propertiespagegui.ui
