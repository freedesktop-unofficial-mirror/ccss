
AM_CPPFLAGS = \
	$(CCSS_CAIRO_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_builddir)

lib_LTLIBRARIES = libccss-cairo-1.la

libccss_cairo_1_la_DEPENDENCIES = \
	ccss-cairo.sym

libccss_cairo_1_la_LDFLAGS = \
	-no-undefined \
	-version-info $(VERSION_INFO) \
	-export-symbols $(srcdir)/ccss-cairo.sym

libccss_cairo_1_la_LIBADD = \
	$(CCSS_CAIRO_LIBS) \
	../ccss/libccss-1.la

libccss_cairo_1_la_SOURCES = \
	$(headers_DATA) \
	ccss-background.c \
	ccss-background.h \
	ccss-background-parser.c \
	ccss-background-parser.h \
	ccss-border.c \
	ccss-border.h \
	ccss-border-parser.c \
	ccss-border-parser.h \
	ccss-border-image.c \
	ccss-border-image.h \
	ccss-border-image-parser.c \
	ccss-border-image-parser.h \
	ccss-cairo.c \
	ccss-cairo-property.c \
	ccss-cairo-property.h \
	ccss-cairo-style.c \
	ccss-cairo-style.h \
	ccss-cairo-style-priv.h \
	ccss-color.c \
	ccss-color.h \
	ccss-image.c \
	ccss-image.h \
	ccss-position.c \
	ccss-position.h

headersdir = $(includedir)/libccss-1/ccss-cairo

headers_DATA = \
	ccss-cairo.h \
	ccss-cairo-style.h

EXTRA_DIST = \
	ccss-cairo.sym

