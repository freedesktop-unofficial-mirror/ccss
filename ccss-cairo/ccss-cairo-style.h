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

#ifndef CCSS_CAIRO_STYLE_H
#define CCSS_CAIRO_STYLE_H

#ifndef CCSS_CAIRO_H
  #ifndef CCSS_CAIRO_BUILD
    #error "Only <ccss-cairo/ccss-cairo.h> can be included directly."
  #endif
#endif

#include <cairo.h>
#include <ccss/ccss.h>

CCSS_BEGIN_DECLS

/**
 * ccss_cairo_gap_side_t:
 * @CCSS_CAIRO_GAP_SIDE_LEFT:	gap on the left.
 * @CCSS_CAIRO_GAP_SIDE_RIGHT:	gap on the right.
 * @CCSS_CAIRO_GAP_SIDE_TOP:	gap on top.
 * @CCSS_CAIRO_GAP_SIDE_BOTTOM:	gap on the bottom.
 *
 * This enum mirrors #GtkPositionType.
 **/
typedef enum {
	CCSS_CAIRO_GAP_SIDE_LEFT,
	CCSS_CAIRO_GAP_SIDE_RIGHT,
	CCSS_CAIRO_GAP_SIDE_TOP,
	CCSS_CAIRO_GAP_SIDE_BOTTOM
} ccss_cairo_gap_side_t;

typedef void
(*ccss_cairo_style_draw_f) (ccss_style_t const	*self,
			    cairo_t		*cr,
			    int			 x,
			    int			 y,
			    int			 width,
			    int			 height);

void
ccss_cairo_style_draw_line (ccss_style_t const	*self,
			    cairo_t		*cr, 
			    int			 x1,
			    int			 x2,
			    int			 y1,
			    int			 y2);

void
ccss_cairo_style_draw_outline (ccss_style_t const	*self,
			       cairo_t			*cr, 
			       int			 x,
			       int			 y,
			       int			 width,
			       int			 height);

void
ccss_cairo_style_draw_rectangle (ccss_style_t const	*self,
				 cairo_t		*cr, 
				 int			 x,
				 int			 y,
				 int			 width,
				 int			 height);

void
ccss_cairo_style_draw_rectangle_with_gap (ccss_style_t const		*self,
					  cairo_t			*cr, 
					  int				 x,
					  int				 y,
					  int				 width,
					  int				 height, 
					  ccss_cairo_gap_side_t		 gap_side,
					  int				 gap_start,
					  int				 gap_width);

bool
ccss_cairo_style_get_double (ccss_style_t const	*self,
			     char const		*property_name,
			     double		*value);

bool 
ccss_cairo_style_get_string (ccss_style_t const	 *self,
			     char const		 *property_name,
			     char		**value);

bool
ccss_cairo_style_get_property (ccss_style_t const		 *self,
			       char const			 *property_name,
			       ccss_property_base_t const	**property);

CCSS_END_DECLS

#endif /* CCSS_CAIRO_STYLE_H */

