METASOURCES = AUTO

kde_module_LTLIBRARIES = kcm_fileshare.la

kcm_fileshare_la_SOURCES = fileshare.cpp controlcenter.ui

kcm_fileshare_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
kcm_fileshare_la_LIBADD = $(LIB_KIO)

INCLUDES= $(all_includes)

messages:
	$(XGETTEXT) $(kcm_fileshare_la_SOURCES) -o $(podir)/kcmfileshare.pot

xdg_apps_DATA = fileshare.desktop
