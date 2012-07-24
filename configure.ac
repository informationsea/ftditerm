#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([ftditerm], [0.1], [okamura@infomationsea.info])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/ftditerm.c])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_HEADERS([config.h])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST


# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AM_PROG_AR
AC_CONFIG_MACRO_DIR([m4])
ACLOCAL_AMFLAGS="-I m4"
#AC_DISABLE_SHARED
AC_CHECK_TOOLS([WINDRES], [windres])
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET


# Checks for libraries.
AC_CHECK_LIB([ftdi], [ftdi_init], [], AC_MSG_ERROR(Test for libftdi failed.))
AC_CHECK_LIB([usb], [usb_init], [], AC_MSG_ERROR(Test for libusb failed.))
AC_CHECK_LIB([m], [sin], [], AC_MSG_ERROR(Test for libm failed.))

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h termios.h unistd.h])
AC_CHECK_HEADERS([ftdi.h usb.h], [], AC_MSG_ERROR(Test for libftdi failed.))

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
AC_CHECK_FUNCS([atexit memset select])

AC_CONFIG_FILES([src/Makefile Makefile])
AC_OUTPUT
