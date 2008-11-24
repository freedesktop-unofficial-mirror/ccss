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

#ifndef CCSS_CAIRO_STYLE_H
#define CCSS_CAIRO_STYLE_H

#include <stdint.h>
#include <cairo.h>
#include <ccss/ccss-macros.h>

CCSS_BEGIN_DECLS

// TODO document
// this can be casted from GtkPositionType
typedef enum {
	CCSS_GAP_SIDE_LEFT,
	CCSS_GAP_SIDE_RIGHT,
	CCSS_GAP_SIDE_TOP,
	CCSS_GAP_SIDE_BOTTOM
} ccss_cairo_gap_side_t;

void
ccss_style_draw_line (ccss_style_t const	*self,
		      cairo_t			*cr, 
		      int32_t			 x1,
		      int32_t			 x2,
		      int32_t			 y1,
		      int32_t			 y2);

void
ccss_style_draw_outline (ccss_style_t const	*self,
			 cairo_t		*cr, 
			 int32_t		 x,
			 int32_t		 y,
			 int32_t		 width,
			 int32_t		 height);

void
ccss_style_draw_rectangle (ccss_style_t const	*self,
			   cairo_t		*cr, 
			   int32_t		 x,
			   int32_t		 y,
			   int32_t		 width,
			   int32_t		 height);

void
ccss_style_draw_gap (ccss_style_t const		*self,
		     cairo_t			*cr, 
		     int32_t			 x,
		     int32_t			 y,
		     int32_t			 width,
		     int32_t			 height, 
		     ccss_cairo_gap_side_t	 gap_side,
		     int32_t			 gap_start,
		     int32_t			 gap_width);

CCSS_END_DECLS

#endif /* CCSS_CAIRO_STYLE_H */

