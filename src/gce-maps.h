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

#ifndef GCE_MAPS_H
#define GCE_MAPS_H

#include <glib.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

char const *		gce_maps_match_detail		(char const *gce_detail);
char const *		gce_maps_get_detail		(char const *gtk_detail);

GtkStateType		gce_maps_match_state		(char const *gce_state);
char const *		gce_maps_get_state		(GtkStateType state);

GtkShadowType		gce_maps_match_shadow		(char const *gce_shadow);
char const *		gce_maps_get_shadow		(GtkShadowType shadow);

GtkArrowType		gce_maps_match_arrow		(char const *gce_arrow);
char const *		gce_maps_get_arrow		(GtkArrowType gtk_arrow);

GtkPositionType		gce_maps_match_position		(char const *gce_position);
char const *		gce_maps_get_position		(GtkPositionType gtk_position);

GdkWindowEdge		gce_maps_match_window_edge	(char const *gce_window_edge);
char const *		gce_maps_get_window_edge	(GdkWindowEdge gdk_window_edge);

GtkOrientation		gce_maps_match_orientation	(char const *css_orientation);
char const *		gce_maps_get_orientation	(GtkOrientation gtk_orientation);

GtkExpanderStyle	gce_maps_match_expander_style	(char const *css_expander_style);
char const *		gce_maps_get_expander_style	(GtkExpanderStyle gtk_expander_style);

G_END_DECLS

#endif /* GCE_MAPS_H */

