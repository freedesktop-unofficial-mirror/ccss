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

#ifndef CCSS_BACKGROUND_H
#define CCSS_BACKGROUND_H

#include <ccss/ccss-macros.h>
#include <ccss/ccss-color.h>
#include <ccss/ccss-position.h>

CCSS_BEGIN_DECLS

/*
 * Background attachment.
 */

typedef enum {
	CCSS_BACKGROUND_SCROLL = 0,
	CCSS_BACKGROUND_FIXED
} ccss_background_attachment_type_t;

typedef struct ccss_background_attachment_ ccss_background_attachment_t;

ccss_background_attachment_type_t
ccss_background_attachment_get_attachment (ccss_background_attachment_t const *self);

/*
 * Background image.
 */

typedef struct ccss_background_image_ ccss_background_image_t;

char const *
ccss_background_image_get_uri (ccss_background_image_t const *self);

/*
 * Background position.
 */

typedef struct ccss_background_position_ ccss_background_position_t;

ccss_position_t const *
ccss_background_position_get_horizontal_position (ccss_background_position_t const *self);

ccss_position_t const *
ccss_background_position_get_vertical_position (ccss_background_position_t const *self);

typedef enum {
	CCSS_BACKGROUND_REPEAT = 0,
	CCSS_BACKGROUND_REPEAT_X,
	CCSS_BACKGROUND_REPEAT_Y,
	CCSS_BACKGROUND_NO_REPEAT
} ccss_background_repeat_type_t;

/*
 * Background repeat.
 */

typedef struct ccss_background_repeat_ ccss_background_repeat_t;

ccss_background_repeat_type_t
ccss_background_repeat_get_repeat (ccss_background_repeat_t const *self);

/*
 * Background size.
 */

typedef struct ccss_background_size_ ccss_background_size_t;

ccss_position_t const *
ccss_background_size_get_height (ccss_background_size_t const *self);

ccss_position_t const *
ccss_background_size_get_width (ccss_background_size_t const *self);

/*
 * Background.
 */

typedef struct ccss_background_ ccss_background_t;

ccss_background_attachment_t const *
ccss_background_get_attachment (ccss_background_t const *self);

ccss_color_t const *
ccss_background_get_color (ccss_background_t const *self);

ccss_background_image_t const *
ccss_background_get_image (ccss_background_t const *self);

ccss_background_position_t const *
ccss_background_get_position (ccss_background_t const *self);

ccss_background_repeat_t const *
ccss_background_get_repeat (ccss_background_t const *self);

ccss_background_size_t const *
ccss_background_get_size (ccss_background_t const *self);

CCSS_END_DECLS

#endif /* CCSS_BACKGROUND_H */

