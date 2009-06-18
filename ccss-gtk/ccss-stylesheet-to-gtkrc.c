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

int
main (int	  argc,
      char	**argv)
{
	ccss_grammar_t			*grammar;
	ccss_stylesheet_t		*stylesheet;
	char				*gtkrc;
	int				 ret = EXIT_FAILURE;

	if (argc <= 1) {
		fprintf (stderr, "Need filename.\n");
		return EXIT_FAILURE;
	}

	gtk_init (&argc, &argv);

	grammar = ccss_gtk_grammar_create ();
	stylesheet = ccss_grammar_create_stylesheet_from_file (grammar, argv[1],
							       NULL);
	ccss_grammar_destroy (grammar), grammar = NULL;

	if (NULL == stylesheet) {
		goto bail;
	}

	gtkrc = ccss_gtk_stylesheet_to_gtkrc (stylesheet);
	ccss_stylesheet_destroy (stylesheet), stylesheet = NULL;

	if (NULL == gtkrc) {
		fprintf (stderr, "Conversion failed.\n");
		goto bail;
	}

	puts (gtkrc);
	puts ("\n");

	g_free (gtkrc), gtkrc = NULL;

	ret = EXIT_SUCCESS;

bail:
	return ret;
}

