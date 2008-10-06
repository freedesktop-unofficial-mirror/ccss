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

#ifndef CCSS_BACKGROUND_H
#define CCSS_BACKGROUND_H

#include <stdbool.h>
#include <stdint.h>
#include <cairo.h>
#include <glib.h>
#include <libcroco/libcroco.h>
#include <ccss/ccss-features.h>
#include <ccss/ccss-color.h>
#include <ccss/ccss-image.h>
#include <ccss/ccss-position.h>
#include <ccss/ccss-property.h>

G_BEGIN_DECLS

typedef struct {
	ccss_property_spec_t	spec;
	enum ccss_background_attachment {
		CCSS_BACKGROUND_SCROLL,
		CCSS_BACKGROUND_FIXED
	}			attachment;
} ccss_background_attachment_t;

typedef struct {
	ccss_property_spec_t	spec;
	ccss_image_t		image;
} ccss_background_image_t;

typedef struct {
	ccss_property_spec_t	spec;
	ccss_position_t		hpos;
	ccss_position_t		vpos;
} ccss_background_position_t;

typedef struct {
	ccss_property_spec_t	spec;
	enum ccss_background_repeat {
		CCSS_BACKGROUND_REPEAT = 0,
		CCSS_BACKGROUND_REPEAT_X,
		CCSS_BACKGROUND_REPEAT_Y,
		CCSS_BACKGROUND_NO_REPEAT
	}			repeat;
} ccss_background_repeat_t;

typedef struct {
	ccss_property_spec_t	spec;
	ccss_position_t		width;
	ccss_position_t		height;
} ccss_background_size_t;

typedef struct {
	ccss_background_attachment_t	bg_attachment;
	ccss_color_t			bg_color;
	ccss_background_image_t		bg_image;
	ccss_background_position_t	bg_position;
	ccss_background_repeat_t		bg_repeat;
	ccss_background_size_t		bg_size;
} ccss_background_t;

ccss_background_t *	ccss_background_new	(void);
void			ccss_background_free	(ccss_background_t *self);

bool ccss_background_parse (ccss_background_t *self, char const *property, 
			   CRTerm const *values);

void ccss_background_fill (ccss_background_attachment_t const *bg_attachment, 
			  ccss_color_t const *bg_color,
			  ccss_background_image_t const *bg_image,
			  ccss_background_position_t const *bg_position,
			  ccss_background_repeat_t const *bg_repeat,
			  ccss_background_size_t const *bg_size,
			  cairo_t *cr, int32_t x, int32_t y, 
			  int32_t width, int32_t height);

#ifdef CCSS_DEBUG
void ccss_background_dump	(ccss_background_t const *self);
void ccss_background_image_dump	(ccss_background_image_t const *self);
#endif

G_END_DECLS

#endif /* CCSS_BACKGROUND_H */

