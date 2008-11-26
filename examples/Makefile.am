
AM_CPPFLAGS = \
	$(CCSS_EXAMPLES_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_builddir)

AM_LDFLAGS = \
	$(CCSS_EXAMPLES_LIBS) \
	../ccss/libccss-1.la \
	../ccss-cairo/libccss-cairo-1.la

noinst_PROGRAMS = \
	example-1 \
	example-2 \
	example-3 \
	example-4

if CCSS_WITH_SOUP
noinst_PROGRAMS += example-5
endif

example_1_SOURCES = example-1.c

example_2_SOURCES = example-2.c

example_3_SOURCES = example-3.c

example_4_SOURCES = example-4.c

example_5_SOURCES = example-5.c

EXTRA_DIST = \
	example-3.png

