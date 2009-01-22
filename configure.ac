AC_INIT(classify,0.0.1,mailto:herzi@gnome-de.org)
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)

AC_PROG_CC
AM_PROG_CC_C_O

AC_PROG_LIBTOOL

AC_ARG_WITH([platform],
            [AS_HELP_STRING([--with-platform],[specify the development platform (gnome,maemo,auto) @<:@default=auto@:>@])],
            [],
            [with_platform=auto])

if test "${with_platform}" == "auto"; then
        AC_MSG_CHECKING([for the platform])
        if test -f /etc/maemo_version; then
                maemo="Maemo $(cat /etc/maemo_version)"
                with_platform="maemo"
        else
                maemo="GNOME"
                with_platform="gnome"
        fi
        AC_MSG_RESULT([$maemo])
fi

AC_MSG_CHECKING([for platform dependencies])
case "${with_platform}" in
maemo)
        extra_pkgs="hildon-1"
        AC_DEFINE_UNQUOTED(HAVE_HILDON,1,[do we use hildon-1])
        ;;
gnome)
        extra_pkgs=
        ;;
*)
        AC_MSG_ERROR([invalid platform $with_platform])
        ;;
esac
AC_MSG_RESULT([$extra_pkgs])

PKG_CHECK_EXISTS([gio-2.0],
		 [extra_pkgs="${extra_pkgs} gio-2.0"],
		 [])
AM_CONDITIONAL(WITH_GIO,[echo "${extra_pkgs}" | grep -q "gio-2.0"])

PKG_CHECK_MODULES(CLASSIFY,[gtk+-2.0 libxml-2.0 uuid $extra_pkgs])

AC_OUTPUT(Makefile)
