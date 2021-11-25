#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rhasher], [0.00], [no-reply])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_SRCDIR([rhasher.c])
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

# Optional clues
AC_ARG_WITH([libreadline],
    [AS_HELP_STRING([--with-libreadline],
        [support fancy command line editing @<:@default=check@:>@])],
    [AC_DEFINE([WITH_LIBREADLINE], [], [Use libreadline])],
    [with_libreadline=no]
)
          
LIBREADLINE=
AS_IF([test "x$with_libreadline" != xno],
[AC_CHECK_LIB([readline], [readline],
    [AC_SUBST([LIBREADLINE], ["-lreadline"])
    []],
    [AC_MSG_FAILURE(
        [--with-libreadline was given, but test for readline failed])]
    )])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
