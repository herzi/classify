AC_INIT(classify,0.0.1,mailto:herzi@gnome-de.org)
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)

AC_PROG_CC
AM_PROG_CC_C_O

AC_PROG_LIBTOOL

gio=

PKG_CHECK_EXISTS([gio-2.0],
		 [gio=gio-2.0],
		 [])
AM_CONDITIONAL(WITH_GIO,[test -n "$gio"])

PKG_CHECK_MODULES(CLASSIFY,[$gio gtk+-2.0 libxml-2.0 uuid])

AC_OUTPUT(Makefile)
