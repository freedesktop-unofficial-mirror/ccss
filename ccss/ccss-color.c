/* The Cairo CSS Drawing Library.
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
#include "ccss-color.h"
#include "ccss-function-priv.h"

static const struct {
	char const		*name;
	const ccss_color_t	color;
} _color_map[] = {
  { "aliceblue",		{ CCSS_PROPERTY_SPEC_SET, 0xf0/255., 0xf8/255., 0xff/255. } },
  { "antiquewhite",		{ CCSS_PROPERTY_SPEC_SET, 0xfa/255., 0xeb/255., 0xd7/255. } },
  { "aqua",			{ CCSS_PROPERTY_SPEC_SET, 0x00/255., 0xff/255., 0xff/255. } },
  { "aquamarine",		{ CCSS_PROPERTY_SPEC_SET, 0x7f/255., 0xff/255., 0xd4/255. } },
  { "azure",			{ CCSS_PROPERTY_SPEC_SET, 0xf0/255., 0xff/255., 0xff/255. } },
  { "beige",			{ CCSS_PROPERTY_SPEC_SET, 0xf5/255., 0xf5/255., 0xdc/255. } },
  { "bisque",			{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0xe4/255., 0xc4/255. } },
  { "black",			{ CCSS_PROPERTY_SPEC_SET, 0x00/255., 0x00/255., 0x00/255. } },
  { "blanchedalmond",		{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0xeb/255., 0xcd/255. } },
  { "blue",			{ CCSS_PROPERTY_SPEC_SET, 0x00/255., 0x00/255., 0xff/255. } },
  { "blueviolet",		{ CCSS_PROPERTY_SPEC_SET, 0x8a/255., 0x2b/255., 0xe2/255. } },
  { "brown",			{ CCSS_PROPERTY_SPEC_SET, 0xa5/255., 0x2a/255., 0x2a/255. } },
  { "burlywood",		{ CCSS_PROPERTY_SPEC_SET, 0xde/255., 0xb8/255., 0x87/255. } },
  { "cadetblue",		{ CCSS_PROPERTY_SPEC_SET, 0x5f/255., 0x9e/255., 0xa0/255. } },
  { "chartreuse",		{ CCSS_PROPERTY_SPEC_SET, 0x7f/255., 0xff/255., 0x00/255. } },
  { "chocolate",		{ CCSS_PROPERTY_SPEC_SET, 0xd2/255., 0x69/255., 0x1e/255. } },
  { "coral",			{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0x7f/255., 0x50/255. } },
  { "cornflowerblue",		{ CCSS_PROPERTY_SPEC_SET, 0x64/255., 0x95/255., 0xed/255. } },
  { "cornsilk",			{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0xf8/255., 0xdc/255. } },
  { "crimson",			{ CCSS_PROPERTY_SPEC_SET, 0xdc/255., 0x14/255., 0x3c/255. } },
  { "cyan",			{ CCSS_PROPERTY_SPEC_SET, 0x00/255., 0xff/255., 0xff/255. } },
  { "darkblue",			{ CCSS_PROPERTY_SPEC_SET, 0x00/255., 0x00/255., 0x8b/255. } },
  { "darkcyan",			{ CCSS_PROPERTY_SPEC_SET, 0x00/255., 0x8b/255., 0x8b/255. } },
  { "darkgoldenrod",		{ CCSS_PROPERTY_SPEC_SET, 0xb8/255., 0x86/255., 0x0b/255. } },
  { "darkgray",			{ CCSS_PROPERTY_SPEC_SET, 0xa9/255., 0xa9/255., 0xa9/255. } },
  { "darkgrey",			{ CCSS_PROPERTY_SPEC_SET, 0xa9/255., 0xa9/255., 0xa9/255. } },
  { "darkgreen",		{ CCSS_PROPERTY_SPEC_SET, 0x00/255., 0x64/255., 0x00/255. } },
  { "darkkhaki",		{ CCSS_PROPERTY_SPEC_SET, 0xbd/255., 0xb7/255., 0x6b/255. } },
  { "darkmagenta",		{ CCSS_PROPERTY_SPEC_SET, 0x8b/255., 0x00/255., 0x8b/255. } },
  { "darkolivegreen",		{ CCSS_PROPERTY_SPEC_SET, 0x55/255., 0x6b/255., 0x2f/255. } },
  { "darkorange",		{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0x8c/255., 0x00/255. } },
  { "darkorchid",		{ CCSS_PROPERTY_SPEC_SET, 0x99/255., 0x32/255., 0xcc/255. } },
  { "darkred",			{ CCSS_PROPERTY_SPEC_SET, 0x8b/255., 0x00/255., 0x00/255. } },
  { "darksalmon",		{ CCSS_PROPERTY_SPEC_SET, 0xe9/255., 0x96/255., 0x7a/255. } },
  { "darkseagreen",		{ CCSS_PROPERTY_SPEC_SET, 0x8f/255., 0xbc/255., 0x8f/255. } },
  { "darkslateblue",		{ CCSS_PROPERTY_SPEC_SET, 0x48/255., 0x3d/255., 0x8b/255. } },
  { "darkslategray",		{ CCSS_PROPERTY_SPEC_SET, 0x2f/255., 0x4f/255., 0x4f/255. } },
  { "darkslategrey",		{ CCSS_PROPERTY_SPEC_SET, 0x2f/255., 0x4f/255., 0x4f/255. } },
  { "darkturquoise",		{ CCSS_PROPERTY_SPEC_SET, 0x00/255., 0xce/255., 0xd1/255. } },
  { "darkviolet",		{ CCSS_PROPERTY_SPEC_SET, 0x94/255., 0x00/255., 0xd3/255. } },
  { "deeppink",			{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0x14/255., 0x93/255. } },
  { "deepskyblue",		{ CCSS_PROPERTY_SPEC_SET, 0x00/255., 0xbf/255., 0xff/255. } },
  { "dimgray",			{ CCSS_PROPERTY_SPEC_SET, 0x69/255., 0x69/255., 0x69/255. } },
  { "dimgrey",			{ CCSS_PROPERTY_SPEC_SET, 0x69/255., 0x69/255., 0x69/255. } },
  { "dodgerblue",		{ CCSS_PROPERTY_SPEC_SET, 0x1e/255., 0x90/255., 0xff/255. } },
  { "firebrick",		{ CCSS_PROPERTY_SPEC_SET, 0xb2/255., 0x22/255., 0x22/255. } },
  { "floralwhite",		{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0xfa/255., 0xf0/255. } },
  { "forestgreen",		{ CCSS_PROPERTY_SPEC_SET, 0x22/255., 0x8b/255., 0x22/255. } },
  { "fuchsia",			{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0x00/255., 0xff/255. } },
  { "gainsboro",		{ CCSS_PROPERTY_SPEC_SET, 0xdc/255., 0xdc/255., 0xdc/255. } },
  { "ghostwhite",		{ CCSS_PROPERTY_SPEC_SET, 0xf8/255., 0xf8/255., 0xff/255. } },
  { "gold",			{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0xd7/255., 0x00/255. } },
  { "goldenrod",		{ CCSS_PROPERTY_SPEC_SET, 0xda/255., 0xa5/255., 0x20/255. } },
  { "gray",			{ CCSS_PROPERTY_SPEC_SET, 0x80/255., 0x80/255., 0x80/255. } },
  { "grey",			{ CCSS_PROPERTY_SPEC_SET, 0x80/255., 0x80/255., 0x80/255. } },
  { "green",			{ CCSS_PROPERTY_SPEC_SET, 0x00/255., 0x80/255., 0x00/255. } },
  { "greenyellow",		{ CCSS_PROPERTY_SPEC_SET, 0xad/255., 0xff/255., 0x2f/255. } },
  { "honeydew",			{ CCSS_PROPERTY_SPEC_SET, 0xf0/255., 0xff/255., 0xf0/255. } },
  { "hotpink",			{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0x69/255., 0xb4/255. } },
  { "indianred",		{ CCSS_PROPERTY_SPEC_SET, 0xcd/255., 0x5c/255., 0x5c/255. } },
  { "indigo",			{ CCSS_PROPERTY_SPEC_SET, 0x4b/255., 0x00/255., 0x82/255. } },
  { "ivory",			{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0xff/255., 0xf0/255. } },
  { "khaki",			{ CCSS_PROPERTY_SPEC_SET, 0xf0/255., 0xe6/255., 0x8c/255. } },
  { "lavender",			{ CCSS_PROPERTY_SPEC_SET, 0xe6/255., 0xe6/255., 0xfa/255. } },
  { "lavenderblush",		{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0xf0/255., 0xf5/255. } },
  { "lawngreen",		{ CCSS_PROPERTY_SPEC_SET, 0x7c/255., 0xfc/255., 0x00/255. } },
  { "lemonchiffon",		{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0xfa/255., 0xcd/255. } },
  { "lightblue",		{ CCSS_PROPERTY_SPEC_SET, 0xad/255., 0xd8/255., 0xe6/255. } },
  { "lightcoral",		{ CCSS_PROPERTY_SPEC_SET, 0xf0/255., 0x80/255., 0x80/255. } },
  { "lightcyan",		{ CCSS_PROPERTY_SPEC_SET, 0xe0/255., 0xff/255., 0xff/255. } },
  { "lightgoldenrodyellow",	{ CCSS_PROPERTY_SPEC_SET, 0xfa/255., 0xfa/255., 0xd2/255. } },
  { "lightgray",		{ CCSS_PROPERTY_SPEC_SET, 0xd3/255., 0xd3/255., 0xd3/255. } },
  { "lightgrey",		{ CCSS_PROPERTY_SPEC_SET, 0xd3/255., 0xd3/255., 0xd3/255. } },
  { "lightgreen",		{ CCSS_PROPERTY_SPEC_SET, 0x90/255., 0xee/255., 0x90/255. } },
  { "lightpink",		{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0xb6/255., 0xc1/255. } },
  { "lightsalmon",		{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0xa0/255., 0x7a/255. } },
  { "lightseagreen",		{ CCSS_PROPERTY_SPEC_SET, 0x20/255., 0xb2/255., 0xaa/255. } },
  { "lightskyblue",		{ CCSS_PROPERTY_SPEC_SET, 0x87/255., 0xce/255., 0xfa/255. } },
  { "lightslategray",		{ CCSS_PROPERTY_SPEC_SET, 0x77/255., 0x88/255., 0x99/255. } },
  { "lightslategrey",		{ CCSS_PROPERTY_SPEC_SET, 0x77/255., 0x88/255., 0x99/255. } },
  { "lightsteelblue",		{ CCSS_PROPERTY_SPEC_SET, 0xb0/255., 0xc4/255., 0xde/255. } },
  { "lightyellow",		{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0xff/255., 0xe0/255. } },
  { "lime",			{ CCSS_PROPERTY_SPEC_SET, 0x00/255., 0xff/255., 0x00/255. } },
  { "limegreen",		{ CCSS_PROPERTY_SPEC_SET, 0x32/255., 0xcd/255., 0x32/255. } },
  { "linen",			{ CCSS_PROPERTY_SPEC_SET, 0xfa/255., 0xf0/255., 0xe6/255. } },
  { "magenta",			{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0x00/255., 0xff/255. } },
  { "maroon",			{ CCSS_PROPERTY_SPEC_SET, 0x80/255., 0x00/255., 0x00/255. } },
  { "mediumaquamarine",		{ CCSS_PROPERTY_SPEC_SET, 0x66/255., 0xcd/255., 0xaa/255. } },
  { "mediumblue",		{ CCSS_PROPERTY_SPEC_SET, 0x00/255., 0x00/255., 0xcd/255. } },
  { "mediumorchid",		{ CCSS_PROPERTY_SPEC_SET, 0xba/255., 0x55/255., 0xd3/255. } },
  { "mediumpurple",		{ CCSS_PROPERTY_SPEC_SET, 0x93/255., 0x70/255., 0xd8/255. } },
  { "mediumseagreen",		{ CCSS_PROPERTY_SPEC_SET, 0x3c/255., 0xb3/255., 0x71/255. } },
  { "mediumslateblue",		{ CCSS_PROPERTY_SPEC_SET, 0x7b/255., 0x68/255., 0xee/255. } },
  { "mediumspringgreen",	{ CCSS_PROPERTY_SPEC_SET, 0x00/255., 0xfa/255., 0x9a/255. } },
  { "mediumturquoise",		{ CCSS_PROPERTY_SPEC_SET, 0x48/255., 0xd1/255., 0xcc/255. } },
  { "mediumvioletred",		{ CCSS_PROPERTY_SPEC_SET, 0xc7/255., 0x15/255., 0x85/255. } },
  { "midnightblue",		{ CCSS_PROPERTY_SPEC_SET, 0x19/255., 0x19/255., 0x70/255. } },
  { "mintcream",		{ CCSS_PROPERTY_SPEC_SET, 0xf5/255., 0xff/255., 0xfa/255. } },
  { "mistyrose",		{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0xe4/255., 0xe1/255. } },
  { "moccasin",			{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0xe4/255., 0xb5/255. } },
  { "navajowhite",		{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0xde/255., 0xad/255. } },
  { "navy",			{ CCSS_PROPERTY_SPEC_SET, 0x00/255., 0x00/255., 0x80/255. } },
  { "oldlace",			{ CCSS_PROPERTY_SPEC_SET, 0xfd/255., 0xf5/255., 0xe6/255. } },
  { "olive",			{ CCSS_PROPERTY_SPEC_SET, 0x80/255., 0x80/255., 0x00/255. } },
  { "olivedrab",		{ CCSS_PROPERTY_SPEC_SET, 0x6b/255., 0x8e/255., 0x23/255. } },
  { "orange",			{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0xa5/255., 0x00/255. } },
  { "orangered",		{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0x45/255., 0x00/255. } },
  { "orchid",			{ CCSS_PROPERTY_SPEC_SET, 0xda/255., 0x70/255., 0xd6/255. } },
  { "palegoldenrod",		{ CCSS_PROPERTY_SPEC_SET, 0xee/255., 0xe8/255., 0xaa/255. } },
  { "palegreen",		{ CCSS_PROPERTY_SPEC_SET, 0x98/255., 0xfb/255., 0x98/255. } },
  { "paleturquoise",		{ CCSS_PROPERTY_SPEC_SET, 0xaf/255., 0xee/255., 0xee/255. } },
  { "palevioletred",		{ CCSS_PROPERTY_SPEC_SET, 0xd8/255., 0x70/255., 0x93/255. } },
  { "papayawhip",		{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0xef/255., 0xd5/255. } },
  { "peachpuff",		{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0xda/255., 0xb9/255. } },
  { "peru",			{ CCSS_PROPERTY_SPEC_SET, 0xcd/255., 0x85/255., 0x3f/255. } },
  { "pink",			{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0xc0/255., 0xcb/255. } },
  { "plum",			{ CCSS_PROPERTY_SPEC_SET, 0xdd/255., 0xa0/255., 0xdd/255. } },
  { "powderblue",		{ CCSS_PROPERTY_SPEC_SET, 0xb0/255., 0xe0/255., 0xe6/255. } },
  { "purple",			{ CCSS_PROPERTY_SPEC_SET, 0x80/255., 0x00/255., 0x80/255. } },
  { "red",			{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0x00/255., 0x00/255. } },
  { "rosybrown",		{ CCSS_PROPERTY_SPEC_SET, 0xbc/255., 0x8f/255., 0x8f/255. } },
  { "royalblue",		{ CCSS_PROPERTY_SPEC_SET, 0x41/255., 0x69/255., 0xe1/255. } },
  { "saddlebrown",		{ CCSS_PROPERTY_SPEC_SET, 0x8b/255., 0x45/255., 0x13/255. } },
  { "salmon",			{ CCSS_PROPERTY_SPEC_SET, 0xfa/255., 0x80/255., 0x72/255. } },
  { "sandybrown",		{ CCSS_PROPERTY_SPEC_SET, 0xf4/255., 0xa4/255., 0x60/255. } },
  { "seagreen",			{ CCSS_PROPERTY_SPEC_SET, 0x2e/255., 0x8b/255., 0x57/255. } },
  { "seashell",			{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0xf5/255., 0xee/255. } },
  { "sienna",			{ CCSS_PROPERTY_SPEC_SET, 0xa0/255., 0x52/255., 0x2d/255. } },
  { "silver",			{ CCSS_PROPERTY_SPEC_SET, 0xc0/255., 0xc0/255., 0xc0/255. } },
  { "skyblue",			{ CCSS_PROPERTY_SPEC_SET, 0x87/255., 0xce/255., 0xeb/255. } },
  { "slateblue",		{ CCSS_PROPERTY_SPEC_SET, 0x6a/255., 0x5a/255., 0xcd/255. } },
  { "slategray",		{ CCSS_PROPERTY_SPEC_SET, 0x70/255., 0x80/255., 0x90/255. } },
  { "slategrey",		{ CCSS_PROPERTY_SPEC_SET, 0x70/255., 0x80/255., 0x90/255. } },
  { "snow",			{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0xfa/255., 0xfa/255. } },
  { "springgreen",		{ CCSS_PROPERTY_SPEC_SET, 0x00/255., 0xff/255., 0x7f/255. } },
  { "steelblue",		{ CCSS_PROPERTY_SPEC_SET, 0x46/255., 0x82/255., 0xb4/255. } },
  { "tan",			{ CCSS_PROPERTY_SPEC_SET, 0xd2/255., 0xb4/255., 0x8c/255. } },
  { "teal",			{ CCSS_PROPERTY_SPEC_SET, 0x00/255., 0x80/255., 0x80/255. } },
  { "thistle",			{ CCSS_PROPERTY_SPEC_SET, 0xd8/255., 0xbf/255., 0xd8/255. } },
  { "tomato",			{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0x63/255., 0x47/255. } },
  { "turquoise",		{ CCSS_PROPERTY_SPEC_SET, 0x40/255., 0xe0/255., 0xd0/255. } },
  { "violet",			{ CCSS_PROPERTY_SPEC_SET, 0xee/255., 0x82/255., 0xee/255. } },
  { "wheat",			{ CCSS_PROPERTY_SPEC_SET, 0xf5/255., 0xde/255., 0xb3/255. } },
  { "white",			{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0xff/255., 0xff/255. } },
  { "whitesmoke",		{ CCSS_PROPERTY_SPEC_SET, 0xf5/255., 0xf5/255., 0xf5/255. } },
  { "yellow",			{ CCSS_PROPERTY_SPEC_SET, 0xff/255., 0xff/255., 0x00/255. } },
  { "yellowgreen",		{ CCSS_PROPERTY_SPEC_SET, 0x9a/255., 0xcd/255., 0x32/255. } },
};

void
ccss_color_init (ccss_color_t *self)
{
	memset (self, 0, sizeof (*self));

	self->spec = CCSS_PROPERTY_SPEC_UNSET;
}

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
	unsigned int	r, g, b;

	g_return_val_if_fail (color, false);

	len = strlen (color);
	if (len % 3 || len < 3 || len > 12)
		return false;

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

static ccss_property_spec_t
parse (ccss_color_t	 *self,
       ccss_block_t	 *block,
       char const	 *property_name,
       CRTerm const	**value)
{
	ccss_property_spec_t	 type;
	char const		*function;
	char const		*str;
	char			*color;
	bool			 ret;

	g_return_val_if_fail (self, CCSS_PROPERTY_SPEC_UNSET);

	if (!*value) {
		return CCSS_PROPERTY_SPEC_UNSET;
	}

	switch ((*value)->type) {
	case TERM_IDENT:
		type = ccss_property_parse_spec (value);
		if (type != CCSS_PROPERTY_SPEC_SET) {
			/* Value already advanced. */
			return type;
		}
		str = cr_string_peek_raw_str ((*value)->content.str);
		ret = parse_name (self, str);
		if (ret) {
			*value = (*value)->next;
			return CCSS_PROPERTY_SPEC_SET;
		}
		return CCSS_PROPERTY_SPEC_UNSET;
	case TERM_HASH:
		str = cr_string_peek_raw_str ((*value)->content.str);
		ret = parse_hex (self, str);
		if (ret) {
			*value = (*value)->next;
			return CCSS_PROPERTY_SPEC_SET;
		}
		return CCSS_PROPERTY_SPEC_UNSET;
	case TERM_RGB:
		self->red = (*value)->content.rgb->red;
		self->green = (*value)->content.rgb->green;
		self->blue = (*value)->content.rgb->blue;
		*value = (*value)->next;
		return CCSS_PROPERTY_SPEC_SET;
	case TERM_FUNCTION:
		function = cr_string_peek_raw_str ((*value)->content.str);
		color = ccss_function_invoke (block, function, property_name,
					      (*value)->ext_content.func_param);
		if (color) {
			if (g_str_has_prefix (color, "rgb(")) {

				/* FIXME error handling. */
				char *iter;
				iter = color + strlen ("rgb(");

				self->red = g_ascii_strtod (iter, &iter);
				if (!iter) { g_warning ("Could not parse '%s'", color); goto bail; }

				self->green = g_ascii_strtod (iter + 1, &iter);
				if (!iter) { g_warning ("Could not parse '%s'", color); goto bail; }

				self->blue = g_ascii_strtod (iter + 1, &iter);
				if (!iter) { g_warning ("Could not parse '%s'", color); goto bail; }

				g_free (color), color = NULL;
				*value = (*value)->next;
				return CCSS_PROPERTY_SPEC_SET;
			}
			/* FIXME recognize "rgba". */
bail:
			g_free (color), color = NULL;
		}
		return CCSS_PROPERTY_SPEC_UNSET;
	/* fall thru for all other enum values to prevent compiler warnings */
	case TERM_NO_TYPE:
	case TERM_NUMBER:
	case TERM_STRING:
	case TERM_URI:
	case TERM_UNICODERANGE:
	default:
		g_assert_not_reached ();
		return CCSS_PROPERTY_SPEC_UNSET;
	}
}

bool
ccss_color_parse (ccss_color_t	 *self,
		  ccss_block_t	 *block,
		  char const	 *property_name,
		  CRTerm const	**values)
{
	if (!*values) {
		return false;
	}

	self->spec = parse (self, block, property_name, values);
	return self->spec != CCSS_PROPERTY_SPEC_UNSET;
}

