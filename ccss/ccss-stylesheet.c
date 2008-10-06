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
#include "ccss-block.h"
#include "ccss-node.h"
#include "ccss-parser.h"
#include "ccss-selector-group-priv.h"
#include "ccss-selector.h"
#include "ccss-stylesheet.h"

/**
 * ccss_stylesheet_t:
 * @blocks:		List owning all blocks parsed from the stylesheet.
 * @type_rules:		Associates type names with all applying selectors.
 * @class_rules:	Associates class names with all applying selectors.
 * @id_rules:		Associates IDs with all applying selectors.
 *
 * Represents a parsed instance of a stylesheet.
 **/
struct ccss_stylesheet_ {
	GSList		*blocks;
	GHashTable	*type_rules;
	GHashTable	*class_rules;
	GHashTable	*id_rules;
};

static ccss_stylesheet_t *
ccss_stylesheet_new (void)
{
	ccss_stylesheet_t *self;

	self = g_new0 (ccss_stylesheet_t, 1);
	self->type_rules = g_hash_table_new (g_str_hash, g_str_equal);
	self->class_rules = g_hash_table_new (g_str_hash, g_str_equal);
	self->id_rules = g_hash_table_new (g_str_hash, g_str_equal);

	return self;
}

static void
fix_dangling_selectors (ccss_stylesheet_t *self)
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
	g_hash_table_iter_init (&iter, self->type_rules);
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
			fixup_group = g_hash_table_lookup (self->type_rules, dangling_key);
			if (fixup_group) {
				ccss_selector_group_merge_base (group, fixup_group);
			}
			item = item->next;
		}
		if (have_dangling) {
			ccss_selector_group_clear_dangling_selectors (group);
		}
	}
}

/**
 * ccss_stylesheet_new_from_buffer:
 * @buffer:	buffer to parse.
 * @size:	size of the buffer.
 *
 * Create a new stylesheet instance based on a CSS string.
 *
 * Returns: a #ccss_stylesheet_t representation of the CSS string.
 **/
ccss_stylesheet_t *
ccss_stylesheet_new_from_buffer (char const	*buffer, 
				size_t		 size)
{
	ccss_stylesheet_t	*self;

	g_critical ("%s() not tested", __FUNCTION__);

	self = ccss_stylesheet_new ();

	self->blocks = ccss_parser_parse_buffer (buffer, size, self->type_rules, 
						self->class_rules, self->id_rules);

	fix_dangling_selectors (self);

	return self;
}

/**
 * ccss_stylesheet_new_from_file:
 * @css_file: file to parse.
 *
 * Create a new stylesheet instance based on a CSS file.
 *
 * Returns: a #ccss_stylesheet_t representation of the CSS file.
 **/
ccss_stylesheet_t *
ccss_stylesheet_new_from_file (char const *css_file)
{
	ccss_stylesheet_t	*self;

	self = ccss_stylesheet_new ();

	self->blocks = ccss_parser_parse_file (css_file, self->type_rules, 
					      self->class_rules, self->id_rules);

	fix_dangling_selectors (self);

	return self;
}

/**
 * ccss_stylesheet_free:
 * @self: a #ccss_stylesheet_t.
 * 
 * Free the stylesheet and all associated resources.
 **/
void
ccss_stylesheet_free (ccss_stylesheet_t *self)
{
	GSList		*iter;
	ccss_block_t	*block;

	g_assert (self);

	iter = self->blocks;
	while (iter) {
		block = (ccss_block_t *) iter->data;
		iter = g_slist_remove (iter, block);
		ccss_block_free (block);
	}

	g_hash_table_destroy (self->type_rules);
	g_hash_table_destroy (self->class_rules);
	g_hash_table_destroy (self->id_rules);
	g_free (self);
}

/**
 * ccss_stylesheet_query_type:
 * @self:	a #ccss_stylesheet_t.
 * @type_name:	the type to query for, e.g. `h1'.
 *
 * Query the stylesheet for styling information regarding a type.
 *
 * Returns: a #ccss_selector_group_t containing the requested information of %NULL.
 **/
ccss_selector_group_t const *
ccss_stylesheet_query_type (ccss_stylesheet_t const	*self,
			   char const			*type_name)
{
	ccss_selector_group_t const *group;

	g_assert (self && type_name && self->type_rules);

	group = (ccss_selector_group_t const *) g_hash_table_lookup (self->type_rules, type_name);

	return group;
}

/*
 * `iter' is used for the recursion.
 */
static bool
collect_type_r (ccss_stylesheet_t const	*self,
	      ccss_node_t const		*node, 
	      ccss_node_t const		*iter,
	      ccss_selector_group_t	*result_group,
	      bool			 as_base)
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

		group = g_hash_table_lookup (self->type_rules, type_name);
		if (group) {
			ret = ccss_selector_group_query_collect (group, node, result_group, as_base);
		}

		/* Try to match base types. */
		base = node_class->get_base_style (iter);
		if (base) {
			ret |= collect_type_r (self, node, base, result_group, true);
			node_class->release (base);
		}
	} else {
		g_warning ("No type name");
	}

	return ret;
}

/**
 * ccss_stylesheet_query_collect:
 * @self:		a #ccss_stylesheet_t.
 * @node:		a #ccss_node_t implementation that is used by libccss to retrieve information about the underlying document.
 * @result_group:	a #ccss_selector_group_t that accumulates the results of the query.
 * @as_base:		whether the results should be accumulates with lowered priority, e.g. when querying for base style information.
 *
 * Query the stylesheet for styling information regarding a document node and collect the results.
 *
 * Returns: %TRUE if styling information has been found.
 **/
bool
ccss_stylesheet_query_collect (ccss_stylesheet_t const	*self,
			      ccss_node_t const		*node, 
			      ccss_selector_group_t	*result_group,
			      bool			 as_base)
{
	ccss_node_class_t const		*node_class;
	ccss_selector_group_t const	*group;
	bool				 ret;

	node_class = node->node_class;

	g_return_val_if_fail (self && node && result_group, false);

	ret = false;

	/* Match wildcard styles. */
	group = g_hash_table_lookup (self->type_rules, "*");
	if (group) {
		ret |= ccss_selector_group_query_collect (group, node, 
							 result_group, false);
	}			

	/* match style by type information */
	ret |= collect_type_r (self, node, node, result_group, as_base);

	/* match by class name 
	ret |= match_class (self, node, style);
	*/

	/* match by id
	ret |= query_id (self, node, style);
	*/

	return ret;
}

/*
 * `iter' is used for the recursion.
 */
static bool
apply_type_r (ccss_stylesheet_t const	*self,
	      ccss_node_t const		*node, 
	      ccss_node_t const		*iter, 
	      ccss_style_t		*style)
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

		group = g_hash_table_lookup (self->type_rules, type_name);
		if (group) {
			ret = ccss_selector_group_query_apply (group, node, style);
		}

		/* Try to match base types. */
		base = node_class->get_base_style (iter);
		if (base) {
			ret |= apply_type_r (self, node, base, style);
			node_class->release (base);
		}

	} else {
		g_warning ("No type name");
	}

	return ret;
}

/*
 * Merge properties from global style classes.
 * Note: not yet supported by libcroco.
 */
static bool
match_class (ccss_stylesheet_t const	*self,
	     ccss_node_t const		*node, 
	     ccss_style_t		*style)
{
	ccss_node_class_t const	*node_class;
	ccss_selector_group_t	*group;
	char const		*class_name;
	bool			 ret;

	node_class = node->node_class;

	ret = false;
	class_name = node_class->get_class (node);
	if (class_name) {
		group = g_hash_table_lookup (self->class_rules, class_name);
		if (group) {
			ret |= ccss_selector_group_query_apply (group, node, style);
		}		
	}

	return ret;
}

/*
 * Merge properties from global style classes.
 * Note: not yet supported by libcroco.
 */
static bool
match_id (ccss_stylesheet_t const	*self,
	  ccss_node_t const		*node, 
	  ccss_style_t			*style)
{
	ccss_node_class_t const	*node_class;
	ccss_selector_group_t	*group;
	char const		*id;
	bool			 ret;

	node_class = node->node_class;

	ret = false;
	id = node_class->get_id (node);
	if (id) {
		group = g_hash_table_lookup (self->id_rules, id);
		if (group) {
			ret |= ccss_selector_group_query_apply (group, node, style);
		}		
	}

	return ret;
}

/**
 * ccss_stylesheet_query_apply:
 * @self:	a #ccss_stylesheet_t.
 * @node:	a #ccss_node_t implementation that is used by libccss to retrieve information about the underlying document.
 * @style:	a #ccss_style_t that the results of the query are applied to.
 *
 * Query the stylesheet for styling information regarding a document node and apply the results to a #ccss_style_t object.
 *
 * Returns: %TRUE if styling information has been found.
 **/
bool
ccss_stylesheet_query_apply (ccss_stylesheet_t const	*self,
			    ccss_node_t const		*node, 
			    ccss_style_t			*style)
{
	ccss_node_class_t const	*node_class;
	bool			 have_type;
	bool			 have_class;
	bool			 have_id;

	node_class = node->node_class;

	g_return_val_if_fail (self && node && style, false);

	/* match style by type information */
	have_type = apply_type_r (self, node, node, style);

	/* match by class name */
	have_class = match_class (self, node, style);

	/* match by id */
	have_id = match_id (self, node, style);

	return have_type || have_class || have_id;
}

/**
 * ccss_stylesheet_iter_init:
 * @self:	a #ccss_stylesheet_iter_t.
 * @stylesheet:	the stylesheet to iterate.
 *
 * Modifying the underlying stylesheet invalidates the iterator.
 **/
void
ccss_stylesheet_iter_init (ccss_stylesheet_iter_t		*self,
			  ccss_stylesheet_t const	*stylesheet)
{
	g_return_if_fail (stylesheet && stylesheet->type_rules);

	g_hash_table_iter_init (self, stylesheet->type_rules);
}

/**
 * ccss_stylesheet_iter_next:
 * @self:	a #ccss_stylesheet_iter_t.
 * @type_name:	type the selector group regards, e.g. DIV in HTML.
 * @group:	the #ccss_selector_group_t containing rules for #type_name.
 *
 * Returns: %FALSE when the last element is reached.
 **/
bool
ccss_stylesheet_iter_next (ccss_stylesheet_iter_t		 *self,
			  char const			**type_name,
			  ccss_selector_group_t const	**group)
{
	return g_hash_table_iter_next (self, (gpointer *) type_name,
				       (gpointer *) group);
}

#ifdef CCSS_DEBUG

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

	g_return_if_fail (self && self->class_rules);

	g_hash_table_iter_init (&iter, self->type_rules);
	while (g_hash_table_iter_next (&iter, (gpointer) &key, (gpointer) &value)) {
		ccss_selector_group_dump (value);
	}

	g_hash_table_iter_init (&iter, self->class_rules);
	while (g_hash_table_iter_next (&iter, (gpointer) &key, (gpointer) &value)) {
		ccss_selector_group_dump (value);
	}

	g_hash_table_iter_init (&iter, self->id_rules);
	while (g_hash_table_iter_next (&iter, (gpointer) &key, (gpointer) &value)) {
		ccss_selector_group_dump (value);
	}
}

#endif /* CCSS_DEBUG */

