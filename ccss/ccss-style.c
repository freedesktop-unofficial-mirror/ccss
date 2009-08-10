/* vim: set ts=8 sw=8 noexpandtab: */

/* The `C' CSS Library.
 * Copyright (C) 2008 Robert Staudinger
 * Copyright (C) 2009 Intel Corporation
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
#include "ccss-property-generic.h"
#include "ccss-style-priv.h"
#include "config.h"

/**
 * ccss_style_create:
 *
 * Create a new #ccss_style_t.
 **/
ccss_style_t *
ccss_style_create (void)
{
	ccss_style_t *self;

	self = g_new0 (ccss_style_t, 1);
	self->properties = g_hash_table_new ((GHashFunc) g_direct_hash,
					     (GEqualFunc) g_direct_equal);
#ifdef CCSS_DEBUG
	self->selectors = g_hash_table_new_full ((GHashFunc) g_direct_hash,
						 (GEqualFunc) g_direct_equal,
						 NULL,
						 g_free);
#endif

	return self;
}

/**
 * ccss_style_destroy:
 * @self: a #ccss_style_t.
 *
 * Frees the style an all associated resources.
 **/
void
ccss_style_destroy (ccss_style_t *self)
{
	g_return_if_fail (self && self->properties);

	if (self->stylesheet) {
		ccss_stylesheet_destroy (self->stylesheet), self->stylesheet = NULL;
	}

	g_hash_table_destroy (self->properties), self->properties = NULL;
#ifdef CCSS_DEBUG
	g_hash_table_destroy (self->selectors), self->selectors = NULL;
#endif
	g_free (self);
}

/**
 * ccss_style_hash:
 * @self: a #ccss_style_t.
 *
 * Calculates a hash value that uniquely identifies a style. If two styles
 * have the same hash value they are equal.
 *
 * A hash value of 0 is returned for %NULL or empty styles.
 *
 * Returns: hash value.
 **/
uint32_t
ccss_style_hash (ccss_style_t const *self)
{
	uint32_t hash = 0;

	g_return_val_if_fail (self, 0);
	g_return_val_if_fail (self->properties, 0);
	g_return_val_if_fail (g_hash_table_size (self->properties), 0);

	{
	ccss_property_t const	*property;
	GHashTableIter		 iter;
	int			 n_properties = g_hash_table_size (self->properties);
	void			*property_ptrs[n_properties * sizeof (void *) + 1];
	char const		*property_str = (char const *) property_ptrs;
	int			 i = 0;

	/* Accumulate string of property pointers. */
	g_hash_table_iter_init (&iter, self->properties);
	while (g_hash_table_iter_next (&iter, NULL, (gpointer *) &property)) {
		property_ptrs[i] = (void *) property;
		i++;
	}
	property_ptrs[i] = NULL;

	/* sdbm hash */
	while (0 != (i = *property_str++)) {
		hash = i + (hash << 6) + (hash << 16) - hash;
	}

	}

	return hash;
}

/**
 * ccss_style_get_stylesheet:
 * @self: a #ccss_gramar_t.
 *
 * Returns: the #ccss_stylesheet_t associated with @self.
 *	    If @self is a nil object, 0 will be returned.
 **/
ccss_stylesheet_t *
ccss_style_get_stylesheet (ccss_style_t const *self)
{
	g_return_val_if_fail (self, NULL);

	return self->stylesheet;
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
	ccss_property_t const	*property;
	bool			 ret;

	g_return_val_if_fail (self && property_name && value, false);

	property = NULL;
	ret = ccss_style_get_property (self, property_name, &property);
	if (!ret) {
		return false;
	}
	g_return_val_if_fail (property, false);

	/* Have conversion function? */
	g_return_val_if_fail (property->vtable, false);
	if (NULL == property->vtable->convert) {
		return false;
	}

	return property->vtable->convert (property,
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
	ccss_property_t const	*property;
	bool			 ret;

	g_return_val_if_fail (self && property_name && value, false);

	property = NULL;
	ret = ccss_style_get_property (self, property_name, &property);
	if (!ret) {
		return false;
	}
	g_return_val_if_fail (property, false);

	/* Have conversion function? */
	g_return_val_if_fail (property->vtable, false);
	if (NULL == property->vtable->convert) {
		return false;
	}

	return property->vtable->convert (property,
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
			 ccss_property_t const	**property)
{
	GQuark property_id;

	g_return_val_if_fail (self && property_name && property, false);

	property_id = g_quark_try_string (property_name);
	if (0 == property_id) {
		/* Property unknown, no need to look up. */
		return false;
	}

	return g_hash_table_lookup_extended (self->properties,
					     (gconstpointer) property_id, NULL,
					     (void **) property);
}

/**
 * ccss_style_set_property:
 * @self:		a #ccss_style_t.
 * @property_name:	name of the property.
 * @value:		the property to insert into the style.
 *
 * Insert custom property. This is for custom property implementations only.
 **/
void
ccss_style_set_property	(ccss_style_t		*self,
			 char const		*property_name,
			 ccss_property_t const	*value)
{
	GQuark property_id;

	g_return_if_fail (self && property_name && value);

	property_id = g_quark_from_string (property_name);
	g_hash_table_insert (self->properties,
			     (gpointer) property_id, (gpointer) value);
}

/**
 * @self:	a #ccss_style_t.
 * @property:	the property.
 * @selector:	the selector that caused @property to be applied to this style.
 *
 * Track which selector caused assignment of a property to a style. This is for
 * debugging purpose only, when serialising @self the properties will be
 * annotated with selector information.
 **/
void
ccss_style_set_property_selector (ccss_style_t		*self,
				  ccss_property_t const	*property,
				  ccss_selector_t const	*selector)
{
#ifdef CCSS_DEBUG
	GString *annotation;

	g_return_if_fail (self);
	g_return_if_fail (property);
	g_return_if_fail (selector);

	annotation = g_string_new ("'");
	ccss_selector_serialize_selector (selector, annotation);
	g_string_append (annotation, "'");

	g_string_append (annotation, ", ");
	ccss_selector_serialize_specificity (selector, annotation);

	/* Hash takes ownership. */
	g_hash_table_insert (self->selectors, (gpointer) property, annotation->str);
	g_string_free (annotation, false);
#endif
}

/**
 * @self:	a #ccss_style_t.
 * @property:	the property.
 * @selector:	the selector that caused @property to be applied to this style.
 *
 * Track which selector caused assignment of a property to a style. This is for
 * debugging purpose only, when serialising @self the properties will be
 * annotated with selector information.
 *
 * See: #ccss_style_set_property_selector
 **/
void
ccss_style_set_property_selector_string (ccss_style_t		*self,
					 ccss_property_t const	*property,
					 char const		*selector)
{
#ifdef CCSS_DEBUG
	g_return_if_fail (self);
	g_return_if_fail (property);
	g_return_if_fail (selector);

	g_hash_table_insert (self->selectors, (gpointer) property, g_strdup (selector));
#endif
}

/**
 * ccss_style_interpret_property:
 * @self:          a #ccss_style_t.
 * @property_name: name of the property.
 * @property_ctor: property constructor function.
 * @user_data:     user data passed to @property_ctor.
 * @property:      place to store the interpreted property.
 *
 * Query for a custom property, interpret it with @property_ctor and
 * return the interpreted property in @property.
 *
 * Returns: %TRUE if property was found and interpretation
 * was successful.
 **/
bool
ccss_style_interpret_property (ccss_style_t const	 *self,
                               char const		 *property_name,
                               ccss_property_create_f	  property_ctor,
                               void			 *user_data,
                               ccss_property_t		**property)
{
	GQuark				 property_id;
	ccss_property_generic_t const	*generic_property;

	g_return_val_if_fail (self, false);
	g_return_val_if_fail (property_name, false);
	g_return_val_if_fail (property_ctor, false);

	property_id = g_quark_try_string (property_name);
	if (0 == property_id) {
		/* Property unknown, no need to look up. */
		return false;
	}

	g_hash_table_lookup_extended (self->properties,
				      (gconstpointer) property_id, NULL,
				      (void **) &generic_property);

	if (generic_property) {
		*property = property_ctor (ccss_stylesheet_get_grammar (self->stylesheet),
					   generic_property->values,
					   user_data);
		return (bool) *property;
	}

	return false;
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
		if (NULL == strval) {
			strval = g_strdup ("<unknown>");
		}
#ifdef CCSS_DEBUG
{
		/* Also dump a comment which selector caused each
		 * property. */
		char const *selector = (char const *)
				g_hash_table_lookup (self->selectors,
						     property);
		printf ("%s: %s; /* %s */\n",
			g_quark_to_string (property_id),
			strval,
			selector);
}
#else
		/* Plain dump in CSS syntax. */
		printf ("%s: %s;\n",
			g_quark_to_string (property_id),
			strval);
#endif /* CCSS_DEBUG */
		g_free (strval), strval = NULL;
	}
}

