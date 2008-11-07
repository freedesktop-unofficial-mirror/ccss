
AM_CPPFLAGS = \
	$(CCSS_CLUTTER_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_builddir)

noinst_LTLIBRARIES = libccss-clutter.la

libccss_clutter_la_LIBADD = \
	$(CCSS_CLUTTER_LIBS) \
	./../ccss/libccss-1.la

libccss_clutter_la_SOURCES = \
	ccss-clutter.c \
	ccss-clutter.h

LDADD = \
	$(libccss_clutter_la_LIBADD) \
	./libccss-clutter.la

noinst_PROGRAMS = \
	example-1 \
	example-2

example_1_SOURCES = example-1.c

example_2_SOURCES = example-2.c

EXTRA_DIST = \
	example-1.css \
	example-2.css \
	foo.svg

