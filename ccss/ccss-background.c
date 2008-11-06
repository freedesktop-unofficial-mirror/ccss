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

#include <math.h>
#include <string.h>
#include <glib.h>
#include "ccss-background.h"

static cairo_pattern_t *
create_pattern (ccss_background_image_t const	*bg_image,
		double				 width,
		double				 height,
		double				 tile_width,
		double				 tile_height)
{
	cairo_t		*cr;
	cairo_surface_t	*surface;
	cairo_status_t	 status;
	cairo_pattern_t *pattern;

	g_return_val_if_fail (bg_image && bg_image->image.pattern, NULL);

	/* Setup. */
	surface = NULL;
	status = cairo_pattern_get_surface (bg_image->image.pattern, &surface);
	if (status != CAIRO_STATUS_SUCCESS) {
		g_warning ("%s", cairo_status_to_string (status));
		return NULL;
	}

	surface = cairo_surface_create_similar (surface,
						CAIRO_CONTENT_COLOR_ALPHA,
						width, height);
	cr = cairo_create (surface);

	/* Drawing. */
	cairo_pattern_set_extend (bg_image->image.pattern, CAIRO_EXTEND_REPEAT);
	cairo_set_source (cr, bg_image->image.pattern);
	cairo_paint (cr);

	/* Cleanup. */
	pattern = cairo_pattern_create_for_surface (surface);
	cairo_destroy (cr), cr = NULL;
	cairo_surface_destroy (surface), surface = NULL;

	return pattern;
}

static void
repeat (ccss_background_image_t const	*bg_image,
	cairo_t				*cr,
	int32_t				 width,
	int32_t				 height,
	double				 tile_width,
	double				 tile_height)
{
	cairo_pattern_t	*pattern;

	/* Create pattern for (width + tile_width, height + tile_height)
	 * so we can account for `background-position'. */
	pattern = create_pattern (bg_image,
				  width + tile_width, height + tile_height,
				  tile_width, tile_height);
	g_return_if_fail (pattern);

	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_NONE);
	cairo_set_source (cr, pattern);
	cairo_pattern_destroy (pattern), pattern = NULL;
	cairo_fill_preserve (cr);
}

static void
repeat_x (ccss_background_image_t const	*bg_image,
	  cairo_t			*cr,
	  int32_t			 width,
	  int32_t			 height,
	  double			 tile_width,
	  double			 tile_height)
{
	cairo_pattern_t	*pattern;

	/* Create pattern for (width + tile_width, tile_height)
	 * so we can account for `background-position'. */
	pattern = create_pattern (bg_image,
				  width + tile_width, tile_height,
				  tile_width, tile_height);
	g_return_if_fail (pattern);

	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_NONE);
	cairo_set_source (cr, pattern);
	cairo_pattern_destroy (pattern), pattern = NULL;
	cairo_fill_preserve (cr);
}

static void
repeat_y (ccss_background_image_t const	*bg_image,
	  cairo_t			*cr,
	  int32_t			 width,
	  int32_t			 height,
	  double			 tile_width,
	  double			 tile_height)
{
	cairo_pattern_t	*pattern;

	/* Create pattern for (tile_width, height + tile_height)
	 * so we can account for `background-position'. */
	pattern = create_pattern (bg_image,
				  tile_width, height + tile_height,
				  tile_width, tile_height);
	g_return_if_fail (pattern);

	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_NONE);
	cairo_set_source (cr, pattern);
	cairo_pattern_destroy (pattern), pattern = NULL;
	cairo_fill_preserve (cr);
}

static void
no_repeat (ccss_background_image_t const	*bg_image,
	   cairo_t			*cr,
	   int32_t			 width,
	   int32_t			 height)
{
	cairo_pattern_set_extend (bg_image->image.pattern, CAIRO_EXTEND_NONE);
	cairo_set_source (cr, bg_image->image.pattern);
	cairo_fill_preserve (cr);
}

/**
 * ccss_background_fill:
 *
 * This function requires that a closed path exists, which is filled with 
 * the background. The path is not modified.
 **/
void
ccss_background_fill (ccss_background_attachment_t const	*bg_attachment, 
		     ccss_color_t const			*bg_color,
		     ccss_background_image_t const	*bg_image,
		     ccss_background_position_t const	*bg_position,
		     ccss_background_repeat_t const	*bg_repeat,
		     ccss_background_size_t const	*bg_size,
		     cairo_t				*cr,
		     int32_t				 x,
		     int32_t				 y, 
		     int32_t				 width,
		     int32_t				 height)
{
	cairo_status_t	status;
	double		dx;
	double		dy;

	g_return_if_fail (bg_attachment);
	/* FIXME, we need "transparent" color g_return_if_fail (bg_color); */
	g_return_if_fail (bg_image);
	g_return_if_fail (bg_position);
	g_return_if_fail (bg_repeat);
	g_return_if_fail (bg_size);

	cairo_save (cr);

	if (bg_color && bg_color->spec == CCSS_PROPERTY_SPEC_SET) {

		cairo_set_source_rgb (cr, bg_color->red, 
					  bg_color->green, 
					  bg_color->blue);
		cairo_fill_preserve (cr);
	} else {
		g_warning ("Invalid `background-color'");
#ifdef CCSS_DEBUG
		ccss_color_dump (bg_color);
		printf ("\n\n");
#endif
	}

	if (bg_image && bg_image->spec == CCSS_PROPERTY_SPEC_SET) {

		double tile_width;
		double tile_height;
		double xoff;
		double yoff;

		tile_width = ccss_position_get_hsize (&bg_size->width, 
						     width, height,
						     bg_image->image.width,
						     bg_image->image.height);
		tile_height = ccss_position_get_vsize (&bg_size->height,
						      width, height,
						      bg_image->image.width,
						      bg_image->image.height);

		xoff = ccss_position_get_pos (&bg_position->hpos,
					     width, tile_width);
		yoff = ccss_position_get_pos (&bg_position->vpos,
					     height, tile_height);

		dx = tile_width / bg_image->image.width;
		dy = tile_height / bg_image->image.height;

		switch (bg_repeat->repeat) {
		case CCSS_BACKGROUND_REPEAT:
			/* Normalise offsets, we don't create an infinite
			 * background pattern. */
			xoff = fmod (xoff, tile_width) - tile_width;
			yoff = fmod (yoff, tile_height) - tile_height;
			/* FIXME: not rounding can make the background edges
			 * blurry, but when rounding we might be 1px off.
			cairo_translate (cr, x + lround (xoff), y + lround (yoff)); */
			cairo_translate (cr, x + xoff, y + yoff);
			cairo_scale (cr, dx, dy);
			repeat (bg_image, cr, width / dx, height / dy,
				tile_width / dx, tile_height / dy);
			break;
		case CCSS_BACKGROUND_REPEAT_X:
			/* Normalise vertical offset, we don't create an infinite
			 * background pattern. */
			xoff = fmod (xoff, tile_width) - tile_width;
			/* FIXME: not rounding can make the background edges
			 * blurry, but when rounding we might be 1px off.
			cairo_translate (cr, x + lround (xoff), y + lround (yoff)); */
			cairo_translate (cr, x + xoff, y + yoff);
			cairo_scale (cr, dx, dy);
			repeat_x (bg_image, cr, width / dx, height / dy,
				  tile_width / dx, tile_height / dy);
			break;
		case CCSS_BACKGROUND_REPEAT_Y:
			/* Normalise horizontal offset, we don't create an infinite
			 * background pattern. */
			yoff = fmod (yoff, tile_height) - tile_height;
			/* FIXME: not rounding can make the background edges
			 * blurry, but when rounding we might be 1px off.
			cairo_translate (cr, x + lround (xoff), y + lround (yoff)); */
			cairo_translate (cr, x + xoff, y + yoff);
			cairo_scale (cr, dx, dy);
			repeat_y (bg_image, cr, width / dx, height / dy,
				  tile_width / dx, tile_height / dy);
			break;
		case CCSS_BACKGROUND_NO_REPEAT:
			/* FIXME: not rounding can make the background edges
			 * blurry, but when rounding we might be 1px off.
			cairo_translate (cr, x + lround (xoff), y + lround (yoff)); */
			cairo_translate (cr, x + xoff, y + yoff);
			cairo_scale (cr, dx, dy);
			no_repeat (bg_image, cr, width / dx, height / dy);
			break;
		default:
			g_assert_not_reached ();
			/* Need some code here when building w/o assertions. */
			break;
		}

		status = cairo_status (cr);
		if (status != CAIRO_STATUS_SUCCESS) {
			g_warning ("%s", cairo_status_to_string (status));
		}
	} else {
		g_warning ("Invalid `background-image'");
#ifdef CCSS_DEBUG
// TODO		dump
//		printf ("\n\n");
#endif
	}

	cairo_restore (cr);
}

