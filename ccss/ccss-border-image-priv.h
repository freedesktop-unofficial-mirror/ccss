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

#ifndef CCSS_BORDER_IMAGE_PRIV_H
#define CCSS_BORDER_IMAGE_PRIV_H

#include <ccss/ccss-macros.h>
#include <ccss/ccss-border-image.h>
#include <ccss/ccss-position-priv.h>
#include <ccss/ccss-property-impl.h>

CCSS_BEGIN_DECLS

struct ccss_border_image_ {
	/*< private >*/
	CCSS_DEPRECATED (ccss_property_t		 base);

	CCSS_DEPRECATED (char				*uri);
	CCSS_DEPRECATED (ccss_position_t		 top);
	CCSS_DEPRECATED (ccss_position_t		 right);
	CCSS_DEPRECATED (ccss_position_t		 bottom);
	CCSS_DEPRECATED (ccss_position_t		 left);
	CCSS_DEPRECATED (ccss_border_image_tiling_t	 top_middle_bottom_horizontal_tiling);
	CCSS_DEPRECATED (ccss_border_image_tiling_t	 left_middle_right_vertical_tiling);
};

CCSS_END_DECLS

#endif /* CCSS_BORDER_IMAGE_PRIV_H */

