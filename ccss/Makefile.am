
AM_CPPFLAGS = \
	$(CCSS_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_builddir)

lib_LTLIBRARIES = libccss-1.la

libccss_1_la_DEPENDENCIES = \
	ccss.sym

libccss_1_la_LDFLAGS = \
	-no-undefined \
	-version-info $(VERSION_INFO) \
	-export-symbols $(srcdir)/ccss.sym

libccss_1_la_LIBADD = \
	$(CCSS_LIBS)

libccss_1_la_SOURCES = \
	$(headers_DATA) \
	ccss-block.c \
	ccss-block-priv.h \
	ccss-function.c \
	ccss-function-priv.h \
	ccss.c \
	ccss-node.c \
	ccss-node-priv.h \
	ccss-parser.c \
	ccss-parser.h \
	ccss-property.c \
	ccss-property-generic.c \
	ccss-property-generic.h \
	ccss-selector-group.c \
	ccss-selector-group.h \
	ccss-selector.c \
	ccss-selector.h \
	ccss-style.c \
	ccss-style-priv.h \
	ccss-stylesheet.c

headersdir = $(includedir)/libccss-1/ccss

headers_DATA = \
	ccss-block.h \
	ccss-function.h \
	ccss.h \
	ccss-macros.h \
	ccss-node.h \
	ccss-property.h \
	ccss-style.h \
	ccss-stylesheet.h

EXTRA_DIST = \
	ccss.sym

