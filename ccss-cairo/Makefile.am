
AM_CPPFLAGS = \
	-I$(top_builddir) \
	-I$(top_srcdir) \
	$(CCSS_CAIRO_CFLAGS)

lib_LTLIBRARIES = libccss-cairo-1.la

libccss_cairo_1_la_DEPENDENCIES = \
	ccss-cairo.sym

libccss_cairo_1_la_LDFLAGS = \
	-no-undefined \
	-version-info $(VERSION_INFO) \
	-export-symbols $(srcdir)/ccss-cairo.sym

libccss_cairo_1_la_LIBADD = \
	../ccss/libccss-1.la \
	$(CCSS_CAIRO_LIBS)

libccss_cairo_1_la_SOURCES = \
	$(headers_DATA) \
	ccss-cairo-background.c \
	ccss-cairo-background.h \
	ccss-cairo-border.c \
	ccss-cairo-border.h \
	ccss-cairo-border-image.c \
	ccss-cairo-border-image.h \
	ccss-cairo-image.c \
	ccss-cairo-image.h \
  ccss-cairo-image-cache.c \
  ccss-cairo-image-cache.h \
	ccss-cairo-grammar.c \
	ccss-cairo-property.c \
	ccss-cairo-property.h \
	ccss-cairo-style.c \
	ccss-cairo-style.h

headersdir = $(includedir)/libccss-1/ccss-cairo

headers_DATA = \
	ccss-cairo.h \
	ccss-cairo-grammar.h \
	ccss-cairo-style.h

EXTRA_DIST = \
	ccss-cairo.sym

