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
#include <ccss/ccss.h>
#include <clutter/clutter.h>
#include <clutter-cairo/clutter-cairo.h>

static void
paint_css (ccss_stylesheet_t	*stylesheet,
	   cairo_t		*cr,
	   int			 x,
	   int			 y,
	   int			 width,
	   int			 height)
{
	ccss_style_t	*style;
	gboolean	 ret;

	/* Query the stylesheet for the type to be drawn. */
	style = ccss_style_new ();
	ret = ccss_stylesheet_query_type (stylesheet, "foo", style);
	g_return_if_fail (ret);

	/* Draw the style covering the actor. */
	ccss_style_draw_rectangle (style, cr, x, y, width, height);

	ccss_style_free (style);
}

int
main (int	  argc,
      char	**argv)
{
	ClutterActor		*stage;
	ClutterActor		*actor;
	ccss_stylesheet_t	*stylesheet;
	cairo_t			*cr;
	ClutterColor		 stage_color = { 0x00, 0x00, 0x00, 0xff };

	clutter_init (&argc, &argv);
	ccss_init ();

	/* Load the stylesheet. */
	stylesheet = ccss_stylesheet_new_from_file ("example-1.css");

	stage = clutter_stage_get_default ();
	clutter_actor_set_size (stage, 480, 320);
	clutter_stage_set_color (CLUTTER_STAGE (stage), &stage_color);

	actor = clutter_cairo_new (150, 30);
	clutter_actor_set_position (actor, 35, 35);
	clutter_container_add_actor (CLUTTER_CONTAINER (stage), actor);

	cr = clutter_cairo_create (CLUTTER_CAIRO (actor));
	paint_css (stylesheet, cr, 0, 0, 150, 30);
	cairo_destroy (cr);

	clutter_actor_show (actor);
	clutter_actor_show (stage);
	clutter_main ();

	ccss_stylesheet_free (stylesheet);

	return EXIT_SUCCESS;
}
