
AM_CPPFLAGS = \
	-I$(top_builddir) \
	-I$(top_srcdir) \
	$(CCSS_EXAMPLES_CFLAGS)

AM_LDFLAGS = \
	../ccss/libccss-1.la \
	../ccss-cairo/libccss-cairo-1.la \
	$(CCSS_EXAMPLES_LIBS)

noinst_PROGRAMS = \
	example-1 \
	example-2 \
	example-3 \
	example-4 \
	example-6 \
	example-7

if CCSS_WITH_SOUP
noinst_PROGRAMS += example-5
endif

example_1_SOURCES = example-1.c

example_2_SOURCES = example-2.c

example_3_SOURCES = example-3.c

example_4_SOURCES = example-4.c

example_5_SOURCES = example-5.c

example_6_SOURCES = example-6.c

example_7_SOURCES = example-7.c

EXTRA_DIST = \
	example-3.png \
	example-5.svg

