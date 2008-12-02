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
#include "ccss-block-priv.h"
#include "ccss-grammar-priv.h"
#include "ccss-node.h"
#include "ccss-selector-group.h"
#include "ccss-selector.h"
#include "ccss-style-priv.h"
#include "ccss-stylesheet-priv.h"
#include "config.h"

ccss_stylesheet_t *
ccss_stylesheet_create (void)
{
	ccss_stylesheet_t *self;

	self = g_new0 (ccss_stylesheet_t, 1);
	self->reference_count = 1;
	self->blocks = g_hash_table_new_full (g_direct_hash, g_direct_equal,
					      NULL, (GDestroyNotify) ccss_block_destroy);
	self->groups = g_hash_table_new (g_str_hash, g_str_equal);

	return self;
}

void
ccss_stylesheet_fix_dangling_selectors (ccss_stylesheet_t *self)
{
	GHashTableIter		 iter;
	char const		*key;
	char const		*dangling_key;
	ccss_selector_group_t 	*group;
	ccss_selector_group_t	*fixup_group;
	GSList const		*item;
	bool			 have_dangling;
	void			*k;
	void			*v;

	/* fix up dangling associations to base styles, walk the tree of type rules */
	g_hash_table_iter_init (&iter, self->groups);
	while (g_hash_table_iter_next (&iter, &k, &v)) {
		
		/* walk extra mile to prevent warnings */
		key = (char const *) k;
		group = (ccss_selector_group_t *) v;

		item = ccss_selector_group_get_dangling_selectors (group);
		have_dangling = (bool) item;
		/* walk the list of dangling selectors in the current group */
		while (item) {
			ccss_selector_t const *selector;
			/* Merge fixup-group to satisfy dangling selectors.
			 * If no fixup_group is found that's ok too, the remaining dangling
			 * selectors will be removed in clear_dangling_selectors(). */
			selector = (ccss_selector_t const *) item->data;
			dangling_key = ccss_selector_get_key (selector);
			fixup_group = g_hash_table_lookup (self->groups, dangling_key);
			if (fixup_group) {
				ccss_selector_group_merge_as_base (group,
								   fixup_group);
			}
			item = item->next;
		}
		if (have_dangling) {
			ccss_selector_group_clear_dangling_selectors (group);
		}
	}
}

/**
 * ccss_stylesheet_add_from_file:
 * @self:	#ccss_stylesheet_t instance or %NULL.
 * @css_file:	file to parse.
 * @precedence:	see #ccss_stylesheet_precedence_t.
 * @user_data:	user-data passed to property- and function-handlers.
 *
 * Load a CSS file with a given precedence.
 *
 * Returns: a #ccss_stylesheet_t representation of the CSS file.
 **/
ccss_stylesheet_t *
ccss_stylesheet_add_from_file (ccss_stylesheet_t		*self,
			       char const			*css_file,
			       ccss_stylesheet_precedence_t	 precedence,
			       void				*user_data)
{
	enum CRStatus ret;

	g_return_val_if_fail (self && css_file, NULL);

	ret = ccss_grammar_parse_file (self->grammar, css_file, precedence,
				       user_data, self->groups, self->blocks);
	if (CR_OK == ret) {
		ccss_stylesheet_fix_dangling_selectors (self);
		return self;
	}

	return NULL;
}

/**
 * ccss_stylesheet_destroy:
 * @self: a #ccss_stylesheet_t.
 * 
 * Decreases the reference count on @self by one. If the result is zero, then
 * @self and all associated resources are freed. See ccss_stylesheet_reference().
 **/
void
ccss_stylesheet_destroy (ccss_stylesheet_t *self)
{
	g_assert (self);

	self->reference_count--;

	if (0 == self->reference_count) {
		ccss_grammar_destroy (self->grammar), self->grammar = NULL;
		g_hash_table_destroy (self->blocks), self->blocks = NULL;
		g_hash_table_destroy (self->groups), self->groups = NULL;
		g_free (self);
	}
}

/**
 * ccss_stylesheet_reference:
 * @self: a #ccss_stylesheet_t.
 *
 * Increases the reference count on @self by one. This prevents @self from being
 * destroyed until a matching call to ccss_stylesheet_destroy() is made.
 *
 * The number of references to a #ccss_stylesheet_t can be acquired using
 * ccss_stylesheet_get_reference_count().
 *
 * Returns: the referenced #ccss_stylesheet_t.
 **/
ccss_stylesheet_t *
ccss_stylesheet_reference (ccss_stylesheet_t *self)
{
	g_return_val_if_fail (self, NULL);

	self->reference_count++;

	return self;
}

/**
 * ccss_stylesheet_get_reference_count:
 * @self: a #ccss_stylesheet_t.
 *
 * Returns: the current reference count of @self.
 *	    If @self is a nil object, 0 will be returned.
 **/
unsigned int
ccss_stylesheet_get_reference_count (ccss_stylesheet_t const *self)
{
	g_return_val_if_fail (self, 0);

	return self->reference_count;
}

/**
 * ccss_stylesheet_query_type:
 * @self:	a #ccss_stylesheet_t.
 * @type_name:	the type to query for, e.g. `h1'.
 * @style:	a #ccss_style_t that the results of the query are applied to.
 *
 * Query the stylesheet for styling information regarding a type.
 *
 * Returns: a #ccss_selector_group_t containing the requested information of %NULL.
 **/
bool
ccss_stylesheet_query_type (ccss_stylesheet_t	*self,
			    char const		*type_name,
			    ccss_style_t	*style)
{
	ccss_selector_group_t const *group;

	g_assert (self && type_name && self->groups);

	if (style->stylesheet && 
	    style->stylesheet != self) {
		g_warning ("style %p already associated to stylesheet %p", 
			   (void *) style, (void *) self);
		return false;
	} else if (NULL == style->stylesheet) {
		style->stylesheet = ccss_stylesheet_reference (self);
	}

	group = (ccss_selector_group_t const *) g_hash_table_lookup (self->groups, type_name);
	if (!group)
		return false;

	return ccss_selector_group_apply_type (group, type_name, style);
}

/*
 * `iter' is used for the recursion.
 */
static bool
query_type_r (ccss_stylesheet_t const	*self,
	      ccss_node_t const		*node, 
	      ccss_node_t const		*iter,
	      bool			 as_base,
	      ccss_selector_group_t	*result_group)
{
	ccss_node_class_t const	*node_class;
	ccss_selector_group_t	*group;
	char const		*type_name;
	bool			 ret;

	node_class = node->node_class;

	if (!iter) {
		iter = node;
	}

	type_name = node_class->get_type (iter);

	ret = false;
	if (type_name) {

		ccss_node_t *base;

		group = g_hash_table_lookup (self->groups, type_name);
		if (group) {
			ret = ccss_selector_group_query (group, node, as_base, result_group);
		}

		/* Try to match base types. */
		base = node_class->get_base_style (iter);
		if (base) {
			ret |= query_type_r (self, node, base, true, result_group);
			node_class->release (base);
		}
	} else {
		g_warning ("No type name");
	}

	return ret;
}

/*
 * Do not recurse containers.
 */
static bool
query_node (ccss_stylesheet_t const	*self,
	    ccss_node_t const		*node, 
	    ccss_style_t		*style)
{
	ccss_node_class_t const		*node_class;
	ccss_selector_group_t const	*universal_group;
	ccss_selector_group_t		*result_group;
	char const			*inline_css;
	enum CRStatus			 status;
	bool				 ret;

	g_return_val_if_fail (self && node && style, false);

	result_group = ccss_selector_group_create ();
	ret = false;

	/* Match wildcard styles. */
	universal_group = g_hash_table_lookup (self->groups, "*");
	if (universal_group) {
		ret |= ccss_selector_group_query (universal_group, node,
						  false, result_group);
	}			

	/* Match style by type information. */
	ret |= query_type_r (self, node, node, false, result_group);

	/* Handle inline styling. */
	node_class = node->node_class;
	inline_css = node_class->get_style (node);
	if (inline_css) {
		ptrdiff_t instance;
		instance = node_class->get_instance (node);
		if (instance == 0) {
			g_warning ("Inline CSS `%s' but instance == 0\n", inline_css);
		} else {
			/* FIXME: user_data inline styling. Maybe require
			 * having the node's style registered explicitely? */
			status = ccss_grammar_parse_inline (self->grammar,
							    inline_css, 
							    CCSS_STYLESHEET_AUTHOR,
							    instance, NULL,
							    result_group,
							    self->blocks);
			ret &= (status == CR_OK);
		}
	}

	/* Apply collected style. */
	ret &= ccss_selector_group_apply (result_group, node, style);

	ccss_selector_group_destroy (result_group), result_group = NULL;

	return ret;
}

static void
inherit_container_style (ccss_style_t const	*container_style,
			 GHashTable		*inherit,
			 ccss_style_t		*style)
{
	GHashTableIter			 iter;
	GQuark				 property_id;
	ccss_property_base_t const	*property;
	GSList				*removals;

	/* Check which properties from the `inherit' set can be resolved. */
	removals = NULL;
	g_hash_table_iter_init (&iter, inherit);
	while (g_hash_table_iter_next (&iter, (gpointer *) &property_id, NULL)) {

		/* Look up property in the container's style. */
		property = (ccss_property_base_t const *)
				g_hash_table_lookup (container_style->properties,
						     (gpointer) property_id);
		if (property &&
		    (CCSS_PROPERTY_STATE_NONE == property->state ||
		     CCSS_PROPERTY_STATE_SET == property->state)) {

			bool is_resolved;

			if (property->property_class->property_inherit) {
				is_resolved = property->property_class->property_inherit (
						container_style, 
						style);
			} else {
				g_hash_table_insert (style->properties,
						     (gpointer) property_id,
						     (gpointer) property);
				is_resolved = true;
			}

			/* Remember inherited properties, we can't
			 * modify the hash while iterating. */
			if (is_resolved) {
				removals = g_slist_prepend (removals,
						    (gpointer) property_id);
			}
		}
	}

	while (removals) {
		property_id = (GQuark) removals->data;
		removals = g_slist_remove (removals, (gpointer) property_id);
		g_hash_table_remove (inherit, (gpointer) property_id);
	}
}

/**
 * query_container_r:
 * @self:	a #ccss_style_t.
 * @node:	the document's node instance this style is associated to.
 * @inherit:	a #GHashTable of properties to inherit.
 * @style:	a #ccss_style_t that the results of the query are applied to.
 *
 * Inherit style properties from the container if specified in the CSS.
 *
 * Returns: %TRUE if looking up inherited styles from the container was 
 * successful or if no inheritance is required.
 **/
static bool
query_container_r (ccss_stylesheet_t const	*self,
		   ccss_node_t const		*node,
		   GHashTable			*inherit,
		   ccss_style_t			*style)
{
	ccss_node_class_t const	*node_class;
	ccss_node_t		*container;
	ccss_style_t		*container_style;
	bool			 ret;

	g_return_val_if_fail (style && node, false);

	/* Actually inherit. */
	node_class = node->node_class;
	container = node_class->get_container (node);
	if (!container)
		return false;

	/* Have styling? */
	container_style = ccss_style_create ();
	ret = query_node (self, container, container_style);
	if (ret) {
		inherit_container_style (container_style, inherit, style);
	}
	ccss_style_destroy (container_style), container_style = NULL;

	if (0 == g_hash_table_size (inherit)) {
		/* Nothing more to inherit, good! */
		ret = true;
	} else {
		/* Recurse. */
		query_container_r (self, container, inherit, style);
	}

	node_class->release (container), container = NULL;

	/* Return true if some styling has been found, not necessarily all
	 * properties resolved. */
	return ret;
}

/**
 * ccss_stylesheet_query:
 * @self:	a #ccss_stylesheet_t.
 * @node:	a #ccss_node_t implementation that is used by libccss to retrieve information about the underlying document.
 * @style:	a #ccss_style_t that the results of the query are applied to.
 *
 * Query the stylesheet for styling information regarding a document node and apply the results to a #ccss_style_t object.
 *
 * Returns: %TRUE if styling information has been found.
 **/
bool
ccss_stylesheet_query (ccss_stylesheet_t 	*self,
		       ccss_node_t const	*node, 
		       ccss_style_t		*style)
{
	GHashTable			*inherit;
	GHashTableIter			 iter;
	GQuark				 property_id;
	ccss_property_base_t const	*property;
	bool				 ret;

	g_assert (self && node && style);

	if (style->stylesheet && 
	    style->stylesheet != self) {
		g_warning ("style %p already associated to stylesheet %p", 
			   (void *) style, (void *) self);
		return false;
	} else if (NULL == style->stylesheet) {
		style->stylesheet = ccss_stylesheet_reference (self);
	}

	/* Apply this node's styling. */
	ret = query_node (self, node, style);

	/* Handle inherited styling. */
	inherit = g_hash_table_new ((GHashFunc) g_direct_hash, 
				    (GEqualFunc) g_direct_equal);

	g_hash_table_iter_init (&iter, style->properties);
	while (g_hash_table_iter_next (&iter, (gpointer *) &property_id, (gpointer *) &property)) {

		if (CCSS_PROPERTY_STATE_INHERIT == property->state) {

			g_hash_table_insert (inherit,
					     (gpointer) property_id,
					     (gpointer) property_id);
		}
	}

	if (0 == g_hash_table_size (inherit)) {
		/* Nothing to inherit, good! */
		ret &= true;
	} else {
		ret &= query_container_r (self, node, inherit, style);
	}

	/* Prune unresolved inherited properties from the style. */
	g_hash_table_iter_init (&iter, inherit);
	while (g_hash_table_iter_next (&iter, (gpointer *) &property_id, (gpointer *) &property)) {

		g_hash_table_remove (style->properties, (gpointer) property_id);
	}
	g_hash_table_destroy (inherit), inherit = NULL;

	return ret;
}

/**
 * ccss_stylesheet_invalidate_node:
 * @self:	a #ccss_stylesheet_t.
 * @instance:	an instance identifyer, as returned by #ccss_node_get_instance_f.
 * 
 * Frees parsed inline CSS asocciated to a document node.
 **/
void
ccss_stylesheet_invalidate_node (ccss_stylesheet_t const	*self,
				 ptrdiff_t			 instance)
{
	g_assert (self && self->blocks);

	g_hash_table_remove (self->blocks, (gconstpointer) instance);
}

/**
 * ccss_stylesheet_foreach:
 * @self:	a #ccss_stylesheet_t.
 * @func:	a #ccss_stylesheet_iterator_f.
 * @user_data:	user data to pass to the iterator function.
 *
 * The iterator function @func is called for each type in the stylesheet.
 **/
void
ccss_stylesheet_foreach (ccss_stylesheet_t const	*self,
			 ccss_stylesheet_iterator_f	 func,
			 void				*user_data)
{
	GHashTableIter 	iter;
	gpointer	type_name;
	gpointer	value;

	g_return_if_fail (self && func);

	g_hash_table_iter_init (&iter, self->groups);
	while (g_hash_table_iter_next (&iter, &type_name, &value)) {

		func (self, (char const *) type_name, user_data);
	}
}

/**
 * ccss_stylesheet_dump:
 * @self:	a #ccss_stylesheet_t.
 *
 * Print informations about the internal state of this object.
 **/
void
ccss_stylesheet_dump (ccss_stylesheet_t const *self)
{
	GHashTableIter		 iter;
	char const		*key;
	ccss_selector_group_t	*value;

	g_hash_table_iter_init (&iter, self->groups);
	while (g_hash_table_iter_next (&iter, (gpointer) &key, (gpointer) &value)) {
		ccss_selector_group_dump (value);
	}
}

