/* vim: set ts=8 sw=8 noexpandtab: */

/* The `C' CSS Library.
 * Copyright (C) 2008 Robert Staudinger
 * Copyright (C) 2009 Intel Corporation.
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
#include "ccss-color-impl.h"
#include "ccss-property-impl.h"
#include "config.h"

static const struct {
	char const		*name;
	const ccss_color_t	 color;
} _color_map[] = {
  { "aliceblue",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xf0/255., 0xf8/255., 0xff/255., 1. } },
  { "antiquewhite",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xfa/255., 0xeb/255., 0xd7/255., 1. } },
  { "aqua",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x00/255., 0xff/255., 0xff/255., 1. } },
  { "aquamarine",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x7f/255., 0xff/255., 0xd4/255., 1. } },
  { "azure",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xf0/255., 0xff/255., 0xff/255., 1. } },
  { "beige",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xf5/255., 0xf5/255., 0xdc/255., 1. } },
  { "bisque",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0xe4/255., 0xc4/255., 1. } },
  { "black",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x00/255., 0x00/255., 0x00/255., 1. } },
  { "blanchedalmond",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0xeb/255., 0xcd/255., 1. } },
  { "blue",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x00/255., 0x00/255., 0xff/255., 1. } },
  { "blueviolet",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x8a/255., 0x2b/255., 0xe2/255., 1. } },
  { "brown",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xa5/255., 0x2a/255., 0x2a/255., 1. } },
  { "burlywood",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xde/255., 0xb8/255., 0x87/255., 1. } },
  { "cadetblue",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x5f/255., 0x9e/255., 0xa0/255., 1. } },
  { "chartreuse",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x7f/255., 0xff/255., 0x00/255., 1. } },
  { "chocolate",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xd2/255., 0x69/255., 0x1e/255., 1. } },
  { "coral",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0x7f/255., 0x50/255., 1. } },
  { "cornflowerblue",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x64/255., 0x95/255., 0xed/255., 1. } },
  { "cornsilk",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0xf8/255., 0xdc/255., 1. } },
  { "crimson",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xdc/255., 0x14/255., 0x3c/255., 1. } },
  { "cyan",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x00/255., 0xff/255., 0xff/255., 1. } },
  { "darkblue",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x00/255., 0x00/255., 0x8b/255., 1. } },
  { "darkcyan",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x00/255., 0x8b/255., 0x8b/255., 1. } },
  { "darkgoldenrod",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xb8/255., 0x86/255., 0x0b/255., 1. } },
  { "darkgray",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xa9/255., 0xa9/255., 0xa9/255., 1. } },
  { "darkgrey",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xa9/255., 0xa9/255., 0xa9/255., 1. } },
  { "darkgreen",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x00/255., 0x64/255., 0x00/255., 1. } },
  { "darkkhaki",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xbd/255., 0xb7/255., 0x6b/255., 1. } },
  { "darkmagenta",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x8b/255., 0x00/255., 0x8b/255., 1. } },
  { "darkolivegreen",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x55/255., 0x6b/255., 0x2f/255., 1. } },
  { "darkorange",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0x8c/255., 0x00/255., 1. } },
  { "darkorchid",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x99/255., 0x32/255., 0xcc/255., 1. } },
  { "darkred",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x8b/255., 0x00/255., 0x00/255., 1. } },
  { "darksalmon",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xe9/255., 0x96/255., 0x7a/255., 1. } },
  { "darkseagreen",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x8f/255., 0xbc/255., 0x8f/255., 1. } },
  { "darkslateblue",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x48/255., 0x3d/255., 0x8b/255., 1. } },
  { "darkslategray",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x2f/255., 0x4f/255., 0x4f/255., 1. } },
  { "darkslategrey",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x2f/255., 0x4f/255., 0x4f/255., 1. } },
  { "darkturquoise",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x00/255., 0xce/255., 0xd1/255., 1. } },
  { "darkviolet",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x94/255., 0x00/255., 0xd3/255., 1. } },
  { "deeppink",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0x14/255., 0x93/255., 1. } },
  { "deepskyblue",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x00/255., 0xbf/255., 0xff/255., 1. } },
  { "dimgray",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x69/255., 0x69/255., 0x69/255., 1. } },
  { "dimgrey",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x69/255., 0x69/255., 0x69/255., 1. } },
  { "dodgerblue",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x1e/255., 0x90/255., 0xff/255., 1. } },
  { "firebrick",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xb2/255., 0x22/255., 0x22/255., 1. } },
  { "floralwhite",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0xfa/255., 0xf0/255., 1. } },
  { "forestgreen",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x22/255., 0x8b/255., 0x22/255., 1. } },
  { "fuchsia",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0x00/255., 0xff/255., 1. } },
  { "gainsboro",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xdc/255., 0xdc/255., 0xdc/255., 1. } },
  { "ghostwhite",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xf8/255., 0xf8/255., 0xff/255., 1. } },
  { "gold",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0xd7/255., 0x00/255., 1. } },
  { "goldenrod",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xda/255., 0xa5/255., 0x20/255., 1. } },
  { "gray",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x80/255., 0x80/255., 0x80/255., 1. } },
  { "grey",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x80/255., 0x80/255., 0x80/255., 1. } },
  { "green",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x00/255., 0x80/255., 0x00/255., 1. } },
  { "greenyellow",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xad/255., 0xff/255., 0x2f/255., 1. } },
  { "honeydew",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xf0/255., 0xff/255., 0xf0/255., 1. } },
  { "hotpink",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0x69/255., 0xb4/255., 1. } },
  { "indianred",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xcd/255., 0x5c/255., 0x5c/255., 1. } },
  { "indigo",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x4b/255., 0x00/255., 0x82/255., 1. } },
  { "ivory",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0xff/255., 0xf0/255., 1. } },
  { "khaki",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xf0/255., 0xe6/255., 0x8c/255., 1. } },
  { "lavender",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xe6/255., 0xe6/255., 0xfa/255., 1. } },
  { "lavenderblush",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0xf0/255., 0xf5/255., 1. } },
  { "lawngreen",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x7c/255., 0xfc/255., 0x00/255., 1. } },
  { "lemonchiffon",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0xfa/255., 0xcd/255., 1. } },
  { "lightblue",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xad/255., 0xd8/255., 0xe6/255., 1. } },
  { "lightcoral",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xf0/255., 0x80/255., 0x80/255., 1. } },
  { "lightcyan",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xe0/255., 0xff/255., 0xff/255., 1. } },
  { "lightgoldenrodyellow",	{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xfa/255., 0xfa/255., 0xd2/255., 1. } },
  { "lightgray",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xd3/255., 0xd3/255., 0xd3/255., 1. } },
  { "lightgrey",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xd3/255., 0xd3/255., 0xd3/255., 1. } },
  { "lightgreen",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x90/255., 0xee/255., 0x90/255., 1. } },
  { "lightpink",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0xb6/255., 0xc1/255., 1. } },
  { "lightsalmon",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0xa0/255., 0x7a/255., 1. } },
  { "lightseagreen",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x20/255., 0xb2/255., 0xaa/255., 1. } },
  { "lightskyblue",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x87/255., 0xce/255., 0xfa/255., 1. } },
  { "lightslategray",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x77/255., 0x88/255., 0x99/255., 1. } },
  { "lightslategrey",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x77/255., 0x88/255., 0x99/255., 1. } },
  { "lightsteelblue",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xb0/255., 0xc4/255., 0xde/255., 1. } },
  { "lightyellow",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0xff/255., 0xe0/255., 1. } },
  { "lime",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x00/255., 0xff/255., 0x00/255., 1. } },
  { "limegreen",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x32/255., 0xcd/255., 0x32/255., 1. } },
  { "linen",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xfa/255., 0xf0/255., 0xe6/255., 1. } },
  { "magenta",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0x00/255., 0xff/255., 1. } },
  { "maroon",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x80/255., 0x00/255., 0x00/255., 1. } },
  { "mediumaquamarine",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x66/255., 0xcd/255., 0xaa/255., 1. } },
  { "mediumblue",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x00/255., 0x00/255., 0xcd/255., 1. } },
  { "mediumorchid",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xba/255., 0x55/255., 0xd3/255., 1. } },
  { "mediumpurple",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x93/255., 0x70/255., 0xd8/255., 1. } },
  { "mediumseagreen",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x3c/255., 0xb3/255., 0x71/255., 1. } },
  { "mediumslateblue",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x7b/255., 0x68/255., 0xee/255., 1. } },
  { "mediumspringgreen",	{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x00/255., 0xfa/255., 0x9a/255., 1. } },
  { "mediumturquoise",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x48/255., 0xd1/255., 0xcc/255., 1. } },
  { "mediumvioletred",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xc7/255., 0x15/255., 0x85/255., 1. } },
  { "midnightblue",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x19/255., 0x19/255., 0x70/255., 1. } },
  { "mintcream",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xf5/255., 0xff/255., 0xfa/255., 1. } },
  { "mistyrose",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0xe4/255., 0xe1/255., 1. } },
  { "moccasin",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0xe4/255., 0xb5/255., 1. } },
  { "navajowhite",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0xde/255., 0xad/255., 1. } },
  { "navy",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x00/255., 0x00/255., 0x80/255., 1. } },
  { "oldlace",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xfd/255., 0xf5/255., 0xe6/255., 1. } },
  { "olive",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x80/255., 0x80/255., 0x00/255., 1. } },
  { "olivedrab",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x6b/255., 0x8e/255., 0x23/255., 1. } },
  { "orange",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0xa5/255., 0x00/255., 1. } },
  { "orangered",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0x45/255., 0x00/255., 1. } },
  { "orchid",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xda/255., 0x70/255., 0xd6/255., 1. } },
  { "palegoldenrod",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xee/255., 0xe8/255., 0xaa/255., 1. } },
  { "palegreen",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x98/255., 0xfb/255., 0x98/255., 1. } },
  { "paleturquoise",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xaf/255., 0xee/255., 0xee/255., 1. } },
  { "palevioletred",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xd8/255., 0x70/255., 0x93/255., 1. } },
  { "papayawhip",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0xef/255., 0xd5/255., 1. } },
  { "peachpuff",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0xda/255., 0xb9/255., 1. } },
  { "peru",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xcd/255., 0x85/255., 0x3f/255., 1. } },
  { "pink",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0xc0/255., 0xcb/255., 1. } },
  { "plum",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xdd/255., 0xa0/255., 0xdd/255., 1. } },
  { "powderblue",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xb0/255., 0xe0/255., 0xe6/255., 1. } },
  { "purple",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x80/255., 0x00/255., 0x80/255., 1. } },
  { "red",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0x00/255., 0x00/255., 1. } },
  { "rosybrown",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xbc/255., 0x8f/255., 0x8f/255., 1. } },
  { "royalblue",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x41/255., 0x69/255., 0xe1/255., 1. } },
  { "saddlebrown",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x8b/255., 0x45/255., 0x13/255., 1. } },
  { "salmon",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xfa/255., 0x80/255., 0x72/255., 1. } },
  { "sandybrown",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xf4/255., 0xa4/255., 0x60/255., 1. } },
  { "seagreen",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x2e/255., 0x8b/255., 0x57/255., 1. } },
  { "seashell",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0xf5/255., 0xee/255., 1. } },
  { "sienna",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xa0/255., 0x52/255., 0x2d/255., 1. } },
  { "silver",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xc0/255., 0xc0/255., 0xc0/255., 1. } },
  { "skyblue",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x87/255., 0xce/255., 0xeb/255., 1. } },
  { "slateblue",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x6a/255., 0x5a/255., 0xcd/255., 1. } },
  { "slategray",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x70/255., 0x80/255., 0x90/255., 1. } },
  { "slategrey",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x70/255., 0x80/255., 0x90/255., 1. } },
  { "snow",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0xfa/255., 0xfa/255., 1. } },
  { "springgreen",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x00/255., 0xff/255., 0x7f/255., 1. } },
  { "steelblue",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x46/255., 0x82/255., 0xb4/255., 1. } },
  { "tan",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xd2/255., 0xb4/255., 0x8c/255., 1. } },
  { "teal",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x00/255., 0x80/255., 0x80/255., 1. } },
  { "thistle",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xd8/255., 0xbf/255., 0xd8/255., 1. } },
  { "tomato",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0x63/255., 0x47/255., 1. } },
  { "transparent",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x00/255., 0x00/255., 0x00/255., 0. } },
  { "turquoise",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x40/255., 0xe0/255., 0xd0/255., 1. } },
  { "violet",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xee/255., 0x82/255., 0xee/255., 1. } },
  { "wheat",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xf5/255., 0xde/255., 0xb3/255., 1. } },
  { "white",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0xff/255., 0xff/255., 1. } },
  { "whitesmoke",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xf5/255., 0xf5/255., 0xf5/255., 1. } },
  { "yellow",			{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0xff/255., 0xff/255., 0x00/255., 1. } },
  { "yellowgreen",		{ { NULL, CCSS_PROPERTY_STATE_SET },
				  0x9a/255., 0xcd/255., 0x32/255., 1. } }
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
			self->alpha = _color_map[i].color.alpha;
			return true;
		}
	}

	return false;
}

static bool
parse_hex (ccss_color_t	*self,
	   char const	*color)
{
	size_t	 len;
	uint32_t result;

	g_return_val_if_fail (color, false);

	len = strlen (color);
	if (sscanf (color, "%x", &result)) {
		if (len == 8) {
			/* #rrggbbaa */
			self->red   = ((result >> 24) & 0xff) / 255.;
			self->green = ((result >> 16) & 0xff) / 255.;
			self->blue  = ((result >>  8) & 0xff) / 255.;
			self->alpha = ((result >>  0) & 0xff) / 255.;
			return true;
		} else if (len == 6) {
			/* #rrggbb */
			self->red   = ((result >> 16) & 0xff) / 255.;
			self->green = ((result >>  8) & 0xff) / 255.;
			self->blue  = ((result >>  0) & 0xff) / 255.;
			self->alpha = 1.0;
			return true;
		} else if (len == 4) {
			/* #rgba */
			self->red   = ((result >> 12) & 0xf) / 15.;
			self->green = ((result >>  8) & 0xf) / 15.;
			self->blue  = ((result >>  4) & 0xf) / 15.;
			self->alpha = ((result >>  0) & 0xf) / 15.;
			return true;
		} else if (len == 3) {
			/* #rgb */
			self->red   = ((result >> 8) & 0xf) / 15.;
			self->green = ((result >> 4) & 0xf) / 15.;
			self->blue  = ((result >> 0) & 0xf) / 15.;
			self->alpha = 1.0;
			return true;
		}
	}

	return false;
}

static bool
parse_rgb_value (float		 *color,
		 CRTerm const   **iter)
{
	if (iter &&
	    *iter &&
	    (*iter)->type == TERM_NUMBER) {
		switch ((*iter)->content.num->type) {
		case NUM_GENERIC:
			*color = CLAMP ((*iter)->content.num->val, 0., 255.) / 255.;
			*iter = (*iter)->next;
			return true;
		case NUM_PERCENTAGE:
			*color = CLAMP ((*iter)->content.num->val, 0., 100.) / 100.;
			*iter = (*iter)->next;
			return true;
		default:
			return false;
		}
	}

	return false;
}

static bool
parse_rgb (ccss_color_t *self,
	   CRTerm const *value)
{
	CRTerm const *iter;

	iter = value;

	if (!parse_rgb_value (&self->red, &iter))
		return false;

	if (!parse_rgb_value (&self->green, &iter))
		return false;

	if (!parse_rgb_value (&self->blue, &iter))
		return false;

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

	if (!parse_rgb_value (&self->red, &iter))
		return false;

	if (!parse_rgb_value (&self->green, &iter))
		return false;

	if (!parse_rgb_value (&self->blue, &iter))
		return false;

	/* alpha */
	if (iter &&
	    iter->type == TERM_NUMBER &&
	    iter->content.num->type == NUM_GENERIC) {
		self->alpha = CLAMP (iter->content.num->val, 0., 1.);
		iter = iter->next;
	} else {
		return false;
	}

	self->base.state = CCSS_PROPERTY_STATE_SET;
	return true;
}

bool
ccss_color_parse (ccss_color_t		 *self,
		  ccss_grammar_t const	 *grammar,
		  void			 *user_data,
		  CRTerm const		**value)
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
		case CCSS_PROPERTY_STATE_NONE:
		case CCSS_PROPERTY_STATE_INHERIT:
			return true;
		case CCSS_PROPERTY_STATE_SET:
			/* Process this. */
			break;
		case CCSS_PROPERTY_STATE_INVALID:
		default:
			return false;
		}
		str = cr_string_peek_raw_str ((*value)->content.str);
		ret = parse_name (self, str);
		if (ret) {
			self->base.state = CCSS_PROPERTY_STATE_SET;
			*value = (*value)->next;
			return true;
		}
		return false;
	case TERM_HASH:
		str = cr_string_peek_raw_str ((*value)->content.str);
		ret = parse_hex (self, str);
		if (ret) {
			self->base.state = CCSS_PROPERTY_STATE_SET;
			*value = (*value)->next;
			return true;
		}
		return false;
	case TERM_RGB:
		if ((*value)->content.rgb->is_percentage) {
			self->red = CLAMP ((*value)->content.rgb->red,
					   0., 100.) / 100.;
			self->green = CLAMP ((*value)->content.rgb->green,
					     0., 100.) / 100.;
			self->blue = CLAMP ((*value)->content.rgb->blue,
					    0., 100.) / 100.;
		} else {
			self->red = CLAMP ((*value)->content.rgb->red,
					   0., 255.) / 255.;
			self->green = CLAMP ((*value)->content.rgb->green,
					     0., 255.) / 255.;
			self->blue = CLAMP ((*value)->content.rgb->blue,
					    0., 255.) / 255.;
		}
		self->alpha = 1.;
		self->base.state = CCSS_PROPERTY_STATE_SET;
		*value = (*value)->next;
		return true;
	case TERM_FUNCTION:
		/* FIXME: Seems like rgb() and rgba() are handled by TERM_RGB,
		 * needs some investigation. */
		str = cr_string_peek_raw_str ((*value)->content.str);
		if (0 == g_strcmp0 (str, "rgb")) {
			return parse_rgb (self, (*value)->ext_content.func_param);
		} else if (0 == g_strcmp0 (str, "rgba")) {
			return parse_rgba (self, (*value)->ext_content.func_param);
		} else if (str){
			char    *rgba;
			int      matches;
			rgba = ccss_grammar_invoke_function (grammar,
							     str,
							     (*value)->ext_content.func_param,
							     user_data);
			g_return_val_if_fail (rgba, false);

			matches = sscanf (rgba,
					  "rgba(%f,%f,%f,%f)",
					  &self->red,
					  &self->green,
					  &self->blue,
					  &self->alpha);
			g_free (rgba);
			if (matches == 4) {
				self->base.state = CCSS_PROPERTY_STATE_SET;
				*value = (*value)->next;
				return true;
			} else {
				g_warning ("%s: Invalid color '%s'",
					   G_STRLOC,
					   rgba);
			}
		}
		g_warning (G_STRLOC " '%s' not recognised.", str);
		break;
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

bool
ccss_color_factory (ccss_grammar_t const	*grammar,
		    ccss_block_t		*self,
		    char const			*name,
		    CRTerm const		*values,
		    void			*user_data)
{
	ccss_property_class_t const     *property_class;
	ccss_color_t			 c;
	bool				 ret;

	property_class = ccss_grammar_lookup_property (grammar, name);
	ccss_property_init (&c.base, property_class);
	ret = ccss_color_parse (&c, grammar, user_data, &values);
	if (ret) {
		ccss_color_t *color = g_new0 (ccss_color_t, 1);
		*color = c;
		ccss_block_add_property (self, name, &color->base);
		return true;
	}

	return false;
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

	* (char **) value = g_strdup_printf ("#%02x%02x%02x%02x",
						(int) (property->red * 255),
						(int) (property->green * 255),
						(int) (property->blue * 255),
						(int) (property->alpha * 255));
	return true;
}

static ccss_property_class_t const _ptable[] = {
    {
	.name = "color",
	.create = NULL,
	.destroy = (ccss_property_destroy_f) g_free,
	.convert = (ccss_property_convert_f) ccss_color_convert,
	.factory = ccss_color_factory,
	.inherit = NULL,
	.serialize = NULL
    }, {
	.name = NULL
    }
};

ccss_property_class_t const *
ccss_color_parser_get_property_classes (void)
{
	return _ptable;
}

