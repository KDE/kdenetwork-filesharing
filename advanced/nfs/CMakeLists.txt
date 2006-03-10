kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#METASOURCES = AUTO
#AM_CPPFLAGS= $(all_includes)
#
#noinst_LTLIBRARIES = libfilesharenfs.la
#
#libfilesharenfs_la_SOURCES = \
#    nfsdialog.cpp \
#    nfshostdlg.cpp \
#    hostprops.ui \
#    nfsdialoggui.ui \
#    nfsfile.cpp \
#    nfsentry.cpp 
#
