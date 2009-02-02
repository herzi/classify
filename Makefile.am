bin_PROGRAMS=classify
lib_LTLIBRARIES=libclassify.la libclassifyui.la
mod_LTLIBRARIES=defaultui.la
moddir=$(libdir)/classify-@MILESTONE@
if WITH_GIO
noinst_PROGRAMS=debugify
endif

BUILT_SOURCES=\
	marshal.c \
	marshal.h \
	$(NULL)

classify_SOURCES=\
	$(BUILT_SOURCES) \
	src/main.c \
	src/parser.c \
	src/parser.h \
	src/user-interface.c \
	src/user-interface.h \
	src/user-interface-factory.c \
	src/user-interface-factory.h \
	$(NULL)
LDADD=\
	$(libclassify_la_LIBADD) \
	libclassify.la \
	$(NULL)
AM_CPPFLAGS=\
	$(CLASSIFY_CFLAGS) \
	$(WARN_CFLAGS) \
	-DBIN_DIR=\""$(bindir)"\" \
	-DMOD_DIR=\""$(moddir)"\" \
	$(NULL)

debugify_SOURCES=\
	tests/debugify.c \
	$(NULL)
debugify_CPPFLAGS=\
	-I$(top_srcdir)/src \
	$(AM_CPPFLAGS) \
	$(NULL)

defaultui_la_SOURCES=\
	src/default-user-interface.c \
	src/default-window.c \
	src/default-window.h \
	src/preferences.c \
	src/preferences.h \
	src/window.c \
	src/window.h \
	$(NULL)
defaultui_la_CPPFLAGS=$(AM_CPPFLAGS) -UHAVE_HILDON -UHAVE_OSSO
defaultui_la_LIBADD=$(LDADD) libclassifyui.la
defaultui_la_LDFLAGS=-avoid-version -module

if WITH_MAEMO
mod_LTLIBRARIES+=hildonui.la

hildonui_la_SOURCES=\
	src/hildon-user-interface.c \
	src/hildon-window.c \
	src/hildon-window.h \
	src/window.c \
	src/window.h \
	$(NULL)
hildonui_la_LIBADD=$(defaultui_la_LIBADD)
hildonui_la_LDFLAGS=$(defaultui_la_LDFLAGS)
endif

libclassify_la_SOURCES=\
	src/task.c \
	src/task.h \
	src/task-list.c \
	src/task-list.h \
	src/task-list-io.c \
	src/task-list-io.h \
	src/task-list-io-text.c \
	src/task-list-io-text.h \
	src/task-list-io-xml.c \
	src/task-list-io-xml.h \
	src/uuid.c \
	src/uuid.h \
	$(NULL)
libclassify_la_LIBADD=$(CLASSIFY_LIBS)
libclassifyui_la_SOURCES=\
	src/main-window.c \
	src/main-window.h \
	src/task-widget.c \
	src/task-widget.h \
	$(NULL)
libclassifyui_la_LIBADD=$(LDADD)

applications_in_files=classify.desktop.in
applications_DATA=$(applications_in_files:.desktop.in=.desktop)
if WITH_MAEMO
applicationsdir=$(datarootdir)/applications/hildon
else
applicationsdir=$(datarootdir)/applications
endif

$(applications_DATA): $(applications_in_files) Makefile
	sed 's#@BINDIR@#$(bindir)/#;' $< > $@

if WITH_MAEMO
osso_backup_DATA=classify.conf
osso_backupdir=$(sysconfdir)/osso-backup/applications
endif

marshaller=src/marshal.list
marshal.h: $(marshaller) Makefile
	@( $(GLIB_GENMARSHAL) --header --prefix=classify_marshal $< ) > xgen-$@ \
		&& (cmp -s xgen-$@ $@ || (echo "Creating $@..."; cp xgen-$@ $@) ) \
		&& rm -f xgen-$@

marshal.c: $(marshaller) Makefile
	@( echo "#include \"marshal.h\""; \
		$(GLIB_GENMARSHAL) --body --prefix=classify_marshal $< ) > xgen-$@ \
		&& (cmp -s xgen-$@ $@ || (echo "Creating $@..."; cp xgen-$@ $@) ) \
		&& rm -f xgen-$@

CLEANFILES=\
	$(applications_DATA) \
	$(NULL)

EXTRA_DIST=\
	$(applications_in_files) \
	$(marshaller) \
	$(osso_backup_DATA) \
	run-classify.sh \
	$(NULL)

