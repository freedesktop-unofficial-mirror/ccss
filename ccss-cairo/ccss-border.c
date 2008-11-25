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

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "ccss-border.h"
#include "config.h"

#define STROKE_IS_SET(stroke_)						       \
		(stroke_ && 						       \
		 stroke_->color->base.state == CCSS_PROPERTY_STATE_SET &&      \
		 stroke_->style->base.state == CCSS_PROPERTY_STATE_SET &&      \
		 stroke_->width->base.state == CCSS_PROPERTY_STATE_SET)

#define JOIN_IS_SET(join_)						       \
		(join_ &&						       \
		 join_->base.state == CCSS_PROPERTY_STATE_SET)

#define _PI (_pi ())

static double
_pi (void)
{
	static double pi = 0.;

	if (pi == 0.) {
		pi = acos (-1.0);
	}

	return pi;
}

/**
 * line:
 * @stroke:	a #ccss_border_stroke_t or %NULL.
 *
 **/
static void
line (ccss_border_stroke_t const	*stroke,
      cairo_t			*cr,
      double			 x1,
      double			 y1,
      double			 x2,
      double			 y2)
{
	cairo_line_to (cr, x2, y2);
}

static void
draw_none_line (ccss_border_stroke_t const	*stroke,
		cairo_t				*cr,
		double				 x1,
		double				 y1,
		double				 x2,
		double				 y2)
{
	cairo_move_to (cr, x1, y1);
}

static void
draw_dotted_line (ccss_border_stroke_t const	*stroke,
		  cairo_t			*cr,
		  double			 x1,
		  double			 y1,
		  double			 x2,
		  double			 y2)
{
	double dash_len;

	dash_len = stroke->width->width;

	cairo_save (cr);

	cairo_move_to (cr, x1, y1);
	line (stroke, cr, x1, y1, x2, y2);

	cairo_set_dash (cr, &dash_len, 1, 0);
	cairo_set_line_width (cr, stroke->width->width);
	cairo_set_source_rgb (cr, stroke->color->red, stroke->color->green, 
				stroke->color->blue);
	cairo_stroke (cr);

	cairo_restore (cr);
}

static void
draw_dashed_line (ccss_border_stroke_t const	*stroke,
		  cairo_t			*cr,
		  double			 x1,
		  double			 y1,
		  double			 x2,
		  double			 y2)
{
	double dashes[2];

	dashes[0] = stroke->width->width * 3;
	dashes[1] = stroke->width->width * 3;

	cairo_save (cr);

	cairo_move_to (cr, x1, y1);
	line (stroke, cr, x1, y1, x2, y2);

	cairo_set_dash (cr, dashes, G_N_ELEMENTS (dashes), 0);
	cairo_set_line_width (cr, stroke->width->width);
	cairo_set_source_rgb (cr, stroke->color->red, stroke->color->green, 
				stroke->color->blue);
	cairo_stroke (cr);

	cairo_restore (cr);
}

static void
draw_solid_line (ccss_border_stroke_t const	*stroke,
		 cairo_t			*cr,
		 double				 x1,
		 double				 y1,
		 double				 x2,
		 double				 y2)
{
	cairo_save (cr);

	cairo_move_to (cr, x1, y1);
	line (stroke, cr, x1, y1, x2, y2);

	cairo_set_line_width (cr, stroke->width->width);
	cairo_set_source_rgb (cr, stroke->color->red, stroke->color->green, 
				stroke->color->blue);
	cairo_stroke (cr);

	cairo_restore (cr);
}

typedef void (*line_draw_f) (ccss_border_stroke_t const *, cairo_t *, 
			     double, double, double, double);

static line_draw_f
get_line_draw_func (ccss_border_stroke_t const	*stroke,
		    bool			 path_only)
{
	if (path_only) {
		return line;
	}

	if (CCSS_PROPERTY_STATE_INVALID == stroke->style->base.state ||
	    CCSS_PROPERTY_STATE_NONE == stroke->style->base.state) {
		return draw_none_line;
	}

	switch (stroke->style->style) {
	case CCSS_BORDER_STYLE_HIDDEN: 
		g_warning ("CCSS_BORDER_STYLE_HIDDEN not implemented");
		return draw_none_line;
	case CCSS_BORDER_STYLE_DOTTED:
		return draw_dotted_line;
	case CCSS_BORDER_STYLE_DASHED:
		return draw_dashed_line;
	case CCSS_BORDER_STYLE_SOLID:
		return draw_solid_line;
	case CCSS_BORDER_STYLE_DOUBLE:
		g_warning ("CCSS_BORDER_STYLE_DOUBLE not implemented");
	case CCSS_BORDER_STYLE_GROOVE:
		g_warning ("CCSS_BORDER_STYLE_GROOVE not implemented");
	case CCSS_BORDER_STYLE_RIDGE:
		g_warning ("CCSS_BORDER_STYLE_RIDGE not implemented");
	case CCSS_BORDER_STYLE_INSET:
		g_warning ("CCSS_BORDER_STYLE_INSET not implemented");
	case CCSS_BORDER_STYLE_OUTSET:
		g_warning ("CCSS_BORDER_STYLE_OUTSET not implemented");
	default:
		g_assert_not_reached ();
		return draw_solid_line;
	}
}

static void
join (ccss_border_stroke_t const	*before,
      ccss_border_stroke_t const	*after,
      cairo_t			*cr,
      double			 xc,
      double			 yc,
      double			 radius,
      double			 angle1,
      double			 angle2)
{
	cairo_arc (cr, xc, yc, radius, angle1, angle2);
}

static void
draw_none_join (ccss_border_stroke_t const	*before,
		ccss_border_stroke_t const	*after,
		cairo_t				*cr,
		double				 xc,
		double				 yc,
		double				 radius,
		double				 angle1,
		double				 angle2)
{
}

static void
draw_solid_join (ccss_border_stroke_t const	*before,
		 ccss_border_stroke_t const	*after,
		 cairo_t			*cr,
		 double				 xc,
		 double				 yc,
		 double				 radius,
		 double				 angle1,
		 double				 angle2)
{
	cairo_save (cr);

	join (before, after, cr, xc, yc, radius, angle1, angle2);

	/* FIXME draw with gradient if colors are different. 
	 * Probably requires that the join has all the color information. */
	if (before) {
		cairo_set_line_width (cr, before->width->width);
		cairo_set_source_rgb (cr,
				      before->color->red, before->color->green, 
				      before->color->blue);
	} else if (after) {
		cairo_set_line_width (cr, after->width->width);
		cairo_set_source_rgb (cr,
				      after->color->red, after->color->green, 
				      after->color->blue);
	} else {
		cairo_set_line_width (cr, 1.);
		cairo_set_source_rgb (cr, 0., 0., 0.);
	}
	
	cairo_stroke (cr);

	cairo_restore (cr);
}

typedef void (*join_draw_f) (ccss_border_stroke_t const *, 
			     ccss_border_stroke_t const *, 
			     cairo_t *, 
			     double, double, double, double, double);

static join_draw_f
get_join_draw_func (ccss_border_stroke_t const	*before,
		    ccss_border_stroke_t const	*after,
		    bool			 path_only)
{
	if (path_only) {
		return join;
	}

	/* FIXME: draw a transition if only one of them is set. */
	if (!STROKE_IS_SET (before) ||
	    !STROKE_IS_SET (after)) {
		return draw_none_join;
	}

	return draw_solid_join;
}

void
ccss_border_clamp_radii (double	 x,
			double	 y,
			double	 width,
			double	 height,
			double	*left_top,
			double	*top_right,
			double	*right_bottom,
			double	*bottom_left)
{
	if (height > 0 && *left_top > height / 2.) *left_top = height / 2.;
	if (width > 0 && *left_top > width / 2.) *left_top = width / 2.;

	if (height > 0 && *top_right > height / 2.) *top_right = height / 2.;
	if (width > 0 && *top_right > width / 2.) *top_right = width / 2.;

	if (height > 0 && *right_bottom > height / 2.) *right_bottom = height / 2.;
	if (width > 0 && *right_bottom > width / 2.) *right_bottom = width / 2.;

	if (height > 0 && *bottom_left > height / 2.) *bottom_left = height / 2.;
	if (width > 0 && *bottom_left > width / 2.) *bottom_left = width / 2.;
}

static void
border (ccss_border_stroke_t const	*left,
	ccss_border_join_t const		*left_top,
	ccss_border_stroke_t const	*top,
	ccss_border_join_t const		*top_right,
	ccss_border_stroke_t const	*right,
	ccss_border_join_t const		*right_bottom,
	ccss_border_stroke_t const	*bottom,
	ccss_border_join_t const		*bottom_left,
	uint32_t			 visibility_flags, 
	cairo_t				*cr,
	double				 x,
	double				 y,
	double				 width,
	double				 height,
	bool				 path_only)
{
	line_draw_f	line_func;
	join_draw_f	join_func;
	double		xoff, yoff;
	double		x1, x2, y1, y2;
	double		xc, yc;
	double		rlt, rtr, rrb, rbl;
	bool		have_segment;

	rlt = left_top && left_top->base.state ? left_top->radius : 0;
	rtr = top_right && top_right->base.state ? top_right->radius : 0;
	rrb = right_bottom && right_bottom->base.state ? right_bottom->radius : 0;
	rbl = bottom_left && bottom_left->base.state ? bottom_left->radius : 0;
	if (!(CCSS_BORDER_ROUNDING_UNRESTRICTED & visibility_flags)) {
		ccss_border_clamp_radii (x, y, width, height, 
					&rlt, &rtr, &rrb, &rbl);
	}

	have_segment = false;

	if (left) {
		line_func = get_line_draw_func (left, path_only);
		xoff = !path_only && left ? left->width->width / 2. : 0;
		yoff = rbl;
		x1 = x + xoff;
		y1 = y + height - yoff;
		x2 = x1;
		y2 = y + rlt;

		if (path_only || !(CCSS_BORDER_VISIBILITY_HIDE_LEFT & visibility_flags)) {
			if (!have_segment)
				cairo_move_to (cr, x1, y1);
			line_func (left, cr, x1, y1, x2, y2);
			have_segment = true;
		}
	}

	if (left_top) {
		join_func = get_join_draw_func (left, top, path_only);
		xoff = !path_only && left ? left->width->width / 2. : 0;
		yoff = !path_only && top ? top->width->width / 2. : 0;
		x1 = x + xoff;
		y1 = y + rlt + yoff;
		xc = x1 + rlt;
		yc = y1;

		if (path_only || !(CCSS_BORDER_VISIBILITY_HIDE_LEFT_TOP & visibility_flags)) {
			if (!have_segment)
				cairo_move_to (cr, x1, y1);
			join_func (left, top, cr, xc, yc, rlt, _PI, 3 * _PI / 2);
			have_segment = true;
		}
	}

	if (top) {
		line_func = get_line_draw_func (top, path_only);
		xoff = rlt;
		yoff = !path_only && top ? top->width->width / 2. : 0;
		x1 = x + xoff;
		y1 = y + yoff;
		x2 = x + width - rtr;
		y2 = y1;

		if (path_only || !(CCSS_BORDER_VISIBILITY_HIDE_TOP & visibility_flags)) {
			if (!have_segment)
				cairo_move_to (cr, x1, y1);
			line_func (top, cr, x1, y1, x2, y2);
			have_segment = true;
		}
	}

	if (top_right) {
		join_func = get_join_draw_func (top, right, path_only);
		xoff = !path_only && right ? right->width->width / 2. : 0;
		yoff = !path_only && top ? top->width->width / 2. : 0;
		x1 = x + width - xoff;
		y1 = y + yoff;
		xc = x1 - rtr;
		yc = y1 + rtr;

		if (path_only || !(CCSS_BORDER_VISIBILITY_HIDE_TOP_RIGHT & visibility_flags)) {
			if (!have_segment)
				cairo_move_to (cr, x1, y1);
			join_func (top, right, cr, xc, yc, rtr, 3 * _PI / 2., 0);
			have_segment = true;
		}
	}

	if (right) {
		line_func = get_line_draw_func (right, path_only);
		xoff = !path_only && right ? right->width->width / 2. : 0;
		yoff = rtr;
		x1 = x + width - xoff;
		y1 = y + yoff;
		x2 = x1;
		y2 = y + height - rrb;

		if (path_only || !(CCSS_BORDER_VISIBILITY_HIDE_RIGHT & visibility_flags)) {
			if (!have_segment)
				cairo_move_to (cr, x1, y1);
			line_func (right, cr, x1, y1, x2, y2);
			have_segment = true;
		}
	}

	if (right_bottom) {
		join_func = get_join_draw_func (right, bottom, path_only);
		xoff = !path_only && right ? right->width->width / 2. : 0;
		yoff = !path_only && bottom ? bottom->width->width / 2. : 0;
		x1 = x + width - xoff;
		y1 = y + height - yoff;
		xc = x1 - rrb;
		yc = y1 - rrb;

		if (path_only || !(CCSS_BORDER_VISIBILITY_HIDE_RIGHT_BOTTOM & visibility_flags)) {
			if (!have_segment)
				cairo_move_to (cr, x1, y1);
			join_func (right, bottom, cr, xc, yc, rrb, 0, _PI / 2.);
			have_segment = true;
		}
	}

	if (bottom) {
		line_func = get_line_draw_func (bottom, path_only);
		xoff = rrb;
		yoff = !path_only && bottom ? bottom->width->width / 2. : 0;
		x1 = x + width - xoff;
		y1 = y + height - yoff;
		x2 = x + rbl;
		y2 = y1;

		if (path_only || !(CCSS_BORDER_VISIBILITY_HIDE_BOTTOM & visibility_flags)) {
			if (!have_segment)
				cairo_move_to (cr, x1, y1);
			line_func (bottom, cr, x1, y1, x2, y2);
			have_segment = true;
		}
	}

	if (bottom_left) {
		join_func = get_join_draw_func (bottom, left, path_only);
		xoff = !path_only && left ? left->width->width / 2. : 0;
		yoff = !path_only && bottom ? bottom->width->width / 2. : 0;
		x1 = x + xoff;
		y1 = y + height - yoff;
		xc = x1 + rbl;
		yc = y1 - rbl;

		if (path_only || !(CCSS_BORDER_VISIBILITY_HIDE_BOTTOM_LEFT & visibility_flags)) {
			if (!have_segment)
				cairo_move_to (cr, x1, y1);
			join_func (bottom, left, cr, xc, yc, rbl, _PI / 2., _PI);
			have_segment = true;
		}
	}
}

void
ccss_border_path (ccss_border_stroke_t const	*left,
		 ccss_border_join_t const	*left_top,
		 ccss_border_stroke_t const	*top,
		 ccss_border_join_t const	*top_right,
		 ccss_border_stroke_t const	*right,
		 ccss_border_join_t const	*right_bottom,
		 ccss_border_stroke_t const	*bottom,
		 ccss_border_join_t const	*bottom_left,
		 cairo_t			*cr,
		 double				 x,
		 double				 y,
		 double				 width,
		 double				 height)
{
	border (left, left_top, top, top_right, 
		right, right_bottom, bottom, bottom_left,
		CCSS_BORDER_VISIBILITY_SHOW_ALL, cr, x, y, width, height, true);

	cairo_close_path (cr);
}

void
ccss_border_draw (ccss_border_stroke_t const	*left,
		 ccss_border_join_t const	*left_top,
		 ccss_border_stroke_t const	*top,
		 ccss_border_join_t const	*top_right,
		 ccss_border_stroke_t const	*right,
		 ccss_border_join_t const	*right_bottom,
		 ccss_border_stroke_t const	*bottom,
		 ccss_border_join_t const	*bottom_left,
		 uint32_t			 visibility_flags, 
		 cairo_t			*cr,
		 double				 x,
		 double				 y,
		 double				 width,
		 double				 height)
{
	border (left, left_top, top, top_right, 
		right, right_bottom, bottom, bottom_left,
		visibility_flags, cr, x, y, width, height, false);
}

