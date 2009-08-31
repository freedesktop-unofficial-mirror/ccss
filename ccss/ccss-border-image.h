/* vim: set ts=8 sw=8 noexpandtab: */

/* The `C' CSS Library.
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
	CCSS_DEPRECATED (ccss_property_t		 base);

	CCSS_DEPRECATED (char				*uri);
	CCSS_DEPRECATED (ccss_position_t		 top);
	CCSS_DEPRECATED (ccss_position_t		 right);
	CCSS_DEPRECATED (ccss_position_t		 bottom);
	CCSS_DEPRECATED (ccss_position_t		 left);
	CCSS_DEPRECATED (ccss_border_image_tiling_t	 top_middle_bottom_horizontal_tiling);
	CCSS_DEPRECATED (ccss_border_image_tiling_t	 left_middle_right_vertical_tiling);
} ccss_border_image_t;

char const *
ccss_border_image_get_uri (ccss_border_image_t const *self);

ccss_position_t const *
ccss_border_image_get_top (ccss_border_image_t const *self);

ccss_position_t const *
ccss_border_image_get_right (ccss_border_image_t const *self);

ccss_position_t const *
ccss_border_image_get_bottom (ccss_border_image_t const *self);

ccss_position_t const *
ccss_border_image_get_left (ccss_border_image_t const *self);

ccss_border_image_tiling_t
ccss_border_image_get_top_middle_bottom_horizontal_tiling (ccss_border_image_t const *self);

ccss_border_image_tiling_t
ccss_border_image_get_left_middle_right_vertical_tiling (ccss_border_image_t const *self);

CCSS_END_DECLS

#endif /* CCSS_BORDER_IMAGE_H */

