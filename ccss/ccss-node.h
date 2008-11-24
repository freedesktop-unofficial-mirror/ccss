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

#ifndef CCSS_NODE_H
#define CCSS_NODE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <ccss/ccss-macros.h>

CCSS_BEGIN_DECLS

typedef struct ccss_node_ ccss_node_t;

/** 
 * ccss_node_is_a_f:
 * @self: a #ccss_node_t.
 *
 * Hook function to query whether a #ccss_node_t is of a certain type.
 *
 * Returns: %TRUE if matches.
 **/
typedef bool (*ccss_node_is_a_f) (ccss_node_t const *self, 
				  char const *type_name);

/** 
 * ccss_node_get_container_f:
 * @self: a #ccss_node_t.
 *
 * Hook function to query the container of a #ccss_node_t.
 *
 * Returns: newly allocated container node or %NULL.
 **/
typedef ccss_node_t * (*ccss_node_get_container_f) (ccss_node_t const *self);

/** 
 * ccss_node_get_base_style_f:
 * @self: a #ccss_node_t.
 *
 * Hook function to query the name of the style a #ccss_node_t derives from.
 *
 * Returns: base style or %NULL. The returned value must be valid until
 * it is released.
 **/
typedef ccss_node_t * (*ccss_node_get_base_style_f) (ccss_node_t const *self);

/** 
 * ccss_node_get_type_f:
 * @self: a #ccss_node_t.
 *
 * Hook function to query the type name of a #ccss_node_t.
 *
 * Returns: node type name or %NULL. The returned value must be valid until
 * the current stylesheet query returns.
 **/
typedef const char * (*ccss_node_get_type_f) (ccss_node_t const *self);

/**
 * ccss_node_get_instance_f:
 * @self: a #ccss_node_t.
 * 
 * Hook function to query for a unique numerical representation of a #ccss_node_t.
 *
 * Returns: unique numerical id or 0. If 0 is returned, node-based css style will not be considered.
 **/
typedef ptrdiff_t (*ccss_node_get_instance_f) (ccss_node_t const *self);

/** 
 * ccss_node_get_id_f:
 * @self: a #ccss_node_t.
 *
 * Hook function to query the ID of a #ccss_node_t.
 *
 * Returns: node ID or %NULL. The returned value must be valid until
 * the current stylesheet query returns.
 **/
typedef const char * (*ccss_node_get_id_f) (ccss_node_t const *self);

/** 
 * ccss_node_get_class_f:
 * @self: a #ccss_node_t.
 *
 * Hook function to query the class name of a #ccss_node_t.
 *
 * Returns: node class name or %NULL. The returned value must be valid until
 * the current stylesheet query returns.
 **/
typedef const char * (*ccss_node_get_class_f) (ccss_node_t const *self);

/** 
 * ccss_node_get_pseudo_class_f:
 * @self: a #ccss_node_t.
 *
 * Hook function to query the pseudo-class name of a #ccss_node_t.
 *
 * Returns: node pseudo-class name or %NULL. The returned value must be valid until
 * the current stylesheet query returns.
 **/
typedef const char * (*ccss_node_get_pseudo_class_f) (ccss_node_t const *self);

/** 
 * ccss_node_get_attribute_f:
 * @self: a #ccss_node_t.
 * @name: attribute name.
 *
 * Hook function to query a #ccss_node_t's attributes.
 *
 * Returns: attribute value or %NULL. The returned value must be valid until
 * the current stylesheet query returns.
 **/
typedef char * (*ccss_node_get_attribute_f) (ccss_node_t const	*self,
					    char const		*name);

/**
 * ccss_node_get_style_f:
 * @self: a #ccss_node_t.
 *
 * Hook function to query a #ccss_node_t's inline CSS style.
 *
 * Returns: the node's CSS properties or %NULL.
 **/
typedef const char * (*ccss_node_get_style_f) (ccss_node_t const *self);

/**
 * ccss_node_get_viewport_f:
 * @self:	a #ccss_node_t.
 * @x:		horizontal position.
 * @y:		vertical position.
 * 
 * Hook function to determine the position of a node in the viewport.
 **/
typedef bool (*ccss_node_get_viewport_f) (ccss_node_t const *self, 
					  uint32_t *x, uint32_t *y,
					  uint32_t *width, uint32_t *height);

/** 
 * ccss_node_release_f:
 * @self: a #ccss_node_t.
 *
 * Hook function to deallocate a #ccss_node_t instance.
 **/
typedef void (*ccss_node_release_f) (ccss_node_t *self);

/**
 * ccss_node_class_t:
 * @is_a:		a #ccss_node_is_a_f.
 * @get_container:	a #ccss_node_get_container_f.
 * @get_base_style:	a #ccss_node_get_base_style_f.
 * @get_instance:	a #ccss_node_get_instance_f.
 * @get_id:		a #ccss_node_get_id_f.
 * @get_type:		a #ccss_node_get_type_f.
 * @get_class:		a #ccss_node_get_class_f.
 * @get_pseudo_class:	a #ccss_node_get_pseudo_class_f.
 * @get_attribute:	a #ccss_node_get_attribute_f.
 * @get_style:		a #ccss_node_get_style_f.
 * @get_viewport:	a #ccss_node_get_viewport_f.
 * @release:		a #ccss_node_release_f.
 *
 * Dispatch table a CCSS consumer has to fill so the selection engine can 
 * retrieve information about the document the document.
 *
 * The implemented dispatch table needs to be passed to #ccss_init.
 **/
typedef struct {
	ccss_node_is_a_f		is_a;
	ccss_node_get_container_f	get_container;
	ccss_node_get_base_style_f	get_base_style;
	ccss_node_get_instance_f	get_instance;
	ccss_node_get_id_f		get_id;
	ccss_node_get_type_f		get_type;
	ccss_node_get_class_f		get_class;
	ccss_node_get_pseudo_class_f	get_pseudo_class;
	ccss_node_get_attribute_f	get_attribute;
	ccss_node_get_style_f		get_style;
	ccss_node_get_viewport_f	get_viewport;
	ccss_node_release_f		release;
} ccss_node_class_t;

/**
 * ccss_node_t:
 * 
 * Stack-allocatable struct representing a document node. Used for querying the 
 * #ccss_stylesheet_t.
 * 
 * <emphasis>Memory management:</emphasis> Unless specified otherwise, objects 
 * of this kind are under the responsibility of the libccss consumer.
 **/
struct ccss_node_ {
	/*< private >*/
	ccss_node_class_t const *node_class;
};

void
ccss_node_init (ccss_node_t		*self,
		ccss_node_class_t	*node_class);

// TODO move to -priv header.
bool ccss_node_is_a (ccss_node_t const *self, char const *type_name);

CCSS_END_DECLS

#endif /* CCSS_NODE_H */

