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
#include "ccss-property-priv.h"
#include "ccss-style-priv.h"

/**
 * ccss_init:
 * @properties:		table of properties that can be used in CSS or %NULL.
 * @functions:		table of functions that can be used in CSS or %NULL.
 * 
 * Initialize the CCSS library before making any calls to it.
 **/
void
ccss_init (ccss_property_impl_t const	*properties,
	   ccss_function_t const	*functions)
{
	ccss_function_subsystem_init (functions);
	ccss_property_subsystem_init ();
	ccss_style_subsystem_init ();
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
	ccss_property_subsystem_shutdown ();
}

