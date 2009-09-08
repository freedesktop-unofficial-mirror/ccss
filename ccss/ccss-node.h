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
 * @self:	a #ccss_node_t.
 * @type_name:	name to match against, e.g. %div.
 *
 * Hook function to query whether a #ccss_node_t is of a certain type.
 *
 * Returns: %TRUE if matches.
 **/
typedef bool (*ccss_node_is_a_f) (ccss_node_t	*self,
				  char const	*type_name);

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
 * ccss_node_get_classes_f:
 * @self: a #ccss_node_t.
 *
 * Hook function to query the class names of a #ccss_node_t.
 *
 * Returns: %NULL-terminated node classes arry or %NULL.
 * The returned values must be valid until the current stylesheet query returns.
 **/
typedef const char ** (*ccss_node_get_classes_f) (ccss_node_t const *self);

/** 
 * ccss_node_get_pseudo_classes_f:
 * @self: a #ccss_node_t.
 *
 * Hook function to query the pseudo-class name of a #ccss_node_t.
 *
 * Returns: %NULL-terminated node pseudo-classes array or %NULL.
 * The returned values must be valid until the current stylesheet query returns.
 **/
typedef const char ** (*ccss_node_get_pseudo_classes_f) (ccss_node_t const *self);

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
 * @descriptor: handle to unload this style from the stylesheet later on.
 *
 * Hook function to query a #ccss_node_t's inline CSS style.
 *
 * See: ccss_stylesheet_unload().
 *
 * Returns: the node's CSS properties or %NULL.
 **/
typedef const char * (*ccss_node_get_style_f) (ccss_node_t const	*self,
					       unsigned int		 descriptor);

/**
 * ccss_node_get_viewport_f:
 * @self:	a #ccss_node_t.
 * @x:		horizontal position.
 * @y:		vertical position.
 * @width:	width of viewport.
 * @height:	height of viewport.
 * 
 * Hook function to determine the position of a node in the viewport.
 *
 * Returns: %TRUE if a valid viewport position has been assigned to the out parameters.
 **/
typedef bool (*ccss_node_get_viewport_f) (ccss_node_t const     *self,
					  double		*x,
					  double		*y,
					  double		*width,
					  double		*height);

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
 * @get_classes:	a #ccss_node_get_classes_f.
 * @get_pseudo_classes:	a #ccss_node_get_pseudo_classes_f.
 * @get_attribute:	a #ccss_node_get_attribute_f.
 * @get_style:		a #ccss_node_get_style_f.
 * @get_viewport:	a #ccss_node_get_viewport_f.
 * @release:		a #ccss_node_release_f.
 *
 * Dispatch table a CCSS consumer has to fill so the selection engine can 
 * retrieve information about the document the document.
 *
 * The implemented dispatch table needs to be passed to #ccss_node_init.
 **/
typedef struct {
	ccss_node_is_a_f		is_a;
	ccss_node_get_container_f	get_container;
	ccss_node_get_base_style_f	get_base_style;
	ccss_node_get_instance_f	get_instance;
	ccss_node_get_id_f		get_id;
	ccss_node_get_type_f		get_type;
	ccss_node_get_classes_f		get_classes;
	ccss_node_get_pseudo_classes_f	get_pseudo_classes;
	ccss_node_get_attribute_f	get_attribute;
	ccss_node_get_style_f		get_style;
	ccss_node_get_viewport_f	get_viewport;
	ccss_node_release_f		release;
	/*< private >*/
	void (*_padding_0) (void);
	void (*_padding_1) (void);
	void (*_padding_2) (void);
	void (*_padding_3) (void);
	void (*_padding_4) (void);
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
	CCSS_DEPRECATED (ccss_node_class_t const *node_class);

	CCSS_DEPRECATED (ptrdiff_t         instance);
	CCSS_DEPRECATED (char const       *id);
	CCSS_DEPRECATED (char const       *type_name);
	CCSS_DEPRECATED (char const      **css_classes);
	CCSS_DEPRECATED (char const      **pseudo_classes);
	CCSS_DEPRECATED (char const       *inline_style);
	CCSS_DEPRECATED (void		  *_padding_0);
	CCSS_DEPRECATED (void		  *_padding_1);
	CCSS_DEPRECATED (void		  *_padding_2);
	CCSS_DEPRECATED (void		  *_padding_3);
	CCSS_DEPRECATED (void		  *_padding_4);
};

void
ccss_node_init (ccss_node_t		*self,
		ccss_node_class_t	*node_class);

CCSS_END_DECLS

#endif /* CCSS_NODE_H */

