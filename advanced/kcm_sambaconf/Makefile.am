kde_module_LTLIBRARIES = libkcm_kcmsambaconf.la ksambakonqiplugin.la


INCLUDES = $(all_includes)

METASOURCES=AUTO
libkcm_kcmsambaconf_la_SOURCES = \
	share.ui \
  kcminterface.ui \
  kcmsambaconf.cpp \
  sambafile.cpp \
  sambashare.cpp \
  sharedlgimpl.cpp

libkcm_kcmsambaconf_la_LIBADD = -lkdeprint $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)
libkcm_kcmsambaconf_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

ksambakonqiplugin_la_SOURCES = \
	ksambapropertiesdialogplugin.cpp \
  konqinterface.ui \
 	sambafile.cpp \
  share.ui \
  sharedlgimpl.cpp \
  sambashare.cpp

ksambakonqiplugin_la_LIBADD = $(LIB_KIO)
ksambakonqiplugin_la_LDFLAGS = $(all_libraries) -module

servicedir    = $(kde_servicesdir)
service_DATA  = ksambakonqiplugin.desktop

EXTRA_DIST = $(service_DATA) \
	kcmsambaconf.cpp \
	kcmsambaconf.h \
	interface.ui \
	share.ui \
	externals.h \
	kcmsambaconf.desktop \
	ksambapropertiesdialogplugin.cpp \
  konqinterface.ui \
	sambafile.cpp \
	ksambapropertiesdialogplugin.h \
	sambafile.h \
  sharedlgimpl.h \
  sambashare.h


apps_DATA = kcmsambaconf.desktop
appsdir = $(kde_appsdir)/Settings/Network


#messages:
#	$(XGETTEXT) $(libkcm_kcm_sambaconf_la_SOURCES) -o $(podir)/kcm_sambaconf.pot

