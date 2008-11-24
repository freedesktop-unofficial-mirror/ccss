
ACLOCAL_AMFLAGS = -I .
DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc --enable-werror

DIST_SUBDIRS = ccss ccss-cairo ccss-clutter doc examples
SUBDIRS = ccss doc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libccss-1.pc

if CCSS_WITH_CAIRO
SUBDIRS += ccss-cairo
endif

if CCSS_WITH_CLUTTER
SUBDIRS += ccss-clutter
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

