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

#include "ccss.h"
#include "ccss-function-priv.h"
#include "ccss-parser.h"
#include "ccss-property-generic.h"
#include "ccss-style-priv.h"

/* FIXME: split out */
#include "ccss-color.h"
#include "ccss-background-parser.h"
#include "ccss-border-parser.h"
#include "ccss-border-image-parser.h"

/**
 * ccss_init:
 * 
 * Initialize the CCSS library before making any calls to it.
 **/
void
ccss_init (void)
{
	ccss_property_class_t const *properties;

	properties = ccss_property_generic_get_ptable ();
	ccss_add_properties (properties);

	ccss_style_subsystem_init ();
}

/**
 * ccss_add_properties:
 * @properties:	Null-terminated array of #ccss_property_class_t to register.
 *
 * Register a set of custom css properties. This function must be between
 * #ccss_init() and instatiation of #ccss_stylesheet_t.
 **/
void
ccss_add_properties (ccss_property_class_t const *properties)
{
	ccss_parser_subsystem_add_properties (properties);
}

/**
 * ccss_add_functions:
 * @properties:	Null-terminated array of #ccss_function_t to register.
 *
 * Register a set of custom css function handlers, like e.g. `url'.
 * This function must be between #ccss_init() and instatiation of #ccss_stylesheet_t.
 **/
void
ccss_add_functions (ccss_function_t const *functions)
{
	ccss_function_subsystem_add_functions (functions);
}

/**
 * ccss_shutdown:
 *
 * Shut down the CCSS library.
 **/
void
ccss_shutdown (void)
{
	ccss_style_subsystem_shutdown ();

	ccss_function_subsystem_shutdown ();
	ccss_parser_subsystem_shutdown ();
}

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
	ccss_shutdown ();
}

