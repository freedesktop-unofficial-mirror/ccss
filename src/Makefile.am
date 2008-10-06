
# Need to use per-target flags because of gce-serialize.c, see
# http://www.gnu.org/software/libtool/manual/automake/Libtool-Issues.html

_cppflags = \
	$(GCE_CFLAGS) \
	-I$(top_srcdir)/libccss \
	-I$(top_builddir)/libccss

enginedir = $(libdir)/gtk-2.0/$(GTK_VERSION)/engines

engine_LTLIBRARIES = libcss.la

libcss_la_CPPFLAGS = $(_cppflags)

libcss_la_LDFLAGS = \
	-module \
	-avoid-version \
	-no-undefined \
	-export-symbols $(srcdir)/exports.sym

libcss_la_LIBADD = \
	$(GCE_LIBS) \
	./../libccss/ccss/libccss-1.la

libcss_la_SOURCES = \
	gce-color.c \
	gce-color.h \
	gce-functions.c \
	gce-functions.h \
	gce-maps.c \
	gce-maps.h \
	gce-node.c \
	gce-node.h \
	gce-rc-style.c \
	gce-rc-style.h \
	gce-serialize.c \
	gce-serialize.h \
	gce-style.c \
	gce-style.h \
	gce-theme.c

noinst_PROGRAMS = css2gtkrc

css2gtkrc_CPPFLAGS = $(_cppflags)

css2gtkrc_SOURCES = \
	css2gtkrc.c \
	gce-serialize.c \
	gce-serialize.h

css2gtkrc_LDADD = \
	$(GCE_LIBS) \
	./../libccss/ccss/libccss-1.la

EXTRA_DIST = \
	exports.sym

