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
#include "ccss-node.h"

static bool
is_a (ccss_node_t const	*self,
      char const	*type_name)
{
	return FALSE;
}

static ccss_node_t * 
get_container (ccss_node_t const *self)
{
	return NULL;
}

static ccss_node_t * 
get_base_style (ccss_node_t const *self)
{
	return NULL;
}

static char const * 
get_type (ccss_node_t const *self)
{
	return NULL;
}

static ptrdiff_t
get_instance (ccss_node_t const *self)
{
	return 0;
}

static char const * 
get_id (ccss_node_t const *self)
{
	return NULL;
}

static char const * 
get_class (ccss_node_t const *self)
{
	return NULL;
}

static char const * 
get_pseudo_class (ccss_node_t const *self)
{
	return NULL;
}

static char * 
get_attribute (ccss_node_t const	*self,
	       char const	*name)
{
	return NULL;
}

static char const * 
get_style (ccss_node_t const *self)
{
	return NULL;
}

static bool
get_viewport (ccss_node_t const	*self,
	      uint32_t		*x,
	      uint32_t		*y,
	      uint32_t		*width,
	      uint32_t		*height)
{
	return false;
}

static void
release (ccss_node_t *self)
{
	return;
}

static const ccss_node_class_t _default_node_class = {
	.is_a			= is_a,
	.get_container		= get_container,
	.get_base_style		= get_base_style,
	.get_instance		= get_instance,
	.get_id			= get_id,
	.get_type		= get_type,
	.get_class		= get_class,
	.get_pseudo_class	= get_pseudo_class,
	.get_attribute		= get_attribute,
	.get_style		= get_style,
	.get_viewport		= get_viewport,
	.release		= release
};

typedef void (*node_f) (void);
#define N_ELEMENTS(vtable_) (sizeof (vtable_) / sizeof (node_f))

/**
 * ccss_node_init:
 * @self:	a #ccss_node_t embedding structure.
 * @node_class:	a #ccss_node_class_t vtable.
 *
 * Initializes @node_class by filling unset functions with the default
 * implementations and attaches it to @self.
 **/
void
ccss_node_init (ccss_node_t		*self,
		ccss_node_class_t	*node_class)
{
	node_f const	*default_vtable;
	node_f		*vtable;

	g_return_if_fail (self && node_class);

	/* Initialize the node class.
	 * FIXME: run only once per node class?  */
	default_vtable = (node_f const *) &_default_node_class;
	vtable = (node_f *) node_class;
	for (unsigned int i = 0; i < N_ELEMENTS (_default_node_class); i++) {
		if (NULL == vtable[i])
			vtable[i] = default_vtable[i];
	}

	/* Initializse node. */
	self->node_class = node_class;
}

/**
 * ccss_node_is_a:
 *
 * Wrapper implementation of #ccss_node_is_a_f. If the `is_a' function is not
 * implemented `get_type' is used as a fallback.
 **/
bool
ccss_node_is_a (ccss_node_t const	*self,
		char const		*type_name)
{
	char const *node_type_name;

	if (self->node_class->is_a != is_a) {
		return self->node_class->is_a (self, type_name);
	} else {
		node_type_name = self->node_class->get_type (self);
		return node_type_name ?
			0 == strcmp (type_name, node_type_name) :
			false;
	}
}

