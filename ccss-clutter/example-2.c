/* The Cairo CSS Drawing Library.
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

#include <stdlib.h>
#include <clutter/clutter.h>
#include <ccss-clutter/ccss-clutter.h>

/* FIXME put your prefix here or fix the `url' functin below to resolve it. */
#define PREFIX "/home/rstaudinger/Desktop/Devel/gtk-css-engine-build/libccss/cccss"

/* Implementation of the CSS url() function. */
static char *
url (ccss_block_t	*block,
     char const		*property_name,
     char const		*function_name,
     GSList const	*args)
{
	g_return_val_if_fail (args, NULL);

	/* The file references from the stylesheet has to be in the current dir.
	 * Admittedly handling relative paths like this is, well, ... */
	return g_strdup_printf ("file://%s/%s", PREFIX, (char *) args->data);
}

/* Dispatch table of CSS function implementations. */
static ccss_function_t const _functions[] = 
{
	{ "url", url },
	{ NULL }
};

static void
create_actor (ccss_stylesheet_t	const	*stylesheet,
	      ClutterActor		*stage,
	      const char		*name,
	      gint			 x,
	      gint			 y)
{
	ClutterActor		*actor;

	actor = clutter_cairo_new (150, 30);
	clutter_actor_set_position (actor, x, y);
	clutter_container_add_actor (CLUTTER_CONTAINER (stage), actor);
	clutter_actor_show (actor);
	/* Set name tag and paint. */
	clutter_actor_set_name (actor, name);
	ccss_clutter_paint (stylesheet, CLUTTER_CAIRO (actor), 0, 0, 150, 30);
}

int
main (int	  argc,
      char	**argv)
{
	ClutterActor		*stage;
	ccss_stylesheet_t	*stylesheet;
	ClutterColor		 stage_color = { 0x00, 0x00, 0x00, 0xff };
	const char 		*names[] = { "foo", "bar", "baz", "qux", "mos" };

	clutter_init (&argc, &argv);

	/* Load the stylesheet. */
	ccss_init (_functions);
	stylesheet = ccss_stylesheet_new_from_file ("example-2.css");

	stage = clutter_stage_get_default ();
	clutter_actor_set_size (stage, 480, 320);
	clutter_stage_set_color (CLUTTER_STAGE (stage), &stage_color);

	for (guint i = 0; i < G_N_ELEMENTS (names); i++) {
		create_actor (stylesheet, stage, names[i], 35, 35 + (i * 50));
	}

	clutter_actor_show (stage);
	clutter_main ();

	ccss_stylesheet_free (stylesheet);
	ccss_shutdown ();

	return EXIT_SUCCESS;
}
