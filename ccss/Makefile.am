
AM_CPPFLAGS = \
	$(CCSS_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_builddir)

lib_LTLIBRARIES = libccss-1.la

libccss_1_la_DEPENDENCIES = \
	ccss.sym

libccss_1_la_LIBADD = $(CCSS_LIBS)

libccss_1_la_LDFLAGS = -no-undefined -version-info $(VERSION_INFO)

if CCSS_DEBUG
else
libccss_1_la_LDFLAGS += -export-symbols ./ccss.sym
endif

headersdir = $(includedir)/libccss-1/ccss

headers_DATA = \
	ccss-background.h \
	ccss-block.h \
	ccss-border.h \
	ccss-color.h \
	ccss-features.h \
	ccss-function.h \
	ccss.h \
	ccss-image.h \
	ccss-node.h \
	ccss-position.h \
	ccss-property.h \
	ccss-selector-group.h \
	ccss-selector.h \
	ccss-style.h \
	ccss-stylesheet.h

if CCSS_WITH_GTK
headers_DATA += \
	ccss-gtk-style.h
endif

libccss_1_la_SOURCES = \
	ccss-background.c \
	ccss-background.h \
	ccss-block.c \
	ccss-block.h \
	ccss-border.c \
	ccss-border.h \
	ccss-color.c \
	ccss-color.h \
	ccss-features.h \
	ccss-function.c \
	ccss-function.h \
	ccss.c \
	ccss.h \
	ccss-image.c \
	ccss-image.h \
	ccss-node.c \
	ccss-node.h \
	ccss-parser.c \
	ccss-parser.h \
	ccss-position.c \
	ccss-position.h \
	ccss-property.c \
	ccss-property.h \
	ccss-selector-group.c \
	ccss-selector-group.h \
	ccss-selector-group-priv.h \
	ccss-selector.c \
	ccss-selector.h \
	ccss-style.c \
	ccss-style.h \
	ccss-stylesheet.c \
	ccss-stylesheet.h

opt_src = 

if CCSS_WITH_GTK
libccss_1_la_SOURCES += \
	ccss-gtk-style.c \
	ccss-gtk-style.h
opt_src += $(libccss_1_la_SOURCES)
endif

BUILT_SOURCES = \
	ccss.sym

CLEANFILES = \
	ccss.sym

EXTRA_DIST = \
	$(opt_src) \
	ccss-features.h.in \
	ccss.sym.in \
	ccss-gtk.sym.in

ccss_sym_in = $(srcdir)/ccss.sym.in

if CCSS_WITH_GTK
ccss_sym_in += $(srcdir)/ccss-gtk.sym.in
endif

ccss.sym: $(ccss_sym_in)
	cat $^ > $@

