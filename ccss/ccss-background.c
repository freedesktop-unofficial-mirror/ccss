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
#include "ccss-background-priv.h"

ccss_background_attachment_type_t
ccss_background_attachment_get_attachment (ccss_background_attachment_t const *self)
{
	g_return_val_if_fail (self, 0);

	return self->attachment;
}

char const *
ccss_background_image_get_uri (ccss_background_image_t const *self)
{
	g_return_val_if_fail (self, NULL);

	return self->uri;
}

ccss_position_t const *
ccss_background_position_get_horizontal_position (ccss_background_position_t const *self)
{
	g_return_val_if_fail (self, NULL);

	return &self->hpos;
}

ccss_position_t const *
ccss_background_position_get_vertical_position (ccss_background_position_t const *self)
{
	g_return_val_if_fail (self, NULL);

	return &self->vpos;
}

ccss_background_repeat_type_t
ccss_background_repeat_get_repeat (ccss_background_repeat_t const *self)
{
	g_return_val_if_fail (self, 0);

	return self->repeat;
}

ccss_position_t const *
ccss_background_size_get_height (ccss_background_size_t const *self)
{
	g_return_val_if_fail (self, NULL);

	return &self->height;
}

ccss_position_t const *
ccss_background_size_get_width (ccss_background_size_t const *self)
{
	g_return_val_if_fail (self, NULL);

	return &self->width;
}

ccss_background_attachment_t const *
ccss_background_get_attachment (ccss_background_t const *self)
{
	g_return_val_if_fail (self, NULL);

	return &self->bg_attachment;
}

ccss_color_t const *
ccss_background_get_color (ccss_background_t const *self)
{
	g_return_val_if_fail (self, NULL);

	return &self->bg_color;
}

ccss_background_image_t const *
ccss_background_get_image (ccss_background_t const *self)
{
	g_return_val_if_fail (self, NULL);

	return &self->bg_image;
}

ccss_background_position_t const *
ccss_background_get_position (ccss_background_t const *self)
{
	g_return_val_if_fail (self, NULL);

	return &self->bg_position;
}

ccss_background_repeat_t const *
ccss_background_get_repeat (ccss_background_t const *self)
{
	g_return_val_if_fail (self, NULL);

	return &self->bg_repeat;
}

ccss_background_size_t const *
ccss_background_get_size (ccss_background_t const *self)
{
	g_return_val_if_fail (self, NULL);

	return &self->bg_size;
}

