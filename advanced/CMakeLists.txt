kde4_header()

add_subdirectory( nfs ) 
add_subdirectory( kcm_sambaconf ) 
add_subdirectory( propsdlgplugin ) 

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS = nfs kcm_sambaconf propsdlgplugin 
