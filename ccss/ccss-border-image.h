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

#ifndef CCSS_BORDER_IMAGE_H
#define CCSS_BORDER_IMAGE_H

#include <stdint.h>
#include <cairo.h>
#include <ccss/ccss-features.h>
#include <ccss/ccss-image.h>
#include <ccss/ccss-macros.h>
#include <ccss/ccss-position.h>
#include <ccss/ccss-property.h>

CCSS_BEGIN_DECLS

typedef enum {
	CCSS_BORDER_IMAGE_TILING_REPEAT = 0,
	CCSS_BORDER_IMAGE_TILING_ROUND,
	CCSS_BORDER_IMAGE_TILING_STRETCH
} ccss_border_image_tiling_t;

typedef struct {
	ccss_property_state_t		state;
	ccss_image_t			image;
	ccss_position_t			top;
	ccss_position_t			right;
	ccss_position_t			bottom;
	ccss_position_t			left;
	ccss_border_image_tiling_t	top_middle_bottom_horizontal_tiling;
	ccss_border_image_tiling_t	left_middle_right_vertical_tiling;
} ccss_border_image_t;

void
ccss_border_image_draw (ccss_border_image_t const	*self,
			cairo_t				*cr, 
			double				 x,
			double				 y,
			double				 width,
			double				 height);

CCSS_END_DECLS

#endif /* CCSS_BORDER_IMAGE_H */

