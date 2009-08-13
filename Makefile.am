
NULL =

ACLOCAL_AMFLAGS = -I build
DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc --enable-werror

DIST_SUBDIRS = \
	build \
	ccss \
	ccss-doc \
	ccss-tests \
	ccss-cairo \
	ccss-cairo-doc \
	ccss-gtk \
	ccss-gtk-doc \
	examples \
	$(NULL)

SUBDIRS = \
	ccss \
	ccss-doc \
	$(NULL)

if ENABLE_GLIB_TEST
SUBDIRS += ccss-tests
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ccss-1.pc

if CCSS_WITH_CAIRO
SUBDIRS += ccss-cairo ccss-cairo-doc
pkgconfig_DATA += ccss-cairo-1.pc
endif

if CCSS_WITH_GTK
SUBDIRS += ccss-gtk ccss-gtk-doc
pkgconfig_DATA += ccss-gtk-1.pc
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
	ccss-1.pc.in \
	ccss-cairo-1.pc.in \
	ccss-gtk-1.pc.in \
	$(NULL)

CLEANFILES = \
	test-report.xml \
	$(NULL)

ChangeLog: configure.in
	@echo Creating ChangeLog ...
	$(top_srcdir)/build/gen-changelog.pl > $@

CSSS_FDO_BASE_PATH=annarchy.freedesktop.org:public_html/ccss
CCSS_FDO_DOCS_PATH=$(CSSS_FDO_BASE_PATH)/ccss-$(CCSS_VERSION_MAJOR).$(CCSS_VERSION_MINOR)-doc

# The tarball-upload target not active to prevent accidental uploads.
upload: doc-upload # tarball-upload

doc-upload:
	ssh annarchy.freedesktop.org "mkdir public_html/ccss/ccss-$(CCSS_VERSION_MAJOR).$(CCSS_VERSION_MINOR)-doc"
	scp -r ccss-doc/html $(CCSS_FDO_DOCS_PATH)/ccss
	scp -r ccss-cairo-doc/html $(CCSS_FDO_DOCS_PATH)/ccss-cairo
	scp -r ccss-gtk-doc/html $(CCSS_FDO_DOCS_PATH)/ccss-gtk

#TARBALL=$(PACKAGE)-$(VERSION).tar.gz
#CHECKSUMS=$(PACKAGE)-$(VERSION).md5sum
#
#tarball-upload: checksum
#	scp $(TARBALL) $(CSSS_FDO_BASE_PATH)
#	scp $(CHECKSUMS) $(CSSS_FDO_BASE_PATH)
#
#$(CHECKSUMS): $(TARBALL)
#	md5sum $< > $@

