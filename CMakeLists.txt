kde4_header()

add_subdirectory( advanced ) 
add_subdirectory( simple ) 

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS = advanced simple 
#
#messages: rc.cpp
#	$(EXTRACTRC) `find . -name '*.ui'` > rc.cpp
#	$(XGETTEXT) rc.cpp `find . -name '*.cpp' -or -name '*.h'` -o $(podir)/kfileshare.pot 
