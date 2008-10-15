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

#ifndef CCSS_STYLE_PRIV_H
#define CCSS_STYLE_PRIV_H

#include <glib.h>
#include <ccss/ccss-features.h>
#include <ccss/ccss-background.h>
#include <ccss/ccss-border.h>
#include <ccss/ccss-color.h>
#include <ccss/ccss-property.h>
#include <ccss/ccss-style.h>

G_BEGIN_DECLS

void
ccss_style_gather_outline (ccss_style_t const		 *self,
			   ccss_border_stroke_t		 *bottom,
			   ccss_border_stroke_t		 *left,
			   ccss_border_stroke_t		 *right,
			   ccss_border_stroke_t		 *top,
			   ccss_border_join_t const	**bottom_left,
			   ccss_border_join_t const	**bottom_right,
			   ccss_border_join_t const	**top_left,
			   ccss_border_join_t const	**top_right);

void
ccss_style_gather_background (ccss_style_t const			 *self,
			      ccss_background_attachment_t const	**bg_attachment, 
			      ccss_color_t const			**bg_color,
			      ccss_background_image_t const		**bg_image,
			      ccss_background_position_t const		**bg_position,
			      ccss_background_repeat_t const		**bg_repeat,
			      ccss_background_size_t const		**bg_size);

G_END_DECLS

#endif /* CCSS_STYLE_PRIV_H */

