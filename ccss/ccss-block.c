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

#include "ccss-block.h"

ccss_block_t *
ccss_block_new (void)
{
	ccss_block_t *self;

	self = g_new0 (ccss_block_t, 1);

	return self;
}

void
ccss_block_free (ccss_block_t *self)
{
	g_assert (self);

	g_free (self);
}

#ifdef CCSS_DEBUG

void
ccss_block_dump (ccss_block_t const *self)
{
	ccss_background_dump (&self->background);
	ccss_border_dump (&self->border);
	ccss_color_dump (&self->color);
}

#endif /* CCSS_DEBUG */

