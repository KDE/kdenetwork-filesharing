kde_module_LTLIBRARIES = kcm_fileshare.la

kcm_fileshare_la_SOURCES = fileshare.cpp

kcm_fileshare_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
kcm_fileshare_la_LIBADD = $(LIB_KIO)

INCLUDES= $(all_includes)

kcm_fileshare_la_METASOURCES = fileshare.moc

messages:
	$(XGETTEXT) $(kcm_fileshare_la_SOURCES) -o $(podir)/kcmfileshare.pot

xdg_apps_DATA = fileshare.desktop
