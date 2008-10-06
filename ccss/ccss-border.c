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
#include "ccss-border.h"

/*
 * Remember which properties were set explicitely, e.g. using "border-top", 
 * so it's not overwritten by a subsequent "border" property.
 */
enum {
	CCSS_BORDER_FLAGS_COMMON_WIDTH	= 1 << 0,
	CCSS_BORDER_FLAGS_SPECIFIC_WIDTH	= 1 << 1,
	CCSS_BORDER_FLAGS_WIDTH_MASK	= CCSS_BORDER_FLAGS_COMMON_WIDTH |
					  CCSS_BORDER_FLAGS_SPECIFIC_WIDTH,

	CCSS_BORDER_FLAGS_COMMON_STYLE	= 1 << 2,
	CCSS_BORDER_FLAGS_SPECIFIC_STYLE = 1 << 3,
	CCSS_BORDER_FLAGS_STYLE_MASK	= CCSS_BORDER_FLAGS_COMMON_STYLE |
					  CCSS_BORDER_FLAGS_SPECIFIC_STYLE,

	CCSS_BORDER_FLAGS_COMMON_COLOR	= 1 << 4,
	CCSS_BORDER_FLAGS_SPECIFIC_COLOR = 1 << 5,
	CCSS_BORDER_FLAGS_COLOR_MASK	= CCSS_BORDER_FLAGS_COMMON_COLOR |
					  CCSS_BORDER_FLAGS_SPECIFIC_COLOR,

	CCSS_BORDER_FLAGS_COMMON_RADIUS	= 1 << 6,
	CCSS_BORDER_FLAGS_SPECIFIC_RADIUS = 1 << 7,
	CCSS_BORDER_FLAGS_RADIUS_MASK	= CCSS_BORDER_FLAGS_COMMON_RADIUS |
					  CCSS_BORDER_FLAGS_SPECIFIC_RADIUS
};

#define SET_COMMON_WIDTH(spec_, border_, val_)			       \
if (CCSS_PROPERTY_SPEC_SET == spec_ && 				       \
    !(CCSS_BORDER_FLAGS_SPECIFIC_WIDTH & (border_).flags)) {	       \
	(border_).width = val_;					       \
	(border_).flags |= CCSS_BORDER_FLAGS_COMMON_WIDTH;	       \
}								       \
if (spec_ != CCSS_PROPERTY_SPEC_UNSET) {				       \
	(border_).width_spec = spec_;				       \
}

#define SET_SPECIFIC_WIDTH(spec_, border_, val_)		       \
if (CCSS_PROPERTY_SPEC_SET == spec_) {				       \
	(border_).width = val_;					       \
	(border_).flags |= CCSS_BORDER_FLAGS_SPECIFIC_WIDTH;	       \
}								       \
if (spec_ != CCSS_PROPERTY_SPEC_UNSET) {				       \
	(border_).width_spec = spec_;				       \
}

#define SET_COMMON_STYLE(spec_, border_, val_)			       \
if (CCSS_PROPERTY_SPEC_SET == spec_ && 				       \
    !(CCSS_BORDER_FLAGS_SPECIFIC_STYLE & (border_).flags)) {	       \
	(border_).style = val_;					       \
	(border_).flags |= CCSS_BORDER_FLAGS_COMMON_STYLE;	       \
}								       \
if (spec_ != CCSS_PROPERTY_SPEC_UNSET) {				       \
	(border_).style_spec = spec_;				       \
}

#define SET_SPECIFIC_STYLE(spec_, border_, val_)		       \
if (CCSS_PROPERTY_SPEC_SET == spec_) {				       \
	(border_).style = val_;					       \
	(border_).flags |= CCSS_BORDER_FLAGS_SPECIFIC_STYLE;	       \
}								       \
if (spec_ != CCSS_PROPERTY_SPEC_UNSET) {				       \
	(border_).style_spec = spec_;				       \
}

#define SET_COMMON_COLOR(spec_, border_, val_)			       \
if (CCSS_PROPERTY_SPEC_SET == spec_ && 				       \
    !(CCSS_BORDER_FLAGS_SPECIFIC_COLOR & (border_).flags)) { 	       \
	(border_).color.red = val_.red;				       \
	(border_).color.green = val_.green;			       \
	(border_).color.blue = val_.blue;			       \
	(border_).flags |= CCSS_BORDER_FLAGS_COMMON_COLOR;	       \
}								       \
if (spec_ != CCSS_PROPERTY_SPEC_UNSET) {				       \
	(border_).color.spec = spec_;				       \
}

#define SET_SPECIFIC_COLOR(spec_, border_, val_)		       \
if (CCSS_PROPERTY_SPEC_SET == spec_) { 				       \
	(border_).color.red = val_.red;				       \
	(border_).color.green = val_.green;			       \
	(border_).color.blue = val_.blue;			       \
	(border_).flags |= CCSS_BORDER_FLAGS_SPECIFIC_COLOR;	       \
}								       \
if (spec_ != CCSS_PROPERTY_SPEC_UNSET) {				       \
	(border_).color.spec = spec_;				       \
}

#define SET_COMMON_RADIUS(spec_, join_, val_)			       \
if (CCSS_PROPERTY_SPEC_SET == spec_ && 				       \
    !(CCSS_BORDER_FLAGS_SPECIFIC_RADIUS & (join_).flags)) {	       \
	(join_).radius = val_;					       \
	(join_).flags |= CCSS_BORDER_FLAGS_COMMON_RADIUS;	       \
}								       \
if (spec_ != CCSS_PROPERTY_SPEC_UNSET) {				       \
	(join_).spec = spec_;					       \
}

#define SET_SPECIFIC_RADIUS(spec_, join_, val_)			       \
if (CCSS_PROPERTY_SPEC_SET == spec_) {				       \
	(join_).radius = val_;					       \
	(join_).flags |= CCSS_BORDER_FLAGS_SPECIFIC_RADIUS;	       \
}								       \
if (spec_ != CCSS_PROPERTY_SPEC_UNSET) {				       \
	(join_).spec = spec_;					       \
}

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

/*!
 * Map between border style css string and internal value.
 */
static const struct {
	ccss_border_style_type_t border_style;
	char const *css;
} _border_style_map[] = {
	{ CCSS_BORDER_STYLE_HIDDEN,	"hidden" },
	{ CCSS_BORDER_STYLE_DOTTED,	"dotted" },
	{ CCSS_BORDER_STYLE_DASHED,	"dashed" },
	{ CCSS_BORDER_STYLE_SOLID,	"solid"  },
	{ CCSS_BORDER_STYLE_DOUBLE,	"double" },
	{ CCSS_BORDER_STYLE_GROOVE,	"groove" },
	{ CCSS_BORDER_STYLE_RIDGE,	"ridge"  },
	{ CCSS_BORDER_STYLE_INSET,	"inset"  },
	{ CCSS_BORDER_STYLE_OUTSET,	"outset" }
};

static bool
match_style (char const		*css_border_style,
	     ccss_border_style_type_t	*style)
{
	g_return_val_if_fail (css_border_style && *css_border_style, false);

	for (unsigned int i = 0; i < G_N_ELEMENTS (_border_style_map); i++) {
		if (0 == strcmp (_border_style_map[i].css, css_border_style)) {
			*style = _border_style_map[i].border_style;
			return true;
		}
	}

	return false;
}

ccss_border_t * 
ccss_border_new (void)
{
	return g_new0 (ccss_border_t, 1);
}

void
ccss_border_free (ccss_border_t *self)
{
	g_free (self);
}

static ccss_property_spec_t
parse_width (CRTerm const		**value,
	     double			 *width)
{
	ccss_property_spec_t spec;

	spec = ccss_property_parse_spec (value);
	if (CCSS_PROPERTY_SPEC_SET == spec &&
	    *value && 
	    TERM_NUMBER == (*value)->type) {

		*width = (*value)->content.num->val;
		(*value) = (*value)->next;
		spec = CCSS_PROPERTY_SPEC_SET;
	}

	return spec;
}

static ccss_property_spec_t
parse_style (CRTerm const		**value,
	     ccss_border_style_type_t		 *style)
{
	ccss_property_spec_t spec;

	spec = ccss_property_parse_spec (value);
	*style = CCSS_BORDER_STYLE_SOLID;
	if (CCSS_PROPERTY_SPEC_SET == spec &&
	    *value && 
	    TERM_IDENT == (*value)->type &&
	    match_style (cr_string_peek_raw_str ((*value)->content.str), style)) {

		(*value) = (*value)->next;
		spec = CCSS_PROPERTY_SPEC_SET;
	}

	return spec;
}

static ccss_property_spec_t
parse_color (CRTerm const		**value,
	     ccss_color_t		 *color)
{
	ccss_color_parse (color, value);

	return color->spec;
}

static ccss_property_spec_t
parse_radius (CRTerm const		**value,
	      double			 *radius)
{
	ccss_property_spec_t spec;

	spec = ccss_property_parse_spec (value);
	if (CCSS_PROPERTY_SPEC_SET == spec &&
	    *value && 
	    TERM_NUMBER == (*value)->type) {

		*radius = (*value)->content.num->val;
		(*value) = (*value)->next;
		spec = CCSS_PROPERTY_SPEC_SET;
	}

	return spec;
}

static bool
parse_stroke_property (ccss_border_stroke_t	*stroke,
		       char const		*property,
		       CRTerm const		*value)
{
	ccss_property_spec_t	 spec;
	double			 width;
	ccss_border_style_type_t	 style;
	ccss_color_t		 color;

	if (0 == strcmp ("width", property)) {

		spec = parse_width (&value, &width);
		SET_SPECIFIC_WIDTH (spec, *stroke, width);

	} else if (0 == strcmp ("style", property)) {

		spec = parse_style (&value, &style);
		SET_SPECIFIC_STYLE (spec, *stroke, style);

	} else if (0 == strcmp ("color", property)) {

		spec = parse_color (&value, &color);
		SET_SPECIFIC_COLOR (spec, *stroke, color);

	} else {
		return false;
	}

	return true;
}

/*
 * Parse properties of the form
 * - border: ;		# only to prevent errors
 * - border: 1px;
 * - border: solid;
 * - border: 1px solid;
 * - border: red;
 * - border: 1px red;
 * - border: 1px solid red;
 */
bool
ccss_border_parse (ccss_border_t	*self,
		  char const	*property,
		  CRTerm const	*values)
{
	CRTerm const		*iter;
	double			 radius;
	double			 width;
	ccss_border_style_type_t	 style;
	ccss_color_t		 color;
	ccss_property_spec_t	 radius_spec;
	ccss_property_spec_t	 width_spec;
	ccss_property_spec_t	 style_spec;

	ccss_color_init (&color);

	/* Radius */
	iter = values;
	if (0 == strcmp ("border-top-right-radius", property)) {

		radius_spec = parse_radius (&iter, &radius);
		if (CCSS_PROPERTY_SPEC_UNSET == radius_spec) {
			return false;

		}
		SET_SPECIFIC_RADIUS (radius_spec, self->top_right, radius);
		return true;

	} else if (0 == strcmp ("border-bottom-right-radius", property)) {

		radius_spec = parse_radius (&iter, &radius);
		if (CCSS_PROPERTY_SPEC_UNSET == radius_spec) {
			return false;
		}
		SET_SPECIFIC_RADIUS (radius_spec, self->right_bottom, radius);
		return true;

	} else if (0 == strcmp ("border-bottom-left-radius", property)) {

		radius_spec = parse_radius (&iter, &radius);
		if (CCSS_PROPERTY_SPEC_UNSET == radius_spec) {
			return false;
		}
		SET_SPECIFIC_RADIUS (radius_spec, self->bottom_left, radius);
		return true;

	} else if (0 == strcmp ("border-top-left-radius", property)) {

		radius_spec = parse_radius (&iter, &radius);
		if (CCSS_PROPERTY_SPEC_UNSET == radius_spec) {
			return false;
		}
		SET_SPECIFIC_RADIUS (radius_spec, self->left_top, radius);
		return true;

	} else if (0 == strcmp ("border-radius", property)) {

		radius_spec = parse_radius (&iter, &radius);
		if (CCSS_PROPERTY_SPEC_UNSET == radius_spec) {
			return false;
		}
		SET_COMMON_RADIUS (radius_spec, self->left_top, radius);
		SET_COMMON_RADIUS (radius_spec, self->top_right, radius);
		SET_COMMON_RADIUS (radius_spec, self->right_bottom, radius);
		SET_COMMON_RADIUS (radius_spec, self->bottom_left, radius);
		return true;
	}

	/* Test for specific properties first. */
	if (0 == strncmp ("border-left-", property, 
			  sizeof ("border-left-") - 1)) {
		bool ret;
		ret = parse_stroke_property (&self->left, 
				property + sizeof ("border-left-") - 1,
				values);
		if (!ret)
			g_warning ("Unknown property `%s'", property);
		return ret;

	} else if (0 == strncmp ("border-top-", property,
				 sizeof ("border-top-") - 1)) {
		bool ret;
		ret = parse_stroke_property (&self->top, 
				property + sizeof ("border-top-") - 1,
				values);
		if (!ret)
			g_warning ("Unknown property `%s'", property);
		return ret;

	} else if (0 == strncmp ("border-right-", property,
				 sizeof ("border-right-") - 1)) {
		bool ret;
		ret = parse_stroke_property (&self->right,
				property + sizeof ("border-right-") - 1,
				values);
		if (!ret)
			g_warning ("Unknown property `%s'", property);
		return ret;

	} else if (0 == strncmp ("border-bottom-", property,
				 sizeof ("border-bottom-") - 1)) {
		bool ret;
		ret = parse_stroke_property (&self->bottom,
				property + sizeof ("border-bottom-") - 1,
				values);
		if (!ret)
			g_warning ("Unknown property `%s'", property);
		return ret;
	}

	/* Now try to parse multi-value properties. */
	iter = values;
	width_spec = parse_width (&iter, &width);
	if (CCSS_PROPERTY_SPEC_NONE == width_spec && !iter) {
		style_spec = CCSS_PROPERTY_SPEC_NONE;
		color.spec = CCSS_PROPERTY_SPEC_NONE;
	} else {
		style_spec = parse_style (&iter, &style);
		parse_color (&iter, &color);
	}

	if (0 == strcmp ("border", property)) {

		SET_COMMON_WIDTH (width_spec, self->left, width);
		SET_COMMON_WIDTH (width_spec, self->top, width);
		SET_COMMON_WIDTH (width_spec, self->right, width);
		SET_COMMON_WIDTH (width_spec, self->bottom, width);

		SET_COMMON_STYLE (style_spec, self->left, style);
		SET_COMMON_STYLE (style_spec, self->top, style);
		SET_COMMON_STYLE (style_spec, self->right, style);
		SET_COMMON_STYLE (style_spec, self->bottom, style);
		
		SET_COMMON_COLOR (color.spec, self->left, color);
		SET_COMMON_COLOR (color.spec, self->top, color);
		SET_COMMON_COLOR (color.spec, self->right, color);
		SET_COMMON_COLOR (color.spec, self->bottom, color);

	} else if (0 == strcmp ("border-left", property)) {

		SET_SPECIFIC_WIDTH (width_spec, self->left, width);
		SET_SPECIFIC_STYLE (style_spec, self->left, style);
		SET_SPECIFIC_COLOR (color.spec, self->left, color);

	} else if (0 == strcmp ("border-top", property)) {

		SET_SPECIFIC_WIDTH (width_spec, self->top, width);
		SET_SPECIFIC_STYLE (style_spec, self->top, style);
		SET_SPECIFIC_COLOR (color.spec, self->top, color);

	} else if (0 == strcmp ("border-right", property)) {

		SET_SPECIFIC_WIDTH (width_spec, self->right, width);
		SET_SPECIFIC_STYLE (style_spec, self->right, style);
		SET_SPECIFIC_COLOR (color.spec, self->right, color);

	} else if (0 == strcmp ("border-bottom", property)) {

		SET_SPECIFIC_WIDTH (width_spec, self->bottom, width);
		SET_SPECIFIC_STYLE (style_spec, self->bottom, style);
		SET_SPECIFIC_COLOR (color.spec, self->bottom, color);

	} else {
		return false;
	}

	return true;
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
{}

static void
draw_dotted_line (ccss_border_stroke_t const	*stroke,
		  cairo_t			*cr,
		  double			 x1,
		  double			 y1,
		  double			 x2,
		  double			 y2)
{
	double dash_len;

	dash_len = stroke->width;

	cairo_save (cr);

	line (stroke, cr, x1, y1, x2, y2);

	cairo_set_dash (cr, &dash_len, 1, 0);
	cairo_set_line_width (cr, stroke->width);
	cairo_set_source_rgb (cr, stroke->color.red, stroke->color.green, 
				stroke->color.blue);
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

	dashes[0] = stroke->width * 3;
	dashes[1] = stroke->width * 3;

	cairo_save (cr);

	line (stroke, cr, x1, y1, x2, y2);

	cairo_set_dash (cr, dashes, G_N_ELEMENTS (dashes), 0);
	cairo_set_line_width (cr, stroke->width);
	cairo_set_source_rgb (cr, stroke->color.red, stroke->color.green, 
				stroke->color.blue);
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

	cairo_set_line_width (cr, stroke->width);
	cairo_set_source_rgb (cr, stroke->color.red, stroke->color.green, 
				stroke->color.blue);
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

	if (CCSS_PROPERTY_SPEC_UNSET == stroke->style_spec ||
	    CCSS_PROPERTY_SPEC_NONE == stroke->style_spec) {
		return draw_none_line;
	}

	switch (stroke->style) {
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

	/* TODO draw with gradient if colors are different. 
	 * Probably requires that the join has all the color information. */
	if (before) {
		cairo_set_line_width (cr, before->width);
		cairo_set_source_rgb (cr,
				      before->color.red, before->color.green, 
				      before->color.blue);
	} else if (after) {
		cairo_set_line_width (cr, after->width);
		cairo_set_source_rgb (cr,
				      after->color.red, after->color.green, 
				      after->color.blue);
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

	rlt = left_top && left_top->spec ? left_top->radius : 0;
	rtr = top_right && top_right->spec ? top_right->radius : 0;
	rrb = right_bottom && right_bottom->spec ? right_bottom->radius : 0;
	rbl = bottom_left && bottom_left->spec ? bottom_left->radius : 0;
	if (!(CCSS_BORDER_ROUNDING_UNRESTRICTED & visibility_flags)) {
		ccss_border_clamp_radii (x, y, width, height, 
					&rlt, &rtr, &rrb, &rbl);
	}

	have_segment = false;

	if (left) {
		line_func = get_line_draw_func (left, path_only);
		xoff = !path_only && left ? left->width / 2. : 0;
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
		xoff = !path_only && left ? left->width / 2. : 0;
		yoff = !path_only && top ? top->width / 2. : 0;
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
		yoff = !path_only && top ? top->width / 2. : 0;
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
		xoff = !path_only && right ? right->width / 2. : 0;
		yoff = !path_only && top ? top->width / 2. : 0;
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
		xoff = !path_only && right ? right->width / 2. : 0;
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
		xoff = !path_only && right ? right->width / 2. : 0;
		yoff = !path_only && bottom ? bottom->width / 2. : 0;
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
		yoff = !path_only && bottom ? bottom->width / 2. : 0;
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
		xoff = !path_only && left ? left->width / 2. : 0;
		yoff = !path_only && bottom ? bottom->width / 2. : 0;
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

#ifdef CCSS_DEBUG

static char const *
lookup_name (ccss_border_style_type_t border_style)
{
	for (unsigned int i = 0; i < G_N_ELEMENTS (_border_style_map); i++) {
		if (_border_style_map[i].border_style == border_style) {
			return _border_style_map[i].css;
		}
	}	

	return NULL;
}

void
ccss_border_join_dump (ccss_border_join_t const *self)
{
	if (self->spec != CCSS_PROPERTY_SPEC_UNSET) {
		printf ("%.1f ", self->radius);
	}

	printf (";\n");
}

void
ccss_border_stroke_dump (ccss_border_stroke_t const *self)
{
	if (self->width_spec != CCSS_PROPERTY_SPEC_UNSET) {
		printf ("%.1f ", self->width);
	}

	if (self->style_spec != CCSS_PROPERTY_SPEC_UNSET) {
		printf ("%s ", lookup_name (self->style));
	}

	ccss_color_dump (&self->color);

	printf (";\n");
}

void
ccss_border_dump (ccss_border_t const *self)
{
	if (CCSS_BORDER_STROKE_IS_SET (self->left)) {
		printf (CCSS_PROPERTY_DUMP_PREFIX "border-left: ");
		ccss_border_stroke_dump (&self->left);
	}

	if (CCSS_BORDER_JOIN_IS_SET (self->left_top)) {
		printf (CCSS_PROPERTY_DUMP_PREFIX "border-top-left-radius: ");
		ccss_border_join_dump (&self->left_top);
	}

	if (CCSS_BORDER_STROKE_IS_SET (self->top)) {
		printf (CCSS_PROPERTY_DUMP_PREFIX "border-top: ");
		ccss_border_stroke_dump (&self->top);
	}

	if (CCSS_BORDER_JOIN_IS_SET (self->top_right)) {
		printf (CCSS_PROPERTY_DUMP_PREFIX "border-top-right-radius: ");
		ccss_border_join_dump (&self->top_right);
	}

	if (CCSS_BORDER_STROKE_IS_SET (self->right)) {
		printf (CCSS_PROPERTY_DUMP_PREFIX "border-right: ");
		ccss_border_stroke_dump (&self->right);
	}

	if (CCSS_BORDER_JOIN_IS_SET (self->right_bottom)) {
		printf (CCSS_PROPERTY_DUMP_PREFIX "border-bottom-right-radius: ");
		ccss_border_join_dump (&self->right_bottom);
	}

	if (CCSS_BORDER_STROKE_IS_SET (self->bottom)) {
		printf (CCSS_PROPERTY_DUMP_PREFIX "border-bottom: ");
		ccss_border_stroke_dump (&self->bottom);
	}

	if (CCSS_BORDER_JOIN_IS_SET (self->bottom_left)) {
		printf (CCSS_PROPERTY_DUMP_PREFIX "border-bottom-left-radius: ");
		ccss_border_join_dump (&self->bottom_left);
	}
}

#endif /* CCSS_DEBUG */

