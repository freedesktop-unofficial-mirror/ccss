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
#include "ccss-color.h"

double
ccss_color_get_red (ccss_color_t const *self)
{
	g_return_val_if_fail (self, 0);

	return self->red;
}

double
ccss_color_get_green (ccss_color_t const *self)
{
	g_return_val_if_fail (self, 0);

	return self->green;
}

double
ccss_color_get_blue (ccss_color_t const *self)
{
	g_return_val_if_fail (self, 0);

	return self->blue;
}

double
ccss_color_get_alpha (ccss_color_t const *self)
{
	g_return_val_if_fail (self, 0);

	return self->alpha;
}

