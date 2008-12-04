
AM_CPPFLAGS = \
	-I$(top_builddir) \
	-I$(top_srcdir) \
	$(CCSS_GTK_CFLAGS)

lib_LTLIBRARIES = libccss-gtk-1.la

libccss_gtk_1_la_DEPENDENCIES = \
	ccss-gtk.sym

libccss_gtk_1_la_LDFLAGS = \
	-no-undefined \
	-version-info $(VERSION_INFO) \
	-export-symbols $(srcdir)/ccss-gtk.sym

libccss_gtk_1_la_LIBADD = \
	../ccss/libccss-1.la \
	$(CCSS_GTK_LIBS)

libccss_gtk_1_la_SOURCES = \
	$(headers_DATA) \
	ccss-gtk.c \
	ccss-gtk-grammar.c \
	ccss-gtk-properties.c \
	ccss-gtk-properties.h \
	ccss-gtk-style.c

headersdir = $(includedir)/libccss-1/ccss-gtk

headers_DATA = \
	ccss-gtk.h \
	ccss-gtk-grammar.h \
	ccss-gtk-style.h

EXTRA_DIST = \
	ccss-gtk.sym

