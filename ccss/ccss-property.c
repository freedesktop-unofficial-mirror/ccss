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

#include <stdio.h>
#include <string.h>
#include "ccss-property.h"

static GQuark _BACKGROUND_ATTACHMENT		= 0;
static GQuark _BACKGROUND_COLOR			= 0;
static GQuark _BACKGROUND_IMAGE			= 0;
static GQuark _BACKGROUND_POSITION		= 0;
static GQuark _BACKGROUND_REPEAT		= 0;
static GQuark _BACKGROUND_SIZE			= 0;

static GQuark _BORDER_BOTTOM_COLOR		= 0;
static GQuark _BORDER_BOTTOM_STYLE		= 0;
static GQuark _BORDER_BOTTOM_WIDTH		= 0;

static GQuark _BORDER_LEFT_COLOR		= 0;
static GQuark _BORDER_LEFT_STYLE		= 0;
static GQuark _BORDER_LEFT_WIDTH		= 0;

static GQuark _BORDER_RIGHT_COLOR		= 0;
static GQuark _BORDER_RIGHT_STYLE		= 0;
static GQuark _BORDER_RIGHT_WIDTH		= 0;

static GQuark _BORDER_TOP_COLOR			= 0;
static GQuark _BORDER_TOP_STYLE			= 0;
static GQuark _BORDER_TOP_WIDTH			= 0;

static GQuark _BORDER_TOP_LEFT_RADIUS		= 0;
static GQuark _BORDER_TOP_RIGHT_RADIUS		= 0;
static GQuark _BORDER_BOTTOM_RIGHT_RADIUS	= 0;
static GQuark _BORDER_BOTTOM_LEFT_RADIUS	= 0;

static GQuark _COLOR				= 0;

ccss_property_spec_t
ccss_property_parse_spec (CRTerm const **value)
{
	char const *str;

	if (!*value) {
		return CCSS_PROPERTY_SPEC_UNSET;
	} else if (TERM_IDENT == (*value)->type) {
		str = cr_string_peek_raw_str ((*value)->content.str);
		if (0 == strcmp ("none", str)) {
			*value = (*value)->next;
			return CCSS_PROPERTY_SPEC_NONE;
		} else if (0 == strcmp ("inherit", str)) {
			g_warning ("Property `inherit' not supported yet.");
			*value = (*value)->next;
			return CCSS_PROPERTY_SPEC_INHERIT;
		}
	}

	return CCSS_PROPERTY_SPEC_SET;
}

bool
ccss_property_is_set (ccss_property_spec_t property)
{
	return CCSS_PROPERTY_SPEC_SET == property || 
	       CCSS_PROPERTY_SPEC_INHERIT == property;
}

GQuark
ccss_property_background_attachment (void)
{
	if (0 == _BACKGROUND_ATTACHMENT)
		_BACKGROUND_ATTACHMENT = g_quark_from_static_string ("background-attachment");

	return _BACKGROUND_ATTACHMENT;
}

GQuark
ccss_property_background_color (void)
{
	if (0 == _BACKGROUND_COLOR)
		_BACKGROUND_COLOR = g_quark_from_static_string ("background-color");

	return _BACKGROUND_COLOR;
}

GQuark
ccss_property_background_image (void)
{
	if (0 == _BACKGROUND_IMAGE)
		_BACKGROUND_IMAGE = g_quark_from_static_string ("background-image");

	return _BACKGROUND_IMAGE;
}

GQuark
ccss_property_background_position (void)
{
	if (0 == _BACKGROUND_POSITION)
		_BACKGROUND_POSITION = g_quark_from_static_string ("background-position");

	return _BACKGROUND_POSITION;
}

GQuark
ccss_property_background_repeat (void)
{
	if (0 == _BACKGROUND_REPEAT)
		_BACKGROUND_REPEAT = g_quark_from_static_string ("background-repeat");

	return _BACKGROUND_REPEAT;
}

GQuark
ccss_property_background_size (void)
{
	if (0 == _BACKGROUND_SIZE)
		_BACKGROUND_SIZE = g_quark_from_static_string ("background-size");

	return _BACKGROUND_SIZE;
}

GQuark
ccss_property_border_bottom_color (void)
{
	if (0 == _BORDER_BOTTOM_COLOR)
		_BORDER_BOTTOM_COLOR = g_quark_from_static_string ("border-bottom-color");

	return _BORDER_BOTTOM_COLOR;
}

GQuark
ccss_property_border_bottom_style (void)
{
	if (0 == _BORDER_BOTTOM_STYLE)
		_BORDER_BOTTOM_STYLE = g_quark_from_static_string ("border-bottom-style");

	return _BORDER_BOTTOM_STYLE;
}

GQuark
ccss_property_border_bottom_width (void)
{
	if (0 == _BORDER_BOTTOM_WIDTH)
		_BORDER_BOTTOM_WIDTH = g_quark_from_static_string ("border-bottom-width");

	return _BORDER_BOTTOM_WIDTH;
}

GQuark
ccss_property_border_left_color (void)
{
	if (0 == _BORDER_LEFT_COLOR)
		_BORDER_LEFT_COLOR = g_quark_from_static_string ("border-left-color");

	return _BORDER_LEFT_COLOR;
}

GQuark
ccss_property_border_left_style (void)
{
	if (0 == _BORDER_LEFT_STYLE)
		_BORDER_LEFT_STYLE = g_quark_from_static_string ("border-left-style");

	return _BORDER_LEFT_STYLE;
}

GQuark
ccss_property_border_left_width (void)
{
	if (0 == _BORDER_LEFT_WIDTH)
		_BORDER_LEFT_WIDTH = g_quark_from_static_string ("border-left-width");

	return _BORDER_LEFT_WIDTH;
}

GQuark
ccss_property_border_right_color (void)
{
	if (0 == _BORDER_RIGHT_COLOR)
		_BORDER_RIGHT_COLOR = g_quark_from_static_string ("border-right-color");

	return _BORDER_RIGHT_COLOR;
}

GQuark
ccss_property_border_right_style (void)
{
	if (0 == _BORDER_RIGHT_STYLE)
		_BORDER_RIGHT_STYLE = g_quark_from_static_string ("border-right-style");

	return _BORDER_RIGHT_STYLE;
}

GQuark
ccss_property_border_right_width (void)
{
	if (0 == _BORDER_RIGHT_WIDTH)
		_BORDER_RIGHT_WIDTH = g_quark_from_static_string ("border-right-width");

	return _BORDER_RIGHT_WIDTH;
}

GQuark
ccss_property_border_top_color (void)
{
	if (0 == _BORDER_TOP_COLOR)
		_BORDER_TOP_COLOR = g_quark_from_static_string ("border-top-color");

	return _BORDER_TOP_COLOR;
}

GQuark
ccss_property_border_top_style (void)
{
	if (0 == _BORDER_TOP_STYLE)
		_BORDER_TOP_STYLE = g_quark_from_static_string ("border-top-style");

	return _BORDER_TOP_STYLE;
}

GQuark
ccss_property_border_top_width (void)
{
	if (0 == _BORDER_TOP_WIDTH)
		_BORDER_TOP_WIDTH = g_quark_from_static_string ("border-top-width");

	return _BORDER_TOP_WIDTH;
}

GQuark
ccss_property_border_top_left_radius (void)
{
	if (0 == _BORDER_TOP_LEFT_RADIUS)
		_BORDER_TOP_LEFT_RADIUS = g_quark_from_static_string ("border-top-left-radius");

	return _BORDER_TOP_LEFT_RADIUS;
}

GQuark
ccss_property_border_top_right_radius (void)
{
	if (0 == _BORDER_TOP_RIGHT_RADIUS)
		_BORDER_TOP_RIGHT_RADIUS = g_quark_from_static_string ("border-top-right-radius");

	return _BORDER_TOP_RIGHT_RADIUS;
}

GQuark
ccss_property_border_bottom_right_radius (void)
{
	if (0 == _BORDER_BOTTOM_RIGHT_RADIUS)
		_BORDER_BOTTOM_RIGHT_RADIUS = g_quark_from_static_string ("border-bottom-left-radius");

	return _BORDER_BOTTOM_RIGHT_RADIUS;
}

GQuark
ccss_property_border_bottom_left_radius (void)
{
	if (0 == _BORDER_BOTTOM_LEFT_RADIUS)
		_BORDER_BOTTOM_LEFT_RADIUS = g_quark_from_static_string ("border-bottom-right-radius");

	return _BORDER_BOTTOM_LEFT_RADIUS;
}

GQuark
ccss_property_color (void)
{
	if (0 == _COLOR)
		_COLOR = g_quark_from_static_string ("color");

	return _COLOR;
}

#ifdef CCSS_DEBUG

void
ccss_property_spec_dump (ccss_property_spec_t const spec)
{
	switch (spec) {
	case CCSS_PROPERTY_SPEC_UNSET:
		printf ("<unset>");
		break;
	case CCSS_PROPERTY_SPEC_NONE:
		printf ("none");
		break;
	case CCSS_PROPERTY_SPEC_INHERIT:
		printf ("<inherit>");
		break;
	case CCSS_PROPERTY_SPEC_SET:
		/* Needs custom printing. */
		break;
	}
}

#endif /* CCSS_DEBUG */

