TESTS_ENVIRONMENT = CK_VERBOSITY=$(CK_VERBOSITY)
TESTS = upstream
check_PROGRAMS = upstream

.ts.c:
	checkmk $< > $@

AM_CFLAGS = -I$(top_builddir)/src @CHECK_CFLAGS@
LDADD = $(top_builddir)/src/libbuf.la @CHECK_LIBS@
