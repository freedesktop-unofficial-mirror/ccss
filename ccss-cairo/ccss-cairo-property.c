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

#include "ccss-cairo-property.h"
#include "config.h"

static GQuark _BACKGROUND_ATTACHMENT		= 0;
static GQuark _BACKGROUND_COLOR			= 0;
static GQuark _BACKGROUND_IMAGE			= 0;
static GQuark _BACKGROUND_POSITION		= 0;
static GQuark _BACKGROUND_REPEAT		= 0;
static GQuark _BACKGROUND_SIZE			= 0;

static GQuark _BORDER_COLOR			= 0;
static GQuark _BORDER_STYLE			= 0;
static GQuark _BORDER_WIDTH			= 0;

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

static GQuark _BORDER_RADIUS			= 0;

static GQuark _BORDER_TOP_LEFT_RADIUS		= 0;
static GQuark _BORDER_TOP_RIGHT_RADIUS		= 0;
static GQuark _BORDER_BOTTOM_LEFT_RADIUS	= 0;
static GQuark _BORDER_BOTTOM_RIGHT_RADIUS	= 0;

static GQuark _BORDER_IMAGE			= 0;

static GQuark _COLOR				= 0;

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
ccss_property_border_color (void)
{
	if (0 == _BORDER_COLOR)
		_BORDER_COLOR = g_quark_from_static_string ("border-color");

	return _BORDER_COLOR;
}

GQuark
ccss_property_border_style (void)
{
	if (0 == _BORDER_STYLE)
		_BORDER_STYLE = g_quark_from_static_string ("border-style");

	return _BORDER_STYLE;
}

GQuark
ccss_property_border_width (void)
{
	if (0 == _BORDER_WIDTH)
		_BORDER_WIDTH = g_quark_from_static_string ("border-width");

	return _BORDER_WIDTH;
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
ccss_property_border_radius (void)
{
	if (0 == _BORDER_RADIUS)
		_BORDER_RADIUS = g_quark_from_static_string ("border-radius");

	return _BORDER_RADIUS;
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
ccss_property_border_bottom_left_radius (void)
{
	if (0 == _BORDER_BOTTOM_LEFT_RADIUS)
		_BORDER_BOTTOM_LEFT_RADIUS = g_quark_from_static_string ("border-bottom-left-radius");

	return _BORDER_BOTTOM_LEFT_RADIUS;
}

GQuark
ccss_property_border_bottom_right_radius (void)
{
	if (0 == _BORDER_BOTTOM_RIGHT_RADIUS)
		_BORDER_BOTTOM_RIGHT_RADIUS = g_quark_from_static_string ("border-bottom-right-radius");

	return _BORDER_BOTTOM_RIGHT_RADIUS;
}

GQuark
ccss_property_color (void)
{
	if (0 == _COLOR)
		_COLOR = g_quark_from_static_string ("color");

	return _COLOR;
}

GQuark
ccss_property_border_image (void)
{
	if (0 == _BORDER_IMAGE)
		_BORDER_IMAGE = g_quark_from_static_string ("border-image");

	return _BORDER_IMAGE;
}

