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

#ifndef CCSS_BORDER_H
#define CCSS_BORDER_H

#include <stdbool.h>
#include <stdint.h>
#include <cairo.h>
#include <ccss/ccss-color.h>
#include <ccss/ccss-macros.h>
#include <ccss/ccss-property.h>

CCSS_BEGIN_DECLS

typedef enum {
	CCSS_BORDER_STYLE_HIDDEN,
	CCSS_BORDER_STYLE_DOTTED,
	CCSS_BORDER_STYLE_DASHED,
	CCSS_BORDER_STYLE_SOLID,
	CCSS_BORDER_STYLE_DOUBLE,
	CCSS_BORDER_STYLE_GROOVE,
	CCSS_BORDER_STYLE_RIDGE,
	CCSS_BORDER_STYLE_INSET,
	CCSS_BORDER_STYLE_OUTSET
} ccss_border_style_type_t;

typedef enum {
	CCSS_BORDER_VISIBILITY_SHOW_ALL			= 0,
	CCSS_BORDER_VISIBILITY_HIDE_LEFT		= 1 << 0,
	CCSS_BORDER_VISIBILITY_HIDE_LEFT_TOP		= 1 << 1,
	CCSS_BORDER_VISIBILITY_HIDE_TOP			= 1 << 2,
	CCSS_BORDER_VISIBILITY_HIDE_TOP_RIGHT		= 1 << 3,
	CCSS_BORDER_VISIBILITY_HIDE_RIGHT		= 1 << 4,
	CCSS_BORDER_VISIBILITY_HIDE_RIGHT_BOTTOM	= 1 << 5,
	CCSS_BORDER_VISIBILITY_HIDE_BOTTOM		= 1 << 6,
	CCSS_BORDER_VISIBILITY_HIDE_BOTTOM_LEFT		= 1 << 7,

	CCSS_BORDER_ROUNDING_UNRESTRICTED		= 1 << 8
} ccss_border_drawing_flags_t;

typedef struct {
	ccss_property_base_t		base;

	ccss_border_style_type_t	style;
} ccss_border_style_t;

typedef struct {
	ccss_property_base_t		base;

	double				width;
} ccss_border_width_t;

typedef struct {
	ccss_color_t const *		color;
	ccss_border_style_t const *	style;
	ccss_border_width_t const *	width;
} ccss_border_stroke_t; 

typedef struct {
	ccss_property_base_t		base;

	double				radius;
} ccss_border_join_t;	/* FIXME: rename to `border-radius'? */

void ccss_border_clamp_radii (double x, double y, double width, double height,
			     double *left_top, double *top_right,
			     double *right_bottom, double *bottom_left);

void ccss_border_path (ccss_border_stroke_t const *left, 
		      ccss_border_join_t const *left_top,
		      ccss_border_stroke_t const *top, 
		      ccss_border_join_t const *top_right,
		      ccss_border_stroke_t const *right,
		      ccss_border_join_t const *right_bottom,
		      ccss_border_stroke_t const *bottom, 
		      ccss_border_join_t const *bottom_left,
		      cairo_t *cr, 
		      double x, double y, double width, double height);

void ccss_border_draw (ccss_border_stroke_t const *left, 
		      ccss_border_join_t const *left_top,
		      ccss_border_stroke_t const *top, 
		      ccss_border_join_t const *top_right,
		      ccss_border_stroke_t const *right,
		      ccss_border_join_t const *right_bottom,
		      ccss_border_stroke_t const *bottom, 
		      ccss_border_join_t const *bottom_left,
		      uint32_t visibility_flags, cairo_t *cr, 
		      double x, double y, double width, double height);

CCSS_END_DECLS

#endif /* CCSS_BORDER_H */

