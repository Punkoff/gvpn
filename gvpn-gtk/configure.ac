dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(gvpn_gtk, 0.1)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CXX
AM_PROG_CC_STDC
AC_HEADER_STDC




dnl ***************************************************************************
dnl Internatinalization
dnl ***************************************************************************
GETTEXT_PACKAGE=gvpn_gtk
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])
AM_GLIB_GNU_GETTEXT
IT_PROG_INTLTOOL([0.35.0])





PKG_CHECK_MODULES(GVPN_GTK, [gtkmm-2.4 >= 2.8 libglademm-2.4 >= 2.6    ])
AC_SUBST(GVPN_GTK_CFLAGS)
AC_SUBST(GVPN_GTK_LIBS)




AC_OUTPUT([
Makefile
src/Makefile
po/Makefile.in
])
