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

#include <ccss-cairo/ccss-cairo.h>
#include "ccss-gtk-grammar.h"
#include "config.h"

static ccss_property_class_t const *_generic_property_class = NULL;

static bool
property_convert (ccss_property_base_t const	*self,
		  ccss_property_type_t		 target,
		  void				*value)
{
	return false;
}

static bool
property_factory (ccss_grammar_t const	*grammar,
		  ccss_block_t		*block,
		  char const		*name,
		  CRTerm const		*values,
		  void			*user_data)
{
	g_return_val_if_fail (grammar, false);
	g_return_val_if_fail (block, false);
	g_return_val_if_fail (name, false);
	g_return_val_if_fail (values, false);

	/* PONDERING: Handle things like GtkSettings `gtk-button-images' in the future? */

	if (0 == g_strcmp0 (name, "x-thickness")) {
		// TODO
	} else if (0 == g_strcmp0 (name, "y-thickness")) {
		// TODO
	} else if (g_str_has_prefix (name, "Gtk")) {
		// TODO introspect style property
	} else if (g_ascii_isupper (name[0])) {
		// TODO introspect style property and feed back to gtk directly.
		// Non-gtk style properties (wnck, nautilus, ...) can't be
		// resolved offline because css2gtkrc doesn't link against them.
		// May cause some breakage, let's see how it goes.
		// TODO link css2gtkrc against gtk.
	}


	/* Fall back. */
	g_return_val_if_fail (_generic_property_class, false);
	g_return_val_if_fail (_generic_property_class->property_factory, false);
	return _generic_property_class->property_factory (grammar, block, name,
							  values, user_data);
}

static ccss_property_class_t const _properties[] = {
    {
	.name = "*",
	.property_create = NULL,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) property_convert,
	.property_factory = property_factory,
	.property_inherit = NULL
    }, {
	.name = NULL
    }
};

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
	ccss_grammar_t *self;

	self = ccss_cairo_grammar_create ();

	if (NULL == _generic_property_class) {
		_generic_property_class = ccss_grammar_lookup_property (self,
									"*");
	}

	/* TODO add properties */

	return self;
}

