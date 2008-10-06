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

#ifndef GCE_NODE_H
#define GCE_NODE_H

#include <ccss/ccss.h>
#include <glib.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GCE_NODE_CACHE_SIZE 24

typedef struct GceNode_ GceNode;

GceNode * gce_node_cache_fetch_primitive_node (GtkWidget *widget, 
					       char const *class_name,
					       char const *pseudo_class,
					       char const *shadow,
					       char const *orientation,
					       char const *gap,
					       char const *edge,
					       char const *expander_style,
					       char const *primitive);
GceNode * gce_node_cache_fetch_node (GtkWidget *widget, 
				     char const *class_name,
				     char const *pseudo_class,
				     char const *shadow,
				     char const *orientation,
				     char const *gap,
				     char const *edge,
				     char const *expander_style);
GceNode * gce_node_cache_get_top_node (void);
void gce_node_cache_release_node (GceNode *node);

GtkWidget *	gce_node_get_widget	(const GceNode *node);
char const *	gce_node_get_primitive	(const GceNode *node);

G_END_DECLS

#endif /* GCE_NODE_H */

