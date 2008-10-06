/* The Clutter Cairo CSS Drawing Library.
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

#include "cccss.h"

typedef struct {
	ccss_node_t	 parent;
	ClutterActor	*actor;
} Node;

static char const *
get_type (Node const *self)
{
	/* Here we could match against different kinds of actors. */
	return "actor";
}

static char const *
get_id (Node const *self)
{
	return clutter_actor_get_name (self->actor);
}

static ccss_node_class_t _node_class = {
	.is_a			= NULL,
	.get_container		= NULL,
	.get_base_style		= NULL,
	.get_id			= (ccss_node_get_id_f) get_id,
	.get_type		= (ccss_node_get_type_f) get_type,
	.get_class		= NULL,
	.get_pseudo_class	= NULL,
	.get_attribute		= NULL,
	.get_viewport		= NULL,
	.release		= NULL
};

gboolean
cccss_paint (ccss_stylesheet_t const	*stylesheet,
	     ClutterCairo		*actor,
	     gint			 x,
	     gint			 y,
	     gint			 width,
	     gint			 height)
{
	cairo_t			*cr;
	ccss_style_t		 style;
	Node			 node;
	gboolean		 ret;

	g_return_val_if_fail (stylesheet && actor, FALSE);

	ccss_node_init ((ccss_node_t *) &node, &_node_class);
	node.actor = CLUTTER_ACTOR (actor);

	ccss_style_init (&style);
	ret = ccss_stylesheet_query_apply (stylesheet,
					   (ccss_node_t const *) &node, &style);
	if (!ret) {
		g_warning ("No styling information for %s#%s", 
				"actor", 
				clutter_actor_get_name (CLUTTER_ACTOR (actor)));
		return FALSE;
	}

	cr = clutter_cairo_create (actor);
	ccss_style_draw_rectangle (&style, cr, x, y, width, height);
	cairo_destroy (cr);

	return TRUE;
}

