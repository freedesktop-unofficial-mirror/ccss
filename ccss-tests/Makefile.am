
include $(top_srcdir)/Makefile.gtest

NULL =

noinst_PROGRAMS = $(TEST_PROGS)

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	$(CCSS_CFLAGS) \
	$(NULL)

LDADD = \
	$(CCSS_LIBS) \
	$(top_builddir)/ccss/libccss-1.la \
	$(NULL)

TESTS_ENVIRONMENT = srcdir=$(srcdir)

TEST_PROGS          += test-parser
test_parser_SOURCES  = test-parser.c

