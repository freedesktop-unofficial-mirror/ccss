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

#include "ccss-background-parser.h"
#include "ccss-border-parser.h"
#include "ccss-border-image-parser.h"
#include "ccss-cairo.h"
#include "ccss-cairo-style-priv.h"
#include "ccss-color.h"

/**
 * ccss_cairo_init:
 * 
 * Initialize the ccss-cairo library before making any calls to it.
 **/
void
ccss_cairo_init (void)
{
	ccss_property_class_t const *properties;

	ccss_init ();
	ccss_style_subsystem_init ();

	properties = ccss_background_get_ptable ();
	ccss_add_properties (properties);

	properties = ccss_border_image_get_ptable ();
	ccss_add_properties (properties);

	properties = ccss_border_get_ptable ();
	ccss_add_properties (properties);

	properties = ccss_color_get_ptable ();
	ccss_add_properties (properties);
}

/**
 * ccss_cairo_shutdown:
 *
 * Shut down the ccss-cairo library.
 **/
void
ccss_cairo_shutdown (void)
{
	ccss_style_subsystem_shutdown ();
	ccss_shutdown ();
}
