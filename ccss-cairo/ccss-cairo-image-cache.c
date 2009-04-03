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

#include <glib.h>
#include "ccss-cairo-image-cache.h"
#include "config.h"

static GHashTable *_image_hash = NULL;

ccss_cairo_image_t const *
ccss_cairo_image_cache_fetch_image (char const *uri)
{
	ccss_cairo_image_t *image;

	if (_image_hash == NULL) {
		_image_hash = g_hash_table_new_full (
				g_str_hash,
				g_str_equal,
				g_free,
				(GDestroyNotify) ccss_cairo_image_destroy);
	}
	
	image = g_hash_table_lookup (_image_hash, uri);
	
	if (!image) {
		image = ccss_cairo_image_create (uri);
		if (image)
			g_hash_table_insert (_image_hash,
					     g_strdup (uri),
					     image);
	}
	
	return image;
}

void
ccss_cairo_image_cache_destroy (void)
{
	g_hash_table_destroy (_image_hash);
	_image_hash = NULL;
}

