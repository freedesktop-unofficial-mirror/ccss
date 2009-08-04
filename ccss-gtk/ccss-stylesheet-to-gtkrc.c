/* vim: set ts=8 sw=8 noexpandtab: */

/* The Gtk+ CSS Drawing Library.
 * Copyright (C) 2008 Robert Staudinger
 *
 * This  library is free  software; you can  redistribute it and/or
 * modify it  under  the terms  of the  GNU Lesser  General  Public
 * License  as published  by the Free  Software  Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed  in the hope that it will be useful,
 * but  WITHOUT ANY WARRANTY; without even  the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License  along  with  this library;  if not,  write to  the Free
 * Software Foundation, Inc., 51  Franklin St, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <ccss-gtk/ccss-gtk.h>
#include <gtk/gtk.h>
#include "config.h"

static bool _dump = FALSE;
static char const *_user_agent_file = NULL;
static char const *_user_file = NULL;
static char const *_author_file = NULL;
static char const **_files = NULL;

static GOptionEntry _options[] = {
	{ "user-agent-css", 'g', 0, G_OPTION_ARG_FILENAME, &_user_agent_file, "user-agent CSS file", "<file>" },
	{ "user-css", 'u', 0, G_OPTION_ARG_FILENAME, &_user_file, "user CSS file", "<file>" },
	{ "author-css", 'a', 0, G_OPTION_ARG_FILENAME, &_author_file, "author CSS files", "<file>" },
	{ "dump", 'd', 0, G_OPTION_ARG_NONE, &_dump, "dump CSS instead of producing GTKRC", NULL},
	{ G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_FILENAME_ARRAY, &_files, NULL, "<files ...>" },
	{ NULL }
};

int
main (int	  argc,
      char	**argv)
{
	ccss_grammar_t		*grammar;
	ccss_stylesheet_t	*stylesheet;
	int			 ret = EXIT_FAILURE;
	GOptionContext		*context;
	GError			*error = NULL;

	context = g_option_context_new (" - CSS to GTKRC converter");
	g_option_context_add_main_entries (context, _options, NULL);
	g_option_context_add_group (context, gtk_get_option_group (TRUE));
	if (!g_option_context_parse (context, &argc, &argv, &error)) {
		g_critical ("%s : %s", G_STRLOC, error->message);
		g_clear_error (&error);
		return EXIT_FAILURE;
	}
	if (!(_user_agent_file || _user_file || _author_file || _files)) {
		char *help = g_option_context_get_help (context, TRUE, NULL);
		fputs (help, stderr);
		g_free (help), help = NULL;
		return EXIT_SUCCESS;
	}
	g_option_context_free (context), context = NULL;

	gtk_init (&argc, &argv);

	grammar = ccss_gtk_grammar_create ();
	stylesheet = ccss_grammar_create_stylesheet (grammar);
	ccss_grammar_destroy (grammar), grammar = NULL;

	if (_user_agent_file) {
		ccss_stylesheet_add_from_file (stylesheet,
					       _user_agent_file,
					       CCSS_STYLESHEET_USER_AGENT,
					       NULL);
	}
	if (_user_file) {
		ccss_stylesheet_add_from_file (stylesheet,
					       _user_file,
					       CCSS_STYLESHEET_USER,
					       NULL);
	}
	if (_author_file) {
		ccss_stylesheet_add_from_file (stylesheet,
					       _author_file,
					       CCSS_STYLESHEET_AUTHOR,
					       NULL);

	}
	for (unsigned int i = 0; _files && _files[i]; i++) {
		ccss_stylesheet_add_from_file (stylesheet,
					       _files[i],
					       CCSS_STYLESHEET_AUTHOR,
					       NULL);
	}

	if (_dump) {
		ccss_stylesheet_dump (stylesheet);
		ret = EXIT_SUCCESS;
	} else {
		char *gtkrc = ccss_gtk_stylesheet_to_gtkrc (stylesheet);
		if (NULL == gtkrc) {
			fprintf (stderr, "Conversion failed.\n");
		} else {
			puts (gtkrc);
			puts ("\n");
			g_free (gtkrc), gtkrc = NULL;
			ret = EXIT_SUCCESS;
		}
	}
	ccss_stylesheet_destroy (stylesheet), stylesheet = NULL;

	return ret;
}

