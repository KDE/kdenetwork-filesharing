kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )
kde4_add_ui3_files(libfilesharenfs_SRCS
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/nfs/hostprops.ui
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/nfs/nfsdialoggui.ui
    )

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
