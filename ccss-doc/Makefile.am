## Process this file with automake to produce Makefile.in

# The name of the module.
DOC_MODULE=ccss

# The top-level SGML file.
DOC_MAIN_SGML_FILE=ccss-docs.sgml

# The directory containing the source code. Relative to $(srcdir)
DOC_SOURCE_DIR=../ccss

# Extra options to pass to gtkdoc-scangobj
SCANGOBJ_OPTIONS= --nogtkinit --type-init-func="ccss_init ()" 

# Extra options to supply to gtkdoc-scan
SCAN_OPTIONS=

# Extra options to supply to gtkdoc-mkdb
MKDB_OPTIONS=--sgml-mode --output-format=xml

# Extra options to supply to gtkdoc-fixref
FIXXREF_OPTIONS=

# Used for dependencies
HFILE_GLOB=$(top_srcdir)/ccss/*.h
CFILE_GLOB=$(top_srcdir)/ccss/*.c

# Header files to ignore when scanning
IGNORE_HFILES =

EXTRA_HFILES =

# Images to copy into HTML directory
HTML_IMAGES =

# Extra SGML files that are included by $(DOC_MAIN_SGML_FILE)
content_files =

# Other files to distribute
extra_files =

# CFLAGS and LDFLAGS for compiling scan program. Only needed
# if $(DOC_MODULE).types is non-empty.
GTKDOC_CFLAGS = # -I$(top_srcdir) $(CCSS_CFLAGS)
GTKDOC_LIBS = # $(CCSS_LIBS) $(top_builddir)/libccss/ccss/libccss.la

if GTK_DOC_INSTALLED
include $(top_srcdir)/gtk-doc.make
CLEANFILES += ccss-scan.*
.PHONY : dist-hook-local
endif

EXTRA_DIST = \
	$(DOC_MAIN_SGML_FILE) \
	ccss.hierarchy \
	$(NULL)

