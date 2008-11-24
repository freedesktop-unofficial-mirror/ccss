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

#ifndef CCSS_CAIRO_PROPERTY_H
#define CCSS_CAIRO_PROPERTY_H

#include <glib.h>
#include <ccss/ccss-macros.h>

CCSS_BEGIN_DECLS

#define CCSS_PROPERTY_BACKGROUND_ATTACHMENT		ccss_property_background_attachment()
#define CCSS_PROPERTY_BACKGROUND_COLOR			ccss_property_background_color()
#define CCSS_PROPERTY_BACKGROUND_IMAGE			ccss_property_background_image()
#define CCSS_PROPERTY_BACKGROUND_POSITION		ccss_property_background_position()
#define CCSS_PROPERTY_BACKGROUND_REPEAT			ccss_property_background_repeat()
#define CCSS_PROPERTY_BACKGROUND_SIZE			ccss_property_background_size()

#define CCSS_PROPERTY_BORDER_COLOR			ccss_property_border_color()
#define CCSS_PROPERTY_BORDER_STYLE			ccss_property_border_style()
#define CCSS_PROPERTY_BORDER_WIDTH			ccss_property_border_width()

#define CCSS_PROPERTY_BORDER_BOTTOM_COLOR		ccss_property_border_bottom_color()
#define CCSS_PROPERTY_BORDER_BOTTOM_STYLE		ccss_property_border_bottom_style()
#define CCSS_PROPERTY_BORDER_BOTTOM_WIDTH		ccss_property_border_bottom_width()

#define CCSS_PROPERTY_BORDER_LEFT_COLOR			ccss_property_border_left_color()
#define CCSS_PROPERTY_BORDER_LEFT_STYLE			ccss_property_border_left_style()
#define CCSS_PROPERTY_BORDER_LEFT_WIDTH			ccss_property_border_left_width()

#define CCSS_PROPERTY_BORDER_RIGHT_COLOR		ccss_property_border_right_color()
#define CCSS_PROPERTY_BORDER_RIGHT_STYLE		ccss_property_border_right_style()
#define CCSS_PROPERTY_BORDER_RIGHT_WIDTH		ccss_property_border_right_width()

#define CCSS_PROPERTY_BORDER_TOP_COLOR			ccss_property_border_top_color()
#define CCSS_PROPERTY_BORDER_TOP_STYLE			ccss_property_border_top_style()
#define CCSS_PROPERTY_BORDER_TOP_WIDTH			ccss_property_border_top_width()

#define CCSS_PROPERTY_BORDER_RADIUS			ccss_property_border_radius()

#define CCSS_PROPERTY_BORDER_TOP_LEFT_RADIUS		ccss_property_border_top_left_radius()
#define CCSS_PROPERTY_BORDER_TOP_RIGHT_RADIUS		ccss_property_border_top_right_radius()
#define CCSS_PROPERTY_BORDER_BOTTOM_RIGHT_RADIUS	ccss_property_border_bottom_right_radius()
#define CCSS_PROPERTY_BORDER_BOTTOM_LEFT_RADIUS		ccss_property_border_bottom_left_radius()

#define CCSS_PROPERTY_BORDER_IMAGE			ccss_property_border_image()

#define CCSS_PROPERTY_COLOR				ccss_property_color()

GQuark ccss_property_background_attachment (void);
GQuark ccss_property_background_color (void);
GQuark ccss_property_background_image (void);
GQuark ccss_property_background_position (void);
GQuark ccss_property_background_repeat (void);
GQuark ccss_property_background_size (void);

GQuark ccss_property_border_color (void);
GQuark ccss_property_border_style (void);
GQuark ccss_property_border_width (void);

GQuark ccss_property_border_bottom_color (void);
GQuark ccss_property_border_bottom_style (void);
GQuark ccss_property_border_bottom_width (void);

GQuark ccss_property_border_left_color (void);
GQuark ccss_property_border_left_style (void);
GQuark ccss_property_border_left_width (void);

GQuark ccss_property_border_right_color (void);
GQuark ccss_property_border_right_style (void);
GQuark ccss_property_border_right_width (void);

GQuark ccss_property_border_top_color (void);
GQuark ccss_property_border_top_style (void);
GQuark ccss_property_border_top_width (void);

GQuark ccss_property_border_radius (void);

GQuark ccss_property_border_top_left_radius (void);
GQuark ccss_property_border_top_right_radius (void);
GQuark ccss_property_border_bottom_right_radius (void);
GQuark ccss_property_border_bottom_left_radius (void);

GQuark ccss_property_border_image (void);

GQuark ccss_property_color (void);

CCSS_END_DECLS

#endif /* CCSS_CAIRO_PROPERTY_H */

