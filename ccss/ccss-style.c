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

#include <string.h>
#include <glib.h>
#include "ccss-style-priv.h"
#include "config.h"

/**
 * ccss_style_new:
 *
 * Create a new #ccss_style_t.
 **/
ccss_style_t *
ccss_style_new (void)
{
	ccss_style_t *self;

	self = g_new0 (ccss_style_t, 1);
	self->properties = g_hash_table_new ((GHashFunc) g_direct_hash, 
					     (GEqualFunc) g_direct_equal);

	return self;
}

/**
 * ccss_style_free:
 * @self: a #ccss_style_t.
 *
 * Frees the style an all associated resources.
 **/
void
ccss_style_free (ccss_style_t *self)
{
	g_return_if_fail (self && self->properties);

	g_hash_table_destroy (self->properties), self->properties = NULL;
	g_free (self);
}

ccss_property_base_t const *
ccss_style_lookup_property (ccss_style_t const	*self,
			    GQuark		 property_id)
{
	g_assert (self);

	return (ccss_property_base_t const *)
			g_hash_table_lookup (self->properties,
					     (gpointer) property_id);

}

/**
 * ccss_style_get_double:
 * @self:		a #ccss_style_t.
 * @property_name:	name of the property.
 * @value:		location to store the converted property.
 *
 * Query a numeric property.
 *
 * Returns: %TRUE if the property was found and could be converted.
 **/
bool 
ccss_style_get_double (ccss_style_t const	*self,
		       char const		*property_name,
		       double			*value)
{
	GQuark				 property_id;
	ccss_property_base_t const	*property;

	g_return_val_if_fail (self && property_name && value, false);

	property_id = g_quark_try_string (property_name);
	if (0 == property_id) {
		g_warning ("Unknown property `%s'", property_name);
		return false;
	}

	property = ccss_style_lookup_property (self, property_id);
	if (NULL == property)
		return false;

	/* Have conversion function? */
	g_return_val_if_fail (property->property_class, false);
	if (NULL == property->property_class->property_convert) {
		return false;
	}

	return property->property_class->property_convert (property,
							   CCSS_PROPERTY_TYPE_DOUBLE,
							   value);
}

/**
 * ccss_style_get_string:
 * @self:		a #ccss_style_t.
 * @property_name:	name of the property.
 * @value:		location to store the converted property.
 *
 * Query a string property.
 * 
 * Returns: %TRUE if the property was found and could be converted.
 **/
bool 
ccss_style_get_string (ccss_style_t const	 *self,
		       char const		 *property_name,
		       char			**value)
{
	GQuark				 property_id;
	ccss_property_base_t const	*property;

	g_return_val_if_fail (self && property_name && value, false);

	property_id = g_quark_try_string (property_name);
	if (0 == property_id) {
		g_warning ("Unknown property `%s'", property_name);
		return false;
	}

	property = ccss_style_lookup_property (self, property_id);
	if (NULL == property)
		return false;

	/* Have conversion function? */
	g_return_val_if_fail (property->property_class, false);
	if (NULL == property->property_class->property_convert) {
		return false;
	}

	return property->property_class->property_convert (property,
							   CCSS_PROPERTY_TYPE_STRING,
							   value);
}

/**
 * ccss_style_get_property:
 * @self:		a #ccss_style_t.
 * @property_name:	name of the property.
 * @value:		location to store the raw property pointer.
 * 
 * Query a custom property.
 *
 * Returns: %TRUE if the property was found.
 **/
bool
ccss_style_get_property	(ccss_style_t const	 *self,
			 char const		 *property_name,
			 void			**value)
{
	GQuark		 property_id;

	g_return_val_if_fail (self && property_name && value, false);

	property_id = g_quark_try_string (property_name);
	if (0 == property_id) {
		g_warning ("Unknown property `%s'", property_name);
		return false;
	}

	return g_hash_table_lookup_extended (self->properties,
					     (gconstpointer) property_id, NULL,
					     value);
}

/**
 * ccss_style_set_property:
 * @self:		a #ccss_style_t.
 * @property_name:	name of the property.
 * @value:		location to store the raw property pointer.
 * 
 * Insert custom property. This is for custom property implementations only.
 **/
void
ccss_style_set_property	(ccss_style_t 			*self,
			 char const			*property_name,
			 ccss_property_base_t const	*value)
{
	GQuark property_id;

	g_return_if_fail (self && property_name && value);

	property_id = g_quark_try_string (property_name);
	g_return_if_fail (property_id);

	g_hash_table_insert (self->properties, (gconstpointer) property_id, value);
}

/**
 * ccss_style_foreach:
 * @self:	a #ccss_style_t.
 * @func:	a #ccss_style_iterator_f.
 * @user_data:	user data to pass to the iterator function.
 *
 * The iterator function @func is called for each type in the style.
 **/
void
ccss_style_foreach (ccss_style_t const		*self,
		    ccss_style_iterator_f	 func,
		    void			*user_data)
{
	GHashTableIter 	 iter;
	GQuark		 property_id;
	gpointer	 value;
	char const	*property_name;

	g_return_if_fail (self && func);

	g_hash_table_iter_init (&iter, self->properties);
	while (g_hash_table_iter_next (&iter, (gpointer) &property_id, &value)) {

		property_name = g_quark_to_string (property_id);
		func (self, property_name, user_data);
	}
}

/**
 * ccss_style_dump:
 * @self:	a ccss_style_t.
 *
 * Print informations about the internal state of this object.
 **/
void
ccss_style_dump (ccss_style_t const *self)
{
	GHashTableIter			 iter;
	GQuark				 property_id;
	ccss_property_base_t const	*property;
	char				*strval;
	bool				 ret;

	g_hash_table_iter_init (&iter, self->properties);
	while (g_hash_table_iter_next (&iter, (gpointer *) &property_id, (gpointer *) &property))  {

		if (NULL == property->property_class || 
		    NULL == property->property_class->property_convert) {
			g_warning ("No conversion function for property `%s'",
				   g_quark_to_string (property_id));
			continue;
		}

		ret = property->property_class->property_convert (property,
								  CCSS_PROPERTY_TYPE_STRING,
								  &strval);
		if (ret) {
			printf ("%s: %s;\n", g_quark_to_string (property_id), strval);
			g_free (strval), strval = NULL;
		} else {
			g_message ("Failed to serialise property `%s'", g_quark_to_string (property_id));
		}
	}
}

