KDE_ICON = AUTO
kde_module_LTLIBRARIES = libkcm_kcmsambaconf.la ksambakonqiplugin.la


INCLUDES = $(all_includes)

METASOURCES=AUTO
libkcm_kcmsambaconf_la_SOURCES = share.ui kcminterface.ui kcmprinterdlg.ui printerdlgimpl.cpp kcmsambaconf.cpp sambafile.cpp sambashare.cpp sharedlgimpl.cpp socketoptionsdlg.ui smbpasswdfile.cpp passwd.cpp common.cpp usertab.ui usertabimpl.cpp filemodedlg.ui filemodedlgimpl.cpp hiddenfileview.cpp dictmanager.cpp qmultichecklistitem.cpp joindomaindlg.ui smbconfconfigwidget.cpp linuxpermissionchecker.cpp 

libkcm_kcmsambaconf_la_LIBADD = -lkdeprint $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)
libkcm_kcmsambaconf_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

ksambakonqiplugin_la_SOURCES = ksambapropertiesdialogplugin.cpp konqinterface.ui sambafile.cpp share.ui sharedlgimpl.cpp sambashare.cpp socketoptionsdlg.ui common.cpp usertab.ui usertabimpl.cpp filemodedlg.ui smbpasswdfile.cpp passwd.cpp filemodedlgimpl.cpp hiddenfileview.cpp dictmanager.cpp qmultichecklistitem.cpp smbconfconfigwidget.cpp linuxpermissionchecker.cpp

ksambakonqiplugin_la_LIBADD = $(LIB_KIO)
ksambakonqiplugin_la_LDFLAGS = $(all_libraries) -module

servicedir    = $(kde_servicesdir)
service_DATA  = ksambakonqiplugin.desktop

EXTRA_DIST = $(service_DATA) \
	kcmsambaconf.cpp \
	kcmsambaconf.h \
	share.ui.h \
	externals.h \
	kcmsambaconf.desktop \
	ksambapropertiesdialogplugin.cpp \
  konqinterface.ui.h \
	sambafile.cpp \
	ksambapropertiesdialogplugin.h \
	sambafile.h \
  sharedlgimpl.h \
  sambashare.h \
  socketoptionsdlg.ui.h \
  smbpasswdfile.h \
  passwd.h \
  common.h \
  usertab.ui.h \
  filemodedlg.ui.h \
  filemodedlgimpl.h \
  hiddenfileview.h \
  dictmanager.h \
  qmultichecklistitem.h \
  joindomaindlh.ui.h

xdg_apps_DATA = kcmsambaconf.desktop

## what's going on with this file I don't know
## I'm just an automake automaton --exa
# DISTCLEANFILES = qmultichecklistitem.cpp

#messages:
#	$(XGETTEXT) $(libkcm_kcm_sambaconf_la_SOURCES) -o $(podir)/kcm_sambaconf.pot

