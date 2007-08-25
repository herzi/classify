bin_PROGRAMS=classify

classify_SOURCES=\
	src/main.c \
	src/preferences.c \
	src/preferences.h \
	src/task-list.c \
	src/task-list.h \
	src/task-list-io-text.c \
	src/task-list-io-text.h \
	src/task-list-io-xml.c \
	src/task-list-io-xml.h \
	src/window.c \
	src/window.h \
	$(NULL)
classify_LDADD=\
	$(CLASSIFY_LIBS) \
	$(NULL)
AM_CPPFLAGS=\
	$(CLASSIFY_CFLAGS) \
	-DBIN_DIR=\""$(bindir)"\"
	$(NULL)

applications_DATA=classify.desktop
applicationsdir=$(datarootdir)/applications

