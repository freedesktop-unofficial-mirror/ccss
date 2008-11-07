
### Build in `noinst' manner so in can be statically ###########################
### linked into -cairo et al variants. #########################################

noinst_LTLIBRARIES = libccss-static.la libccssproperty.la

libccss_static_la_CPPFLAGS = \
	$(CCSS_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_builddir)

libccss_static_la_DEPENDENCIES = \
	libccssproperty.la

libccss_static_la_LIBADD = \
	libccssproperty.la

libccss_static_la_SOURCES = \
	ccss-background.c \
	ccss-background.h \
	ccss-background-parser.c \
	ccss-background-parser.h \
	ccss-block.c \
	ccss-block.h \
	ccss-border.c \
	ccss-border.h \
	ccss-border-parser.c \
	ccss-border-parser.h \
	ccss-color.c \
	ccss-color.h \
	ccss-function.c \
	ccss-function-priv.h \
	ccss.c \
	ccss-image.c \
	ccss-image.h \
	ccss-node.c \
	ccss-parser.c \
	ccss-parser.h \
	ccss-position.c \
	ccss-position.h \
	ccss-selector-group.c \
	ccss-selector-group.h \
	ccss-selector.c \
	ccss-selector.h \
	ccss-style.c \
	ccss-style-priv.h \
	ccss-stylesheet.c

### The actual shared lib. #####################################################

lib_LTLIBRARIES = libccss-1.la

libccss_1_la_DEPENDENCIES = \
	ccss.sym \
	libccss-static.la

libccss_1_la_LDFLAGS = -no-undefined -version-info $(VERSION_INFO)
if CCSS_DEBUG
else
libccss_1_la_LDFLAGS += -export-symbols $(srcdir)/ccss.sym
endif

libccss_1_la_LIBADD = \
	$(CCSS_LIBS) \
	libccss-static.la

libccss_1_la_SOURCES = \
	$(headers_DATA)

headersdir = $(includedir)/libccss-1/ccss

headers_DATA = \
	ccss-function.h \
	ccss.h \
	ccss-macros.h \
	ccss-node.h \
	ccss-style.h \
	ccss-stylesheet.h

# Messing with function pointers in a non-ISO way needs some extra love
# to prevent warnings (not using ISO C).

libccssproperty_la_CPPFLAGS = \
	$(CCSS_COMPAT_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_builddir)

libccssproperty_la_SOURCES = \
	ccss-property.c \
	ccss-property.h

EXTRA_DIST = \
	ccss-features.h.in \
	ccss.sym

