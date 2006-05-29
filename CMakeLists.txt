include_directories(
		${CMAKE_SOURCE_DIR}/filesharing/advanced/propsdlgplugin/
		${CMAKE_SOURCE_DIR}/filesharing/advanced/nfs/
		${CMAKE_SOURCE_DIR}/filesharing/advanced/kcm_sambaconf/
		)

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

set(fileshare_propsdlgplugin_SRCS
	${CMAKE_SOURCE_DIR}/filesharing/advanced/propsdlgplugin/propsdlgshareplugin.cpp
	)

set(libpropsdlgplugin_common_SRCS 
		    ${CMAKE_SOURCE_DIR}/filesharing/advanced/propsdlgplugin/propertiespage.cpp)


add_subdirectory( advanced ) 
add_subdirectory( simple ) 




########### install files ###############





#original Makefile.am contents follow:

#SUBDIRS = advanced simple 
#
#messages: rc.cpp
#	$(EXTRACTRC) `find . -name '*.ui'` > rc.cpp
#	$(XGETTEXT) rc.cpp `find . -name '*.cpp' -or -name '*.h'` -o $(podir)/kfileshare.pot 
