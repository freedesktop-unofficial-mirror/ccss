
AM_CPPFLAGS = \
	$(CCSS_GTK_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_builddir)

lib_LTLIBRARIES = libccss-gtk-1.la

libccss_gtk_1_la_DEPENDENCIES = \
	ccss-gtk.sym

libccss_gtk_1_la_LIBADD = \
	$(CCSS_GTK_LIBS) \
	../ccss/.libs/libccss.lai

libccss_gtk_1_la_LDFLAGS = -no-undefined -version-info $(VERSION_INFO)

if CCSS_DEBUG
else
libccss_gtk_1_la_LDFLAGS += -export-symbols $(srcdir)/ccss-gtk.sym
endif

headersdir = $(includedir)/libccss-1/ccss-gtk

headers_DATA = \
	ccss-gtk.h \
	ccss-gtk-style.h

libccss_gtk_1_la_SOURCES = \
	$(headers_DATA) \
	ccss-gtk-style.c

EXTRA_DIST = \
	ccss-gtk.sym

