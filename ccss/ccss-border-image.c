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

#include <glib.h>
#include "ccss-border-image-priv.h"

char const *
ccss_border_image_get_uri (ccss_border_image_t const *self)
{
	g_return_val_if_fail (self, NULL);

	return self->uri;
}

ccss_position_t const *
ccss_border_image_get_top (ccss_border_image_t const *self)
{
	g_return_val_if_fail (self, NULL);

	return &self->top;
}

ccss_position_t const *
ccss_border_image_get_right (ccss_border_image_t const *self)
{
	g_return_val_if_fail (self, NULL);

	return &self->right;
}

ccss_position_t const *
ccss_border_image_get_bottom (ccss_border_image_t const *self)
{
	g_return_val_if_fail (self, NULL);

	return &self->bottom;
}

ccss_position_t const *
ccss_border_image_get_left (ccss_border_image_t const *self)
{
	g_return_val_if_fail (self, NULL);

	return &self->left;
}

ccss_border_image_tiling_t
ccss_border_image_get_top_middle_bottom_horizontal_tiling (ccss_border_image_t const *self)
{
	g_return_val_if_fail (self, 0);

	return self->top_middle_bottom_horizontal_tiling;
}

ccss_border_image_tiling_t
ccss_border_image_get_left_middle_right_vertical_tiling (ccss_border_image_t const *self)
{
	g_return_val_if_fail (self, 0);

	return self->left_middle_right_vertical_tiling;
}

