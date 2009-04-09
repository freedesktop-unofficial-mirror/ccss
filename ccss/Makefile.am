
NULL =

AM_CPPFLAGS = \
	-I$(top_builddir) \
	-I$(top_srcdir) \
	$(CCSS_CFLAGS) \
	-DCCSS_BUILD \
	$(NULL)

lib_LTLIBRARIES = libccss-1.la

libccss_1_la_DEPENDENCIES = \
	ccss.sym \
	$(NULL)

libccss_1_la_LDFLAGS = \
	-no-undefined \
	-version-info $(VERSION_INFO) \
	-export-symbols $(srcdir)/ccss.sym \
	$(NULL)

libccss_1_la_LIBADD = \
	$(CCSS_LIBS) \
	$(NULL)

libccss_1_la_SOURCES = \
	$(headers_DATA) \
	ccss-background-parser.c \
	ccss-background-parser.h \
	ccss-block.c \
	ccss-block-priv.h \
	ccss-border-parser.c \
	ccss-border-parser.h \
	ccss-border-image-parser.c \
	ccss-border-image-parser.h \
	ccss-color-parser.c \
	ccss-color-parser.h \
	ccss-grammar.c \
	ccss-grammar-function.c \
	ccss-grammar-parse.c \
	ccss-grammar-priv.h \
	ccss-node.c \
	ccss-node-priv.h \
	ccss-padding-parser.c \
	ccss-padding-parser.h \
	ccss-position.c \
	ccss-property.c \
	ccss-property-generic.h \
	ccss-property-parser.c \
	ccss-property-parser.h \
	ccss-selector.c \
	ccss-selector-group.c \
	ccss-selector-group.h \
	ccss-selector.h \
	ccss-style.c \
	ccss-style-priv.h \
	ccss-stylesheet.c \
	ccss-stylesheet-priv.h \
	$(NULL)

headersdir = $(includedir)/libccss-1/ccss

headers_DATA = \
	ccss-background.h \
	ccss-block.h \
	ccss-border.h \
	ccss-border-image.h \
	ccss-color.h \
	ccss-function.h \
	ccss-grammar.h \
	ccss.h \
	ccss-macros.h \
	ccss-node.h \
	ccss-padding.h \
	ccss-position.h \
	ccss-property.h \
	ccss-style.h \
	ccss-stylesheet.h \
	$(NULL)

EXTRA_DIST = \
	ccss.sym \
	$(NULL)

