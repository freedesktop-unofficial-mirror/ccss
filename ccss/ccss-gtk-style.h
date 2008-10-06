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

#ifndef CCSS_GTK_STYLE_H
#define CCSS_GTK_STYLE_H

#include <stdbool.h>
#include <stdint.h>
#include <cairo.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <ccss/ccss-features.h>
#include <ccss/ccss-style.h>

G_BEGIN_DECLS

void ccss_style_draw_gap (ccss_style_t const *self, cairo_t *cr, 
			 int32_t x, int32_t y, int32_t width, int32_t height, 
			 GtkPositionType gap_side, int32_t gap_x, int32_t gap_width);

void ccss_style_draw_polygon (ccss_style_t const *self, cairo_t *cr, 
			     GdkPoint *points, int32_t n_points, bool fill);

G_END_DECLS

#endif /* CCSS_GTK_STYLE_H */

