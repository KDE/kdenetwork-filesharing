kde4_header()

set(libfilesharesamba_SRCS
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/sambafile.cpp
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/sharedlgimpl.cpp
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/sambashare.cpp
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/common.cpp
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/usertabimpl.cpp
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/filemodedlgimpl.cpp
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/smbpasswdfile.cpp
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/passwd.cpp
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/hiddenfileview.cpp
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/dictmanager.cpp
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/qmultichecklistitem.cpp
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/smbconfconfigwidget.cpp
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/linuxpermissionchecker.cpp
)

set(libfilesharenfs_SRCS 
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/nfs/nfsdialog.cpp
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/nfs/nfshostdlg.cpp 
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/nfs/nfsfile.cpp 
    ${CMAKE_SOURCE_DIR}/filesharing/advanced/nfs/nfsentry.cpp
	)


add_subdirectory( nfs ) 
add_subdirectory( kcm_sambaconf ) 
add_subdirectory( propsdlgplugin ) 




########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS = nfs kcm_sambaconf propsdlgplugin 
