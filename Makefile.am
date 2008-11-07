
ACLOCAL_AMFLAGS = -I .
DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc --enable-werror

SUBDIRS = ccss doc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libccss-1.pc

if CCSS_WITH_CLUTTER
SUBDIRS += cccss
endif

if CCSS_WITH_GTK
SUBDIRS += ccss-gtk examples
pkgconfig_DATA += libccss-gtk-1.pc
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
	libccss-1.pc.in

distclean-local:
	rm -rf *.cache *~

