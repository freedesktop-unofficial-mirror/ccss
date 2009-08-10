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

#include <gmodule.h>
#include "ccss-cairo-appearance.h"
#include "ccss-cairo-appearance-parser.h"
#include "config.h"

typedef char const *
(*ccss_appearance_module_get_interface_version) (void);

static GHashTable *_module_hash = NULL;

static ccss_cairo_appearance_module_t *
module_create (char const *name)
{
	char		*module_path;
	ccss_cairo_appearance_module_t	*module = NULL;

	if (NULL == _module_hash) {
		_module_hash = g_hash_table_new (g_str_hash, g_str_equal);
	}

	module_path = g_module_build_path (CCSS_CAIRO_APPEARANCE_MODULE_PATH,
					   name);

	/* Return module path from hash. */
	module = g_hash_table_lookup (_module_hash, module_path);
	if (module) {
	
		module->reference_count++;
	
	} else {

		GModule *gmodule = g_module_open (module_path,
						  G_MODULE_BIND_LAZY | G_MODULE_BIND_LOCAL);
		if (gmodule) {
			/* Check version. */
			ccss_appearance_module_get_interface_version get_version = NULL;
			bool ret = g_module_symbol (gmodule,
						    "ccss_appearance_module_get_interface_version",
						    (gpointer *) &get_version);
			if (ret && get_version) {
				char const *version = get_version ();
				if (0 == g_strcmp0 (version, CCSS_CAIRO_APPEARANCE_MODULE_INTERFACE_VERSION)) {
					module = g_new0 (ccss_cairo_appearance_module_t, 1);
					module->reference_count = 1;
					module->module_path = g_strdup (module_path);
					module->module = gmodule;
				} else {
					g_debug ("%s : skipping module '%s' with version '%s' (require '%s')",
						 G_STRLOC,
						 module_path,
						 version,
						 CCSS_CAIRO_APPEARANCE_MODULE_INTERFACE_VERSION);
				}
			} else {
				g_critical ("%s : failed to look up symbol '%s' of module '%s'",
					    G_STRLOC,
					    "ccss_appearance_module_get_interface_version",
					    module_path);
			}
		} else {
			g_critical ("%s : could not open module '%s'",
				    G_STRLOC,
				    module_path);
		}
		
		
		if (module) {
			/* Return module path from hash. */
			g_hash_table_insert (_module_hash,
					     module->module_path,
					     module);
		}
	}
	g_free (module_path), module_path = NULL;

	return module;
}

static void
module_destroy (ccss_cairo_appearance_module_t *module)
{
	module->reference_count--;

	if (module->reference_count == 0) {

		g_hash_table_remove (_module_hash, module->module_path);
		g_free (module->module_path);
		g_module_close (module->module);
		g_free (module);

		if (g_hash_table_size (_module_hash) == 0) {
			g_hash_table_destroy (_module_hash);
			_module_hash = NULL;
		}
	}
}

static ccss_cairo_appearance_draw_f
module_get_symbol (ccss_cairo_appearance_module_t const       *module,
		   char const		*name)
{
	ccss_cairo_appearance_draw_f draw_func = NULL;

	g_return_val_if_fail (module, NULL);
	g_return_val_if_fail (name, NULL);

	g_module_symbol (module->module, name, (gpointer *) &draw_func);

	return draw_func;
}

static ccss_property_class_t const *
peek_property_class (void);

static bool
appearance_factory (ccss_grammar_t const	*grammar,
		    ccss_block_t		*self,
		    char const			*name,
		    CRTerm const		*values,
		    void			*user_data)
{
	CRTerm const	*iter;
	ccss_cairo_appearance_t	*appearance;
	ccss_cairo_appearance_t	 a;

	ccss_property_init (&a.base, peek_property_class ());

	iter = values;
	if (iter->type == TERM_IDENT) {
		a.appearance = g_strdup (cr_string_peek_raw_str (iter->content.str));
	} else {
		return false;
	}

	iter = iter->next;
	if (iter->type == TERM_IDENT) {
		char const *module_name = cr_string_peek_raw_str (iter->content.str);
		a.module = module_create (module_name);
		a.draw_function = module_get_symbol (a.module, a.appearance);
	}

	if (a.module && !a.draw_function) {
		module_destroy (a.module);
		a.module = NULL;
	}

	a.base.state = CCSS_PROPERTY_STATE_SET;
	appearance = g_new0 (ccss_cairo_appearance_t, 1);
	*appearance = a;
	ccss_block_add_property (self, name, &appearance->base);

	return true;	
}

static bool
appearance_convert (ccss_cairo_appearance_t const		*property,
		    ccss_property_type_t	 target,
		    void			*value)
{
	char *ret;

	g_return_val_if_fail (property && value, false);

	if (CCSS_PROPERTY_TYPE_DOUBLE == target)
		return false;

	if (property->module) {
		ret = g_strdup_printf ("%s \"%s\"", 
				       property->appearance,
				       property->module->module_path);
	
	} else {
		ret = g_strdup (property->appearance);
	}

	* (char **) value = ret;

	return true;
}

static void
appearance_destroy (ccss_cairo_appearance_t *self)
{
	g_free (self->appearance);

	if (self->module) {
		module_destroy (self->module);
	}

	g_free (self);
}

static ccss_property_class_t const _ptable[] = {
    {
	.name = "ccss-appearance",
	.create = NULL,
	.destroy = (ccss_property_destroy_f) appearance_destroy,
	.convert = (ccss_property_convert_f) appearance_convert,
	.factory = appearance_factory,
	.inherit = NULL
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
ccss_cairo_appearance_parser_get_property_classes (void)
{
	return _ptable;
}

