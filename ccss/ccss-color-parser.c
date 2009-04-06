/* vim: set ts=8 sw=8 noexpandtab: */

/* The `C' CSS Library.
 * Copyright (C) 2008 Robert Staudinger
 *
 * Functions parse_hex() and  hex() were derived from pango-1.21.3,
 * Copyright (C) 2000  Red Hat Software, License:  LGPL 2 or later.
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

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "ccss-color-parser.h"
#include "config.h"

static ccss_property_class_t const *
peek_property_class (void);

/* PONDERING: point to property_class with no-op free()? */

static const struct {
	char const		*name;
	const ccss_color_t	 color;
} _color_map[] = {
  { "aliceblue",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xf0/255., 0xf8/255., 0xff/255. } },
  { "antiquewhite",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xfa/255., 0xeb/255., 0xd7/255. } },
  { "aqua",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x00/255., 0xff/255., 0xff/255. } },
  { "aquamarine",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x7f/255., 0xff/255., 0xd4/255. } },
  { "azure",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xf0/255., 0xff/255., 0xff/255. } },
  { "beige",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xf5/255., 0xf5/255., 0xdc/255. } },
  { "bisque",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0xe4/255., 0xc4/255. } },
  { "black",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x00/255., 0x00/255., 0x00/255. } },
  { "blanchedalmond",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0xeb/255., 0xcd/255. } },
  { "blue",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x00/255., 0x00/255., 0xff/255. } },
  { "blueviolet",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x8a/255., 0x2b/255., 0xe2/255. } },
  { "brown",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xa5/255., 0x2a/255., 0x2a/255. } },
  { "burlywood",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xde/255., 0xb8/255., 0x87/255. } },
  { "cadetblue",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x5f/255., 0x9e/255., 0xa0/255. } },
  { "chartreuse",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x7f/255., 0xff/255., 0x00/255. } },
  { "chocolate",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xd2/255., 0x69/255., 0x1e/255. } },
  { "coral",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0x7f/255., 0x50/255. } },
  { "cornflowerblue",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x64/255., 0x95/255., 0xed/255. } },
  { "cornsilk",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0xf8/255., 0xdc/255. } },
  { "crimson",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xdc/255., 0x14/255., 0x3c/255. } },
  { "cyan",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x00/255., 0xff/255., 0xff/255. } },
  { "darkblue",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x00/255., 0x00/255., 0x8b/255. } },
  { "darkcyan",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x00/255., 0x8b/255., 0x8b/255. } },
  { "darkgoldenrod",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xb8/255., 0x86/255., 0x0b/255. } },
  { "darkgray",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xa9/255., 0xa9/255., 0xa9/255. } },
  { "darkgrey",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xa9/255., 0xa9/255., 0xa9/255. } },
  { "darkgreen",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x00/255., 0x64/255., 0x00/255. } },
  { "darkkhaki",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xbd/255., 0xb7/255., 0x6b/255. } },
  { "darkmagenta",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x8b/255., 0x00/255., 0x8b/255. } },
  { "darkolivegreen",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x55/255., 0x6b/255., 0x2f/255. } },
  { "darkorange",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0x8c/255., 0x00/255. } },
  { "darkorchid",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x99/255., 0x32/255., 0xcc/255. } },
  { "darkred",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x8b/255., 0x00/255., 0x00/255. } },
  { "darksalmon",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xe9/255., 0x96/255., 0x7a/255. } },
  { "darkseagreen",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x8f/255., 0xbc/255., 0x8f/255. } },
  { "darkslateblue",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x48/255., 0x3d/255., 0x8b/255. } },
  { "darkslategray",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x2f/255., 0x4f/255., 0x4f/255. } },
  { "darkslategrey",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x2f/255., 0x4f/255., 0x4f/255. } },
  { "darkturquoise",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x00/255., 0xce/255., 0xd1/255. } },
  { "darkviolet",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x94/255., 0x00/255., 0xd3/255. } },
  { "deeppink",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0x14/255., 0x93/255. } },
  { "deepskyblue",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x00/255., 0xbf/255., 0xff/255. } },
  { "dimgray",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x69/255., 0x69/255., 0x69/255. } },
  { "dimgrey",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x69/255., 0x69/255., 0x69/255. } },
  { "dodgerblue",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x1e/255., 0x90/255., 0xff/255. } },
  { "firebrick",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xb2/255., 0x22/255., 0x22/255. } },
  { "floralwhite",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0xfa/255., 0xf0/255. } },
  { "forestgreen",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x22/255., 0x8b/255., 0x22/255. } },
  { "fuchsia",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0x00/255., 0xff/255. } },
  { "gainsboro",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xdc/255., 0xdc/255., 0xdc/255. } },
  { "ghostwhite",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xf8/255., 0xf8/255., 0xff/255. } },
  { "gold",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0xd7/255., 0x00/255. } },
  { "goldenrod",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xda/255., 0xa5/255., 0x20/255. } },
  { "gray",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x80/255., 0x80/255., 0x80/255. } },
  { "grey",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x80/255., 0x80/255., 0x80/255. } },
  { "green",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x00/255., 0x80/255., 0x00/255. } },
  { "greenyellow",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xad/255., 0xff/255., 0x2f/255. } },
  { "honeydew",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xf0/255., 0xff/255., 0xf0/255. } },
  { "hotpink",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0x69/255., 0xb4/255. } },
  { "indianred",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xcd/255., 0x5c/255., 0x5c/255. } },
  { "indigo",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x4b/255., 0x00/255., 0x82/255. } },
  { "ivory",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0xff/255., 0xf0/255. } },
  { "khaki",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xf0/255., 0xe6/255., 0x8c/255. } },
  { "lavender",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xe6/255., 0xe6/255., 0xfa/255. } },
  { "lavenderblush",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0xf0/255., 0xf5/255. } },
  { "lawngreen",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x7c/255., 0xfc/255., 0x00/255. } },
  { "lemonchiffon",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0xfa/255., 0xcd/255. } },
  { "lightblue",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xad/255., 0xd8/255., 0xe6/255. } },
  { "lightcoral",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xf0/255., 0x80/255., 0x80/255. } },
  { "lightcyan",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xe0/255., 0xff/255., 0xff/255. } },
  { "lightgoldenrodyellow",	{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xfa/255., 0xfa/255., 0xd2/255. } },
  { "lightgray",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xd3/255., 0xd3/255., 0xd3/255. } },
  { "lightgrey",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xd3/255., 0xd3/255., 0xd3/255. } },
  { "lightgreen",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x90/255., 0xee/255., 0x90/255. } },
  { "lightpink",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0xb6/255., 0xc1/255. } },
  { "lightsalmon",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0xa0/255., 0x7a/255. } },
  { "lightseagreen",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x20/255., 0xb2/255., 0xaa/255. } },
  { "lightskyblue",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x87/255., 0xce/255., 0xfa/255. } },
  { "lightslategray",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x77/255., 0x88/255., 0x99/255. } },
  { "lightslategrey",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x77/255., 0x88/255., 0x99/255. } },
  { "lightsteelblue",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xb0/255., 0xc4/255., 0xde/255. } },
  { "lightyellow",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0xff/255., 0xe0/255. } },
  { "lime",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x00/255., 0xff/255., 0x00/255. } },
  { "limegreen",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x32/255., 0xcd/255., 0x32/255. } },
  { "linen",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xfa/255., 0xf0/255., 0xe6/255. } },
  { "magenta",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0x00/255., 0xff/255. } },
  { "maroon",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x80/255., 0x00/255., 0x00/255. } },
  { "mediumaquamarine",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x66/255., 0xcd/255., 0xaa/255. } },
  { "mediumblue",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x00/255., 0x00/255., 0xcd/255. } },
  { "mediumorchid",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xba/255., 0x55/255., 0xd3/255. } },
  { "mediumpurple",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x93/255., 0x70/255., 0xd8/255. } },
  { "mediumseagreen",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x3c/255., 0xb3/255., 0x71/255. } },
  { "mediumslateblue",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x7b/255., 0x68/255., 0xee/255. } },
  { "mediumspringgreen",	{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x00/255., 0xfa/255., 0x9a/255. } },
  { "mediumturquoise",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x48/255., 0xd1/255., 0xcc/255. } },
  { "mediumvioletred",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xc7/255., 0x15/255., 0x85/255. } },
  { "midnightblue",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x19/255., 0x19/255., 0x70/255. } },
  { "mintcream",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xf5/255., 0xff/255., 0xfa/255. } },
  { "mistyrose",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0xe4/255., 0xe1/255. } },
  { "moccasin",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0xe4/255., 0xb5/255. } },
  { "navajowhite",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0xde/255., 0xad/255. } },
  { "navy",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x00/255., 0x00/255., 0x80/255. } },
  { "oldlace",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xfd/255., 0xf5/255., 0xe6/255. } },
  { "olive",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x80/255., 0x80/255., 0x00/255. } },
  { "olivedrab",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x6b/255., 0x8e/255., 0x23/255. } },
  { "orange",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0xa5/255., 0x00/255. } },
  { "orangered",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0x45/255., 0x00/255. } },
  { "orchid",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xda/255., 0x70/255., 0xd6/255. } },
  { "palegoldenrod",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xee/255., 0xe8/255., 0xaa/255. } },
  { "palegreen",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x98/255., 0xfb/255., 0x98/255. } },
  { "paleturquoise",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xaf/255., 0xee/255., 0xee/255. } },
  { "palevioletred",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xd8/255., 0x70/255., 0x93/255. } },
  { "papayawhip",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0xef/255., 0xd5/255. } },
  { "peachpuff",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0xda/255., 0xb9/255. } },
  { "peru",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xcd/255., 0x85/255., 0x3f/255. } },
  { "pink",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0xc0/255., 0xcb/255. } },
  { "plum",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xdd/255., 0xa0/255., 0xdd/255. } },
  { "powderblue",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xb0/255., 0xe0/255., 0xe6/255. } },
  { "purple",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x80/255., 0x00/255., 0x80/255. } },
  { "red",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0x00/255., 0x00/255. } },
  { "rosybrown",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xbc/255., 0x8f/255., 0x8f/255. } },
  { "royalblue",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x41/255., 0x69/255., 0xe1/255. } },
  { "saddlebrown",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x8b/255., 0x45/255., 0x13/255. } },
  { "salmon",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xfa/255., 0x80/255., 0x72/255. } },
  { "sandybrown",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xf4/255., 0xa4/255., 0x60/255. } },
  { "seagreen",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x2e/255., 0x8b/255., 0x57/255. } },
  { "seashell",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0xf5/255., 0xee/255. } },
  { "sienna",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xa0/255., 0x52/255., 0x2d/255. } },
  { "silver",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xc0/255., 0xc0/255., 0xc0/255. } },
  { "skyblue",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x87/255., 0xce/255., 0xeb/255. } },
  { "slateblue",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x6a/255., 0x5a/255., 0xcd/255. } },
  { "slategray",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x70/255., 0x80/255., 0x90/255. } },
  { "slategrey",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x70/255., 0x80/255., 0x90/255. } },
  { "snow",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0xfa/255., 0xfa/255. } },
  { "springgreen",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x00/255., 0xff/255., 0x7f/255. } },
  { "steelblue",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x46/255., 0x82/255., 0xb4/255. } },
  { "tan",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xd2/255., 0xb4/255., 0x8c/255. } },
  { "teal",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x00/255., 0x80/255., 0x80/255. } },
  { "thistle",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xd8/255., 0xbf/255., 0xd8/255. } },
  { "tomato",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0x63/255., 0x47/255. } },
  { "turquoise",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x40/255., 0xe0/255., 0xd0/255. } },
  { "violet",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xee/255., 0x82/255., 0xee/255. } },
  { "wheat",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xf5/255., 0xde/255., 0xb3/255. } },
  { "white",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0xff/255., 0xff/255. } },
  { "whitesmoke",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xf5/255., 0xf5/255., 0xf5/255. } },
  { "yellow",			{ { NULL, CCSS_PROPERTY_STATE_SET }, 0xff/255., 0xff/255., 0x00/255. } },
  { "yellowgreen",		{ { NULL, CCSS_PROPERTY_STATE_SET }, 0x9a/255., 0xcd/255., 0x32/255. } },
};

static bool
parse_name (ccss_color_t	*self,
	    char const	*css_color_name)
{
	g_return_val_if_fail (css_color_name && self, false);

	for (unsigned int i = 0; i < G_N_ELEMENTS (_color_map); i++) {
		if (0 == g_ascii_strcasecmp (_color_map[i].name, css_color_name)) {
			self->red = _color_map[i].color.red;
			self->green = _color_map[i].color.green;
			self->blue = _color_map[i].color.blue;
			return true;
		}
	}

	return false;
}

static gboolean
hex (char const		*color,
     int		 len,
     unsigned int	*c)
{
	const char *end;

	*c = 0;
	for (end = color + len; color != end; color++) {
		if (g_ascii_isxdigit (*color)) {
			*c = (*c << 4) | g_ascii_xdigit_value (*color);
		} else {
			return false;
		}
	}

	return true;
}

static bool
parse_hex (ccss_color_t	*self,
	   char const	*color)
{
	size_t		len;
	uint32_t	result;
	unsigned int	r, g, b;

	g_return_val_if_fail (color, false);

	len = strlen (color);

	/* With alpha channel. */
	if (sscanf (color + 1, "%x", &result)) {
		if (len == 8) {
			/* #rrggbbaa */
			self->red   = (result >> 24) & 0xff;
			self->green = (result >> 16) & 0xff;
			self->blue  = (result >>  8) & 0xff;
			self->alpha = result & 0xff;
			return true;
		}
	}

	if (len % 3 || len < 3 || len > 12)
		return false;

	/* Without alpha channel. */
	len /= 3;

	if (!hex (color, len, &r) ||
	    !hex (color + len, len, &g) ||
	    !hex (color + len * 2, len, &b))
		return false;

	if (self) {
		int bits = len * 4;
		r <<= 16 - bits;
		g <<= 16 - bits;
		b <<= 16 - bits;
		while (bits < 16) {
			r |= (r >> bits);
			g |= (g >> bits);
			b |= (b >> bits);
			bits *= 2;
		}
		self->red   = r / 65535.;
		self->green = g / 65535.;
		self->blue  = b / 65535.;
	}

	return true;
}

static bool
parse_rgb (ccss_color_t *self,
	   CRTerm const *value)
{
	CRTerm const *iter;

	iter = value;

	/* "r" */
	if (iter &&
	    iter->type == TERM_NUMBER &&
	    iter->content.num->type == NUM_GENERIC) {
		self->red = CLAMP (iter->content.num->val, 0.0, 1.0);
		iter = iter->next;
	} else {
		return false;
	}

	/* "g" */
	if (iter &&
	    iter->type == TERM_NUMBER &&
	    iter->content.num->type == NUM_GENERIC) {
		self->green = CLAMP (iter->content.num->val, 0.0, 1.0);
		iter = iter->next;
	} else {
		return false;
	}

	/* "b" */
	if (iter &&
	    iter->type == TERM_NUMBER &&
	    iter->content.num->type == NUM_GENERIC) {
		self->blue = CLAMP (iter->content.num->val, 0.0, 1.0);
		iter = iter->next;
	} else {
		return false;
	}

	self->alpha = 1.0;

	self->base.state = CCSS_PROPERTY_STATE_SET;
	return true;
}

static bool
parse_rgba (ccss_color_t *self,
	    CRTerm const *value)
{
	CRTerm const *iter;

	iter = value;

	/* "r" */
	if (iter &&
	    iter->type == TERM_NUMBER &&
	    iter->content.num->type == NUM_GENERIC) {
		self->red = CLAMP (iter->content.num->val, 0.0, 1.0);
		iter = iter->next;
	} else {
		return false;
	}

	/* "g" */
	if (iter &&
	    iter->type == TERM_NUMBER &&
	    iter->content.num->type == NUM_GENERIC) {
		self->green = CLAMP (iter->content.num->val, 0.0, 1.0);
		iter = iter->next;
	} else {
		return false;
	}

	/* "b" */
	if (iter &&
	    iter->type == TERM_NUMBER &&
	    iter->content.num->type == NUM_GENERIC) {
		self->blue = CLAMP (iter->content.num->val, 0.0, 1.0);
		iter = iter->next;
	} else {
		return false;
	}

	/* "a" */
	if (iter &&
	    iter->type == TERM_NUMBER &&
	    iter->content.num->type == NUM_GENERIC) {
		self->alpha = CLAMP (iter->content.num->val, 0.0, 1.0);
		iter = iter->next;
	} else {
		return false;
	}

	self->base.state = CCSS_PROPERTY_STATE_SET;
	return true;
}

bool
ccss_color_parse (ccss_color_t	 *self,
		  CRTerm const	**value)
{
	char const	*str;
	bool		 ret;

	g_return_val_if_fail (self, false);

	if (!*value) {
		return false;
	}

	switch ((*value)->type) {
	case TERM_IDENT:
		self->base.state = ccss_property_parse_state (value);
		switch (self->base.state) {
		case CCSS_PROPERTY_STATE_INVALID:
			return false;
		case CCSS_PROPERTY_STATE_NONE:
		case CCSS_PROPERTY_STATE_INHERIT:
			return true;
		case CCSS_PROPERTY_STATE_SET:
			break;
		}
		str = cr_string_peek_raw_str ((*value)->content.str);
		ret = parse_name (self, str);
		if (ret) {
			*value = (*value)->next;
			return true;
		}
		return false;
	case TERM_HASH:
		str = cr_string_peek_raw_str ((*value)->content.str);
		ret = parse_hex (self, str);
		if (ret) {
			*value = (*value)->next;
			return true;
		}
		return false;
	case TERM_RGB:
		self->red = (*value)->content.rgb->red;
		self->green = (*value)->content.rgb->green;
		self->blue = (*value)->content.rgb->blue;
		*value = (*value)->next;
		return true;
	case TERM_FUNCTION:
		str = cr_string_peek_raw_str ((*value)->content.str);
		if (0 == g_strcmp0 (str, "rgb")) {
			return parse_rgb (self, (*value)->ext_content.func_param);
		} else if (0 == g_strcmp0 (str, "rgba")) {
			return parse_rgba (self, (*value)->ext_content.func_param);		
		} else {
			g_warning (G_STRLOC " '%s' not recognised.", str);
		}
	/* fall thru for all other enum values to prevent compiler warnings */
	case TERM_NO_TYPE:
	case TERM_NUMBER:
	case TERM_STRING:
	case TERM_URI:
	case TERM_UNICODERANGE:
		break;
	}

	return false;
}

ccss_property_base_t *
ccss_color_create (ccss_grammar_t const	*grammar,
		   CRTerm const		*value,
		   void			*user_data)
{
	ccss_color_t	*self, c;
	bool		 ret;

	/* Default to opaque. */
	c.alpha = 1.0;

	ret = ccss_color_parse (&c, &value);
	if (ret) {
		c.base.property_class = peek_property_class ();
		self = g_new0 (ccss_color_t, 1);
		*self = c;
		return &self->base;
	}

	return NULL;
}

void
ccss_color_destroy (ccss_color_t *self)
{
	g_return_if_fail (self);

	g_free (self);
}

bool
ccss_color_convert (ccss_color_t const		*property,
		    ccss_property_type_t	 target,
		    void			*value)
{
	g_return_val_if_fail (property && value, false);

	if (CCSS_PROPERTY_TYPE_DOUBLE == target)
		return false;

	* (char **) value = g_strdup_printf ("#%02x%02x%02x", 
						(int) (property->red * 255),
						(int) (property->green * 255),
						(int) (property->blue * 255));
	return true;
}

static ccss_property_class_t const _ptable[] = {
    {
	.name = "color",
	.property_create = ccss_color_create,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) ccss_color_convert,
	.property_factory = NULL,
	.property_inherit = NULL
    }, {
	.name = NULL
    }
};

static ccss_property_class_t const *
peek_property_class (void)
{
	return &_ptable[0];
}

ccss_property_class_t const *
ccss_color_get_ptable (void)
{
	return _ptable;
}

