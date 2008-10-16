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

#include <stdio.h>
#include <string.h>
#include <libcroco/libcroco.h>
#include "config.h"
#include "ccss-background-parser.h"
#include "ccss-block.h"
#include "ccss-border-parser.h"
#include "ccss-color.h"
#include "ccss-parser.h"
#include "ccss-property.h"
#include "ccss-selector.h"
#include "ccss-selector-group-priv.h"

typedef struct {
	GSList		*blocks;
	GHashTable	*groups;
	ccss_block_t	*block;
} ccss_parser_info_t;

#define HANDLER_GET_INFO(handler_) ((ccss_parser_info_t *) handler->app_data)

static ccss_attribute_selector_match_t
map_attribute_selector_match (enum AttrMatchWay cr_match)
{
	switch (cr_match) {
	case SET:
		return CCSS_ATTRIBUTE_SELECTOR_MATCH_EXISTS;
	case EQUALS:
		return CCSS_ATTRIBUTE_SELECTOR_MATCH_EQUALS;
	case INCLUDES:
	case DASHMATCH:
	case NO_MATCH:
	default:
		g_assert_not_reached ();
	}

	/* fall back to (my interpretation of) default behaviour */
	return CCSS_ATTRIBUTE_SELECTOR_MATCH_EXISTS;
}

static ccss_selector_t *
walk_additional_selector (CRAdditionalSel *cr_add_sel)
{
	ccss_selector_t			*selector;
	char const			*name;
	char const			*value;
	ccss_attribute_selector_match_t	 match;

	g_return_val_if_fail (cr_add_sel, NULL);

	name = NULL;
	value = NULL;
	selector = NULL;
	switch (cr_add_sel->type) {
	case CLASS_ADD_SELECTOR:
		name = cr_string_peek_raw_str (cr_add_sel->content.class_name);
		selector = ccss_class_selector_new (name);
		break;
	case PSEUDO_CLASS_ADD_SELECTOR:
		name = cr_string_peek_raw_str (cr_add_sel->content.pseudo->name);
		selector = ccss_pseudo_class_selector_new (name);
		break;
	case ID_ADD_SELECTOR:
		name = cr_string_peek_raw_str (cr_add_sel->content.id_name);
		selector = ccss_id_selector_new (name);
		break;
	case ATTRIBUTE_ADD_SELECTOR:
		name = cr_string_peek_raw_str (cr_add_sel->content.attr_sel->name);
		value = cr_string_peek_raw_str (cr_add_sel->content.attr_sel->value);
		match = map_attribute_selector_match (cr_add_sel->content.attr_sel->match_way);
		selector = ccss_attribute_selector_new (name, value, match);
		break;
	case NO_ADD_SELECTOR:
	default:
		g_assert_not_reached ();
		return NULL;
	}

	if (cr_add_sel->next) {
		ccss_selector_t *refinement;
		refinement = walk_additional_selector (cr_add_sel->next);
		ccss_selector_refine (selector, refinement);
	}

	return selector;
}

static ccss_selector_t *
walk_simple_selector_r (CRSimpleSel *cr_simple_sel)
{
	ccss_selector_t *selector;

	g_return_val_if_fail (cr_simple_sel, NULL);

	selector = NULL;
	if (UNIVERSAL_SELECTOR & cr_simple_sel->type_mask) {
		selector = ccss_universal_selector_new ();
	} else if (TYPE_SELECTOR & cr_simple_sel->type_mask) {
		selector = ccss_type_selector_new (cr_string_peek_raw_str (cr_simple_sel->name));
	} else {
		char const *sel;
		sel = cr_simple_sel->name ? cr_string_peek_raw_str (cr_simple_sel->name) : NULL;
		g_warning ("Unknown selector '%s'\n", sel);
		return NULL;
	}

	g_return_val_if_fail (selector, NULL);

	if (cr_simple_sel->add_sel) {
		ccss_selector_t *refinement;
		refinement = walk_additional_selector (cr_simple_sel->add_sel);
		ccss_selector_refine (selector, refinement);
	}

	if (cr_simple_sel->next) {
		ccss_selector_t *descendant;
		descendant = walk_simple_selector_r (cr_simple_sel->next);
		if (COMB_WS == cr_simple_sel->next->combinator) {
			selector = ccss_selector_append_descendant (selector,
								   descendant);
		} else if (COMB_GT == cr_simple_sel->next->combinator) {
			selector = ccss_selector_append_child (selector,
							      descendant);
		} else {
			g_assert_not_reached ();
		}
	}

	return selector;
}

static void
start_selector_cb (CRDocHandler	*handler,
		   CRSelector	*cr_sel)
{
	ccss_parser_info_t *info;

	info = HANDLER_GET_INFO (handler);

	if (info && info->block) {
		char *location;

		location = cr_parsing_location_to_string (&cr_sel->location, DUMP_LINE);
		g_warning ("Looks like there may be a problem in the block before %s, possibly a missing semicolon.", location);
		g_free (location), location = NULL;

		/* Discard what's been accumulated so far. */
		ccss_block_free (info->block);
		info->block = NULL;
	}

	info->block = ccss_block_new ();
}

static void
property_cb (CRDocHandler	*handler,
	     CRString		*name,
	     CRTerm		*values,
	     gboolean	 	 is_important)
{
	ccss_parser_info_t	*info;
	char const 		*property;

	info = HANDLER_GET_INFO (handler);

	g_assert (info && info->block);

	property = cr_string_peek_raw_str (name);
	if (g_str_has_prefix (property, "background")) {

		ccss_block_parse_background (info->block, property, values);

	} else if (g_str_has_prefix (property, "border")) {

		 ccss_block_parse_border (info->block, property, values);

	} else if (0 == g_strcmp0 ("color", property)) {

		ccss_color_t *color, c;
		bool ret;

		ret = ccss_color_parse (&c, (CRTerm const **) &values);
		if (ret) {
			color = ccss_block_new_color (info->block);
			*color = c;
		}
	} else {
		/* Generic property. */
		ccss_property_t	*prop, p;
		char const	*s;

		memset (&p, 0, sizeof (p));
		switch (values->type) {
		case TERM_NUMBER:
			p.spec = CCSS_PROPERTY_SPEC_SET;
			p.type = CCSS_PROPERTY_TYPE_DOUBLE;
			p.content.dval = values->content.num->val;
			break;
		case TERM_IDENT:
		case TERM_STRING:
			s = cr_string_peek_raw_str (values->content.str);
			if (0 == g_strcmp0 ("none", s)) {
				p.spec = CCSS_PROPERTY_SPEC_NONE;
			} else if (0 == g_strcmp0 ("inherit", s)) {
				/* FIXME */
				g_warning ("CSS `inherit' not supported yet");
				return;
			} else {
				p.spec = CCSS_PROPERTY_SPEC_SET;
				p.type = CCSS_PROPERTY_TYPE_STRING;
				p.content.sval = g_strdup (s);
			}
			break;
		default:
			g_warning ("Unknown property type %d", values->type);
			return;
		}

		/*  Coming here means success. */
		prop = ccss_block_new_property (info->block, property);
		*prop = p;
	}
}

static void
end_selector_cb (CRDocHandler	*handler,
		 CRSelector	*cr_sel)
{
	ccss_parser_info_t	*info;
	ccss_selector_t		*selector;
	ccss_selector_group_t	*group;
	CRSelector		*iter;
	char const		*key;

	g_assert (HANDLER_GET_INFO (handler));

	info = HANDLER_GET_INFO (handler);

	iter = cr_sel;
	do {
		selector = walk_simple_selector_r (iter->simple_sel);
		if (selector) {
			ccss_selector_set_block (selector, info->block);

			g_assert (ccss_selector_is_type (selector));

			key = ccss_selector_get_key (selector);
			g_assert (key);

			group = (ccss_selector_group_t *) g_hash_table_lookup (info->groups, key);
			if (!group) {
				group = ccss_selector_group_new ();
				g_hash_table_insert (info->groups, (char *) key, group);
			}
			ccss_selector_group_add_selector (group, selector);
		}
	} while (NULL != (iter = iter->next));

	info->block = NULL;
}

GSList *
ccss_parser_parse_file (char const	*css_file,
		       GHashTable	*groups)
{
	CRParser		*parser;
	CRDocHandler		*handler;
	ccss_parser_info_t	 info;
	enum CRStatus		 ret;

	g_assert (css_file && groups);

	parser = cr_parser_new_from_file ((guchar *) css_file, CR_UTF_8);

	handler = cr_doc_handler_new ();
	handler->app_data = (gpointer) &info;
	info.blocks = NULL;
	info.groups = groups;
	info.block = NULL;

	handler->start_selector = start_selector_cb;
        handler->property = property_cb;
	handler->end_selector = end_selector_cb;

	cr_parser_set_sac_handler (parser, handler);

	ret = cr_parser_parse (parser);
	if (ret != CR_OK) {
		/* From the department of awesome error handling: */
		g_warning ("Parsing failed but all I can offer for consolation is this: %d\n", ret);
	}

/* See http://bugzilla.gnome.org/show_bug.cgi?id=553937 . */
#ifndef CCSS_ENABLE_LIBCROCO_WORKAROUND
	cr_doc_handler_unref (handler), handler = NULL;
#endif
	cr_parser_destroy (parser), parser = NULL;

	return info.blocks;
}

GSList *
ccss_parser_parse_buffer (char const	*buffer,
			 size_t		 size,
			 GHashTable	*groups)
{
	CRParser		*parser;
	CRDocHandler		*handler;
	ccss_parser_info_t	 info;
	enum CRStatus		 ret;

	g_assert (buffer && size && groups);

	parser = cr_parser_new_from_buf ((guchar *) buffer, (gulong) size, 
					 CR_UTF_8, false);

	handler = cr_doc_handler_new ();
	handler->app_data = (gpointer) &info;
	info.blocks = NULL;
	info.groups = groups;
	info.block = NULL;

	handler->start_selector = start_selector_cb;
        handler->property = property_cb;
	handler->end_selector = end_selector_cb;

	cr_parser_set_sac_handler (parser, handler);
	ret = cr_parser_parse (parser);

/* See http://bugzilla.gnome.org/show_bug.cgi?id=553937 . */
#ifndef CCSS_ENABLE_LIBCROCO_WORKAROUND
	cr_doc_handler_unref (handler), handler = NULL;
#endif
	cr_parser_destroy (parser);

	return info.blocks;
}

