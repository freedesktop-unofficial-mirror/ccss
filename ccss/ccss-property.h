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

#ifndef CCSS_PROPERTY_H
#define CCSS_PROPERTY_H

#include <glib.h>
#include <stdbool.h>
#include <libcroco/libcroco.h>
#include <ccss/ccss-features.h>

G_BEGIN_DECLS

#define CCSS_PROPERTY_DUMP_PREFIX "    "

typedef enum {
	CCSS_PROPERTY_BACKGROUND = 0,
	CCSS_PROPERTY_BACKGROUND_ATTACHMENT,
	CCSS_PROPERTY_BACKGROUND_COLOR,
	CCSS_PROPERTY_BACKGROUND_IMAGE,
	CCSS_PROPERTY_BACKGROUND_POSITION,
	CCSS_PROPERTY_BACKGROUND_REPEAT,
	CCSS_PROPERTY_BACKGROUND_SIZE,
	CCSS_PROPERTY_BORDER,
	CCSS_PROPERTY_BORDER_COLOR,
	CCSS_PROPERTY_BORDER_STYLE,
	CCSS_PROPERTY_BORDER_WIDTH,
	CCSS_PROPERTY_BORDER_BOTTOM,
	CCSS_PROPERTY_BORDER_BOTTOM_COLOR,
	CCSS_PROPERTY_BORDER_BOTTOM_STYLE,
	CCSS_PROPERTY_BORDER_BOTTOM_WIDTH,
	CCSS_PROPERTY_BORDER_LEFT,
	CCSS_PROPERTY_BORDER_LEFT_COLOR,
	CCSS_PROPERTY_BORDER_LEFT_STYLE,
	CCSS_PROPERTY_BORDER_LEFT_WIDTH,
	CCSS_PROPERTY_BORDER_RIGHT,
	CCSS_PROPERTY_BORDER_RIGHT_COLOR,
	CCSS_PROPERTY_BORDER_RIGHT_STYLE,
	CCSS_PROPERTY_BORDER_RIGHT_WIDTH,
	CCSS_PROPERTY_BORDER_TOP,
	CCSS_PROPERTY_BORDER_TOP_COLOR,
	CCSS_PROPERTY_BORDER_TOP_STYLE,
	CCSS_PROPERTY_BORDER_TOP_WIDTH,
	CCSS_PROPERTY_BORDER_RADIUS,
	CCSS_PROPERTY_BORDER_TOP_LEFT_RADIUS,
	CCSS_PROPERTY_BORDER_TOP_RIGHT_RADIUS,
	CCSS_PROPERTY_BORDER_BOTTOM_RIGHT_RADIUS,
	CCSS_PROPERTY_BORDER_BOTTOM_LEFT_RADIUS,
	CCSS_PROPERTY_COLOR
} ccss_property_t;

/* TODO maybe make this flags, include a CCSS_PROPERTY_SPEC_RESOLVED
 * and resolve in ccss_style_init(). */
typedef enum {
	CCSS_PROPERTY_SPEC_UNSET = 0,
	CCSS_PROPERTY_SPEC_NONE,
	CCSS_PROPERTY_SPEC_INHERIT,
	CCSS_PROPERTY_SPEC_SET
} ccss_property_spec_t;

ccss_property_spec_t ccss_property_parse_spec (CRTerm const **value);

bool ccss_property_is_set (ccss_property_spec_t property);

#ifdef CCSS_DEBUG
void ccss_property_spec_dump (ccss_property_spec_t const spec);
#endif

G_END_DECLS

#endif /* CCSS_PROPERTY_H */

