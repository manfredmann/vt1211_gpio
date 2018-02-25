AC_PREREQ([2.69])
AC_INIT([vt1211_gpio], [1.0], [roman@serov.co])
AC_CONFIG_SRCDIR([src/vt1211_gpio.c])

# Checks for programs.
AC_PROG_CC
AM_INIT_AUTOMAKE([foreign])

AC_PROG_RANLIB

AC_CHECK_HEADERS([stdint.h])
AC_CHECK_HEADERS([sys/io.h])

AC_CHECK_HEADER_STDBOOL
AC_TYPE_UINT16_T
AC_TYPE_UINT8_T

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
