bin_PROGRAMS=classify

classify_SOURCES=\
	src/main.c \
	src/task-list.c \
	src/task-list.h \
	src/window.c \
	src/window.h \
	$(NULL)
classify_LDADD=\
	$(CLASSIFY_LIBS) \
	$(NULL)
AM_CPPFLAGS=\
	$(CLASSIFY_CFLAGS) \
	$(NULL)

applications_DATA=classify.desktop
applicationsdir=$(datarootdir)/applications

