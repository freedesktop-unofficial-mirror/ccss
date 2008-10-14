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

#define CCSS_PROPERTY_BACKGROUND_ATTACHMENT		ccss_property_background_attachment()
#define CCSS_PROPERTY_BACKGROUND_COLOR			ccss_property_background_color()
#define CCSS_PROPERTY_BACKGROUND_IMAGE			ccss_property_background_image()
#define CCSS_PROPERTY_BACKGROUND_POSITION		ccss_property_background_position()
#define CCSS_PROPERTY_BACKGROUND_REPEAT			ccss_property_background_repeat()
#define CCSS_PROPERTY_BACKGROUND_SIZE			ccss_property_background_size()

#define CCSS_PROPERTY_BORDER_BOTTOM_COLOR		ccss_property_border_bottom_color()
#define CCSS_PROPERTY_BORDER_BOTTOM_STYLE		ccss_property_border_bottom_style()
#define CCSS_PROPERTY_BORDER_BOTTOM_WIDTH		ccss_property_border_bottom_width()

#define CCSS_PROPERTY_BORDER_LEFT_COLOR			ccss_property_border_left_color()
#define CCSS_PROPERTY_BORDER_LEFT_STYLE			ccss_property_border_left_style()
#define CCSS_PROPERTY_BORDER_LEFT_WIDTH			ccss_property_border_left_width()

#define CCSS_PROPERTY_BORDER_RIGHT_COLOR		ccss_property_border_right_color()
#define CCSS_PROPERTY_BORDER_RIGHT_STYLE		ccss_property_border_right_style()
#define CCSS_PROPERTY_BORDER_RIGHT_WIDTH		ccss_property_border_right_width()

#define CCSS_PROPERTY_BORDER_TOP_COLOR			ccss_property_border_top_color()
#define CCSS_PROPERTY_BORDER_TOP_STYLE			ccss_property_border_top_style()
#define CCSS_PROPERTY_BORDER_TOP_WIDTH			ccss_property_border_top_width()

#define CCSS_PROPERTY_BORDER_TOP_LEFT_RADIUS		ccss_property_border_top_left_radius()
#define CCSS_PROPERTY_BORDER_TOP_RIGHT_RADIUS		ccss_property_border_top_right_radius()
#define CCSS_PROPERTY_BORDER_BOTTOM_RIGHT_RADIUS	ccss_property_border_bottom_right_radius()
#define CCSS_PROPERTY_BORDER_BOTTOM_LEFT_RADIUS		ccss_property_border_bottom_left_radius()

#define CCSS_PROPERTY_COLOR				ccss_property_color()

#define CCSS_PROPERTY_DUMP_PREFIX "    "

/* TODO maybe make this flags, include a CCSS_PROPERTY_SPEC_RESOLVED
 * and resolve in ccss_style_init(). */
typedef enum {
	CCSS_PROPERTY_SPEC_UNSET = 0,
	CCSS_PROPERTY_SPEC_NONE,
	CCSS_PROPERTY_SPEC_INHERIT,
	CCSS_PROPERTY_SPEC_SET
} ccss_property_spec_t;

typedef enum {
	CCSS_PROPERTY_TYPE_DOUBLE,
	CCSS_PROPERTY_TYPE_STRING
} ccss_property_type_t;

typedef bool (*ccss_property_convert_f) (void const *property,
					 ccss_property_type_t target,
					 void *value);

void ccss_property_init		(void);
void ccss_property_shutdown	(void);


void ccss_property_register_conversion_function (GQuark property,
						 ccss_property_convert_f function);

bool ccss_property_convert (void const *property, GQuark property_id,
			    ccss_property_type_t target, void *value);

ccss_property_spec_t ccss_property_parse_spec (CRTerm const **value);

bool ccss_property_is_set (ccss_property_spec_t property);

GQuark ccss_property_background_attachment (void);
GQuark ccss_property_background_color (void);
GQuark ccss_property_background_image (void);
GQuark ccss_property_background_position (void);
GQuark ccss_property_background_repeat (void);
GQuark ccss_property_background_size (void);

GQuark ccss_property_border_bottom_color (void);
GQuark ccss_property_border_bottom_style (void);
GQuark ccss_property_border_bottom_width (void);

GQuark ccss_property_border_left_color (void);
GQuark ccss_property_border_left_style (void);
GQuark ccss_property_border_left_width (void);

GQuark ccss_property_border_right_color (void);
GQuark ccss_property_border_right_style (void);
GQuark ccss_property_border_right_width (void);

GQuark ccss_property_border_top_color (void);
GQuark ccss_property_border_top_style (void);
GQuark ccss_property_border_top_width (void);

GQuark ccss_property_border_top_left_radius (void);
GQuark ccss_property_border_top_right_radius (void);
GQuark ccss_property_border_bottom_right_radius (void);
GQuark ccss_property_border_bottom_left_radius (void);

GQuark ccss_property_color (void);

#ifdef CCSS_DEBUG
void ccss_property_spec_dump (ccss_property_spec_t const spec);
#endif

G_END_DECLS

#endif /* CCSS_PROPERTY_H */

