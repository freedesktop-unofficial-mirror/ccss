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
#include <ccss/ccss-property.h>
#include <ccss/ccss-style-priv.h>
#include "ccss-background.h"
#include "ccss-border.h"
#include "ccss-border-image.h"
#include "ccss-cairo-style-priv.h"
#include "ccss-cairo-property.h"
#include "config.h"

static const ccss_background_attachment_t const _bg_attachment = {
	.base.state = CCSS_PROPERTY_STATE_SET,
	.attachment = CCSS_BACKGROUND_SCROLL
};

static const ccss_color_t const _bg_color = {
	.base.state = CCSS_PROPERTY_STATE_SET,
	.red = 1.,
	.green = 1.,
	.blue = 1.
};

static const ccss_background_image_t const _bg_image = {
	.base.state = CCSS_PROPERTY_STATE_NONE
};

static const ccss_background_position_t const _bg_position = {
	.base.state = CCSS_PROPERTY_STATE_SET,
	.hpos = { CCSS_POSITION_LENGTH, 0 },
	.vpos = { CCSS_POSITION_LENGTH, 0 }
};

static const ccss_background_repeat_t const _bg_repeat = {
	.base.state = CCSS_PROPERTY_STATE_SET,
	.repeat = CCSS_BACKGROUND_REPEAT
};

static const ccss_background_size_t const _bg_size = {
	.base.state = CCSS_PROPERTY_STATE_SET,
	.width = { CCSS_POSITION_AUTO, 0 },
	.height = { CCSS_POSITION_AUTO, 0 }
};

/* FIXME: don't draw borders by default? */
static const ccss_border_style_t const _border_style = {
	.base.state = CCSS_PROPERTY_STATE_INVALID,
	.style = CCSS_BORDER_STYLE_SOLID
};

static const ccss_border_width_t const _border_width = {
	.base.state = CCSS_PROPERTY_STATE_SET,
	.width = 1.
};

static const ccss_border_join_t const _border_join = {
	.base.state = CCSS_PROPERTY_STATE_INVALID,
	.radius = 0
};

static const ccss_color_t const _color = {
	.base.state = CCSS_PROPERTY_STATE_SET,
	.red = 0.,
	.green = 0.,
	.blue = 0.
};

static const struct {

	ccss_background_attachment_t const	*bg_attachment;
	ccss_color_t const			*bg_color;
	ccss_background_image_t const		*bg_image;
	ccss_background_position_t const	*bg_position;
	ccss_background_repeat_t const		*bg_repeat;
	ccss_background_size_t const		*bg_size;

	ccss_color_t const			*bottom_color;
	ccss_border_style_t const		*bottom_style;
	ccss_border_width_t const		*bottom_width;

	ccss_color_t const			*left_color;
	ccss_border_style_t const		*left_style;
	ccss_border_width_t const		*left_width;

	ccss_color_t const			*right_color;
	ccss_border_style_t const		*right_style;
	ccss_border_width_t const		*right_width;

	ccss_color_t const			*top_color;
	ccss_border_style_t const		*top_style;
	ccss_border_width_t const		*top_width;

	ccss_border_join_t const		*bottom_left_radius;
	ccss_border_join_t const		*bottom_right_radius;
	ccss_border_join_t const		*top_left_radius;
	ccss_border_join_t const		*top_right_radius;

	ccss_color_t const			*color;	

} _default_style = {

	.bg_attachment = &_bg_attachment,
	.bg_color = &_bg_color,
	.bg_image = &_bg_image,
	.bg_position = &_bg_position,
	.bg_repeat = &_bg_repeat,
	.bg_size = &_bg_size,

	.bottom_color = &_color,
	.bottom_style = &_border_style,
	.bottom_width = &_border_width,

	.left_color = &_color,
	.left_style = &_border_style,
	.left_width = &_border_width,

	.right_color = &_color,
	.right_style = &_border_style,
	.right_width = &_border_width,

	.top_color = &_color,
	.top_style = &_border_style,
	.top_width = &_border_width,

	.bottom_left_radius = &_border_join,
	.bottom_right_radius = &_border_join,
	.top_left_radius = &_border_join,
	.top_right_radius = &_border_join,

	.color = &_color
};

/* FIXME: maybe they should also be in `block', where the generic conversion function is? */


typedef struct {
	GQuark id;
	GQuark fallback;	
} fallback_map_t;

fallback_map_t *_fallback_map = NULL;

void
ccss_style_subsystem_init (void)
{
	fallback_map_t fm[] = {
		{ CCSS_PROPERTY_BORDER_COLOR, CCSS_PROPERTY_COLOR },

		{ CCSS_PROPERTY_BORDER_BOTTOM_COLOR, CCSS_PROPERTY_BORDER_COLOR },
		{ CCSS_PROPERTY_BORDER_BOTTOM_STYLE, CCSS_PROPERTY_BORDER_STYLE },
		{ CCSS_PROPERTY_BORDER_BOTTOM_WIDTH, CCSS_PROPERTY_BORDER_WIDTH },

		{ CCSS_PROPERTY_BORDER_LEFT_COLOR, CCSS_PROPERTY_BORDER_COLOR },
		{ CCSS_PROPERTY_BORDER_LEFT_STYLE, CCSS_PROPERTY_BORDER_STYLE },
		{ CCSS_PROPERTY_BORDER_LEFT_WIDTH, CCSS_PROPERTY_BORDER_WIDTH },

		{ CCSS_PROPERTY_BORDER_RIGHT_COLOR, CCSS_PROPERTY_BORDER_COLOR },
		{ CCSS_PROPERTY_BORDER_RIGHT_STYLE, CCSS_PROPERTY_BORDER_STYLE },
		{ CCSS_PROPERTY_BORDER_RIGHT_WIDTH, CCSS_PROPERTY_BORDER_WIDTH },

		{ CCSS_PROPERTY_BORDER_TOP_COLOR, CCSS_PROPERTY_BORDER_COLOR },
		{ CCSS_PROPERTY_BORDER_TOP_STYLE, CCSS_PROPERTY_BORDER_STYLE },
		{ CCSS_PROPERTY_BORDER_TOP_WIDTH, CCSS_PROPERTY_BORDER_WIDTH },

		{ CCSS_PROPERTY_BORDER_TOP_LEFT_RADIUS,		CCSS_PROPERTY_BORDER_RADIUS },
		{ CCSS_PROPERTY_BORDER_TOP_RIGHT_RADIUS,	CCSS_PROPERTY_BORDER_RADIUS },
		{ CCSS_PROPERTY_BORDER_BOTTOM_RIGHT_RADIUS,	CCSS_PROPERTY_BORDER_RADIUS },
		{ CCSS_PROPERTY_BORDER_BOTTOM_LEFT_RADIUS,	CCSS_PROPERTY_BORDER_RADIUS },

		{ 0, 0 }
	};

	g_assert (NULL == _fallback_map);

	_fallback_map = g_memdup (&fm, sizeof (fm));
}

void
ccss_style_subsystem_shutdown (void)
{
	g_assert (_fallback_map != NULL);

	g_free (_fallback_map), _fallback_map = NULL;
}

/* Look up a property, take fallback into account. */
static ccss_property_base_t const *
lookup_property_r (ccss_style_t const	*self,
		   GQuark		 property_id)
{
	ccss_property_base_t const *property;

	property = (ccss_property_base_t const *) 
			g_hash_table_lookup (self->properties,
					     (gpointer) property_id);
	if (property) {
		return property;
	}

	for (unsigned int i = 0; _fallback_map[i].id != 0; i++) {
		if (property_id == _fallback_map[i].id) {
			return lookup_property_r (self, _fallback_map[i].fallback);
		}
	}

	return NULL;
}

/**
 * ccss_cairo_style_draw_line:
 * @self:	a #ccss_style_t.
 * @cr:		the target to draw onto.
 * @x1:		the starting x coordinate.
 * @x2:		the ending x coordinate.
 * @y1:		the starting y coordinate.
 * @y2:		the ending y coordinate.
 *
 * Draw a line using the given style instance.
 **/
void
ccss_cairo_style_draw_line (ccss_style_t const	*self,
			    cairo_t		*cr, 
			    int			 x1,
			    int			 x2,
			    int			 y1,
			    int			 y2)
{
	ccss_border_stroke_t		 stroke;
	double				 off;

	stroke.color = (ccss_color_t *) lookup_property_r (self,
						CCSS_PROPERTY_BORDER_COLOR);
	if (NULL == stroke.color) {
		stroke.color = _default_style.top_color;
	}

	stroke.style = (ccss_border_style_t *) lookup_property_r (self,
						CCSS_PROPERTY_BORDER_STYLE);
	if (NULL == stroke.style) {
		stroke.style = _default_style.top_style;
	}

	stroke.width = (ccss_border_width_t *) lookup_property_r (self,
						CCSS_PROPERTY_BORDER_WIDTH);
	if (NULL == stroke.width) {
		stroke.width = _default_style.top_width;
	}

	/* Unlike borders, lines are not drawn inside the box, 
	 * account for that. */
	off = stroke.width->width / 2.;

	if (y1 == y2) {
		ccss_border_draw (NULL, NULL, &stroke, NULL,
				 NULL, NULL, NULL, NULL,
				 CCSS_BORDER_VISIBILITY_SHOW_ALL,
				 cr, x1, y1 - off, x2 - x1, 0);
	} else {
		ccss_border_draw (&stroke, NULL, NULL, NULL,
				 NULL, NULL, NULL, NULL,
				 CCSS_BORDER_VISIBILITY_SHOW_ALL,
				 cr, x1 - off, y1, 0, y2 - y1);
	}
}

static void
gather_stroke (ccss_style_t const		*self,
	       GQuark				 color_prop,
	       ccss_color_t const		*color_fallback,
	       GQuark				 style_prop,
	       ccss_border_style_t const	*style_fallback,
	       GQuark				 width_prop,
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
	     GQuark			 prop,
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
		CCSS_PROPERTY_BORDER_BOTTOM_COLOR, _default_style.bottom_color, 
		CCSS_PROPERTY_BORDER_BOTTOM_STYLE, _default_style.bottom_style, 
		CCSS_PROPERTY_BORDER_BOTTOM_WIDTH, _default_style.bottom_width, 
		bottom);

	gather_stroke (self, 
		CCSS_PROPERTY_BORDER_LEFT_COLOR, _default_style.left_color, 
		CCSS_PROPERTY_BORDER_LEFT_STYLE, _default_style.left_style, 
		CCSS_PROPERTY_BORDER_LEFT_WIDTH, _default_style.left_width, 
		left);

	gather_stroke (self, 
		CCSS_PROPERTY_BORDER_RIGHT_COLOR, _default_style.right_color, 
		CCSS_PROPERTY_BORDER_RIGHT_STYLE, _default_style.right_style, 
		CCSS_PROPERTY_BORDER_RIGHT_WIDTH, _default_style.right_width, 
		right);

	gather_stroke (self, 
		CCSS_PROPERTY_BORDER_TOP_COLOR, _default_style.top_color, 
		CCSS_PROPERTY_BORDER_TOP_STYLE, _default_style.top_style, 
		CCSS_PROPERTY_BORDER_TOP_WIDTH, _default_style.top_width, 
		top);

	*bottom_left = gather_join (self, CCSS_PROPERTY_BORDER_BOTTOM_LEFT_RADIUS,
				   _default_style.bottom_left_radius);

	*bottom_right = gather_join (self, CCSS_PROPERTY_BORDER_BOTTOM_RIGHT_RADIUS,
				    _default_style.bottom_right_radius);

	*top_left = gather_join (self, CCSS_PROPERTY_BORDER_TOP_LEFT_RADIUS,
			        _default_style.top_left_radius);

	*top_right = gather_join (self, CCSS_PROPERTY_BORDER_TOP_RIGHT_RADIUS,
				 _default_style.top_right_radius);

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
		*bg_attachment = _default_style.bg_attachment;

	*bg_color = (ccss_color_t const *)
		g_hash_table_lookup (
			self->properties,
			(gpointer) CCSS_PROPERTY_BACKGROUND_COLOR);
	if (!*bg_color)
		*bg_color = _default_style.bg_color;

	*bg_image = (ccss_background_image_t const *)
		g_hash_table_lookup (
			self->properties,
			(gpointer) CCSS_PROPERTY_BACKGROUND_IMAGE);
	if (!*bg_image)
		*bg_image = _default_style.bg_image;

	*bg_position = (ccss_background_position_t const *)
		g_hash_table_lookup (
			self->properties,
			(gpointer) CCSS_PROPERTY_BACKGROUND_POSITION);
	if (!*bg_position)
		*bg_position = _default_style.bg_position;

	*bg_repeat = (ccss_background_repeat_t const *)
		g_hash_table_lookup (
			self->properties,
			(gpointer) CCSS_PROPERTY_BACKGROUND_REPEAT);
	if (!*bg_repeat)
		*bg_repeat = _default_style.bg_repeat;

	*bg_size = (ccss_background_size_t const *)
		g_hash_table_lookup (
			self->properties,
			(gpointer) CCSS_PROPERTY_BACKGROUND_SIZE);
	if (!*bg_size)
		*bg_size = _default_style.bg_size;
}

/**
 * ccss_cairo_style_draw_outline:
 * @self:	a #ccss_style_t.
 * @cr:		the target to draw onto.
 * @x:		the starting x coordinate.
 * @y:		the starting y coordinate.
 * @width:	width of the outline to draw.
 * @height:	height of the outline to draw.
 *
 * Draw an outline using this style instance. Information about how to draw
 * this style's background is diregarded.
 **/
void
ccss_cairo_style_draw_outline (ccss_style_t const	*self,
			       cairo_t			*cr, 
			       int			 x,
			       int			 y,
			       int			 width,
			       int			 height)
{
	ccss_border_stroke_t		 bottom, left, right, top;
	ccss_border_join_t const	*bottom_left;
	ccss_border_join_t const	*bottom_right;
	ccss_border_join_t const	*top_left;
	ccss_border_join_t const	*top_right;

	gather_outline (self, &bottom, &left, &right, &top,
			&bottom_left, &bottom_right, &top_left, &top_right);

	ccss_border_draw (&left, top_left, 
			  &top, top_right,
			  &right, bottom_right,
			  &bottom, bottom_left,
			  CCSS_BORDER_VISIBILITY_SHOW_ALL,
			  cr, x, y, width, height);
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
				 int			 x,
				 int			 y,
				 int			 width,
				 int			 height)
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

	int32_t l, t, w, h;

	gather_outline (self, &bottom, &left, &right, &top,
			&bottom_left, &bottom_right, &top_left, &top_right);

	gather_background (self, &bg_attachment, &bg_color, &bg_image, 
			   &bg_position, &bg_repeat, &bg_size);

	ccss_border_path (&left, top_left, 
			  &top, top_right,
			  &right, bottom_right,
			  &bottom, bottom_left,
			  cr, x, y, width, height);

	/* FIXME: background size is calculated against allocation
	 * when using `fixed'. */
	if (CCSS_BACKGROUND_FIXED == bg_attachment->attachment) {
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

	ccss_background_fill (bg_attachment, bg_color, bg_image, bg_position, 
			      bg_repeat, bg_size, cr, l, t, w, h);

	cairo_new_path (cr);

	/* PONDERING: should border-image vs. border be resolved at style application time, 
	 * i.e. should a higher-priority border override border-image? */
	border_image = 	(ccss_border_image_t const *) 
		g_hash_table_lookup (
			self->properties,
			(gpointer) CCSS_PROPERTY_BORDER_IMAGE);

	if (border_image) {

		ccss_border_image_draw (border_image, cr,
					x, y, width, height);

	} else {
		ccss_border_draw (&left, top_left, 
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
					  int				 x,
					  int				 y,
					  int				 width,
					  int				 height, 
					  ccss_cairo_gap_side_t		 gap_side,
					  int				 gap_start,
					  int				 gap_width)
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

	ccss_border_join_t bottom_left;
	ccss_border_join_t bottom_right;
	ccss_border_join_t top_left;
	ccss_border_join_t top_right;
	int32_t l, t, w, h;

	gather_outline (self, &bottom, &left, &right, &top,
			&bl, &br, &tl, &tr);

	gather_background (self, &bg_attachment, &bg_color, &bg_image, 
			   &bg_position, &bg_repeat, &bg_size);


	/* The rounding radii will have to be adjusted for certain gap
	 * positions, so we work on a copied set of them. */
	bottom_left = *bl;
	bottom_right = *br;
	top_left = *tl;
	top_right = *tr;

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

	ccss_border_clamp_radii (x, y, width, height,
				&top_left.radius, &top_right.radius,
				&bottom_right.radius, &bottom_left.radius);

	ccss_border_path (&left, &top_left, 
			 &top, &top_right,
			 &right, &bottom_right,
			 &bottom, &bottom_left,
			 cr, x, y, width, height);

	/* FIXME: background size is calculated against allocation
	 * when using `fixed'. */
	if (CCSS_BACKGROUND_FIXED == bg_attachment->attachment) {
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

	ccss_background_fill (bg_attachment, bg_color, bg_image, bg_position, 
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
				ccss_border_draw (&left, NULL, NULL, NULL,
					 NULL, NULL, NULL, &bottom_left,
					 CCSS_BORDER_VISIBILITY_HIDE_BOTTOM_LEFT |
					 CCSS_BORDER_ROUNDING_UNRESTRICTED,
					 cr, x, y + gap_start + gap_width - 1, 
					 0, height - gap_start - gap_width);
			}
			/* Rounding reaches until start of gap? */
			if (top_left.radius < gap_start) {
				ccss_border_draw (&left, &top_left, NULL, NULL,
					 NULL, NULL, NULL, NULL,
					 CCSS_BORDER_VISIBILITY_HIDE_LEFT_TOP |
					 CCSS_BORDER_ROUNDING_UNRESTRICTED,
					 cr, x, y, 0, gap_start + 1);
			}
		}
		ccss_border_draw (&left, &top_left,
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
				ccss_border_draw (NULL, &top_left, &top, NULL,
					 NULL, NULL, NULL, NULL,
					 CCSS_BORDER_VISIBILITY_HIDE_LEFT_TOP |
					 CCSS_BORDER_ROUNDING_UNRESTRICTED,
					 cr, x, y, gap_start + 1, 0);
			}
			/* Rounding reaches until start of gap?
			 * With Gtk+ the portion following the gap might not be visible. */
			if (top_right.radius < width - gap_start - gap_width &&
			    gap_start + gap_width < width) {
				ccss_border_draw (NULL, NULL, &top, &top_right,
					 NULL, NULL, NULL, NULL,
					 CCSS_BORDER_VISIBILITY_HIDE_TOP_RIGHT |
					 CCSS_BORDER_ROUNDING_UNRESTRICTED,
					 cr, x + gap_start + gap_width - 1, y, 
					 width - gap_start - gap_width, 0);
			}
		}
		ccss_border_draw (&left, &top_left, 
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
				ccss_border_draw (NULL, NULL, NULL, &top_right,
					 &right, NULL, NULL, NULL,
					 CCSS_BORDER_VISIBILITY_HIDE_TOP_RIGHT |
					 CCSS_BORDER_ROUNDING_UNRESTRICTED,
					 cr, x + width, y, 0, gap_start + 1);
			}
			/* Rounding reaches until start of gap?
			 * With Gtk+ the portion following the gap might not be visible. */
			if (bottom_right.radius < height - gap_start - gap_width &&
			    gap_start + gap_width < height) {
				ccss_border_draw (NULL, NULL, NULL, NULL,
					 &right, &bottom_right, NULL, NULL,
					 CCSS_BORDER_VISIBILITY_HIDE_RIGHT_BOTTOM |
					 CCSS_BORDER_ROUNDING_UNRESTRICTED,
					 cr, x + width, y + gap_start + gap_width - 1, 
					 0, height - gap_start - gap_width);
			}
		}
		ccss_border_draw (&left, &top_left,
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
				ccss_border_draw (NULL, NULL, NULL, NULL,
					 NULL, &bottom_right, &bottom, NULL,
					 CCSS_BORDER_VISIBILITY_HIDE_RIGHT_BOTTOM |
					 CCSS_BORDER_ROUNDING_UNRESTRICTED,
					 cr, x + gap_start + gap_width - 1, y + height,
					 width - gap_start - gap_width, 0);
			}
			/* Rounding reaches until start of gap? */
			if (bottom_left.radius < gap_start) {
				ccss_border_draw (NULL, NULL, NULL, NULL,
					 NULL, NULL, &bottom, &bottom_left,
					 CCSS_BORDER_VISIBILITY_HIDE_BOTTOM_LEFT |
					 CCSS_BORDER_ROUNDING_UNRESTRICTED,
					 cr, x, y + height, gap_start + 1, 0);
			}
		}
		ccss_border_draw (&left, &top_left,
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
	GQuark				 property_id;
	ccss_property_base_t const	*property;

	g_return_val_if_fail (self && property_name && value, false);

	property_id = g_quark_try_string (property_name);
	if (0 == property_id) {
		/* Property unknown, no need to look up. */
		return false;
	}

	property = lookup_property_r (self, property_id);
	if (NULL == property)
		return false;

	/* Have conversion function? */
	g_return_val_if_fail (property->property_class, false);
	if (NULL == property->property_class->property_convert) {
		return false;
	}

	return property->property_class->property_convert (property,
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
	GQuark				 property_id;
	ccss_property_base_t const	*property;

	g_return_val_if_fail (self && property_name && value, false);

	property_id = g_quark_try_string (property_name);
	if (0 == property_id) {
		/* Property unknown, no need to look up. */
		return false;
	}

	property = lookup_property_r (self, property_id);
	if (NULL == property)
		return false;

	/* Have conversion function? */
	g_return_val_if_fail (property->property_class, false);
	if (NULL == property->property_class->property_convert) {
		return false;
	}

	return property->property_class->property_convert (property,
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
	GQuark property_id;

	g_return_val_if_fail (self && property_name && property, false);

	property_id = g_quark_try_string (property_name);
	if (0 == property_id) {
		/* Property unknown, no need to look up. */
		return false;
	}

	*property = lookup_property_r (self, property_id);

	return (bool) *property;
}

