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

#ifndef CCSS_CAIRO_IMAGE_H
#define CCSS_CAIRO_IMAGE_H

#include <cairo.h>
#include <libcroco/libcroco.h>
#include <ccss/ccss.h>

CCSS_BEGIN_DECLS

typedef struct {
	cairo_pattern_t *pattern;
	double		 width;
	double		 height;
} ccss_cairo_image_t;

ccss_cairo_image_t *
ccss_cairo_image_create (char const *uri);

void
ccss_cairo_image_destroy (ccss_cairo_image_t *self);

CCSS_END_DECLS

#endif /* CCSS_CAIRO_IMAGE_H */

