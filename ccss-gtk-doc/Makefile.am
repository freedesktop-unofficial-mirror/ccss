## Process this file with automake to produce Makefile.in

# The name of the module.
DOC_MODULE=ccss-gtk

# The top-level SGML file.
DOC_MAIN_SGML_FILE=ccss-gtk-docs.sgml

# The directory containing the source code. Relative to $(srcdir)
DOC_SOURCE_DIR=../ccss-gtk

# Extra options to pass to gtkdoc-scangobj
SCANGOBJ_OPTIONS= --nogtkinit --type-init-func="ccss_gtk_init ()" 

# Extra options to supply to gtkdoc-scan
SCAN_OPTIONS=

# Extra options to supply to gtkdoc-mkdb
MKDB_OPTIONS=--sgml-mode --output-format=xml

# Extra options to supply to gtkdoc-fixref
FIXXREF_OPTIONS=

# Used for dependencies
HFILE_GLOB=$(top_srcdir)/ccss-gtk/*.h
CFILE_GLOB=$(top_srcdir)/ccss-gtk/*.c

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
GTKDOC_CFLAGS = # -I$(top_srcdir) $(CCSS_GTK_CFLAGS)
GTKDOC_LIBS = # $(CCSS_GTK_LIBS) $(top_builddir)/ccss-gtk/libccss-gtk-1.la

if GTK_DOC_INSTALLED
include $(top_srcdir)/gtk-doc.make
CLEANFILES += scan-*
.PHONY : dist-hook-local
endif

