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
#include "gce-node.h"

/*
 * TODO: split into load- and run-time variants?
 */
struct GceNode_ {
	ccss_node_t		 parent;
	GtkWidget		*widget;
	/* custom attributes { */
	char const		*class_name;
	char const		*pseudo_class;
	char const		*gap;
	char const		*shadow;
	char const		*orientation;
	char const		*edge;
	char const		*expander_style;
	enum {
		UNSET,
		CONTAINER,
		PRIMITIVE,
		TYPE 
	} flavor;
	union {
		char const	*primitive;
		GType		 gtype;
	} impl;
};

static struct {
	GceNode nodes[GCE_NODE_CACHE_SIZE];
	int	cursor;
} _node_cache = {
	.cursor = -1
};

static ccss_node_class_t * peek_node_class (void);

GceNode *
gce_node_cache_get_top_node (void)
{
	return &_node_cache.nodes[_node_cache.cursor];
}

static GceNode *
fetch_node (void)
{
	GceNode *node;

	g_assert (_node_cache.cursor < (GCE_NODE_CACHE_SIZE - 1));

	_node_cache.cursor++;
	node = &_node_cache.nodes[_node_cache.cursor];
	memset (node, 0, sizeof (*node));

	ccss_node_init ((ccss_node_t *) node, peek_node_class ());

	return node;
}

GceNode *
gce_node_cache_fetch_node (GtkWidget	*widget, 
			   char const	*class_name,
			   char const	*pseudo_class,
			   char const	*shadow,
			   char const	*orientation,
			   char const	*gap,
			   char const	*edge,
			   char const	*expander_style)
{
	GceNode *node;

	node = fetch_node ();

	node->widget = widget;
	node->class_name = class_name;
	node->pseudo_class = pseudo_class;
	node->gap = gap;
	node->shadow = shadow;
	node->orientation = orientation;
	node->edge = edge;
	node->expander_style = expander_style;
	node->flavor = UNSET;	

	return node;
}

GceNode *
gce_node_cache_fetch_primitive_node (GtkWidget	*widget, 
				     char const	*class_name,
				     char const	*pseudo_class,
				     char const	*shadow,
				     char const	*orientation,
				     char const	*gap,
				     char const	*edge,
				     char const	*expander_style,
				     char const	*primitive)
{
	GceNode *node;

	node = gce_node_cache_fetch_node (widget, class_name, pseudo_class,
					  shadow, orientation, gap, edge, 
					  expander_style);
	node->flavor = PRIMITIVE;	
	node->impl.primitive = primitive;

	return node;
}

void
gce_node_cache_release_node (GceNode *node)
{
	g_assert (_node_cache.cursor >= 0 && 
		  node == &_node_cache.nodes[_node_cache.cursor]);

	_node_cache.cursor--;
}

static gboolean
is_a (GceNode const	*node,
      char const	*type_name)
{
	GType	gtype;
	bool	is_matching;

	switch (node->flavor) {
	case PRIMITIVE:
		if (0 == strcmp (type_name, node->impl.primitive)) {
			return TRUE;
		}
		/* fall thru */
	case UNSET:
	case CONTAINER:
	case TYPE:
		gtype = g_type_from_name (type_name);
		is_matching = g_type_is_a (G_OBJECT_TYPE (node->widget), gtype);
		return is_matching;
	default:
		g_warning ("Unknown flavor %d\n", node->flavor);
		g_assert_not_reached ();
		return FALSE;
	}
}

static GceNode *
get_container (GceNode const *node)
{
	GceNode		*container_node;
	GtkWidget	*container;

	g_assert (node);

	container_node = NULL;
	switch (node->flavor) {
	case UNSET:
	case CONTAINER:
		container = gtk_widget_get_parent (node->widget);
		if (container) {
			container_node = fetch_node ();
			container_node->widget = (GtkWidget *) g_object_ref (G_OBJECT (container));
			container_node->flavor = CONTAINER;
			container_node->impl.gtype = G_OBJECT_TYPE (G_OBJECT (container));
		}
		break;
	case PRIMITIVE:
		container_node = fetch_node ();
		container_node->widget = (GtkWidget *) g_object_ref (G_OBJECT (node->widget));
		container_node->flavor = UNSET;
		break;
	case TYPE:
		g_assert_not_reached ();
		/* fall thru */
	default:
		container_node = NULL;
	}

	return container_node;
}

static GceNode *
get_base_style (GceNode const *node)
{
	GceNode		*base_node;

	g_assert (node);

	switch (node->flavor) {
	case UNSET:
		if (GTK_TYPE_WIDGET == G_OBJECT_TYPE (G_OBJECT (node->widget))) {
			/* already at top of inheritance hierarchy */
			base_node = NULL;
		} else {
			/* inherit from superclass widget */
			base_node = fetch_node ();
			base_node->widget = (GtkWidget *) g_object_ref (G_OBJECT (node->widget));
			base_node->flavor = TYPE;
			base_node->impl.gtype = g_type_parent (G_OBJECT_TYPE (G_OBJECT (node->widget)));

		}
		break;
	case CONTAINER:
		if (GTK_TYPE_WIDGET == node->impl.gtype) {
			/* already at top of inheritance hierarchy */
			base_node = NULL;
		} else {
			/* inherit from superclass widget */
			base_node = fetch_node ();
			base_node->widget = (GtkWidget *) g_object_ref (G_OBJECT (node->widget));
			base_node->flavor = CONTAINER;
			base_node->impl.gtype = g_type_parent (node->impl.gtype);
		} 
		break;
	case PRIMITIVE:
		base_node = NULL;
		break;
	case TYPE:
		if (GTK_TYPE_WIDGET == node->impl.gtype) {
			/* already at top of inheritance hierarchy */
			base_node = NULL;
		} else {
			/* inherit from superclass widget */
			base_node = fetch_node ();
			base_node->widget = (GtkWidget *) g_object_ref (G_OBJECT (node->widget));
			base_node->flavor = TYPE, 
			base_node->impl.gtype = g_type_parent (node->impl.gtype);
		} 
		break;
	default:
		g_assert_not_reached ();
		base_node = NULL;
	}

	return base_node;
}

static char const *
get_id (GceNode const *node)
{
	g_return_val_if_fail (node, NULL);

	switch (node->flavor) {
	case UNSET:
	case CONTAINER:
	case PRIMITIVE:
		return gtk_widget_get_name (node->widget);
	case TYPE:
	default:
		g_assert_not_reached ();
		return NULL;
	}
}

static char const *
get_type (GceNode const *node)
{
	g_return_val_if_fail (node, NULL);

	switch (node->flavor) {
	case UNSET:
		return G_OBJECT_TYPE_NAME (G_OBJECT (node->widget));
	case CONTAINER:
	case TYPE:
		return g_type_name (node->impl.gtype);
	case PRIMITIVE:
		g_assert (node->impl.primitive);
		return node->impl.primitive;
	default:
		g_assert_not_reached ();
		return NULL;
	}
}

static char const *
get_class (GceNode const *node)
{
	g_return_val_if_fail (node, NULL);

	return node->class_name;
}

static char const *
get_pseudo_class (GceNode const *node)
{
	g_return_val_if_fail (node, NULL);

	return node->pseudo_class;
}

static char *
_to_string (GValue const *value)
{
	switch (G_VALUE_TYPE (value)) {
	case G_TYPE_BOOLEAN:
		return g_value_get_boolean (value) ?
				g_strdup ("true") : 
				g_strdup ("false");
	case G_TYPE_CHAR:
		return g_strdup_printf ("%c", g_value_get_char (value));
	case G_TYPE_UCHAR:
		return g_strdup_printf ("%c", g_value_get_uchar (value));
	case G_TYPE_INT:
		return g_strdup_printf ("%d", g_value_get_int (value));
	case G_TYPE_UINT:
		return g_strdup_printf ("%u", g_value_get_uint (value));
	case G_TYPE_LONG:
		return g_strdup_printf ("%ld", g_value_get_long (value));
	case G_TYPE_ULONG:
		return g_strdup_printf ("%ld", g_value_get_ulong (value));
/* Not supported by C99. TODO: enable after splitting out libccss.
	case G_TYPE_INT64:
		return g_strdup_printf ("%Ld", g_value_get_int64 (value));
	case G_TYPE_UINT64:
		return g_strdup_printf ("%Ld", g_value_get_uint64 (value));
*/
	case G_TYPE_FLOAT:
		return g_strdup_printf ("%.03f", g_value_get_float (value));
	case G_TYPE_DOUBLE:
		return g_strdup_printf ("%.03f", g_value_get_double (value));
	case G_TYPE_STRING:
		return g_strdup (g_value_get_string (value));
	default:
		g_assert_not_reached ();
		return NULL;
	}
}

static char *
get_attribute (GceNode const	*node, 
	       char const	*name)
{
	GParamSpec	*param;
	GValue		 property = { 0, };

	g_assert (node && node->widget);

	if (node->flavor == TYPE) {
		g_warning ("Querying attribute `%s' on something not a widget", name);
		return NULL;
	}

	/* Attributes provided in the drawing function take precedence. */
	if (0 == strcmp ("shadow", name)) {
		return g_strdup (node->shadow);
	} else if (0 == strcmp ("orientation", name)) {
		return g_strdup (node->orientation);
	} else if (0 == strcmp ("edge", name)) {
		return g_strdup (node->edge);
	} else if (0 == strcmp ("expander-style", name)) {
		return g_strdup (node->expander_style);
	} else if (0 == strcmp ("gap", name)) {
		return g_strdup (node->gap);
	}

	/* Now try to find an apropriate style property. */
	param = gtk_widget_class_find_style_property (
			GTK_WIDGET_CLASS (G_OBJECT_GET_CLASS (node->widget)),
			name);
	if (param) {
		g_value_init (&property, param->value_type);
		gtk_widget_style_get_property (node->widget, name, &property);
		return _to_string (&property);
	}

	/* Next look for an apropriate gobject property. */
	param = g_object_class_find_property (
			G_OBJECT_GET_CLASS (node->widget),
			name);
	if (param) {
		g_value_init (&property, param->value_type);
		g_object_get_property (G_OBJECT (node->widget), name, &property);
		return _to_string (&property);
	}

	return NULL;
}

static void
release (GceNode *node)
{
	g_assert (node == gce_node_cache_get_top_node ());

	if (node->widget) {
		g_object_unref (G_OBJECT (node->widget));
	}

	gce_node_cache_release_node (node);
}

static ccss_node_class_t _node_class = {
	.is_a			= (ccss_node_is_a_f) is_a,
	.get_container		= (ccss_node_get_container_f) get_container,
	.get_base_style		= (ccss_node_get_base_style_f) get_base_style,
	.get_id			= (ccss_node_get_id_f) get_id,
	.get_type		= (ccss_node_get_type_f) get_type,
	.get_class		= (ccss_node_get_class_f) get_class,
	.get_pseudo_class	= (ccss_node_get_pseudo_class_f) get_pseudo_class,
	.get_attribute		= (ccss_node_get_attribute_f) get_attribute,
	.release		= (ccss_node_release_f) release
};

static ccss_node_class_t *
peek_node_class (void)
{
	return &_node_class;
}

GtkWidget *
gce_node_get_widget (const GceNode *node)
{
	return node->widget;
}

char const *
gce_node_get_primitive (const GceNode *node)
{
	return node->impl.primitive;
}

