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

#include <string.h>
#include <glib.h>
#include "ccss-node-priv.h"
#include "config.h"

static bool
is_a (ccss_node_t const	*self,
      char const	*type_name)
{
	return false;
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

static char const **
get_classes (ccss_node_t const *self)
{
	return NULL;
}

static char const **
get_pseudo_classes (ccss_node_t const *self)
{
	return NULL;
}

static char *
get_attribute (ccss_node_t const	*self,
	       char const		*name)
{
	return NULL;
}

static char const *
get_style (ccss_node_t const    *self,
	   unsigned int		 descriptor)
{
	return NULL;
}

static bool
get_viewport (ccss_node_t const	*self,
	      double		*x,
	      double		*y,
	      double		*width,
	      double		*height)
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
	.get_classes		= get_classes,
	.get_pseudo_classes	= get_pseudo_classes,
	.get_attribute		= get_attribute,
	.get_style		= get_style,
	.get_viewport		= get_viewport,
	.release		= release
};

typedef void (*node_f) (void);

/**
 * ccss_node_create:
 * @node_class:	a #ccss_node_class_t vtable.
 * @n_methods:  number of methods in @node_class.
 * @user_data:  data to associate with the node, typically a pointer to the documents native node.
 *
 * Creates a new #ccss_node_t instance and of class @node_class.
 *
 * Returns: a #ccss_node_t.
 **/
ccss_node_t *
ccss_node_create (ccss_node_class_t const       *node_class,
		  unsigned int			 n_methods,
		  void				*user_data)
{
	ccss_node_t	*self;
	node_f const	*user_vtable;
	node_f const	*default_vtable;
	node_f		*vtable;

	g_return_val_if_fail (node_class, NULL);
	g_return_val_if_fail (n_methods > 0, NULL);
	g_return_val_if_fail (n_methods <= CCSS_NODE_CLASS_N_METHODS (_default_node_class), NULL);

	self = g_new0 (ccss_node_t, 1);

	user_vtable = (node_f const *) node_class;
	default_vtable = (node_f const *) &_default_node_class;
	vtable = (node_f *) self; /* The node class is at the start of the node. */
	for (unsigned int i = 0; i < n_methods; i++) {
		if (user_vtable[i])
			vtable[i] = user_vtable[i];
		else
			vtable[i] = default_vtable[i];
	}

	self->user_data = user_data;

	return self;
}

/**
 * ccss_node_destroy:
 * @self: a #ccss_node_t.
 *
 * Frees @self.
 */
void
ccss_node_destroy (ccss_node_t *self)
{
	g_free (self);
}

/**
 * ccss_node_is_a:
 *
 * Wrapper implementation of #ccss_node_is_a_f. If the `is_a' function is not
 * implemented `get_type' is used as a fallback.
 **/
bool
ccss_node_is_a (ccss_node_t	*self,
		char const	*type_name)
{
	char const *node_type_name;

	g_return_val_if_fail (self, false);
	g_return_val_if_fail (type_name, false);

	if (self->node_class.is_a != is_a) {
		return self->node_class.is_a (self, type_name);
	} else {
		node_type_name = ccss_node_get_type (self);
		return node_type_name ?
			0 == g_strcmp0 (type_name, node_type_name) :
			false;
	}
}

/**
 * ccss_node_get_user_data:
 * @self: a #ccss_node_t.
 *
 * Retrieve the user data associated with the node.
 *
 * Returns: node user data.
 **/
void *
ccss_node_get_user_data (ccss_node_t const *self)
{
	g_return_val_if_fail (self, NULL);

	return self->user_data;
}

ccss_node_t *
ccss_node_get_container (ccss_node_t *self)
{
	g_return_val_if_fail (self, NULL);

	return self->node_class.get_container (self);
}

ccss_node_t *
ccss_node_get_base_style (ccss_node_t   *self)
{
	g_return_val_if_fail (self, NULL);

	return self->node_class.get_base_style (self);
}

char const *
ccss_node_get_type (ccss_node_t		*self)
{
	g_return_val_if_fail (self, NULL);

	if (NULL == self->type_name)
		self->type_name = self->node_class.get_type (self);

	return self->type_name;
}

ptrdiff_t
ccss_node_get_instance (ccss_node_t     *self)
{
	g_return_val_if_fail (self, 0);

	if (0 == self->instance)
		self->instance = self->node_class.get_instance (self);

	return self->instance;
}

char const *
ccss_node_get_id (ccss_node_t   *self)
{
	g_return_val_if_fail (self, NULL);

	if (NULL == self->id)
		self->id = self->node_class.get_id (self);

	return self->id;
}

const char **
ccss_node_get_classes (ccss_node_t      *self)
{
	g_return_val_if_fail (self, NULL);

	if (NULL == self->css_classes)
		self->css_classes = self->node_class.get_classes (self);

	return self->css_classes;
}

const char **
ccss_node_get_pseudo_classes (ccss_node_t       *self)
{
	g_return_val_if_fail (self, NULL);

	if (NULL == self->pseudo_classes)
		self->pseudo_classes = self->node_class.get_pseudo_classes (self);

	return self->pseudo_classes;
}

char *
ccss_node_get_attribute (ccss_node_t const	*self,
			 char const		*name)
{
	g_return_val_if_fail (self, NULL);

	return self->node_class.get_attribute (self, name);
}

char const *
ccss_node_get_style (ccss_node_t	*self,
		     unsigned int	 descriptor)
{
	g_return_val_if_fail (self, NULL);

	if (NULL == self->inline_style)
		self->inline_style = self->node_class.get_style (self,
								 descriptor);

	return self->inline_style;
}

bool
ccss_node_get_viewport (ccss_node_t const	*self,
			double			*x,
			double			*y,
			double			*width,
			double			*height)
{
	g_return_val_if_fail (self, false);

	return self->node_class.get_viewport (self, x, y, width, height);
}

void
ccss_node_release (ccss_node_t *self)
{
	g_return_if_fail (self);

	self->node_class.release (self);
}

