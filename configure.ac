AC_INIT(classify,0.0.1,mailto:herzi@gnome-de.org)
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)

dnl  --------------------
dnl | version management |----------------------------------------------------
dnl  --------------------

AC_PROG_SED
MAJOR=$(echo AC_PACKAGE_VERSION | sed 's@^\([[0-9]]*\)\.[[0-9]]*\.[[0-9]]*$@\1@')
MINOR=$(echo AC_PACKAGE_VERSION | sed 's@^[[0-9]]*\.\([[0-9]]*\)\.[[0-9]]*$@\1@')
MICRO=$(echo AC_PACKAGE_VERSION | sed 's@^[[0.9]]*\.[[0.9]]*\.\([[0.9]]*\)$@\1@')

AC_PROG_AWK

AC_MSG_CHECKING([for milestone version])
MILESTONE_MINOR=$(echo "${MINOR}" | awk '{x = $1; x += x%2; print x}')
MILESTONE="${MAJOR}.${MILESTONE_MINOR}"
AC_SUBST(MILESTONE)
AC_MSG_RESULT([$MILESTONE])

dnl  --------------
dnl | build system |----------------------------------------------------------
dnl  --------------

AC_PROG_CC
AM_PROG_CC_C_O

CXX=no
F77=no

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

AM_CONDITIONAL(WITH_MAEMO,[test "${with_platform}" == "maemo"])

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

if test -n "${extra_pkgs}"; then
        AC_CHECK_LIB(gtk-x11-2.0,hildon_gtk_im_context_show,[AC_DEFINE_UNQUOTED(HAVE_MAEMO_GTK,,[whether we have maemo-gtk+])])
fi

PKG_CHECK_EXISTS([gio-2.0],
		 [extra_pkgs="${extra_pkgs} gio-2.0"],
		 [])
AM_CONDITIONAL(WITH_GIO,[echo "${extra_pkgs}" | grep -q "gio-2.0"])

AM_PATH_GLIB_2_0
PKG_CHECK_MODULES(CLASSIFY,[gmodule-2.0 gtk+-2.0 libxml-2.0 uuid $extra_pkgs])

dnl  ----------------
dnl | compiler flags |--------------------------------------------------------
dnl  ----------------

GNOME_COMPILE_WARNINGS

dnl  ----------------------
dnl | configuration output |--------------------------------------------------
dnl  ----------------------

AC_OUTPUT(Makefile)

