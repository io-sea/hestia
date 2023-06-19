AC_INIT([hestia_sample_app], [0.0.1], [maintainer@example.com])

AC_CONFIG_AUX_DIR([autotools])
AC_CONFIG_SRCDIR([src/main.cc])

AM_INIT_AUTOMAKE([subdir-objects])
AC_PROG_CC
AC_PROG_CXX

PKG_CHECK_MODULES(HESTIA, [hestia])
AC_SUBST(HESTIA_LIBS)
AC_SUBST(HESTIA_CFLAGS)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT