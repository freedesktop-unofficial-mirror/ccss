/* The CSS Theme Engine for Gtk+.
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
#include <libcroco/libcroco.h>
#include "gce-serialize.h"

/*
 * Node implementation for querying the stylesheet.
 */

typedef struct {
	ccss_node_t	 parent;
	char const	*type_name;
	char const	*id;
	char const	*pseudo_class;
} Node;

static char const *
get_type (Node const *self)
{
	return self->type_name;
}

static char const *
get_id (Node const *self)
{
	return self->id;
}

static char const *
get_pseudo_class (Node const *self)
{
	return self->pseudo_class;
}

static ccss_node_class_t _node_class = {
	.is_a			= NULL,
	.get_container		= NULL,
	.get_base_style		= NULL,
	.get_id			= (ccss_node_get_id_f) get_id,
	.get_type		= (ccss_node_get_type_f) get_type,
	.get_class		= NULL,
	.get_pseudo_class	= (ccss_node_get_pseudo_class_f) get_pseudo_class,
	.get_attribute		= NULL,
	.get_viewport		= NULL,
	.release		= NULL
};

/*
 * Gtkrc creation infrastructure.
 */

enum { NORMAL = 0, ACTIVE, PRELIGHT, SELECTED, INSENSITIVE, N_STATES };
enum { RED = 0, GREEN, BLUE, N_COLORS };
enum {
	FG_SET		= 1 << 0,
	BG_SET		= 1 << 1,
	BASE_SET	= 1 << 2,
	TEXT_SET	= 1 << 3
};
enum {
	NORMAL_SET	= 1 << 0,
	ACTIVE_SET	= 1 << 1,
	PRELIGHT_SET	= 1 << 2,
	SELECTED_SET	= 1 << 3,
	INSENSITIVE_SET	= 1 << 4
};

struct RcBlock {
	struct RcState {
		double	fg[N_COLORS];
		double	bg[N_COLORS];
		double	base[N_COLORS];
		double	text[N_COLORS];
		guint	flags;
	} colors[N_STATES];
	guint		 flags;
	char const	*type_name;
};

static gboolean
accumulate_state (ccss_stylesheet_t const	*stylesheet,
		  char const			*type_name,
		  char const			*state_name,
		  struct RcState		*state)
{
	ccss_style_t		 style;
	Node			 node;
	gboolean		 ret;

	ccss_node_init ((ccss_node_t *) &node, &_node_class);
	node.type_name = type_name;
	node.id = NULL;
	node.pseudo_class = state_name;
	
	ccss_style_init (&style);
	ret = ccss_stylesheet_query_apply (stylesheet,
					  (ccss_node_t const *) &node, &style);
	if (!ret) {
		return false;
	}

	ret = ccss_style_get_color (&style, &state->fg[RED], &state->fg[GREEN], &state->fg[BLUE]);
	if (ret) {
		state->flags |= FG_SET;
	}

	ret = ccss_style_get_background_color (&style, &state->bg[RED], &state->bg[GREEN], &state->bg[BLUE]);
	if (ret) {
		state->flags |= BG_SET;
		/* FIXME: also setting "base" to the background color, let's see how this works out. */
		state->base[RED] = state->bg[RED];
		state->base[GREEN] = state->bg[GREEN];
		state->base[BLUE] = state->bg[BLUE];
		state->flags |= BASE_SET;
	}

	return (gboolean) state->flags;
}

static gboolean 
accumulate (ccss_stylesheet_t const	*stylesheet,
	    struct RcBlock		*block)
{
	gboolean ret;

	/* Querying for `normal' state without any- and with the `normal' pseudo class. */
	ret = accumulate_state (stylesheet, block->type_name, NULL, &block->colors[NORMAL]);
	if (ret) {
		block->flags |= NORMAL_SET;
	}
	ret = accumulate_state (stylesheet, block->type_name, "normal", &block->colors[NORMAL]);
	if (ret) {
		block->flags |= NORMAL_SET;
	}

	ret = accumulate_state (stylesheet, block->type_name, "active", &block->colors[ACTIVE]);
	if (ret) {
		block->flags |= ACTIVE_SET;
	}

	ret = accumulate_state (stylesheet, block->type_name, "prelight", &block->colors[PRELIGHT]);
	if (ret) {
		block->flags |= PRELIGHT_SET;
	}

	ret = accumulate_state (stylesheet, block->type_name, "selected", &block->colors[SELECTED]);
	if (ret) {
		block->flags |= SELECTED_SET;
	}

	ret = accumulate_state (stylesheet, block->type_name, "insensitive", &block->colors[INSENSITIVE]);
	if (ret) {
		block->flags |= INSENSITIVE_SET;
	}

	return (gboolean) block->flags;
}

static void
serialize_state (struct RcState const	*state,
		 char const		*state_name,
		 GString		*rc_string)
{
	if (FG_SET & state->flags) {
		g_string_append_printf (rc_string, "\tfg[%s] = { %.3f, %.3f, %.3f }\n", state_name, 
					state->fg[RED], state->fg[GREEN], state->fg[BLUE]);
	}

	if (BG_SET & state->flags) {
		g_string_append_printf (rc_string, "\tbg[%s] = { %.3f, %.3f, %.3f }\n", state_name, 
					state->bg[RED], state->bg[GREEN], state->bg[BLUE]);
	}

	if (BASE_SET & state->flags) {
		g_string_append_printf (rc_string, "\tbase[%s] = { %.3f, %.3f, %.3f }\n", state_name, 
					state->base[RED], state->base[GREEN], state->base[BLUE]);
	}

	if (TEXT_SET & state->flags) {
		g_string_append_printf (rc_string, "\ttext[%s] = { %.3f, %.3f, %.3f }\n", state_name, 
					state->text[RED], state->text[GREEN], state->text[BLUE]);
	}
}

static gboolean
serialize (struct RcBlock const	*block,
	   GString		*rc_string)
{
	char	*style;
	char	*style_name;

	if (strlen (block->type_name) > 3 &&
	    0 == strncmp ("Gtk", block->type_name, 3)) {
		style = g_ascii_strdown (block->type_name + 3, -1);		
	} else if (0 == strcmp ("*", block->type_name)) {
		style = g_strdup ("default");
	} else {
		return FALSE;
	}

	style_name = g_strdup_printf ("gce-%s", style);
	g_free (style), style = NULL;

	g_string_append_printf (rc_string, "style \"%s\" {\n", style_name);

	if (NORMAL_SET & block->flags) {
		serialize_state (&block->colors[NORMAL], "NORMAL", rc_string);
	}

	if (ACTIVE_SET & block->flags) {
		serialize_state (&block->colors[ACTIVE], "ACTIVE", rc_string);
	}

	if (PRELIGHT_SET & block->flags) {
		serialize_state (&block->colors[PRELIGHT], "PRELIGHT", rc_string);
	}

	if (SELECTED_SET & block->flags) {
		serialize_state (&block->colors[SELECTED], "SELECTED", rc_string);
	}

	if (INSENSITIVE_SET & block->flags) {
		serialize_state (&block->colors[INSENSITIVE], "INSENSITIVE", rc_string);
	}

	g_string_append (rc_string, "}\n");

	g_string_append_printf (rc_string, "class \"%s\" style \"%s\"\n\n", block->type_name, style_name);
	g_free (style_name), style_name = NULL;

	return TRUE;
}

char *
gce_serialize (ccss_stylesheet_t const *stylesheet)
{
	ccss_stylesheet_iter_t		 iter;
	char const			*type_name;
	ccss_selector_group_t const	*group;
	struct RcBlock			 block;
	GString				*rc_string;
	char				*str;
	gboolean			 ret;

	rc_string = g_string_new ("");

	ccss_stylesheet_iter_init (&iter, stylesheet);

	type_name = NULL;
	group = NULL;
	while (ccss_stylesheet_iter_next (&iter, &type_name, &group)) {

		/* Only feed widget styles back into gtk, not primitives. */
		if (strcmp ("*", type_name) != 0 &&
		    strncmp ("Gtk", type_name, 3) != 0) {
			continue;
		}

		memset (&block, 0, sizeof (block));
		block.type_name = type_name;
		ret = accumulate (stylesheet, &block);
		if (ret) {
			serialize (&block, rc_string);
		}
	}

	str = rc_string->str;
	g_string_free (rc_string, FALSE), rc_string = NULL;
	return str;
}

