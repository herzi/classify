bin_PROGRAMS=classify
lib_LTLIBRARIES=libclassify.la
noinst_PROGRAMS=debugify

classify_SOURCES=\
	src/main.c \
	src/parser.c \
	src/parser.h \
	src/preferences.c \
	src/preferences.h \
	src/window.c \
	src/window.h \
	$(NULL)
LDADD=\
	$(CLASSIFY_LIBS) \
	libclassify.la \
	$(NULL)
AM_CPPFLAGS=\
	$(CLASSIFY_CFLAGS) \
	-DBIN_DIR=\""$(bindir)"\"
	$(NULL)

debugify_SOURCES=\
	tests/debugify.c \
	$(NULL)
debugify_CPPFLAGS=\
	-I$(top_srcdir)/src \
	$(AM_CPPFLAGS) \
	$(NULL)

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

applications_DATA=classify.desktop
applicationsdir=$(datarootdir)/applications

