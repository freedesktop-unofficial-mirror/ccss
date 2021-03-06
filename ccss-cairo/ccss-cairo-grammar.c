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

#include "ccss-cairo-appearance-parser.h"
#include "ccss-cairo-grammar.h"

/**
 * ccss_cairo_grammar_create:
 *
 * Create a new cairo-grammar instance.
 *
 * The cairo-grammar object provides a factory to load stylesheets restricted to 
 * the supported CSS subset.
 *
 * Returns: a new #ccss_grammar_t instance.
 **/
ccss_grammar_t *
ccss_cairo_grammar_create (void)
{
	ccss_grammar_t *self;

	self = ccss_grammar_create_css ();
	ccss_grammar_add_properties (self, ccss_cairo_appearance_parser_get_property_classes ());

	return self;
}

