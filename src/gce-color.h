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

#ifndef GCE_COLOR_H
#define GCE_COLOR_H

#include <glib.h>
#include <gdk/gdk.h>

G_BEGIN_DECLS

gboolean gce_color_lookup (char const *spec, GdkColor *color);

gboolean gce_color_mix (double factor, GdkColor const *color1, GdkColor const *color2, GdkColor *result);

gboolean gce_color_shade (double factor, GdkColor *color);

G_END_DECLS

#endif /* GCE_COLOR_H */

