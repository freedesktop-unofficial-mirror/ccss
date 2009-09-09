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

/* Direct access to struct members for fun and profit. */
#include <ccss/ccss-macros.h>

#include "ccss/ccss-border-image-priv.h"
#include "ccss-cairo-border-image.h"
#include "ccss-cairo-image-cache.h"
#include "config.h"

typedef enum {
	ORIENTATION_HORIZONTAL,
	ORIENTATION_VERTICAL
} orientation_t;

static cairo_pattern_t *
create_tile (ccss_cairo_image_t const	*image,
	     double			 x,
	     double			 y,
	     double			 width,
	     double			 height)
{
	cairo_t		*cr;
	cairo_surface_t	*surface;
	cairo_pattern_t *pattern;
	cairo_status_t	 status;

	g_return_val_if_fail (image->pattern, NULL);

	/* Setup. */
	surface = NULL;
	status = cairo_pattern_get_surface (image->pattern, &surface);
	if (status != CAIRO_STATUS_SUCCESS) {
		g_warning ("%s", cairo_status_to_string (status));
		return NULL;
	}

	surface = cairo_surface_create_similar (surface,
						CAIRO_CONTENT_COLOR_ALPHA,
						width, height);
	cr = cairo_create (surface);

	/* Drawing. */
	cairo_pattern_set_extend (image->pattern, CAIRO_EXTEND_NONE);
	cairo_translate (cr, -1 * x, -1 * y);
	cairo_set_source (cr, image->pattern);
	cairo_paint (cr);
		
	/* Cleanup. */
	pattern = cairo_pattern_create_for_surface (surface);
	cairo_destroy (cr), cr = NULL;
	cairo_surface_destroy (surface), surface = NULL;

	return pattern;
}

static cairo_pattern_t *
create_border (ccss_cairo_image_t const		*image,
	       orientation_t			 orientation,
	       ccss_border_image_tiling_t	 tiling,
	       double				 width,
	       double				 height,
	       double				 tile_x,
	       double				 tile_y,
	       double				 tile_width,
	       double				 tile_height)
{
	cairo_t		*cr;
	cairo_surface_t	*surface;
	cairo_pattern_t *tile;
	cairo_pattern_t *pattern;
	cairo_status_t	 status;

	g_return_val_if_fail (image && image->pattern, NULL);

	/* Extract tile. */
	tile = create_tile (image, tile_x, tile_y, tile_width, tile_height);
	if (!tile)
		return false;

	/* Setup. */
	surface = NULL;
	status = cairo_pattern_get_surface (tile, &surface);
	if (status != CAIRO_STATUS_SUCCESS) {
		g_warning ("%s", cairo_status_to_string (status));
		return NULL;
	}

	surface = cairo_surface_create_similar (surface,
						CAIRO_CONTENT_COLOR_ALPHA,
						width, height);
	cr = cairo_create (surface);

	/* Drawing. */
	/* FIXME: support more tiling variants. s*/
	g_warn_if_fail (tiling == CCSS_BORDER_IMAGE_TILING_STRETCH);
	cairo_pattern_set_extend (tile, CAIRO_EXTEND_PAD);
	switch (orientation) {
	case ORIENTATION_HORIZONTAL:
		cairo_scale (cr, width / tile_width, 1);
		break;
	case ORIENTATION_VERTICAL:
		cairo_scale (cr, 1, height / tile_height);
		break;	
	}
	cairo_set_source (cr, tile);
	cairo_paint (cr);
		
	/* Cleanup. */
	cairo_pattern_destroy (tile), tile = NULL;
	pattern = cairo_pattern_create_for_surface (surface);
	cairo_destroy (cr), cr = NULL;
	cairo_surface_destroy (surface), surface = NULL;

	return pattern;
}

static cairo_pattern_t *
create_middle (ccss_cairo_image_t const		*image,
	       ccss_border_image_tiling_t	 horizontal_tiling,
	       ccss_border_image_tiling_t	 vertical_tiling,
	       double				 width,
	       double				 height,
	       double				 tile_x,
	       double				 tile_y,
	       double				 tile_width,
	       double				 tile_height)
{
	cairo_t		*cr;
	cairo_surface_t	*surface;
	cairo_pattern_t *tile;
	cairo_pattern_t *pattern;
	cairo_status_t	 status;

	g_return_val_if_fail (image && image->pattern, NULL);

	/* Extract tile. */
	tile = create_tile (image, tile_x, tile_y, tile_width, tile_height);
	if (!tile)
		return false;

	/* Setup. */
	surface = NULL;
	status = cairo_pattern_get_surface (tile, &surface);
	if (status != CAIRO_STATUS_SUCCESS) {
		g_warning ("%s", cairo_status_to_string (status));
		return NULL;
	}

	surface = cairo_surface_create_similar (surface,
						CAIRO_CONTENT_COLOR_ALPHA,
						width, height);
	cr = cairo_create (surface);

	/* Drawing. */
	/* FIXME: support more tiling variants. s*/
	g_warn_if_fail (horizontal_tiling == CCSS_BORDER_IMAGE_TILING_STRETCH &&
			vertical_tiling == CCSS_BORDER_IMAGE_TILING_STRETCH);
	cairo_pattern_set_extend (tile, CAIRO_EXTEND_PAD);
	cairo_scale (cr, width / tile_width, height / tile_height);
	cairo_set_source (cr, tile);
	cairo_paint (cr);
		
	/* Cleanup. */
	cairo_pattern_destroy (tile), tile = NULL;
	pattern = cairo_pattern_create_for_surface (surface);
	cairo_destroy (cr), cr = NULL;
	cairo_surface_destroy (surface), surface = NULL;

	return pattern;
}

static void
paint (cairo_pattern_t		*tile,
       cairo_t			*cr, 
       double			 x,
       double			 y)
{
	cairo_save (cr);
	cairo_translate (cr, x, y);

	cairo_pattern_set_extend (tile, CAIRO_EXTEND_NONE);
	cairo_set_source (cr, tile);
	cairo_paint (cr);

	cairo_restore (cr);
}

void
ccss_cairo_border_image_draw (ccss_border_image_t const	*self,
			      cairo_t			*cr, 
			      double			 x,
			      double			 y,
			      double			 width,
			      double			 height)
{
	ccss_cairo_image_t const	*image;
	cairo_pattern_t			*tile;
	double				 top_width, top_height;
	double				 right_width, right_height;
	double				 bottom_width, bottom_height;
	double				 left_width, left_height;
	double				 middle_width, middle_height;
	double				 xoff, yoff;

	g_return_if_fail (self && cr);
	image = ccss_cairo_image_cache_fetch_image (self->uri);
	g_return_if_fail (image);

	/* Tile extents, see http://www.w3.org/TR/css3-background/#the-border-image . */

	top_width = image->width
			- ccss_position_get_size (&self->left, image->width)
			- ccss_position_get_size (&self->right, image->width);
	top_height = ccss_position_get_size (&self->top, image->height);

	right_width = ccss_position_get_size (&self->right, image->width);
	right_height = image->height
			- ccss_position_get_size (&self->top, image->height)
			- ccss_position_get_size (&self->bottom, image->height);

	bottom_width = top_width;
	bottom_height = ccss_position_get_size (&self->bottom, image->height);

	left_width = ccss_position_get_size (&self->left, image->width);
	left_height = right_height;

	middle_width = top_width;
	middle_height = right_height;

	/* top-left */
	tile = create_tile (image, 
			    0, 0,
			    left_width, top_height);
	if (tile) {
		xoff = x;
		yoff = y;
		paint (tile, cr, xoff, yoff);
		cairo_pattern_destroy (tile), tile = NULL;
	}

	/* top */
	tile = create_border (image, ORIENTATION_HORIZONTAL,
			      self->top_middle_bottom_horizontal_tiling,
			      width - left_width - right_width, top_height,
			      left_width, 0, top_width, top_height);
	if (tile) {
		xoff = x + left_width;
		yoff = y;
		paint (tile, cr, xoff, yoff);
		cairo_pattern_destroy (tile), tile = NULL;
	}

	/* top-right */
	tile = create_tile (image,
			    left_width + top_width, 0,
			    right_width, top_height);
	if (tile) {
		xoff = x + width - left_width;
		yoff = y;
		paint (tile, cr, xoff, yoff);
		cairo_pattern_destroy (tile), tile = NULL;
	}

	/* right */
	tile = create_border (image, ORIENTATION_VERTICAL,
			      self->left_middle_right_vertical_tiling,
			      right_width, height - top_height - bottom_height,
			      left_width + middle_width, top_height, right_width, right_height);
	if (tile) {
		xoff = x + width - left_width;
		yoff = y + top_height;
		paint (tile, cr, xoff, yoff);
		cairo_pattern_destroy (tile), tile = NULL;
	}

	/* bottom-right */
	tile = create_tile (image,
			    left_width + bottom_width, top_height + right_height,
			    right_width, bottom_height);
	if (tile) {
		xoff = x + width - left_width;
		yoff = y + height - bottom_height;
		paint (tile, cr, xoff, yoff);
		cairo_pattern_destroy (tile), tile = NULL;
	}

	/* bottom */
	tile = create_border (image, ORIENTATION_HORIZONTAL,
			      self->top_middle_bottom_horizontal_tiling,
			      width - left_width - right_width, bottom_height,
			      left_width, top_height + middle_height, bottom_width, bottom_height);
	if (tile) {
		xoff = x + left_width;
		yoff = y + height - bottom_height;
		paint (tile, cr, xoff, yoff);
		cairo_pattern_destroy (tile), tile = NULL;
	}

	/* bottom-left */
	tile = create_tile (image,
			    0, top_height + left_height,
			    left_width, bottom_height);
	if (tile) {
		xoff = x;
		yoff = y + height - bottom_height;
		paint (tile, cr, xoff, yoff);
		cairo_pattern_destroy (tile), tile = NULL;
	}

	/* left */
	tile = create_border (image, ORIENTATION_VERTICAL,
			      self->left_middle_right_vertical_tiling,
			      left_width, height - top_height - bottom_height,
			      0, top_height, left_width, left_height);
	if (tile) {
		xoff = x;
		yoff = y + top_height;
		paint (tile, cr, xoff, yoff);
		cairo_pattern_destroy (tile), tile = NULL;
	}

	/* middle */
	tile = create_middle (image, 
			      self->top_middle_bottom_horizontal_tiling,
			      self->left_middle_right_vertical_tiling,
			      width - left_width - right_width,
			      height - top_height - bottom_height,
			      left_width, top_height,
			      middle_width, middle_height);
	if (tile) {
		xoff = x + left_width;
		yoff = y + top_height;
		paint (tile, cr, xoff, yoff);
		cairo_pattern_destroy (tile), tile = NULL;
	}
}

