KDE_ICON = AUTO
kde_module_LTLIBRARIES = libkcm_kcmsambaconf.la ksambakonqiplugin.la


INCLUDES = $(all_includes)

METASOURCES=AUTO
libkcm_kcmsambaconf_la_SOURCES = share.ui kcminterface.ui kcmprinterdlg.ui printerdlgimpl.cpp kcmsambaconf.cpp sambafile.cpp sambashare.cpp sharedlgimpl.cpp socketoptionsdlg.ui smbpasswdfile.cpp passwd.cpp common.cpp usertab.ui usertabimpl.cpp filemodedlg.ui filemodedlgimpl.cpp hiddenfileview.cpp dictmanager.cpp qmultichecklistitem.cpp joindomaindlg.ui smbconfconfigwidget.cpp linuxpermissionchecker.cpp userselectdlg.ui groupselectdlg.ui expertuserdlg.ui

libkcm_kcmsambaconf_la_LIBADD = -lkdeprint $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)
libkcm_kcmsambaconf_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

ksambakonqiplugin_la_SOURCES = ksambapropertiesdialogplugin.cpp konqinterface.ui sambafile.cpp share.ui sharedlgimpl.cpp sambashare.cpp socketoptionsdlg.ui common.cpp usertab.ui usertabimpl.cpp filemodedlg.ui smbpasswdfile.cpp passwd.cpp filemodedlgimpl.cpp hiddenfileview.cpp dictmanager.cpp qmultichecklistitem.cpp smbconfconfigwidget.cpp  linuxpermissionchecker.cpp userselectdlg.ui groupselectdlg.ui expertuserdlg.ui

ksambakonqiplugin_la_LIBADD = $(LIB_KIO)
ksambakonqiplugin_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module

servicedir    = $(kde_servicesdir)
service_DATA  = ksambakonqiplugin.desktop

EXTRA_DIST = $(service_DATA) \
	kcmsambaconf.cpp \
	kcmsambaconf.h \
	share.ui.h \
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
  filemodedlgimpl.h \
  hiddenfileview.h \
  dictmanager.h \
  qmultichecklistitem.h 

#KDE 3.2: xdg_apps_DATA = kcmsambaconf.desktop
mydata_DATA = kcmsambaconf.desktop
mydatadir = $(kde_appsdir)/Settings/Network

## what's going on with this file I don't know
## I'm just an automake automaton --exa
# DISTCLEANFILES = qmultichecklistitem.cpp

messages:
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/kcm_sambaconf.pot; \
	fi

