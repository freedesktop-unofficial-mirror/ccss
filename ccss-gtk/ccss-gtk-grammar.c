/* The Gtk+ CSS Drawing Library.
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

#include <string.h>
#include <gtk/gtk.h>
#include <ccss-cairo/ccss-cairo.h>
#include "ccss-gtk-grammar.h"
#include "ccss-gtk-property.h"
#include "config.h"

/**
 * ccss_gtk_grammar_create:
 *
 * Create a new gtk-grammar instance.
 *
 * The gtk-grammar object provides a factory to load stylesheets restricted to 
 * the supported CSS subset.
 *
 * Returns: a new #ccss_grammar_t instance.
 **/
ccss_grammar_t *
ccss_gtk_grammar_create (void)
{
	ccss_grammar_t			*self;
	ccss_property_class_t const	*fallback_property_class;

	self = ccss_cairo_grammar_create ();

	fallback_property_class = ccss_grammar_lookup_property (self, "*");
	g_return_val_if_fail (fallback_property_class, NULL);
	ccss_gtk_property_set_fallback_class (fallback_property_class);

	ccss_grammar_add_properties (self, ccss_gtk_property_get_property_classes ());

	return self;
}


