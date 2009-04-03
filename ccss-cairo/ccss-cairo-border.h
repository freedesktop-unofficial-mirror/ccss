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

#ifndef CCSS_CAIRO_BORDER_H
#define CCSS_CAIRO_BORDER_H

#include <stdint.h>
#include <cairo.h>
#include <ccss/ccss.h>

CCSS_BEGIN_DECLS

void
ccss_cairo_border_clamp_radii (double     x,
			       double     y, 
			       double     width, 
			       double     height,
			       double    *left_top, 
			       double    *top_right,
			       double    *right_bottom, 
			       double    *bottom_left);

void
ccss_cairo_border_path (ccss_border_stroke_t const      *left, 
			ccss_border_join_t const	*left_top,
			ccss_border_stroke_t const      *top, 
			ccss_border_join_t const	*top_right,
			ccss_border_stroke_t const      *right,
			ccss_border_join_t const	*right_bottom,
			ccss_border_stroke_t const      *bottom, 
			ccss_border_join_t const	*bottom_left,
			cairo_t				*cr, 
			double				 x, 
			double				 y, 
			double				 width,
			double				 height);

void
ccss_cairo_border_draw (ccss_border_stroke_t const      *left, 
			ccss_border_join_t const	*left_top,
			ccss_border_stroke_t const      *top, 
			ccss_border_join_t const	*top_right,
			ccss_border_stroke_t const      *right,
			ccss_border_join_t const	*right_bottom,
			ccss_border_stroke_t const      *bottom, 
			ccss_border_join_t const	*bottom_left,
			uint32_t			 visibility_flags, 
			cairo_t				*cr, 
			double				 x,
			double				 y, 
			double				 width, 
			double				 height);

CCSS_END_DECLS

#endif /* CCSS_CAIRO_BORDER_H */

