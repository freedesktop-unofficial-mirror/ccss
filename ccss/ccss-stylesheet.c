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
#include "ccss-block.h"
#include "ccss-node.h"
#include "ccss-parser.h"
#include "ccss-selector-group.h"
#include "ccss-selector.h"
#include "ccss-stylesheet.h"

/**
 * ccss_stylesheet_t:
 * @blocks:		List owning all blocks parsed from the stylesheet.
 * @groups:		Associates type names with all applying selectors.
 *
 * Represents a parsed instance of a stylesheet.
 **/
struct ccss_stylesheet_ {
	GSList		*blocks;
	GHashTable	*groups;
};

static ccss_stylesheet_t *
ccss_stylesheet_new (void)
{
	ccss_stylesheet_t *self;

	self = g_new0 (ccss_stylesheet_t, 1);
	self->groups = g_hash_table_new (g_str_hash, g_str_equal);

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
	enum CRStatus		 ret;

	self = ccss_stylesheet_new ();

	ret = ccss_parser_parse_buffer (buffer, size, 
					CCSS_STYLESHEET_AUTHOR,
					self->groups, &self->blocks);

	fix_dangling_selectors (self);

	// FIXME handle `ret'

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
	enum CRStatus		 ret;

	self = ccss_stylesheet_new ();

	ret = ccss_parser_parse_file (css_file, CCSS_STYLESHEET_AUTHOR,
				      self->groups, &self->blocks);

	fix_dangling_selectors (self);

	// FIXME handle `ret'

	return self;
}

/**
 * ccss_stylesheet_load_from_file:
 * @self:	#ccss_stylesheet_t instance or %NULL.
 * @css_file:	file to parse.
 * @precedence:	see #ccss_stylesheet_precedence_t.
 *
 * Load a CSS file with a given precedence.
 *
 * Returns: a #ccss_stylesheet_t representation of the CSS file.
 **/
ccss_stylesheet_t *
ccss_stylesheet_load_from_file (ccss_stylesheet_t		*self,
				char const			*css_file,
				ccss_stylesheet_precedence_t	 precedence)
{
	enum CRStatus ret;

	if (!self) {
		self = ccss_stylesheet_new ();
	}

	ret = ccss_parser_parse_file (css_file, precedence,
				      self->groups, &self->blocks);

	fix_dangling_selectors (self);

	// FIXME handle `ret'

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

	g_hash_table_destroy (self->groups);
	g_free (self);
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
ccss_stylesheet_query_type (ccss_stylesheet_t const	*self,
			    char const			*type_name,
			    ccss_style_t		*style)
{
	ccss_selector_group_t const *group;

	g_assert (self && type_name && self->groups);

	group = (ccss_selector_group_t const *) g_hash_table_lookup (self->groups, type_name);
	if (!group)
		return false;

	return ccss_selector_group_apply_type (group, type_name, style);
}

/*
 * `iter' is used for the recursion.
 */
static bool
collect_type_r (ccss_stylesheet_t const	*self,
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
			ret |= collect_type_r (self, node, base, true, result_group);
			node_class->release (base);
		}
	} else {
		g_warning ("No type name");
	}

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
ccss_stylesheet_query (ccss_stylesheet_t const	*self,
		       ccss_node_t const	*node, 
		       ccss_style_t		*style)
{
	ccss_node_class_t const		*node_class;
	ccss_selector_group_t const	*universal_group;
	ccss_selector_group_t		*result_group;
	char const			*inline_css;
	GSList				*block_list;
	enum CRStatus			 status;
	bool				 ret;

	g_return_val_if_fail (self && node && style, false);

	result_group = ccss_selector_group_new ();
	ret = false;

	/* Match wildcard styles. */
	universal_group = g_hash_table_lookup (self->groups, "*");
	if (universal_group) {
		ret |= ccss_selector_group_query (universal_group, node,
						  false, result_group);
	}			

	/* Match style by type information. */
	ret |= collect_type_r (self, node, node, false, result_group);

	/* Handle inline styling. */
	node_class = node->node_class;
	inline_css = node_class->get_style (node);
	block_list = NULL;
	if (inline_css) {
		ptrdiff_t instance;
		instance = node_class->get_instance (node);
		if (instance == 0) {
			g_warning ("Inline CSS `%s' but instance == 0\n", inline_css);
		} else {
			status = ccss_parser_parse_inline (inline_css, 
							   CCSS_STYLESHEET_AUTHOR,
							   instance, result_group,
							   &block_list);
			ret &= (status == CR_OK);
		}
	}

	/* Apply collected style. */
	ret &= ccss_selector_group_apply (result_group, node, style);

	ccss_selector_group_free (result_group), result_group = NULL;

	return ret;
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

	g_hash_table_iter_init (&iter, self->groups);
	while (g_hash_table_iter_next (&iter, (gpointer) &key, (gpointer) &value)) {
		ccss_selector_group_dump (value);
	}
}

#endif /* CCSS_DEBUG */

