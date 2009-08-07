
NULL =

AM_CPPFLAGS = \
	-I$(top_builddir) \
	-I$(top_srcdir) \
	$(CCSS_EXAMPLES_CFLAGS) \
	-DCCSS_CAIRO_APPEARANCE_MODULE_INTERFACE_VERSION=\"$(CCSS_CAIRO_APPEARANCE_MODULE_INTERFACE_VERSION)\" \
	$(NULL)

AM_LDFLAGS = \
	../ccss/libccss-1.la \
	../ccss-cairo/libccss-cairo-1.la \
	$(CCSS_EXAMPLES_LIBS) \
	$(NULL)

noinst_PROGRAMS = \
	example-1 \
	example-2 \
	example-3 \
	example-4 \
	example-4-1 \
	example-4-2 \
	example-6 \
	example-7 \
	example-8 \
	example-9 \
	example-10 \
	$(NULL)

noinst_LTLIBRARIES = libexample-8-noinst.la

pkglibdir = $(CCSS_CAIRO_APPEARANCE_MODULE_PATH)
pkglib_LTLIBRARIES = libexample-8.la

if CCSS_WITH_SOUP
noinst_PROGRAMS += example-5
endif

example_1_SOURCES = example-1.c

example_2_SOURCES = example-2.c

example_3_SOURCES = example-3.c

example_4_SOURCES = example-4.c

example_4_1_SOURCES = example-4-1.c

example_4_2_SOURCES = example-4-2.c

example_5_SOURCES = example-5.c

example_6_SOURCES = example-6.c

example_7_SOURCES = example-7.c

libexample_8_noinst_la_LDFLAGS = -module -no-undefined
libexample_8_noinst_la_SOURCES = example-8.c

example_8_SOURCES = 
example_8_LDADD = libexample-8-noinst.la

libexample_8_la_LDFLAGS = -avoid-version -export-dynamic -module -no-undefined
libexample_8_la_SOURCES = example-8.c

example_9_SOURCES = example-9.c

example_10_SOURCES = example-10.c

EXTRA_DIST = \
	example-3.png \
	example-5.svg \
	$(NULL)

