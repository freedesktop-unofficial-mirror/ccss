
AM_CPPFLAGS = \
	$(CCSS_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_builddir)

noinst_LTLIBRARIES = libcccss.la

libcccss_la_LIBADD = \
	$(CCSS_LIBS) \
	./../ccss/libccss-1.la

libcccss_la_SOURCES = \
	cccss.c \
	cccss.h

LDADD = \
	$(libcccss_la_LIBADD) \
	./libcccss.la

noinst_PROGRAMS = \
	example-1 \
	example-2

example_1_SOURCES = example-1.c

example_2_SOURCES = example-2.c

EXTRA_DIST = \
	example-1.css \
	example-2.css \
	foo.svg

