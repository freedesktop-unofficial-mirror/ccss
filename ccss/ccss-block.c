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

#include "ccss-block.h"

ccss_block_t *
ccss_block_new (void)
{
	ccss_block_t *self;

	self = g_new0 (ccss_block_t, 1);
	self->properties = g_hash_table_new_full ((GHashFunc) g_direct_hash, 
						  (GEqualFunc) g_direct_equal,
						  NULL,
						  (GDestroyNotify) g_free);

	return self;
}

void
ccss_block_free (ccss_block_t *self)
{
	g_assert (self);

	g_free (self);
}

ccss_background_t *
ccss_block_new_background (ccss_block_t *self)
{
	ccss_background_t *background;

	background = (ccss_background_t *) g_hash_table_lookup (
						self->properties,
						(gpointer) CCSS_PROPERTY_BACKGROUND);
	if (!background) {
		ccss_background_attachment_t	*bg_attachment;
		ccss_color_t			*bg_color;
		ccss_background_image_t		*bg_image;
		ccss_background_position_t	*bg_position;
		ccss_background_repeat_t	*bg_repeat;
		ccss_background_size_t		*bg_size;

		background = g_new0 (ccss_background_t, 1);
		g_hash_table_insert (self->properties,
				     (gpointer) CCSS_PROPERTY_BACKGROUND,
				     (gpointer) background);

		/* Consolidate already existing background properties. */

		bg_attachment = (ccss_background_attachment_t *) ccss_block_get_background_attachment (self);
		if (bg_attachment) {
			background->bg_attachment = *bg_attachment;
			g_hash_table_remove (self->properties, (gpointer) CCSS_PROPERTY_BACKGROUND_ATTACHMENT);
		}

		bg_color = (ccss_color_t *) ccss_block_get_background_color (self);
		if (bg_color) {
			background->bg_color = *bg_color;
			g_hash_table_remove (self->properties, (gpointer) CCSS_PROPERTY_BACKGROUND_COLOR);
		}

		bg_image = (ccss_background_image_t *) ccss_block_get_background_image (self);
		if (bg_image) {
			background->bg_image = *bg_image;
			g_hash_table_remove (self->properties, (gpointer) CCSS_PROPERTY_BACKGROUND_IMAGE);
		}

		bg_position = (ccss_background_position_t *) ccss_block_get_background_position (self);
		if (bg_position) {
			background->bg_position = *bg_position;
			g_hash_table_remove (self->properties, (gpointer) CCSS_PROPERTY_BACKGROUND_POSITION);
		}

		bg_repeat = (ccss_background_repeat_t *) ccss_block_get_background_repeat (self);
		if (bg_repeat) {
			background->bg_repeat = *bg_repeat;
			g_hash_table_remove (self->properties, (gpointer) CCSS_PROPERTY_BACKGROUND_REPEAT);
		}

		bg_size = (ccss_background_size_t *) ccss_block_get_background_size (self);
		if (bg_size) {
			background->bg_size = *bg_size;
			g_hash_table_remove (self->properties, (gpointer) CCSS_PROPERTY_BACKGROUND_SIZE);
		}		
	}
	
	return background;
}

ccss_background_attachment_t *
ccss_block_new_background_attachment (ccss_block_t *self)
{
	ccss_background_t		*background;
	ccss_background_attachment_t	*bg_attachment;

	g_return_val_if_fail (self && self->properties, NULL);

	/* Already allocated as part of `background'? */
	background = (ccss_background_t *) ccss_block_get_background (self);
	if (background)
		return &background->bg_attachment;

	bg_attachment = (ccss_background_attachment_t *) g_hash_table_lookup (
						self->properties,
						(gpointer) CCSS_PROPERTY_BACKGROUND_ATTACHMENT);
	if (!bg_attachment) {
		bg_attachment = g_new0 (ccss_background_attachment_t, 1);
		g_hash_table_insert (self->properties,
				     (gpointer) CCSS_PROPERTY_BACKGROUND_ATTACHMENT,
				     (gpointer) bg_attachment);
	}
	
	return bg_attachment;
}

ccss_color_t *
ccss_block_new_background_color (ccss_block_t *self)
{
	ccss_background_t	*background;
	ccss_color_t		*bg_color;

	g_return_val_if_fail (self && self->properties, NULL);

	/* Already allocated as part of `background'? */
	background = (ccss_background_t *) ccss_block_get_background (self);
	if (background)
		return &background->bg_color;

	bg_color = (ccss_color_t *) g_hash_table_lookup (
						self->properties,
						(gpointer) CCSS_PROPERTY_BACKGROUND_COLOR);
	if (!bg_color) {
		bg_color = g_new0 (ccss_color_t, 1);
		g_hash_table_insert (self->properties,
				     (gpointer) CCSS_PROPERTY_BACKGROUND_COLOR,
				     (gpointer) bg_color);
	}
	
	return bg_color;
}

ccss_background_image_t *
ccss_block_new_background_image (ccss_block_t *self)
{
	ccss_background_t	*background;
	ccss_background_image_t	*bg_image;

	g_return_val_if_fail (self && self->properties, NULL);

	/* Already allocated as part of `background'? */
	background = (ccss_background_t *) ccss_block_get_background (self);
	if (background)
		return &background->bg_image;

	bg_image = (ccss_background_image_t *) g_hash_table_lookup (
						self->properties,
						(gpointer) CCSS_PROPERTY_BACKGROUND_IMAGE);
	if (!bg_image) {
		bg_image = g_new0 (ccss_background_image_t, 1);
		g_hash_table_insert (self->properties,
				     (gpointer) CCSS_PROPERTY_BACKGROUND_IMAGE,
				     (gpointer) bg_image);
	}
	
	return bg_image;
}

ccss_background_position_t *
ccss_block_new_background_position (ccss_block_t *self)
{
	ccss_background_t		*background;
	ccss_background_position_t	*bg_position;

	g_return_val_if_fail (self && self->properties, NULL);

	/* Already allocated as part of `background'? */
	background = (ccss_background_t *) ccss_block_get_background (self);
	if (background)
		return &background->bg_position;

	bg_position = (ccss_background_position_t *) g_hash_table_lookup (
						self->properties,
						(gpointer) CCSS_PROPERTY_BACKGROUND_POSITION);
	if (!bg_position) {
		bg_position = g_new0 (ccss_background_position_t, 1);
		g_hash_table_insert (self->properties,
				     (gpointer) CCSS_PROPERTY_BACKGROUND_POSITION,
				     (gpointer) bg_position);
	}
	
	return bg_position;
}

ccss_background_repeat_t *
ccss_block_new_background_repeat (ccss_block_t *self)
{
	ccss_background_t		*background;
	ccss_background_repeat_t	*bg_repeat;

	g_return_val_if_fail (self && self->properties, NULL);

	/* Already allocated as part of `background'? */
	background = (ccss_background_t *) ccss_block_get_background (self);
	if (background)
		return &background->bg_repeat;

	bg_repeat = (ccss_background_repeat_t *) g_hash_table_lookup (
						self->properties,
						(gpointer) CCSS_PROPERTY_BACKGROUND_REPEAT);
	if (!bg_repeat) {
		bg_repeat = g_new0 (ccss_background_repeat_t, 1);
		g_hash_table_insert (self->properties,
				     (gpointer) CCSS_PROPERTY_BACKGROUND_REPEAT,
				     (gpointer) bg_repeat);
	}
	
	return bg_repeat;
}

ccss_background_size_t *
ccss_block_new_background_size (ccss_block_t *self)
{
	ccss_background_t	*background;
	ccss_background_size_t	*bg_size;

	g_return_val_if_fail (self && self->properties, NULL);

	/* Already allocated as part of `background'? */
	background = (ccss_background_t *) ccss_block_get_background (self);
	if (background)
		return &background->bg_size;

	bg_size = (ccss_background_size_t *) g_hash_table_lookup (
						self->properties,
						(gpointer) CCSS_PROPERTY_BACKGROUND_SIZE);
	if (!bg_size) {
		bg_size = g_new0 (ccss_background_size_t, 1);
		g_hash_table_insert (self->properties,
				     (gpointer) CCSS_PROPERTY_BACKGROUND_SIZE,
				     (gpointer) bg_size);
	}
	
	return bg_size;
}

ccss_border_t *
ccss_block_new_border (ccss_block_t *self)
{

}

ccss_color_t *
ccss_block_new_border_color (ccss_block_t *self)
{

}

ccss_border_style_type_t *
ccss_block_new_border_style (ccss_block_t *self)
{

}

double *
ccss_block_new_border_width (ccss_block_t *self)
{

}

ccss_border_stroke_t *
ccss_block_new_border_bottom (ccss_block_t *self)
{

}

ccss_color_t *
ccss_block_new_border_bottom_color (ccss_block_t *self)
{

}

ccss_border_style_type_t *
ccss_block_new_border_bottom_style (ccss_block_t *self)
{

}

double *
ccss_block_new_border_bottom_with (ccss_block_t *self)
{

}

ccss_border_stroke_t *
ccss_block_new_border_left (ccss_block_t *self)
{

}

ccss_border_stroke_t *
ccss_block_new_border_right (ccss_block_t *self)
{

}

ccss_border_stroke_t *
ccss_block_new_border_top (ccss_block_t *self)
{

}

ccss_border_join_t *
ccss_block_new_border_radius (ccss_block_t *self)
{

}

ccss_border_join_t *
ccss_block_new_border_bottom_left_radius (ccss_block_t *self)
{

}

ccss_border_join_t *
ccss_block_new_border_bottom_right_radius (ccss_block_t *self)
{

}

ccss_border_join_t *
ccss_block_new_border_top_left_radius (ccss_block_t *self)
{

}

ccss_border_join_t *
ccss_block_new_border_top_right_radius (ccss_block_t *self)
{

}

ccss_color_t *
ccss_block_new_color (ccss_block_t *self)
{

}

ccss_background_t const *
ccss_block_get_background (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_background_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BACKGROUND);
}

ccss_background_attachment_t const *
ccss_block_get_background_attachment (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_background_attachment_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BACKGROUND_ATTACHMENT);
}

ccss_color_t const *
ccss_block_get_background_color (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_color_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BACKGROUND_COLOR);
}

ccss_background_image_t const *
ccss_block_get_background_image (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_background_image_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BACKGROUND_IMAGE);
}

ccss_background_position_t const *
ccss_block_get_background_position (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_background_position_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BACKGROUND_POSITION);
}

ccss_background_repeat_t const *
ccss_block_get_background_repeat (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_background_repeat_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BACKGROUND_REPEAT);
}

ccss_background_size_t const *
ccss_block_get_background_size (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_background_size_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BACKGROUND_SIZE);
}

ccss_border_t const *
ccss_block_get_border (ccss_block_t const *self)
{

}

ccss_color_t const *
ccss_block_get_border_color (ccss_block_t const *self)
{

}

ccss_border_style_type_t 
ccss_block_get_border_style (ccss_block_t const *self)
{

}

double
ccss_block_get_border_width (ccss_block_t const *self)
{

}

ccss_border_stroke_t const *
ccss_block_get_border_bottom (ccss_block_t const *self)
{

}

ccss_color_t const *
ccss_block_get_border_bottom_color (ccss_block_t const *self)
{

}

ccss_border_style_type_t
ccss_block_get_border_bottom_style (ccss_block_t const *self)
{

}

double
ccss_block_get_border_bottom_with (ccss_block_t const *self)
{

}

ccss_border_stroke_t const *
ccss_block_get_border_left (ccss_block_t const *self)
{

}

ccss_color_t const *
ccss_block_get_border_left_color (ccss_block_t const *self)
{

}

ccss_border_style_type_t
ccss_block_get_border_left_style (ccss_block_t const *self)
{

}

double
ccss_block_get_border_left_width (ccss_block_t const *self)
{

}

ccss_border_stroke_t const *
ccss_block_get_border_right (ccss_block_t const *self)
{

}

ccss_color_t const *
ccss_block_get_border_right_color (ccss_block_t const *self)
{

}

ccss_border_style_type_t
ccss_block_get_border_right_style (ccss_block_t const *self)
{

}

double
ccss_block_get_border_right_width (ccss_block_t const *self)
{

}

ccss_border_stroke_t const *
ccss_block_get_border_top (ccss_block_t const *self)
{

}

ccss_color_t const *
ccss_block_get_border_top_color (ccss_block_t const *self)
{

}

ccss_border_style_type_t
ccss_block_get_border_top_style (ccss_block_t const *self)
{

}

double
ccss_block_get_border_top_width (ccss_block_t const *self)
{

}

ccss_border_join_t const *
ccss_block_get_border_radius (ccss_block_t const *self)
{

}

ccss_border_join_t const *
ccss_block_get_border_bottom_left_radius (ccss_block_t const *self)
{

}

ccss_border_join_t const *
ccss_block_get_border_bottom_right_radius (ccss_block_t const *self)
{

}

ccss_border_join_t const *
ccss_block_get_border_top_left_radius (ccss_block_t const *self)
{

}

ccss_border_join_t const *
ccss_block_get_border_top_right_radius (ccss_block_t const *self)
{

}

ccss_color_t const *
ccss_block_get_color (ccss_block_t const *self)
{

}

#ifdef CCSS_DEBUG

void
ccss_block_dump (ccss_block_t const *self)
{
	ccss_background_dump (&self->background);
	ccss_border_dump (&self->border);
	ccss_color_dump (&self->color);
}

#endif /* CCSS_DEBUG */

