####### kdevelop will overwrite this part!!! (begin)##########
kde_module_LTLIBRARIES = libkcm_kcm_sambaconf.la ksambakonqiplugin.la


INCLUDES = $(all_includes)


LDFLAGS =

libkcm_kcm_sambaconf_la_METASOURCES=AUTO

libkcm_kcm_sambaconf_la_SOURCES = sharetype.ui share.ui interface.ui home.ui kcm_sambaconf.cpp 
libkcm_kcm_sambaconf_la_LIBADD = -lkdeprint $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)

libkcm_kcm_sambaconf_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)     

ksambakonqiplugin_la_SOURCES = ksambapropertiesdialogplugin.cpp sambafile.cpp
ksambakonqiplugin_la_LIBADD = $(LIB_KIO)
ksambakonqiplugin_la_LDFLAGS = $(all_libraries) -module


EXTRA_DIST = kcm_sambaconf.cpp kcm_sambaconf.h home.ui interface.ui share.ui sharetype.ui externals.h kcm_sambaconf.desktop \
						 ksambapropertiesdialogplugin.cpp sambafile.cpp ksambapropertiesdialogplugin.h sambafile.h

install-data-local:
	$(mkinstalldirs) ${KDEDIR}/applnk/Settings/Network/
	$(INSTALL_DATA) $(srcdir)/kcm_sambaconf.desktop ${KDEDIR}/applnk/Settings/Network/kcm_sambaconf.desktop

uninstall-local:
	-rm -f ${KDEDIR}/applnk/Settings/Network/kcm_sambaconf.desktop

####### kdevelop will overwrite this part!!! (end)############
# These paths are KDE specific. Use them:
# kde_appsdir         Where your application's menu entry (.desktop) should go to.
# kde_icondir         Where your icon should go to - better use KDE_ICON.
# kde_sounddir        Where your sounds should go to.
# kde_htmldir         Where your docs should go to. (contains lang subdirs)
# kde_datadir         Where you install application data. (Use a subdir)
# kde_locale          Where translation files should go to. (contains lang subdirs)
# kde_cgidir          Where cgi-bin executables should go to.
# kde_confdir         Where config files should go to (system-wide ones with default values).
# kde_mimedir         Where mimetypes .desktop files should go to.
# kde_servicesdir     Where services .desktop files should go to.
# kde_servicetypesdir Where servicetypes .desktop files should go to.
# kde_toolbardir      Where general toolbar icons should go to (deprecated, use KDE_ICON).
# kde_wallpaperdir    Where general wallpapers should go to.
# kde_templatesdir    Where templates for the "New" menu (Konqueror/KDesktop) should go to.
# kde_bindir          Where executables should go to. Use bin_PROGRAMS or bin_SCRIPTS.
# kde_libdir          Where shared libraries should go to. Use lib_LTLIBRARIES.
# kde_moduledir       Where modules (e.g. parts) should go to. Use kde_module_LTLIBRARIES.
# kde_styledir        Where Qt/KDE widget styles should go to (new in KDE 3).
# kde_designerdir     Where Qt Designer plugins should go to (new in KDE 3).

messages:
	$(XGETTEXT) $(libkcm_kcm_sambaconf_la_SOURCES) -o $(podir)/kcm_sambaconf.pot

