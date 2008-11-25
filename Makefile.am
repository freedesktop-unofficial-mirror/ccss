
ACLOCAL_AMFLAGS = -I .
DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc --enable-werror

DIST_SUBDIRS = ccss ccss-doc ccss-cairo ccss-cairo-doc examples
SUBDIRS = ccss ccss-doc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libccss-1.pc

if CCSS_WITH_CAIRO
SUBDIRS += ccss-cairo ccss-cairo-doc
pkgconfig_DATA += libccss-cairo-1.pc
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
	libccss-cairo-1.pc.in

