/* vim: set ts=8 sw=8 noexpandtab: */

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

#include <string.h>
#include <glib.h>
#include <ccss/ccss-style-priv.h>	/* PONDERING: use only public headers? */
#include "ccss-cairo-appearance.h"
#include "ccss-cairo-background.h"
#include "ccss-cairo-border.h"
#include "ccss-cairo-border-image.h"
#include "ccss-cairo-style.h"
#include "ccss-cairo-property.h"
#include "config.h"

typedef struct {
	ccss_property_base_t	base;
	ccss_cairo_gap_side_t   side;
} gap_side_t;

typedef struct {
	ccss_property_base_t	base;
	double			start;
} gap_start_t;

typedef struct {
	ccss_property_base_t	base;
	double			width;
} gap_width_t;


static ccss_property_class_t const *
peek_property_class (char const *property_name);

struct {
	char const *name;
	char const *fallback;
} const _fallback_map[] = {
    { "border-color", "color" },

    { "border-bottom-color", "border-color" },
    { "border-bottom-style", "border-style" },
    { "border-bottom-width", "border-width" },

    { "border-left-color", "border-color" },
    { "border-left-style", "border-style" },
    { "border-left-width", "border-width" },

    { "border-right-color", "border-color" },
    { "border-right-style", "border-style" },
    { "border-right-width", "border-width" },

    { "border-top-color", "border-color" },
    { "border-top-style", "border-style" },
    { "border-top-width", "border-width" },

    { "border-top-left-radius", "border-radius" },
    { "border-top-right-radius", "border-radius" },
    { "border-bottom-right-radius", "border-radius" },
    { "border-bottom-left-radius", "border-radius" },

    { NULL }
};

/* Look up a property, take fallback into account. */
static ccss_property_base_t const *
lookup_property_r (ccss_style_t const	*self,
		   char const		*property_name)
{
	GQuark				 property_id;
	ccss_property_base_t const	*property = NULL;

	property_id = g_quark_try_string (property_name);
	if (property_id) {
		property = (ccss_property_base_t const *)
				g_hash_table_lookup (self->properties,
						     (gpointer) property_id);
	}
	if (property) {
		return property;
	}

	for (unsigned int i = 0; _fallback_map[i].name != NULL; i++) {
		if (0 == g_strcmp0 (property_name, _fallback_map[i].name)) {
			return lookup_property_r (self, _fallback_map[i].fallback);
		}
	}

	return NULL;
}

static void
gather_stroke (ccss_style_t const		*self,
	       char const			*color_prop,
	       ccss_color_t const		*color_fallback,
	       char const			*style_prop,
	       ccss_border_style_t const	*style_fallback,
	       char const			*width_prop,
	       ccss_border_width_t const	*width_fallback,
	       ccss_border_stroke_t		*stroke)
{
	stroke->color = (ccss_color_t *) lookup_property_r (self, color_prop);
	if (NULL == stroke->color) {
		stroke->color = color_fallback;
	}

	stroke->style = (ccss_border_style_t *) lookup_property_r (self, style_prop);
	if (NULL == stroke->style) {
		stroke->style = style_fallback;
	}

	stroke->width = (ccss_border_width_t *) lookup_property_r (self, width_prop);
	if (NULL == stroke->width) {
		stroke->width = width_fallback;
	}
}

static ccss_border_join_t const *
gather_join (ccss_style_t const		*self,
	     char const			*prop,
	     ccss_border_join_t const	*fallback)
{
	ccss_border_join_t const *join;

	join = (ccss_border_join_t *) lookup_property_r (self, prop);
	if (NULL == join) {
		join = fallback;
	}

	return join;
}

static void
gather_outline (ccss_style_t const		 *self,
		ccss_border_stroke_t		 *bottom,
		ccss_border_stroke_t		 *left,
		ccss_border_stroke_t		 *right,
		ccss_border_stroke_t		 *top,
		ccss_border_join_t const	**bottom_left,
		ccss_border_join_t const	**bottom_right,
		ccss_border_join_t const	**top_left,
		ccss_border_join_t const	**top_right)
{
	gather_stroke (self, 
		"border-bottom-color", NULL, 
		"border-bottom-style", NULL, 
		"border-bottom-width", NULL, 
		bottom);

	gather_stroke (self, 
		"border-left-color", NULL, 
		"border-left-style", NULL, 
		"border-left-width", NULL, 
		left);

	gather_stroke (self, 
		"border-right-color", NULL, 
		"border-right-style", NULL, 
		"border-right-width", NULL, 
		right);

	gather_stroke (self, 
		"border-top-color", NULL, 
		"border-top-style", NULL, 
		"border-top-width", NULL, 
		top);

	*bottom_left = gather_join (self, "border-bottom-left-radius", NULL);

	*bottom_right = gather_join (self, "border-bottom-right-radius", NULL);

	*top_left = gather_join (self, "border-top-left-radius", NULL);

	*top_right = gather_join (self, "border-top-right-radius", NULL);

}

static void
gather_background (ccss_style_t const			 *self,
		   ccss_background_attachment_t const	**bg_attachment, 
		   ccss_color_t const			**bg_color,
		   ccss_background_image_t const	**bg_image,
		   ccss_background_position_t const	**bg_position,
		   ccss_background_repeat_t const	**bg_repeat,
		   ccss_background_size_t const		**bg_size)
{
	*bg_attachment = (ccss_background_attachment_t const *) 
		g_hash_table_lookup (
			self->properties,
			(gpointer) CCSS_PROPERTY_BACKGROUND_ATTACHMENT);
	if (!*bg_attachment)
		*bg_attachment = NULL;

	*bg_color = (ccss_color_t const *)
		g_hash_table_lookup (
			self->properties,
			(gpointer) CCSS_PROPERTY_BACKGROUND_COLOR);
	if (!*bg_color)
		*bg_color = NULL;

	*bg_image = (ccss_background_image_t const *)
		g_hash_table_lookup (
			self->properties,
			(gpointer) CCSS_PROPERTY_BACKGROUND_IMAGE);
	if (!*bg_image)
		*bg_image = NULL;

	*bg_position = (ccss_background_position_t const *)
		g_hash_table_lookup (
			self->properties,
			(gpointer) CCSS_PROPERTY_BACKGROUND_POSITION);
	if (!*bg_position)
		*bg_position = NULL;

	*bg_repeat = (ccss_background_repeat_t const *)
		g_hash_table_lookup (
			self->properties,
			(gpointer) CCSS_PROPERTY_BACKGROUND_REPEAT);
	if (!*bg_repeat)
		*bg_repeat = NULL;

	*bg_size = (ccss_background_size_t const *)
		g_hash_table_lookup (
			self->properties,
			(gpointer) CCSS_PROPERTY_BACKGROUND_SIZE);
	if (!*bg_size)
		*bg_size = NULL;
}

/**
 * ccss_cairo_style_draw_rectangle:
 * @self:	a #ccss_style_t.
 * @cr:		the target to draw onto.
 * @x:		the starting x coordinate.
 * @y:		the starting y coordinate.
 * @width:	width of the outline to draw.
 * @height:	height of the outline to draw.
 *
 * Draw a rectangle using this style instance.
 **/
void
ccss_cairo_style_draw_rectangle (ccss_style_t const	*self,
				 cairo_t		*cr, 
				 double			 x,
				 double			 y,
				 double			 width,
				 double			 height)
{
	ccss_border_stroke_t		 bottom, left, right, top;
	ccss_border_join_t const	*bottom_left;
	ccss_border_join_t const	*bottom_right;
	ccss_border_join_t const	*top_left;
	ccss_border_join_t const	*top_right;
	ccss_border_image_t const	*border_image;

	ccss_background_attachment_t const	*bg_attachment;
	ccss_color_t const			*bg_color;
	ccss_background_image_t const		*bg_image;
	ccss_background_position_t const	*bg_position;
	ccss_background_repeat_t const		*bg_repeat;
	ccss_background_size_t const		*bg_size;

	double l, t, w, h;

	ccss_cairo_appearance_t *appearance = NULL;
	if (ccss_style_get_property (self,
				     "ccss-appearance",
				     (ccss_property_base_t const **) &appearance) &&
	    appearance->base.state == CCSS_PROPERTY_STATE_SET &&
	    appearance->draw_function) {

		bool ret = appearance->draw_function (self, cr,
						      x, y, width, height);
		if (ret)
			return;
	}

	gather_outline (self, &bottom, &left, &right, &top,
			&bottom_left, &bottom_right, &top_left, &top_right);

	gather_background (self, &bg_attachment, &bg_color, &bg_image, 
			   &bg_position, &bg_repeat, &bg_size);

	ccss_cairo_border_path (&left, top_left, 
				&top, top_right,
				&right, bottom_right,
				&bottom, bottom_left,
				cr, x, y, width, height);

	/* FIXME: background size is calculated against allocation
	 * when using `fixed'. */
	if (bg_attachment &&
	    CCSS_BACKGROUND_FIXED == bg_attachment->attachment) {
		l = self->viewport_x;
		t = self->viewport_y;
		w = self->viewport_width;
		h = self->viewport_height;
	} else {
		l = x;
		t = y;
		w = width;
		h = height;
	}

	ccss_cairo_background_fill (bg_attachment, bg_color, bg_image, bg_position, 
				    bg_repeat, bg_size, cr, l, t, w, h);

	cairo_new_path (cr);

	/* PONDERING: should border-image vs. border be resolved at style application time, 
	 * i.e. should a higher-priority border override border-image? */
	border_image = 	(ccss_border_image_t const *) 
		g_hash_table_lookup (
			self->properties,
			(gpointer) CCSS_PROPERTY_BORDER_IMAGE);

	if (border_image) {
		ccss_cairo_border_image_draw (border_image, cr,
					      x, y, width, height);

	} else {
		ccss_cairo_border_draw (&left, top_left, 
					&top, top_right,
					&right, bottom_right,
					&bottom, bottom_left,
					CCSS_BORDER_VISIBILITY_SHOW_ALL,
					cr, x, y, width, height);
	}
}

/**
 * ccss_cairo_style_draw_rectangle_with_gap:
 * @self:	a ccss_style_t.
 * @cr:		the target to draw onto.
 * @x:		the starting x coordinate.
 * @y:		the starting y coordinate.
 * @width:	width of the outline to draw.
 * @height:	height of the outline to draw.
 * @gap_side:	side in which to leave the gap.
 * @gap_start:	starting position of the gap.
 * @gap_width:	width of the gap.
 *
 * Draw a rectangle with gap using this style instance.
 **/
void
ccss_cairo_style_draw_rectangle_with_gap (ccss_style_t const		*self,
					  cairo_t			*cr, 
					  double			 x,
					  double			 y,
					  double			 width,
					  double			 height,
					  ccss_cairo_gap_side_t		 gap_side,
					  double			 gap_start,
					  double			 gap_width)
{
	ccss_border_stroke_t		 bottom, left, right, top;
	ccss_border_join_t const	*bl;
	ccss_border_join_t const	*br;
	ccss_border_join_t const	*tl;
	ccss_border_join_t const	*tr;

	ccss_background_attachment_t const	*bg_attachment;
	ccss_color_t const			*bg_color;
	ccss_background_image_t const		*bg_image;
	ccss_background_position_t const	*bg_position;
	ccss_background_repeat_t const		*bg_repeat;
	ccss_background_size_t const		*bg_size;

	ccss_border_join_t bottom_left = { .base.state = CCSS_PROPERTY_STATE_SET, .radius = 0 };
	ccss_border_join_t bottom_right = { .base.state = CCSS_PROPERTY_STATE_SET, .radius = 0 };
	ccss_border_join_t top_left = { .base.state = CCSS_PROPERTY_STATE_SET, .radius = 0 };
	ccss_border_join_t top_right = { .base.state = CCSS_PROPERTY_STATE_SET, .radius = 0 };

	double l, t, w, h;

	ccss_cairo_appearance_t *appearance = NULL;
	if (ccss_style_get_property (self,
				     "ccss-appearance",
				     (ccss_property_base_t const **) &appearance) &&
	    appearance->base.state == CCSS_PROPERTY_STATE_SET &&
	    appearance->draw_function) {

		static GQuark gap_side_id = 0;
		static GQuark gap_start_id = 0;
		static GQuark gap_width_id = 0;

		gap_side_t gap_side_property;
		gap_start_t gap_start_property;
		gap_width_t gap_width_property;

		bool ret;

		if (gap_side_id == 0)
			gap_side_id = g_quark_from_static_string ("ccss-gap-side");
		ccss_property_init (&gap_side_property.base,
				    peek_property_class ("ccss-gap-side"));
		gap_side_property.side = gap_side;
		gap_side_property.base.state = CCSS_PROPERTY_STATE_SET;
		g_hash_table_insert (self->properties,
				     (gpointer) gap_side_id,
				     &gap_side_property);

		if (gap_start_id == 0)
			gap_start_id = g_quark_from_static_string ("ccss-gap-start");
		ccss_property_init (&gap_start_property.base,
				    peek_property_class ("ccss-gap-start"));
		gap_start_property.start = gap_start;
		gap_start_property.base.state = CCSS_PROPERTY_STATE_SET;
		g_hash_table_insert (self->properties,
				     (gpointer) gap_start_id,
				     &gap_start_property);

		if (gap_width_id == 0)
			gap_width_id = g_quark_from_static_string ("ccss-gap-width");
		ccss_property_init (&gap_width_property.base,
				    peek_property_class ("ccss-gap-width"));
		gap_width_property.width = gap_width;
		gap_width_property.base.state = CCSS_PROPERTY_STATE_SET;
		g_hash_table_insert (self->properties,
				     (gpointer) gap_width_id,
				     &gap_width_property);

		ret = appearance->draw_function (self, cr,
						 x, y, width, height);

		g_hash_table_remove (self->properties, (gpointer) gap_side_id);
		g_hash_table_remove (self->properties, (gpointer) gap_start_id);
		g_hash_table_remove (self->properties, (gpointer) gap_width_id);

		if (ret)
			return;
	}

	gather_outline (self, &bottom, &left, &right, &top,
			&bl, &br, &tl, &tr);

	gather_background (self, &bg_attachment, &bg_color, &bg_image, 
			   &bg_position, &bg_repeat, &bg_size);


	/* The rounding radii will have to be adjusted for certain gap
	 * positions, so we work on a copied set of them. */
	if (bl) bottom_left = *bl;
	if (br) bottom_right = *br;
	if (tl) top_left = *tl;
	if (tr) top_right = *tr;

	switch (gap_side) {
	case CCSS_CAIRO_GAP_SIDE_LEFT:
		if (bottom_left.radius > height - gap_start - gap_width)
			bottom_left.radius = height - gap_start - gap_width;
		if (top_left.radius > gap_start)
			top_left.radius = gap_start;
		break;
	case CCSS_CAIRO_GAP_SIDE_TOP:
		if (top_left.radius > gap_start)
			top_left.radius = gap_start;
		if (top_right.radius > width - gap_start - gap_width)
			top_right.radius = width - gap_start - gap_width;
		break;
	case CCSS_CAIRO_GAP_SIDE_RIGHT:
		if (top_right.radius > gap_start)
			top_right.radius = gap_start;
		if (bottom_right.radius > height - gap_start - gap_width)
			bottom_right.radius = height - gap_start - gap_width;
		break;
	case CCSS_CAIRO_GAP_SIDE_BOTTOM:
		if (bottom_left.radius > gap_start)
			bottom_left.radius = gap_start;
		if (bottom_right.radius > width - gap_start - gap_width)
			bottom_right.radius = width - gap_start - gap_width;
		break;
	default:
		g_assert_not_reached ();
		return;	/* prevent error building without assertions */
	}

	if (top_left.radius < 0)	top_left.radius = 0;
	if (top_right.radius < 0)	top_right.radius = 0;
	if (bottom_right.radius < 0)	bottom_right.radius = 0;
	if (bottom_left.radius < 0)	bottom_left.radius = 0;

	ccss_cairo_border_clamp_radii (x, y, width, height,
				       &top_left.radius, &top_right.radius,
				       &bottom_right.radius, &bottom_left.radius);

	ccss_cairo_border_path (&left, &top_left, 
				&top, &top_right,
				&right, &bottom_right,
				&bottom, &bottom_left,
				cr, x, y, width, height);

	/* FIXME: background size is calculated against allocation
	 * when using `fixed'. */
	if (bg_attachment && 
	    CCSS_BACKGROUND_FIXED == bg_attachment->attachment) {
		l = self->viewport_x;
		t = self->viewport_y;
		w = self->viewport_width;
		h = self->viewport_height;
	} else {
		l = x;
		t = y;
		w = width;
		h = height;
	}

	ccss_cairo_background_fill (bg_attachment, bg_color, bg_image, bg_position, 
				    bg_repeat, bg_size, cr, l, t, w, h);

	cairo_new_path (cr);

	/* Drawing functions start at bottom-left corder and
	 * proceed clock-wise. */
	switch (gap_side) {
	case CCSS_CAIRO_GAP_SIDE_LEFT:
		/* Draw gap only if it's not over the whole border. */
		if (gap_start > y || gap_width < height) {
			/* Rounding reaches until start of gap?
			 * With Gtk+ the portion following the gap might not be visible. */
			if (bottom_left.radius < height - gap_start - gap_width &&
			    gap_start + gap_width < height) {
				ccss_cairo_border_draw (
					&left, NULL, NULL, NULL,
					NULL, NULL, NULL, &bottom_left,
					CCSS_BORDER_VISIBILITY_HIDE_BOTTOM_LEFT |
					CCSS_BORDER_ROUNDING_UNRESTRICTED,
					cr, x, y + gap_start + gap_width - 1, 
					0, height - gap_start - gap_width + 1);
			}
			/* Rounding reaches until start of gap? */
			if (top_left.radius < gap_start) {
				ccss_cairo_border_draw (
					&left, &top_left, NULL, NULL,
					NULL, NULL, NULL, NULL,
					CCSS_BORDER_VISIBILITY_HIDE_LEFT_TOP |
					CCSS_BORDER_ROUNDING_UNRESTRICTED,
					cr, x, y, 0, gap_start + 1);
			}
		}
		ccss_cairo_border_draw (&left, &top_left,
					&top, &top_right,
					&right, &bottom_right,
					&bottom, &bottom_left,
					CCSS_BORDER_VISIBILITY_HIDE_LEFT,
					cr, x, y, width, height);
		break;
	case CCSS_CAIRO_GAP_SIDE_TOP:
		/* Draw gap only if it's not over the whole border. */
		if (gap_start > x || gap_width < width) {
			/* Rounding reaches until start of gap? */
			if (top_left.radius < gap_start) {
				ccss_cairo_border_draw (
					NULL, &top_left, &top, NULL,
					NULL, NULL, NULL, NULL,
					CCSS_BORDER_VISIBILITY_HIDE_LEFT_TOP |
					CCSS_BORDER_ROUNDING_UNRESTRICTED,
					cr, x, y, gap_start + 1, 0);
			}
			/* Rounding reaches until start of gap?
			 * With Gtk+ the portion following the gap might not be visible. */
			if (top_right.radius < width - gap_start - gap_width &&
			    gap_start + gap_width < width) {
				ccss_cairo_border_draw (
					NULL, NULL, &top, &top_right,
					NULL, NULL, NULL, NULL,
					CCSS_BORDER_VISIBILITY_HIDE_TOP_RIGHT |
					CCSS_BORDER_ROUNDING_UNRESTRICTED,
					cr, x + gap_start + gap_width - 1, y, 
					width - gap_start - gap_width + 1, 0);
			}
		}
		ccss_cairo_border_draw (&left, &top_left, 
					&top, &top_right,
					&right, &bottom_right,
					&bottom, &bottom_left,
					CCSS_BORDER_VISIBILITY_HIDE_TOP,
					cr, x, y, width, height);
		break;
	case CCSS_CAIRO_GAP_SIDE_RIGHT:
		/* Draw gap only if it's not over the whole border. */
		if (gap_start > y || gap_width < height) {
			/* Rounding reaches until start of gap? */
			if (top_right.radius < gap_start) {
				ccss_cairo_border_draw (
					NULL, NULL, NULL, &top_right,
					&right, NULL, NULL, NULL,
					CCSS_BORDER_VISIBILITY_HIDE_TOP_RIGHT |
					CCSS_BORDER_ROUNDING_UNRESTRICTED,
					cr, x + width, y, 0, gap_start + 1);
			}
			/* Rounding reaches until start of gap?
			 * With Gtk+ the portion following the gap might not be visible. */
			if (bottom_right.radius < height - gap_start - gap_width &&
			    gap_start + gap_width < height) {
				ccss_cairo_border_draw (
					NULL, NULL, NULL, NULL,
					&right, &bottom_right, NULL, NULL,
					CCSS_BORDER_VISIBILITY_HIDE_RIGHT_BOTTOM |
					CCSS_BORDER_ROUNDING_UNRESTRICTED,
					cr, x + width, y + gap_start + gap_width - 1, 
					0, height - gap_start - gap_width + 1);
			}
		}
		ccss_cairo_border_draw (&left, &top_left,
					&top, &top_right, 
					&right, &bottom_right,
					&bottom, &bottom_left,
					CCSS_BORDER_VISIBILITY_HIDE_RIGHT,
					cr, x, y, width, height);
		break;
	case CCSS_CAIRO_GAP_SIDE_BOTTOM:
		/* Draw gap only if it's not over the whole border. */
		if (gap_start > x || gap_width < width) {
			/* Rounding reaches until start of gap? &&
			 * With Gtk+ the portion following the gap might not be visible. */
			if (bottom_right.radius < width - gap_start - gap_width &&
			    gap_start + gap_width < width) {
				ccss_cairo_border_draw (
					NULL, NULL, NULL, NULL,
					NULL, &bottom_right, &bottom, NULL,
					CCSS_BORDER_VISIBILITY_HIDE_RIGHT_BOTTOM |
					CCSS_BORDER_ROUNDING_UNRESTRICTED,
					cr, x + gap_start + gap_width - 1, y + height,
					width - gap_start - gap_width + 1, 0);
			}
			/* Rounding reaches until start of gap? */
			if (bottom_left.radius < gap_start) {
				ccss_cairo_border_draw (
					NULL, NULL, NULL, NULL,
					NULL, NULL, &bottom, &bottom_left,
					CCSS_BORDER_VISIBILITY_HIDE_BOTTOM_LEFT |
					CCSS_BORDER_ROUNDING_UNRESTRICTED,
					cr, x, y + height, gap_start + 1, 0);
			}
		}
		ccss_cairo_border_draw (&left, &top_left,
					&top, &top_right,
					&right, &bottom_right,
					&bottom, &bottom_left,
					CCSS_BORDER_VISIBILITY_HIDE_BOTTOM,
					cr, x, y, width, height);
		break;
	default:
		g_assert_not_reached ();
		return;	/* prevent error building without assertions */
	}
}

/**
 * ccss_cairo_style_get_double:
 * @self:		a #ccss_style_t.
 * @property_name:	name of the property.
 * @value:		location to store the converted property.
 *
 * Query a numeric property with fallbacks, e.g. `border-color' if `border-left-color' is not given.
 *
 * Returns: %TRUE if the property was found and could be converted.
 **/
bool 
ccss_cairo_style_get_double (ccss_style_t const	*self,
			     char const		*property_name,
			     double		*value)
{
	ccss_property_base_t const *property;

	g_return_val_if_fail (self && property_name && value, false);

	property = lookup_property_r (self, property_name);
	if (NULL == property)
		return false;

	/* Have conversion function? */
	g_return_val_if_fail (property->property_class, false);
	if (NULL == property->property_class->convert) {
		return false;
	}

	return property->property_class->convert (property,
						  CCSS_PROPERTY_TYPE_DOUBLE,
						  value);
}

/**
 * ccss_cairo_style_get_string:
 * @self:		a #ccss_style_t.
 * @property_name:	name of the property.
 * @value:		location to store the converted property.
 *
 * Query a string property with fallbacks, e.g. `border-color' if `border-left-color' is not given.
 * 
 * Returns: %TRUE if the property was found and could be converted.
 **/
bool 
ccss_cairo_style_get_string (ccss_style_t const	 *self,
			     char const		 *property_name,
			     char		**value)
{
	ccss_property_base_t const *property;

	g_return_val_if_fail (self && property_name && value, false);

	property = lookup_property_r (self, property_name);
	if (NULL == property)
		return false;

	/* Have conversion function? */
	g_return_val_if_fail (property->property_class, false);
	if (NULL == property->property_class->convert) {
		return false;
	}

	return property->property_class->convert (property,
						  CCSS_PROPERTY_TYPE_STRING,
						  value);
}

/**
 * ccss_cairo_style_get_property:
 * @self:		a #ccss_style_t.
 * @property_name:	name of the property.
 * @value:		location to store the property.
 * 
 * Query a custom property with fallbacks, e.g. `border-color' if `border-left-color' is not given.
 *
 * Returns: %TRUE if the property was found.
 **/
bool
ccss_cairo_style_get_property (ccss_style_t const		 *self,
			       char const			 *property_name,
			       ccss_property_base_t const	**property)
{
	g_return_val_if_fail (self && property_name && property, false);

	*property = lookup_property_r (self, property_name);

	return (bool) *property;
}

static void
gap_destroy_nop (ccss_property_base_t *property)
{
	/* Gap properties are stack-allocated. */
}

static bool
gap_side_convert (gap_side_t const	*property,
		  ccss_property_type_t	 target,
		  void			*value)
{
	g_return_val_if_fail (property && value, false);

	if (CCSS_PROPERTY_TYPE_DOUBLE == target)
		return false;

	switch (property->side) {
	case CCSS_CAIRO_GAP_SIDE_LEFT:
		* (char const **) value = g_strdup ("left");
		break;
	case CCSS_CAIRO_GAP_SIDE_RIGHT:
		* (char const **) value = g_strdup ("right");
		break;
	case CCSS_CAIRO_GAP_SIDE_TOP:
		* (char const **) value = g_strdup ("top");
		break;
	case CCSS_CAIRO_GAP_SIDE_BOTTOM:
		* (char const **) value = g_strdup ("bottom");
		break;
	}

	return true;
}

static bool
gap_start_convert (gap_start_t const	*property,
		   ccss_property_type_t	 target,
		   void			*value)
{
	g_return_val_if_fail (property && value, false);

	switch (target) {
	case CCSS_PROPERTY_TYPE_DOUBLE:
		* (double *) value = property->start;
		return true;
	case CCSS_PROPERTY_TYPE_STRING:
		* (char **) value = g_strdup_printf ("%f", property->start);
		return true;
	default:
		g_assert_not_reached ();
		return false;
	}

	return false;
}

static bool
gap_width_convert (gap_width_t const	*property,
		   ccss_property_type_t	 target,
		   void			*value)
{
	g_return_val_if_fail (property && value, false);

	switch (target) {
	case CCSS_PROPERTY_TYPE_DOUBLE:
		* (double *) value = property->width;
		return true;
	case CCSS_PROPERTY_TYPE_STRING:
		* (char **) value = g_strdup_printf ("%f", property->width);
		return true;
	default:
		g_assert_not_reached ();
		return false;
	}

	return false;
}

static ccss_property_class_t const _ptable[] = {
    {
	.name = "ccss-gap-side",
	.create = NULL,
	.destroy = (ccss_property_destroy_f) gap_destroy_nop,
	.convert = (ccss_property_convert_f) gap_side_convert,
	.factory = NULL,
	.inherit = NULL,
	.serialize = NULL
    }, {
	.name = "ccss-gap-start",
	.create = NULL,
	.destroy = (ccss_property_destroy_f) gap_destroy_nop,
	.convert = (ccss_property_convert_f) gap_start_convert,
	.factory = NULL,
	.inherit = NULL,
	.serialize = NULL
    }, {
	.name = "ccss-gap-width",
	.create = NULL,
	.destroy = (ccss_property_destroy_f) gap_destroy_nop,
	.convert = (ccss_property_convert_f) gap_width_convert,
	.factory = NULL,
	.inherit = NULL,
	.serialize = NULL
    }, {
	.name = NULL
    }
};

static ccss_property_class_t const *
peek_property_class (char const *property_name)
{
	for (unsigned int i = 0; i < G_N_ELEMENTS (_ptable); i++) {
		if (0 == g_strcmp0 (property_name, _ptable[i].name))
			return &_ptable[i];
	}

	g_return_val_if_fail (0, NULL);

	return NULL;
}

