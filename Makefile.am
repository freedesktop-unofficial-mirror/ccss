
NULL =

ACLOCAL_AMFLAGS = -I build
DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc --enable-werror

DIST_SUBDIRS = \
	build \
	ccss \
	ccss-doc \
	ccss-cairo \
	ccss-cairo-doc \
	ccss-gtk \
	examples \
	$(NULL)

SUBDIRS = \
	ccss \
	ccss-doc \
	$(NULL)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libccss-1.pc

if CCSS_WITH_CAIRO
SUBDIRS += ccss-cairo ccss-cairo-doc
pkgconfig_DATA += libccss-cairo-1.pc
endif

if CCSS_WITH_GTK
SUBDIRS += ccss-gtk
pkgconfig_DATA += libccss-gtk-1.pc
endif

if CCSS_WITH_EXAMPLES
SUBDIRS += examples
endif

EXTRA_DIST = \
	AUTHORS \
	ChangeLog \
	COPYING \
	INSTALL \
	NEWS \
	README \
	TODO \
	autogen.sh \
	libccss-1.pc.in \
	libccss-cairo-1.pc.in \
	libccss-gtk-1.pc.in \
	$(NULL)

