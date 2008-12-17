/* The Gtk+ CSS Drawing Library.
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
#include "ccss-gtk-property.h"
#include "ccss-gtk-stylesheet.h"
#include "config.h"

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
		char fg[8];
		char bg[8];
		char base[8];
		char text[8];
		guint	flags;
	} colors[N_STATES];
	guint		 flags;
	char const	*type_name;
	GSList		*style_properties;
};

static void
style_iterator (ccss_style_t const	 *style,
		char const		 *property_name,
		GSList			**style_properties)
{
	ccss_property_base_t const	*property;
	ccss_gtk_property_t const	*self;
	char				*prefix;
	char				*rc_string;

	self = NULL;
	property = NULL;
	ccss_style_get_property (style, property_name, &property);
	if (property && ccss_gtk_property_is_style_property (property)) {
		self = (ccss_gtk_property_t const *) property;
	} else {
		return;
	}

	if (self->class_name) {
		prefix = g_strdup_printf ("%s::", self->class_name);
	} else {
		prefix = g_strdup ("");
	}

	rc_string = NULL;
	if (G_TYPE_BOOLEAN == self->gtype) {
		rc_string = g_strdup_printf ("%s%s = %s", 
					     prefix, self->property_name,
					     self->content.gboolean_val ? 
					     "TRUE" : "FALSE");
	} else if (G_TYPE_STRING == self->gtype) {
		rc_string = g_strdup_printf ("%s%s = \"%s\"", 
					     prefix, self->property_name,
					     self->content.gchararray_val);
	} else if (G_TYPE_FLOAT == self->gtype) {
		rc_string = g_strdup_printf ("%s%s = %f", 
					     prefix, self->property_name,
					     self->content.gfloat_val);
	} else if (G_TYPE_INT == self->gtype) {
		rc_string = g_strdup_printf ("%s%s = %d", 
					     prefix, self->property_name,
					     self->content.gint_val);
	} else if (G_TYPE_UCHAR == self->gtype) {
		rc_string = g_strdup_printf ("%s%s = %d", 
					     prefix, self->property_name,
					     self->content.guchar_val);
	} else if (GDK_TYPE_COLOR == self->gtype) {
		char *val;
		val = gdk_color_to_string (&self->content.gdkcolor_val);
		rc_string = g_strdup_printf ("%s%s = %s", 
					     prefix, self->property_name,
					     val);
		g_free (val);
	} else if (GTK_TYPE_BORDER == self->gtype) {
		rc_string = g_strdup_printf ("%s%s = { %d, %d, %d, %d }", 
					     prefix, self->property_name,
					     self->content.gtkborder_val.left,
					     self->content.gtkborder_val.right,
					     self->content.gtkborder_val.top,
					     self->content.gtkborder_val.bottom);
	} else if (GTK_TYPE_RELIEF_STYLE == self->gtype) {
		char const *val;
		val = self->content.gtkreliefstyle_val == GTK_RELIEF_NORMAL ? "NORMAL" :
		      self->content.gtkreliefstyle_val == GTK_RELIEF_HALF ? "HALF" :
		      self->content.gtkreliefstyle_val == GTK_RELIEF_NONE ? "NONE" : "";
		rc_string = g_strdup_printf ("%s%s = %s", 
					     prefix, self->property_name,
					     val);
	} else if (GTK_TYPE_REQUISITION == self->gtype) {
		rc_string = g_strdup_printf ("%s%s = { %d, %d }", 
					     prefix, self->property_name,
					     self->content.gtkrequisition_val.width,
					     self->content.gtkrequisition_val.height);
	} else if (GTK_TYPE_SHADOW_TYPE == self->gtype) {
		char const *val;
		val = self->content.gtkshadowtype_val == GTK_SHADOW_NONE ? "NONE" :
		      self->content.gtkshadowtype_val == GTK_SHADOW_IN ? "IN" :
		      self->content.gtkshadowtype_val == GTK_SHADOW_OUT ? "OUT" :
		      self->content.gtkshadowtype_val == GTK_SHADOW_ETCHED_IN ? "ETCHED_IN" :
		      self->content.gtkshadowtype_val == GTK_SHADOW_ETCHED_OUT ? "ETCHED_OUT" : "";
		rc_string = g_strdup_printf ("%s%s = %s", 
					     prefix, self->property_name,
					     val);
	} else if (GTK_TYPE_TOOLBAR_SPACE_STYLE == self->gtype) {
		char const *val;
		val = self->content.gtktoolbarspacestyle_val == GTK_TOOLBAR_SPACE_EMPTY ? "EMPTY" :
		      self->content.gtktoolbarspacestyle_val == GTK_TOOLBAR_SPACE_LINE ? "LINE" : "";
		rc_string = g_strdup_printf ("%s%s = %s",
					     prefix, self->property_name,
					     val);
	} else {
		g_assert_not_reached ();
	}

	if (rc_string) {
		*style_properties = g_slist_prepend (*style_properties, rc_string);
	}
}

static gboolean
accumulate_state (ccss_stylesheet_t 	 *stylesheet,
		  char const		 *type_name,
		  char const		 *state_name,
		  struct RcState	 *state,
		  GSList		**style_properties)
{
	ccss_style_t		*style;
	Node			 node;
	char			*color;
	gboolean		 ret;

	ccss_node_init ((ccss_node_t *) &node, &_node_class);
	node.type_name = type_name;
	node.id = NULL;
	node.pseudo_class = state_name;
	
	style = ccss_stylesheet_query (stylesheet, (ccss_node_t const *) &node);
	if (!style) {
		return false;
	}

	color = NULL;

	ret = ccss_style_get_string (style, "color", &color);
	if (ret && color) {
		state->flags |= TEXT_SET;
		strncpy (state->text, color, 8);
		g_free (color), color = NULL;
	}

	ret = ccss_style_get_string (style, "background-color", &color);
	if (ret) {
		state->flags |= BG_SET;
		strncpy (state->bg, color, 8);
		/* FIXME: also setting "base" to the background color, let's see how this works out. */
		state->flags |= BASE_SET;
		strncpy (state->base, color, 8);
		g_free (color), color = NULL;
	}

	ret = ccss_style_get_string (style, "border-color", &color);
	if (ret && color) {
		state->flags |= FG_SET;
		strncpy (state->fg, color, 8);
		g_free (color), color = NULL;
	}

	/* Extract style properties, only for default state. */
	if (style_properties) {
		ccss_style_foreach (style, (ccss_style_iterator_f) style_iterator, 
				    style_properties);
	}

	ccss_style_destroy (style), style = NULL;

	/* Having colors or style properties means there's something to serialise. */
	return (gboolean) state->flags || 
	       (gboolean) (style_properties && *style_properties);
}

static gboolean 
accumulate (ccss_stylesheet_t	*stylesheet,
	    struct RcBlock	*block)
{
	gboolean ret;

	/* Querying for `normal' state without any- and with the `normal' pseudo class. */
	ret = accumulate_state (stylesheet, block->type_name, NULL,
				&block->colors[NORMAL], &block->style_properties);
	if (ret) {
		block->flags |= NORMAL_SET;
	}
	ret = accumulate_state (stylesheet, block->type_name, "normal",
				&block->colors[NORMAL], NULL);
	if (ret) {
		block->flags |= NORMAL_SET;
	}

	ret = accumulate_state (stylesheet, block->type_name, "active",
				&block->colors[ACTIVE], NULL);
	if (ret) {
		block->flags |= ACTIVE_SET;
	}

	ret = accumulate_state (stylesheet, block->type_name, "prelight",
				&block->colors[PRELIGHT], NULL);
	if (ret) {
		block->flags |= PRELIGHT_SET;
	}

	ret = accumulate_state (stylesheet, block->type_name, "selected",
				&block->colors[SELECTED], NULL);
	if (ret) {
		block->flags |= SELECTED_SET;
	}

	ret = accumulate_state (stylesheet, block->type_name, "insensitive",
				&block->colors[INSENSITIVE], NULL);
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
		g_string_append_printf (rc_string, "\tfg[%s] = '%s'\n", state_name, state->fg);
	}

	if (BG_SET & state->flags) {
		g_string_append_printf (rc_string, "\tbg[%s] = '%s'\n", state_name, state->bg);
	}

	if (BASE_SET & state->flags) {
		g_string_append_printf (rc_string, "\tbase[%s] = '%s'\n", state_name, state->base);
	}

	if (TEXT_SET & state->flags) {
		g_string_append_printf (rc_string, "\ttext[%s] = '%s'\n", state_name, state->text);
	}
}

static gboolean
serialize (struct RcBlock const	*block,
	   GString		*rc_string)
{
	GSList const	*iter;
	char		*style;
	char		*style_name;

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

	/* Colors. */
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

	/* Style properties. */
	iter = block->style_properties;
	while (iter) {
		g_string_append_printf (rc_string, "\t%s\n", (char const *) iter->data);
		iter = iter->next;
	}

	/* Selector, for NULL widget pointer
	 * PONDERING: support complex selectors? */
	g_string_append (rc_string, "\tengine \"css\" {\n");
	g_string_append_printf (rc_string, "\t\tselector = \"%s\"\n", block->type_name);
	g_string_append (rc_string, "\t}\n");

	/* Close block. */
	g_string_append (rc_string, "}\n");

	g_string_append_printf (rc_string, "class \"%s\" style \"%s\"\n\n", block->type_name, style_name);
	g_free (style_name), style_name = NULL;

	return TRUE;
}

static void
iter_func (ccss_stylesheet_t	*stylesheet,
	   char const		*type_name,
	   GString		*rc_string)
{
	struct RcBlock	block;
	gboolean	ret;

	/* Only feed widget styles back into gtk, not primitives. */
	if (strcmp ("*", type_name) != 0 &&
	    strncmp ("Gtk", type_name, 3) != 0) {
		return;
	}

	memset (&block, 0, sizeof (block));
	block.type_name = type_name;
	ret = accumulate (stylesheet, &block);
	if (ret) {
		serialize (&block, rc_string);
	}

	/* Free style properties. */
	while (block.style_properties) {
		g_free (block.style_properties->data);
		block.style_properties = g_slist_delete_link (block.style_properties,
							      block.style_properties);
	}
}

char *
ccss_gtk_stylesheet_to_gtkrc (ccss_stylesheet_t *self)
{
	GString		*rc_string;
	char		*str;

	rc_string = g_string_new ("");

	ccss_stylesheet_foreach (self, 
				 (ccss_stylesheet_iterator_f) iter_func,
				 rc_string);

	str = rc_string->str;
	g_string_free (rc_string, FALSE), rc_string = NULL;
	return str;
}

