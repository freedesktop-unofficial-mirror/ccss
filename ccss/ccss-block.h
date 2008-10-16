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

#ifndef CCSS_BLOCK_H
#define CCSS_BLOCK_H

#include <glib.h>
#include <cairo.h>
#include <ccss/ccss-features.h>
#include <ccss/ccss-color.h>
#include <ccss/ccss-background.h>
#include <ccss/ccss-border.h>
#include <ccss/ccss-property.h>

G_BEGIN_DECLS

typedef struct {
	/*< private >*/
	GHashTable *properties;
} ccss_block_t;

ccss_block_t *	ccss_block_new	(void);
void		ccss_block_free	(ccss_block_t *self);

ccss_property_t *		ccss_block_new_property				(ccss_block_t *self, char const *property_name);

ccss_background_attachment_t * 	ccss_block_new_background_attachment		(ccss_block_t *self);
ccss_color_t * 			ccss_block_new_background_color			(ccss_block_t *self);
ccss_background_image_t * 	ccss_block_new_background_image			(ccss_block_t *self);
ccss_background_position_t * 	ccss_block_new_background_position		(ccss_block_t *self);
ccss_background_repeat_t * 	ccss_block_new_background_repeat		(ccss_block_t *self);
ccss_background_size_t * 	ccss_block_new_background_size			(ccss_block_t *self);

ccss_color_t * 			ccss_block_new_border_bottom_color		(ccss_block_t *self);
ccss_border_style_t * 		ccss_block_new_border_bottom_style		(ccss_block_t *self);
ccss_border_width_t * 		ccss_block_new_border_bottom_width		(ccss_block_t *self);

ccss_color_t * 			ccss_block_new_border_left_color		(ccss_block_t *self);
ccss_border_style_t * 		ccss_block_new_border_left_style		(ccss_block_t *self);
ccss_border_width_t * 		ccss_block_new_border_left_width		(ccss_block_t *self);

ccss_color_t * 			ccss_block_new_border_right_color		(ccss_block_t *self);
ccss_border_style_t * 		ccss_block_new_border_right_style		(ccss_block_t *self);
ccss_border_width_t * 		ccss_block_new_border_right_width		(ccss_block_t *self);

ccss_color_t * 			ccss_block_new_border_top_color			(ccss_block_t *self);
ccss_border_style_t * 		ccss_block_new_border_top_style			(ccss_block_t *self);
ccss_border_width_t * 		ccss_block_new_border_top_width			(ccss_block_t *self);

ccss_border_join_t * 		ccss_block_new_border_bottom_left_radius	(ccss_block_t *self);
ccss_border_join_t * 		ccss_block_new_border_bottom_right_radius	(ccss_block_t *self);
ccss_border_join_t * 		ccss_block_new_border_top_left_radius		(ccss_block_t *self);
ccss_border_join_t * 		ccss_block_new_border_top_right_radius		(ccss_block_t *self);

ccss_color_t *			ccss_block_new_color				(ccss_block_t *self);


ccss_property_t const *			ccss_block_get_property				(ccss_block_t const *self, char const *property_name);

ccss_color_t const * 			ccss_block_get_background_color			(ccss_block_t const *self);
ccss_background_attachment_t const * 	ccss_block_get_background_attachment		(ccss_block_t const *self);
ccss_background_image_t const * 	ccss_block_get_background_image			(ccss_block_t const *self);
ccss_background_position_t const * 	ccss_block_get_background_position		(ccss_block_t const *self);
ccss_background_repeat_t const * 	ccss_block_get_background_repeat		(ccss_block_t const *self);
ccss_background_size_t const * 		ccss_block_get_background_size			(ccss_block_t const *self);

ccss_color_t const * 			ccss_block_get_border_bottom_color		(ccss_block_t const *self);
ccss_border_style_t const * 		ccss_block_get_border_bottom_style		(ccss_block_t const *self);
ccss_border_width_t const *		ccss_block_get_border_bottom_width		(ccss_block_t const *self);

ccss_color_t const * 			ccss_block_get_border_left_color		(ccss_block_t const *self);
ccss_border_style_t const * 		ccss_block_get_border_left_style		(ccss_block_t const *self);
ccss_border_width_t const *		ccss_block_get_border_left_width		(ccss_block_t const *self);

ccss_color_t const * 			ccss_block_get_border_right_color		(ccss_block_t const *self);
ccss_border_style_t const *		ccss_block_get_border_right_style		(ccss_block_t const *self);
ccss_border_width_t const *		ccss_block_get_border_right_width		(ccss_block_t const *self);

ccss_color_t const * 			ccss_block_get_border_top_color			(ccss_block_t const *self);
ccss_border_style_t const * 		ccss_block_get_border_top_style			(ccss_block_t const *self);
ccss_border_width_t const *		ccss_block_get_border_top_width			(ccss_block_t const *self);

ccss_border_join_t const * 		ccss_block_get_border_bottom_left_radius	(ccss_block_t const *self);
ccss_border_join_t const * 		ccss_block_get_border_bottom_right_radius	(ccss_block_t const *self);
ccss_border_join_t const * 		ccss_block_get_border_top_left_radius		(ccss_block_t const *self);
ccss_border_join_t const * 		ccss_block_get_border_top_right_radius		(ccss_block_t const *self);

ccss_color_t const *			ccss_block_get_color				(ccss_block_t const *self);

#ifdef CCSS_DEBUG
void ccss_block_dump (ccss_block_t const *self);
#endif

G_END_DECLS

#endif /* CCSS_BLOCK_H */

