
kde_module_LTLIBRARIES = \
	knfskonqiplugin.la \
  kcm_kcmnfsmodule.la


kcm_kcmnfsmodule_la_SOURCES = \
	cmdialogimpl.cpp \
  cmdialog.ui \
  nfsentry.cpp \
  nfsfile.cpp \
  nfsdialog.ui \
  nfsdialogimpl.cpp \
  nfshostdlg.cpp \
  hostprops.ui \
  kcmnfsmodule.cpp \
  nfsconfigwidget.cpp


kcm_kcmnfsmodule_la_LIBADD = \
	$(LIB_KFILE)

kcm_kcmnfsmodule_la_LDFLAGS = \
	$(all_libraries) \
  -module \
  $(KDE_PLUGIN)

knfskonqiplugin_la_SOURCES = \
	nfshostdlg.cpp \
  hostprops.ui \
  nfspropsdlgplugin.cpp \
  nfsentry.cpp \
  nfsfile.cpp \
  nfsdialog.ui \
	nfsdialogimpl.cpp \
  nfsconfigwidget.cpp

knfskonqiplugin_la_LIBADD = \
	$(LIB_KIO)

knfskonqiplugin_la_LDFLAGS = \
  $(all_libraries) -module

noinst_HEADERS = \
  nfsfile.h \
  nfsentry.h \
  nfspropsdlgplugin.h \
  nfshostdlg.h \
  nfsdialogimpl.h \
  nfsdialog.ui \
  cmdialog.ui \
  hostprops.ui \
  cmdialogimpl.h \
  kcmnfsmodule.h \
  nfsconfigwidget.h


INCLUDES= $(all_includes)

servicedir = $(kde_servicesdir)
service_DATA  = knfskonqiplugin.desktop

apps_DATA = kcmnfsmodule.desktop
appsdir = $(kde_appsdir)/Settings/Network

METASOURCES = AUTO
