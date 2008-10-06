/* The CSS Theme Engine for Gtk+.
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

#include <ccss/ccss.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include "gce-serialize.h"

int
main (int	  argc,
      char	**argv)
{
	ccss_stylesheet_t	*stylesheet;
	char			*gtkrc;

	if (argc <= 1) {
		fprintf (stderr, "Need filename.\n");
		return EXIT_FAILURE;
	}

	stylesheet = ccss_stylesheet_new_from_file (argv[1]);
	g_assert (stylesheet);

	gtkrc = gce_serialize (stylesheet);
	ccss_stylesheet_free (stylesheet), stylesheet = NULL;

	if (NULL == gtkrc) {
		fprintf (stderr, "Conversion failed.\n");
		return EXIT_FAILURE;
	}

	printf (gtkrc);
	printf ("\n");

	g_free (gtkrc), gtkrc = NULL;

	return EXIT_SUCCESS;
}
