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

#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "ccss-property.h"

static GHashTable *_conversion_funcs = NULL;

void
ccss_property_init (void)
{
	g_assert (NULL == _conversion_funcs);

	_conversion_funcs = g_hash_table_new (g_direct_hash, g_direct_equal);
}

void
ccss_property_shutdown	(void)
{
	g_hash_table_destroy (_conversion_funcs), _conversion_funcs = NULL;
}

void
ccss_property_register_conversion_function (GQuark			property,
					    ccss_property_convert_f	function)
{
	g_assert (_conversion_funcs);

	g_hash_table_insert (_conversion_funcs, (gpointer) property,
			     (gpointer) function);
}

bool
ccss_property_convert (void const		*property,
		       GQuark			 property_id,
		       ccss_property_type_t	 target,
		       void			*value)
{
	ccss_property_convert_f	 convert;

	g_assert (property && property_id && _conversion_funcs);

	convert = (ccss_property_convert_f) g_hash_table_lookup (_conversion_funcs, 
								 (gpointer) property_id);
	g_return_val_if_fail (convert, false);

	return convert (property, target, value);
}

ccss_property_spec_t
ccss_property_parse_spec (CRTerm const **value)
{
	char const *str;

	if (!*value) {
		return CCSS_PROPERTY_SPEC_UNSET;
	} else if (TERM_IDENT == (*value)->type) {
		str = cr_string_peek_raw_str ((*value)->content.str);
		if (0 == strcmp ("none", str)) {
			*value = (*value)->next;
			return CCSS_PROPERTY_SPEC_NONE;
		} else if (0 == strcmp ("inherit", str)) {
			*value = (*value)->next;
			return CCSS_PROPERTY_SPEC_INHERIT;
		}
	}

	return CCSS_PROPERTY_SPEC_SET;
}

