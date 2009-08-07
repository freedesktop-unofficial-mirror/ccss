
NULL =

AM_CPPFLAGS = \
	-I$(top_builddir) \
	-I$(top_srcdir) \
	$(CCSS_CAIRO_CFLAGS) \
	-DCCSS_BUILD \
	-DCCSS_CAIRO_BUILD \
	-DCCSS_CAIRO_APPEARANCE_MODULE_INTERFACE_VERSION=\"$(CCSS_CAIRO_APPEARANCE_MODULE_INTERFACE_VERSION)\" \
	-DCCSS_CAIRO_APPEARANCE_MODULE_PATH=\"$(CCSS_CAIRO_APPEARANCE_MODULE_PATH)\" \
	$(NULL)

lib_LTLIBRARIES = libccss-cairo-1.la

libccss_cairo_1_la_DEPENDENCIES = \
	ccss-cairo.sym \
	$(NULL)

libccss_cairo_1_la_LDFLAGS = \
	-no-undefined \
	-version-info $(VERSION_INFO) \
	-export-symbols $(srcdir)/ccss-cairo.sym \
	$(NULL)

libccss_cairo_1_la_LIBADD = \
	$(CCSS_CAIRO_LIBS) \
	$(NULL)

libccss_cairo_1_la_SOURCES = \
	$(headers_DATA) \
	ccss-cairo-appearance.h \
	ccss-cairo-appearance-parser.c \
	ccss-cairo-appearance-parser.h \
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
	ccss-cairo-style.h \
	$(NULL)

headersdir = $(includedir)/ccss-1/ccss-cairo

headers_DATA = \
	ccss-cairo.h \
	ccss-cairo-grammar.h \
	ccss-cairo-style.h \
	$(NULL)

EXTRA_DIST = \
	ccss-cairo.sym \
	$(NULL)

