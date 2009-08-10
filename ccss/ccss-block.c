/* vim: set ts=8 sw=8 noexpandtab: */

/* The `C' CSS Library.
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
#include "config.h"

ccss_block_t *
ccss_block_create (void)
{
	ccss_block_t *self;

	self = g_new0 (ccss_block_t, 1);
	self->reference_count = 1;
	self->properties = g_hash_table_new_full ((GHashFunc) g_direct_hash, 
						  (GEqualFunc) g_direct_equal,
						  NULL,
						  (GDestroyNotify) ccss_property_destroy);

	return self;
}

void
ccss_block_destroy (ccss_block_t *self)
{
	g_return_if_fail (self && self->properties);

	self->reference_count--;

	if (self->reference_count == 0) {
		g_hash_table_destroy (self->properties), self->properties = NULL;
		g_free (self);
	}
}

ccss_block_t *
ccss_block_reference (ccss_block_t *self)
{
	g_return_val_if_fail (self, NULL);

	self->reference_count++;

	return self;
}

/**
 * ccss_block_add_property:
 * @self:		a #ccss_block_t.
 * @property_name:	the property name, e.g. %background-color.
 * @property:		pointer to the property instance.
 *
 * Adds a property to a CSS block.
 **/
void
ccss_block_add_property (ccss_block_t		*self,
			 char const		*property_name,
			 ccss_property_t	*property)
{
	GQuark property_id;

	property_id = g_quark_try_string (property_name);
	if (0 == property_id) {
		property_id = g_quark_from_string (property_name);
	}

	g_hash_table_insert (
			self->properties, 
			(gpointer) property_id,
			(gpointer) property);

}

void
ccss_block_dump (ccss_block_t const *self)
{
	GHashTableIter		 iter;
	GQuark			 property_id;
	ccss_property_t const	*property;
	char			*strval;

	g_hash_table_iter_init (&iter, self->properties);
	while (g_hash_table_iter_next (&iter, (gpointer *) &property_id, (gpointer *) &property))  {

		strval = NULL;
		if (CCSS_PROPERTY_STATE_NONE == property->state ||
		    CCSS_PROPERTY_STATE_INHERIT == property->state) {

			strval = g_strdup (ccss_property_state_serialize (property->state));

		} else if (property->vtable &&
			   property->vtable->convert) {

			property->vtable->convert (property,
						   CCSS_PROPERTY_TYPE_STRING,
						   &strval);
		}

		if (NULL == strval)
			strval = g_strdup ("<unknown>");

		printf ("\t%s: %s;\n",
			g_quark_to_string (property_id),
			strval);
		g_free (strval);
	}
}

