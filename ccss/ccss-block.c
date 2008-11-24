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

#include <glib.h>
#include "ccss-block-priv.h"
#include "ccss-cairo-property.h"
#include "ccss-property-priv.h"

void *
ccss_block_add_property (ccss_block_t	*self,
			 char const	*property_name,
			 void		*value)
{
	GQuark	 property_id;
	void	*old_property;

	property_id = g_quark_try_string (property_name);
	if (0 == property_id) {
		property_id = g_quark_from_string (property_name);
		/* FIXME: also hook into conversion mechanism? */
	}

	old_property = g_hash_table_lookup (self->properties,
					    (gpointer) property_id);

	g_hash_table_insert (self->properties,
			     (gpointer) property_id, value);

	return old_property;
}

ccss_block_t *
ccss_block_new (void)
{
	ccss_block_t *self;

	self = g_new0 (ccss_block_t, 1);
	self->properties = g_hash_table_new_full ((GHashFunc) g_direct_hash, 
						  (GEqualFunc) g_direct_equal,
						  NULL,
						  (GDestroyNotify) g_free);

	return self;
}

void
ccss_block_free (ccss_block_t *self)
{
	g_return_if_fail (self && self->properties);

	g_hash_table_destroy (self->properties), self->properties = NULL;
	g_free (self);
}

/* FIXME: only one mention of a property per block is supported ATM.
 * Need to store a list of properties instead and take care to apply in the correct order. */
void
ccss_block_insert_property (ccss_block_t		*self,
			    char const			*property_name,
			    ccss_property_base_t	*property)
{
	GQuark			 property_id;

	property_id = g_quark_try_string (property_name);
	if (0 == property_id) {
		property_id = g_quark_from_string (property_name);
	}

	g_hash_table_insert (
			self->properties, 
			(gpointer) property_id,
			(gpointer) property);

}

#ifdef CCSS_DEBUG

void
ccss_block_dump (ccss_block_t const *self)
{
	GHashTableIter	 iter;
	GQuark		 property_id;
	void const	*property;
	char		*strval;

	g_hash_table_iter_init (&iter, self->properties);
	while (g_hash_table_iter_next (&iter, (gpointer *) &property_id, (gpointer *) &property))  {

		if (ccss_property_convert (property, property_id, 
		    CCSS_PROPERTY_TYPE_STRING, &strval)) {
			printf ("%s: %s;\n", g_quark_to_string (property_id), strval);
			g_free (strval), strval = NULL;
		} else {
			g_message ("Failed to serialise property `%s'", g_quark_to_string (property_id));
		}
	}
}

#endif /* CCSS_DEBUG */

