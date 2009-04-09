
NULL =

AM_CPPFLAGS = \
	-I$(top_builddir) \
	-I$(top_srcdir) \
	$(CCSS_GTK_CFLAGS) \
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
	../ccss/libccss-1.la \
	$(CCSS_GTK_LIBS) \
	$(NULL)

libccss_gtk_1_la_SOURCES = \
	$(headers_DATA) \
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

EXTRA_DIST = \
	ccss-gtk.sym \
	$(NULL)

