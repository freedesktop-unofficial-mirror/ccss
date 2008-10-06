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
#include "ccss-style.h"

static const ccss_background_attachment_t const _bg_attachment = {
	.spec = CCSS_PROPERTY_SPEC_SET,
	.attachment = CCSS_BACKGROUND_SCROLL
};

static const ccss_color_t const _bg_color = {
	.spec = CCSS_PROPERTY_SPEC_SET,
	.red = 1.,
	.green = 1.,
	.blue = 1.
};

static const ccss_background_image_t const _bg_image = {
	.spec = CCSS_PROPERTY_SPEC_NONE
};

static const ccss_background_position_t const _bg_position = {
	.spec = CCSS_PROPERTY_SPEC_SET,
	.hpos = { CCSS_POSITION_LENGTH, 0 },
	.vpos = { CCSS_POSITION_LENGTH, 0 }
};

static const ccss_background_repeat_t const _bg_repeat = {
	.spec = CCSS_PROPERTY_SPEC_SET,
	.repeat = CCSS_BACKGROUND_REPEAT
};

static const ccss_background_size_t const _bg_size = {
	.spec = CCSS_PROPERTY_SPEC_SET,
	.width = { CCSS_POSITION_AUTO, 0 },
	.height = { CCSS_POSITION_AUTO, 0 }
};

static const ccss_color_t const _color = {
	.spec = CCSS_PROPERTY_SPEC_SET,
	.red = 0.,
	.green = 0.,
	.blue = 0.
};

static const ccss_style_t const _default_style = {
	.bg_attachment = &_bg_attachment,
	.bg_color = &_bg_color,
	.bg_image = &_bg_image,
	.bg_position = &_bg_position,
	.bg_repeat = &_bg_repeat,
	.bg_size = &_bg_size,
	.left = {
		.width = 0,
		.width_spec = CCSS_PROPERTY_SPEC_SET,
		.style = CCSS_BORDER_STYLE_SOLID,
		.style_spec = CCSS_PROPERTY_SPEC_SET,
		.color = { CCSS_PROPERTY_SPEC_SET, 0, 0, 0 } },
	.left_top = {
		.radius = 0,
		.spec = CCSS_PROPERTY_SPEC_SET
	},
	.top = {
		.width = 0,
		.width_spec = CCSS_PROPERTY_SPEC_SET,
		.style = CCSS_BORDER_STYLE_SOLID,
		.style_spec = CCSS_PROPERTY_SPEC_SET,
		.color = { CCSS_PROPERTY_SPEC_SET, 0, 0, 0 } },
	.top_right = {
		.radius = 0,
		.spec = CCSS_PROPERTY_SPEC_SET
	},
	.right = {
		.width = 0,
		.width_spec = CCSS_PROPERTY_SPEC_SET,
		.style = CCSS_BORDER_STYLE_SOLID,
		.style_spec = CCSS_PROPERTY_SPEC_SET,
		.color = { CCSS_PROPERTY_SPEC_SET, 0, 0, 0 } },
	.right_bottom = {
		.radius = 0,
		.spec = CCSS_PROPERTY_SPEC_SET
	},
	.bottom = {
		.width = 0,
		.width_spec = CCSS_PROPERTY_SPEC_SET,
		.style = CCSS_BORDER_STYLE_SOLID,
		.style_spec = CCSS_PROPERTY_SPEC_SET,
		.color = { CCSS_PROPERTY_SPEC_SET, 0, 0, 0 } },
	.bottom_left = {
		.radius = 0,
		.spec = CCSS_PROPERTY_SPEC_SET
	},
	.color = &_color,

	.viewport_x = 0,
	.viewport_y = 0,
	.viewport_width = 0,
	.viewport_height = 0
};

/**
 * ccss_style_init:
 * @self:	a ccss_style_t.
 *
 * Reset the object's internal state.
 **/
void
ccss_style_init (ccss_style_t *self)
{
	memcpy (self, &_default_style, sizeof (*self));
}

/**
 * ccss_style_draw_horizontal_line:
 * @self:	a ccss_style_t.
 * @cr:		the target to draw onto.
 * @x1:		the starting x coordinate.
 * @x2:		the ending x coordinate.
 * @y:		the y coordinate.
 *
 * Draw a horizontal line using this style instance. This takes the `top'
 * border from the CSS, if specified.
 **/
void
ccss_style_draw_line (ccss_style_t const	*self,
		     cairo_t		*cr, 
		     int32_t 		 x1,
		     int32_t 		 x2,
		     int32_t 		 y1,
		     int32_t		 y2)
{
	ccss_border_stroke_t const	*stroke;
	double				 off;

	if (y1 == y2) {
		/* Horizontal: try to use top, then bottom border. 
		 * Fallback based on flags is flawed, however. */
		stroke = CCSS_BORDER_STROKE_IS_SET (self->top) ? &self->top : &self->bottom;
		if (stroke) {
			/* Unlike borders, lines are not drawn inside the box, 
			 * account for that. */
			off = stroke->width / 2.;

			ccss_border_draw (NULL, NULL, stroke, NULL,
					 NULL, NULL, NULL, NULL,
					 CCSS_BORDER_VISIBILITY_SHOW_ALL,
					 cr, x1, y1 - off, x2 - x1, 0);
		}
	} else {
		/* Vertical: try to use left, then right border.
		 * Fallback based on flags is flawed, however. */
		stroke = CCSS_BORDER_STROKE_IS_SET (self->left) ? &self->left : &self->right;
		if (stroke) {
			/* Unlike borders, lines are not drawn inside the box, 
			 * account for that. */
			off = stroke->width / 2.;

			ccss_border_draw (stroke, NULL, NULL, NULL,
					 NULL, NULL, NULL, NULL,
					 CCSS_BORDER_VISIBILITY_SHOW_ALL,
					 cr, x1 - off, y1, 0, y2 - y1);
		}
	}

}

/**
 * ccss_style_draw_outline:
 * @self:	a ccss_style_t.
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
ccss_style_draw_outline (ccss_style_t const	*self, 
			cairo_t			*cr, 
			int32_t			 x,
			int32_t			 y,
			int32_t			 width,
			int32_t			 height)
{
	ccss_border_draw (&self->left, &self->left_top, 
			 &self->top, &self->top_right,
			 &self->right, &self->right_bottom,
			 &self->bottom, &self->bottom_left,
			 CCSS_BORDER_VISIBILITY_SHOW_ALL,
			 cr, x, y, width, height);
}

/**
 * ccss_style_draw_rectangle:
 * @self:	a ccss_style_t.
 * @cr:		the target to draw onto.
 * @x:		the starting x coordinate.
 * @y:		the starting y coordinate.
 * @width:	width of the outline to draw.
 * @height:	height of the outline to draw.
 *
 * Draw a rectangle using this style instance.
 **/
void
ccss_style_draw_rectangle (ccss_style_t const	*self,
			  cairo_t		*cr, 
			  int32_t		 x,
			  int32_t		 y,
			  int32_t		 width,
			  int32_t		 height)
{
	int32_t l, t, w, h;

	ccss_border_path (&self->left, &self->left_top, 
			 &self->top, &self->top_right,
			 &self->right, &self->right_bottom,
			 &self->bottom, &self->bottom_left,
			 cr, x, y, width, height);

	/* FIXME: background size is calculated against allocation
	 * when using `fixed'. */
	if (CCSS_BACKGROUND_FIXED == self->bg_attachment->attachment) {
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

	ccss_background_fill (self->bg_attachment, self->bg_color,
			     self->bg_image, self->bg_position, self->bg_repeat,
			     self->bg_size, cr, l, t, w, h);

	cairo_new_path (cr);

	ccss_border_draw (&self->left, &self->left_top, 
			 &self->top, &self->top_right,
			 &self->right, &self->right_bottom,
			 &self->bottom, &self->bottom_left,
			 CCSS_BORDER_VISIBILITY_SHOW_ALL,
			 cr, x, y, width, height);
}

bool
ccss_style_get_color (ccss_style_t const	*self,
		     double		*red,
		     double		*green,
		     double		*blue)
{
	g_return_val_if_fail (self, false);

	if (NULL == self->color) {
		return false;
	}

	if (red)
		*red = self->color->red;

	if (green)
		*green = self->color->green;

	if (blue)
		*blue = self->color->blue;

	return true;
}

bool
ccss_style_get_background_color (ccss_style_t const	*self,
				double			*red,
				double			*green,
				double			*blue)
{
	g_return_val_if_fail (self, false);

	if (NULL == self->bg_color) {
		return false;
	}

	if (red)
		*red = self->bg_color->red;

	if (green)
		*green = self->bg_color->green;

	if (blue)
		*blue = self->bg_color->blue;

	return true;
}

bool
ccss_style_get_border_color (ccss_style_t const	*self,
			    double		*red,
			    double		*green,
			    double		*blue)
{
	g_return_val_if_fail (self, false);

	/* FIXME hardcoding "left" for now. */

	if (red)
		*red = self->left.color.red;

	if (green)
		*green = self->left.color.green;

	if (blue)
		*blue = self->left.color.blue;

	return true;
}

void
ccss_style_set_viewport (ccss_style_t	*self,
			 int32_t	 x,
			 int32_t	 y,
			 int32_t	 width,
			 int32_t	 height)
{
	self->viewport_x = x;
	self->viewport_y = y;
	self->viewport_width = width;
	self->viewport_height = height;
}


#ifdef CCSS_DEBUG

/**
 * ccss_style_dump:
 * @self:	a ccss_style_t.
 *
 * Print informations about the internal state of this object.
 **/
void
ccss_style_dump (ccss_style_t const *self)
{
	if (self->bg_color)
		ccss_color_dump (self->bg_color);
	if (self->bg_image)
		ccss_background_image_dump (self->bg_image);
	ccss_border_stroke_dump (&self->left);
	ccss_border_stroke_dump (&self->top);
	ccss_border_stroke_dump (&self->right);
	ccss_border_stroke_dump (&self->bottom);
}

#endif /* CCSS_DEBUG */

