KDE_ICON = AUTO
noinst_LTLIBRARIES = libfilesharesamba.la
kde_module_LTLIBRARIES = kcm_kcmsambaconf.la 

AM_CPPFLAGS = $(all_includes)
METASOURCES=AUTO

kcm_kcmsambaconf_la_SOURCES = \
      kcminterface.ui \
      kcmprinterdlg.ui \
      printerdlgimpl.cpp \
      kcmsambaconf.cpp \
      joindomaindlg.ui 
kcm_kcmsambaconf_la_COMPILE_FIRST = usertab.h share.h
      

kcm_kcmsambaconf_la_LIBADD = \
      -lkdeprint \
      libfilesharesamba.la
      
kcm_kcmsambaconf_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

libfilesharesamba_la_SOURCES = \
    sambafile.cpp \
    share.ui \
    sharedlgimpl.cpp \
    sambashare.cpp \
    socketoptionsdlg.ui \
    common.cpp \
    userselectdlg.ui \
    groupselectdlg.ui \
    usertab.ui \
    usertabimpl.cpp \
    filemodedlg.ui \
    filemodedlgimpl.cpp \
    smbpasswdfile.cpp \
    passwd.cpp \
    hiddenfileview.cpp \
    dictmanager.cpp \
    qmultichecklistitem.cpp \
    smbconfconfigwidget.cpp \
    linuxpermissionchecker.cpp \
    expertuserdlg.ui


xdg_apps_DATA = kcmsambaconf.desktop
