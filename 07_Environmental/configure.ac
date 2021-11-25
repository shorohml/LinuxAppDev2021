#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rhasher], [0.00], [no-reply])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([rhash],[rhash_library_init],[],[AC_MSG_ERROR([RHash is needed])])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])
AC_CHECK_HEADER([rhash.h])
AC_CHECK_HEADER([readline/readline.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([strerror])

AC_ARG_WITH([readline],
[AS_HELP_STRING([--with-readline],
    [enable support for readline])],
[AC_DEFINE([WITH_READLINE], [1], [Define if you have libreadline])],
[with_readline=no])

LIBREADLINE=
AS_IF([test "x$with_readline" != xno],
[AC_CHECK_LIB([readline],[readline],[],[AC_MSG_ERROR([readline is needed])])])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
