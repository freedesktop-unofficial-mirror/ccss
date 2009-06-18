
NULL =

AM_CPPFLAGS = \
	-I$(top_builddir) \
	-I$(top_srcdir) \
	$(CCSS_GTK_CFLAGS) \
	-DCCSS_BUILD \
	-DCCSS_CAIRO_BUILD \
	-DCCSS_GTK_BUILD \
	$(NULL)

lib_LTLIBRARIES = libccss-gtk-1.la

libccss_gtk_1_la_DEPENDENCIES = \
	ccss-gtk.sym \
	$(NULL)

libccss_gtk_1_la_LDFLAGS = \
	-no-undefined \
	-version-info $(VERSION_INFO) \
	-export-symbols $(srcdir)/ccss-gtk.sym \
	$(NULL)

libccss_gtk_1_la_LIBADD = \
	$(CCSS_GTK_LIBS) \
	$(NULL)

libccss_gtk_1_la_SOURCES = \
	$(headers_DATA) \
	ccss-gtk-color.c \
	ccss-gtk-color.h \
	ccss-gtk-functions.c \
	ccss-gtk-functions.h \
	ccss-gtk-grammar.c \
	ccss-gtk-property.c \
	ccss-gtk-property.h \
	ccss-gtk-stylesheet.c \
	$(NULL)

headersdir = $(includedir)/libccss-1/ccss-gtk

headers_DATA = \
	ccss-gtk.h \
	ccss-gtk-grammar.h \
	ccss-gtk-stylesheet.h \
	$(NULL)

bin_PROGRAMS = ccss-stylesheet-to-gtkrc

ccss_stylesheet_to_gtkrc_SOURCES = \
	ccss-stylesheet-to-gtkrc.c

ccss_stylesheet_to_gtkrc_LDADD = \
	$(CCSS_GTK_LIBS) \
	libccss-gtk-1.la

EXTRA_DIST = \
	ccss-gtk.sym \
	$(NULL)

