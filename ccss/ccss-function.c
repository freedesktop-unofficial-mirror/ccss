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
#include "ccss-function-impl.h"

/*
 * FIXME: move to reference counting.
 * So statically allocated function structs will never actually be destroyed.
 */

/**
 * ccss_function_create:
 * @name:       function identifier as used in CSS.
 * @function:   function pointer.
 *
 * Create a new function instance.
 *
 * Returns: a #ccss_function_t.
 **/
ccss_function_t *
ccss_function_create (char const	*name,
		      ccss_function_f	 function)
{
	ccss_function_t *self;

	g_return_val_if_fail (name, NULL);
	g_return_val_if_fail (function, NULL);

	self = g_new0 (ccss_function_t, 1);
	self->name = g_strdup (name);
	self->function = function;

	return self;
}

/**
 * ccss_function_destroy:
 *
 * Destroy a function instance.
 **/
void
ccss_function_destroy (ccss_function_t *self)
{
	g_return_if_fail (self);

	self->reference_count--;

	if (0 == self->reference_count) {
		/* This string is owned for dynamically created functions. */
		g_free ((char *) self->name);
		g_free (self);
	}
}

/**
 * ccss_function_reference:
 * @self: a #ccss_function_t.
 *
 * Increases the reference count on @self by one. This prevents @self from being
 * destroyed until a matching call to ccss_function_destroy() is made.
 *
 * The number of references to a #ccss_function_t can be acquired using
 * ccss_function_get_reference_count().
 *
 * Returns: the referenced #ccss_function_t.
 **/
ccss_function_t *
ccss_function_reference (ccss_function_t *self)
{
	g_return_val_if_fail (self, NULL);

	self->reference_count++;

	return self;
}

/**
 * ccss_function_get_reference_count:
 * @self: a #ccss_function_t.
 *
 * Returns: the current reference count of @self.
 *	    If @self is a nil object, 0 will be returned.
 **/
unsigned int
ccss_function_get_reference_count (ccss_function_t const *self)
{
	g_return_val_if_fail (self, 0);

	return self->reference_count;
}

