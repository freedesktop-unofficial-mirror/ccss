
ACLOCAL_AMFLAGS = -I .
DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc --enable-werror

SUBDIRS = ccss doc

if CCSS_WITH_CLUTTER
SUBDIRS += cccss
endif

if CCSS_WITH_GTK
SUBDIRS += ccss-gtk examples
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libccss-1.pc

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

