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

#include "ccss/ccss-style-priv.h"
#include "ccss-gtk-style.h"

/**
 * ccss_style_draw_gap:
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
ccss_style_draw_gap (ccss_style_t	const	*self, 
		    cairo_t		*cr, 
		    int32_t		 x,
		    int32_t		 y,
		    int32_t		 width,
		    int32_t		 height, 
		    GtkPositionType	 gap_side,
		    int32_t		 gap_start,
		    int32_t		 gap_width)
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

	ccss_style_gather_outline (self, &bottom, &left, &right, &top,
				   &bl, &br, &tl, &tr);

	ccss_style_gather_background (self, &bg_attachment, &bg_color, &bg_image, 
				      &bg_position, &bg_repeat, &bg_size);


	/* The rounding radii will have to be adjusted for certain gap
	 * positions, so we work on a copied set of them. */
	bottom_left = *bl;
	bottom_right = *br;
	top_left = *tl;
	top_right = *tr;

	switch (gap_side) {
	case GTK_POS_LEFT:
		if (bottom_left.radius > height - gap_start - gap_width)
			bottom_left.radius = height - gap_start - gap_width;
		if (top_left.radius > gap_start)
			top_left.radius = gap_start;
		break;
	case GTK_POS_TOP:
		if (top_left.radius > gap_start)
			top_left.radius = gap_start;
		if (top_right.radius > width - gap_start - gap_width)
			top_right.radius = width - gap_start - gap_width;
		break;
	case GTK_POS_RIGHT:
		if (top_right.radius > gap_start)
			top_right.radius = gap_start;
		if (bottom_right.radius > height - gap_start - gap_width)
			bottom_right.radius = height - gap_start - gap_width;
		break;
	case GTK_POS_BOTTOM:
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
	case GTK_POS_LEFT:
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
	case GTK_POS_TOP:
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
	case GTK_POS_RIGHT:
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
	case GTK_POS_BOTTOM:
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
 * ccss_style_draw_polygon:
 * @self:	a ccss_style_t.
 * @cr:		the target to draw onto.
 * @points:	an array of #GdkPoint<!-- -->s.
 * @n_points:	length of @points.
 * @fill:	%true if the polygon should be filled.
 *
 * Draw a rectangle with gap using this style instance.
 **/
void
ccss_style_draw_polygon (ccss_style_t const	*self, 
			cairo_t			*cr, 
			GdkPoint		*points,
			int32_t			 n_points,
			bool		 fill)
{

}

