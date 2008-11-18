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

#include <glib.h>
#include "ccss-block-priv.h"
#include "ccss-cairo-property.h"

void *
ccss_block_add_property (ccss_block_t	*self,
			 char const	*property_name,
			 void		*value)
{
	GQuark	 property_id;
	void	*old_property;

	property_id = g_quark_try_string (property_name);
	if (0 == property_id) {
		property_id = g_quark_from_string (property_name);
		/* FIXME: also hook into conversion mechanism? */
	}

	old_property = g_hash_table_lookup (self->properties,
					    (gpointer) property_id);

	g_hash_table_insert (self->properties,
			     (gpointer) property_id, value);

	return old_property;
}

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
	g_return_if_fail (self && self->properties);

	g_hash_table_destroy (self->properties), self->properties = NULL;
	g_free (self);
}

/* Generic property. */
static bool
convert_property (ccss_property_t const	*property,
		  ccss_property_type_t	 target,
		  void			*value)
{
	g_return_val_if_fail (property && value, false);

	switch (target) {
	case CCSS_PROPERTY_TYPE_DOUBLE:
		switch (property->type) {
		case CCSS_PROPERTY_TYPE_DOUBLE:
			* (double *) value = property->content.dval;
			break;
		case CCSS_PROPERTY_TYPE_STRING:
			g_message ("Unable to convert `string' property to `double'");
			return false;
		default:
			g_assert_not_reached ();
			return false;
		}
		break;
	case CCSS_PROPERTY_TYPE_STRING:
		switch (property->type) {
		case CCSS_PROPERTY_TYPE_DOUBLE:
			* (char **) value = g_strdup_printf ("%f", property->content.dval);
			break;
		case CCSS_PROPERTY_TYPE_STRING:
			* (char **) value = g_strdup (property->content.sval);
			break;
		default:
			g_assert_not_reached ();
			return false;
		}
		break;
	default:
		g_assert_not_reached ();
		return false;
	}

	return true;	
}

/* Generic property. */
ccss_property_t *
ccss_block_new_property (ccss_block_t	*self,
			 char const	*property_name)
{
	ccss_property_t	*property;
	GQuark		 property_id;

	g_return_val_if_fail (self && property_name && self->properties, NULL);

	property_id = g_quark_try_string (property_name);
	if (0 == property_id) {
		/* First property of this name, register generic conversion function. */
		property_id = g_quark_from_string (property_name);
		ccss_property_register_conversion_function (property_id, (ccss_property_convert_f) convert_property);
	}

	property = (ccss_property_t *) g_hash_table_lookup (
						self->properties,
						(gpointer) property_id);

	if (!property) {
		property = g_new0 (ccss_property_t, 1);
		g_hash_table_insert (
				self->properties, 
				(gpointer) property_id,
				(gpointer) property);
	}

	return property;
}

ccss_background_attachment_t *
ccss_block_new_background_attachment (ccss_block_t *self)
{
	ccss_background_attachment_t *bg_attachment;

	g_return_val_if_fail (self && self->properties, NULL);

	bg_attachment = (ccss_background_attachment_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BACKGROUND_ATTACHMENT);
	if (!bg_attachment) {
		bg_attachment = g_new0 (ccss_background_attachment_t, 1);
		g_hash_table_insert (
				self->properties,
				(gpointer) CCSS_PROPERTY_BACKGROUND_ATTACHMENT,
				(gpointer) bg_attachment);
	}
	
	return bg_attachment;
}

ccss_color_t *
ccss_block_new_background_color (ccss_block_t *self)
{
	ccss_color_t *bg_color;

	g_return_val_if_fail (self && self->properties, NULL);

	bg_color = (ccss_color_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BACKGROUND_COLOR);
	if (!bg_color) {
		bg_color = g_new0 (ccss_color_t, 1);
		g_hash_table_insert (
				self->properties,
				(gpointer) CCSS_PROPERTY_BACKGROUND_COLOR,
				(gpointer) bg_color);
	}
	
	return bg_color;
}

ccss_background_image_t *
ccss_block_new_background_image (ccss_block_t *self)
{
	ccss_background_image_t *bg_image;

	g_return_val_if_fail (self && self->properties, NULL);

	bg_image = (ccss_background_image_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BACKGROUND_IMAGE);
	if (!bg_image) {
		bg_image = g_new0 (ccss_background_image_t, 1);
		g_hash_table_insert (
				self->properties,
				(gpointer) CCSS_PROPERTY_BACKGROUND_IMAGE,
				(gpointer) bg_image);
	}
	
	return bg_image;
}

ccss_background_position_t *
ccss_block_new_background_position (ccss_block_t *self)
{
	ccss_background_position_t *bg_position;

	g_return_val_if_fail (self && self->properties, NULL);

	bg_position = (ccss_background_position_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BACKGROUND_POSITION);
	if (!bg_position) {
		bg_position = g_new0 (ccss_background_position_t, 1);
		g_hash_table_insert (
				self->properties,
				(gpointer) CCSS_PROPERTY_BACKGROUND_POSITION,
				(gpointer) bg_position);
	}
	
	return bg_position;
}

ccss_background_repeat_t *
ccss_block_new_background_repeat (ccss_block_t *self)
{
	ccss_background_repeat_t *bg_repeat;

	g_return_val_if_fail (self && self->properties, NULL);

	bg_repeat = (ccss_background_repeat_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BACKGROUND_REPEAT);
	if (!bg_repeat) {
		bg_repeat = g_new0 (ccss_background_repeat_t, 1);
		g_hash_table_insert (
				self->properties,
				(gpointer) CCSS_PROPERTY_BACKGROUND_REPEAT,
				(gpointer) bg_repeat);
	}
	
	return bg_repeat;
}

ccss_background_size_t *
ccss_block_new_background_size (ccss_block_t *self)
{
	ccss_background_size_t *bg_size;

	g_return_val_if_fail (self && self->properties, NULL);

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

ccss_color_t *
ccss_block_new_border_color (ccss_block_t *self)
{
	ccss_color_t *color;

	g_return_val_if_fail (self && self->properties, NULL);

	color = (ccss_color_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_COLOR);
	if (!color) {
		color = g_new0 (ccss_color_t, 1);
		g_hash_table_insert (self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_COLOR,
				(gpointer) color);
	}
	
	return color;
}

ccss_border_style_t *
ccss_block_new_border_style (ccss_block_t *self)
{
	ccss_border_style_t *style;

	g_return_val_if_fail (self && self->properties, NULL);

	style = (ccss_border_style_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_STYLE);
	if (!style) {
		style = g_new0 (ccss_border_style_t, 1);
		g_hash_table_insert (self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_STYLE,
				(gpointer) style);
	}
	
	return style;
}

ccss_border_width_t *
ccss_block_new_border_width (ccss_block_t *self)
{
	ccss_border_width_t *width;

	g_return_val_if_fail (self && self->properties, NULL);

	width = (ccss_border_width_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_WIDTH);
	if (!width) {
		width = g_new0 (ccss_border_width_t, 1);
		g_hash_table_insert (self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_WIDTH,
				(gpointer) width);
	}
	
	return width;
}

ccss_color_t *
ccss_block_new_border_bottom_color (ccss_block_t *self)
{
	ccss_color_t *color;

	g_return_val_if_fail (self && self->properties, NULL);

	color = (ccss_color_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_BOTTOM_COLOR);
	if (!color) {
		color = g_new0 (ccss_color_t, 1);
		g_hash_table_insert (self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_BOTTOM_COLOR,
				(gpointer) color);
	}
	
	return color;
}

ccss_border_style_t *
ccss_block_new_border_bottom_style (ccss_block_t *self)
{
	ccss_border_style_t *style;

	g_return_val_if_fail (self && self->properties, NULL);

	style = (ccss_border_style_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_BOTTOM_STYLE);
	if (!style) {
		style = g_new0 (ccss_border_style_t, 1);
		g_hash_table_insert (self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_BOTTOM_STYLE,
				(gpointer) style);
	}
	
	return style;
}

ccss_border_width_t *
ccss_block_new_border_bottom_width (ccss_block_t *self)
{
	ccss_border_width_t *width;

	g_return_val_if_fail (self && self->properties, NULL);

	width = (ccss_border_width_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_BOTTOM_WIDTH);
	if (!width) {
		width = g_new0 (ccss_border_width_t, 1);
		g_hash_table_insert (self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_BOTTOM_WIDTH,
				(gpointer) width);
	}
	
	return width;
}

ccss_color_t *
ccss_block_new_border_left_color (ccss_block_t *self)
{
	ccss_color_t *color;

	g_return_val_if_fail (self && self->properties, NULL);

	color = (ccss_color_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_LEFT_COLOR);
	if (!color) {
		color = g_new0 (ccss_color_t, 1);
		g_hash_table_insert (self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_LEFT_COLOR,
				(gpointer) color);
	}
	
	return color;
}

ccss_border_style_t *
ccss_block_new_border_left_style (ccss_block_t *self)
{
	ccss_border_style_t *style;

	g_return_val_if_fail (self && self->properties, NULL);

	style = (ccss_border_style_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_LEFT_STYLE);
	if (!style) {
		style = g_new0 (ccss_border_style_t, 1);
		g_hash_table_insert (self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_LEFT_STYLE,
				(gpointer) style);
	}
	
	return style;
}

ccss_border_width_t *
ccss_block_new_border_left_width (ccss_block_t *self)
{
	ccss_border_width_t *width;

	g_return_val_if_fail (self && self->properties, NULL);

	width = (ccss_border_width_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_LEFT_WIDTH);
	if (!width) {
		width = g_new0 (ccss_border_width_t, 1);
		g_hash_table_insert (self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_LEFT_WIDTH,
				(gpointer) width);
	}
	
	return width;
}

ccss_color_t *
ccss_block_new_border_right_color (ccss_block_t *self)
{
	ccss_color_t *color;

	g_return_val_if_fail (self && self->properties, NULL);

	color = (ccss_color_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_RIGHT_COLOR);
	if (!color) {
		color = g_new0 (ccss_color_t, 1);
		g_hash_table_insert (self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_RIGHT_COLOR,
				(gpointer) color);
	}
	
	return color;
}

ccss_border_style_t *
ccss_block_new_border_right_style (ccss_block_t *self)
{
	ccss_border_style_t *style;

	g_return_val_if_fail (self && self->properties, NULL);

	style = (ccss_border_style_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_RIGHT_STYLE);
	if (!style) {
		style = g_new0 (ccss_border_style_t, 1);
		g_hash_table_insert (self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_RIGHT_STYLE,
				(gpointer) style);
	}
	
	return style;
}

ccss_border_width_t *
ccss_block_new_border_right_width (ccss_block_t *self)
{
	ccss_border_width_t *width;

	g_return_val_if_fail (self && self->properties, NULL);

	width = (ccss_border_width_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_RIGHT_WIDTH);
	if (!width) {
		width = g_new0 (ccss_border_width_t, 1);
		g_hash_table_insert (self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_RIGHT_WIDTH,
				(gpointer) width);
	}
	
	return width;
}

ccss_color_t *
ccss_block_new_border_top_color (ccss_block_t *self)
{
	ccss_color_t *color;

	g_return_val_if_fail (self && self->properties, NULL);

	color = (ccss_color_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_TOP_COLOR);
	if (!color) {
		color = g_new0 (ccss_color_t, 1);
		g_hash_table_insert (self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_TOP_COLOR,
				(gpointer) color);
	}
	
	return color;
}

ccss_border_style_t *
ccss_block_new_border_top_style (ccss_block_t *self)
{
	ccss_border_style_t *style;

	g_return_val_if_fail (self && self->properties, NULL);

	style = (ccss_border_style_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_TOP_STYLE);
	if (!style) {
		style = g_new0 (ccss_border_style_t, 1);
		g_hash_table_insert (self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_TOP_STYLE,
				(gpointer) style);
	}
	
	return style;
}

ccss_border_width_t *
ccss_block_new_border_top_width (ccss_block_t *self)
{
	ccss_border_width_t *width;

	g_return_val_if_fail (self && self->properties, NULL);

	width = (ccss_border_width_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_TOP_WIDTH);
	if (!width) {
		width = g_new0 (ccss_border_width_t, 1);
		g_hash_table_insert (self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_TOP_WIDTH,
				(gpointer) width);
	}
	
	return width;
}

ccss_border_join_t *
ccss_block_new_border_radius (ccss_block_t *self)
{
	ccss_border_join_t *join;

	g_return_val_if_fail (self && self->properties, NULL);

	join = (ccss_border_join_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_RADIUS);
	if (!join) {
		join = g_new0 (ccss_border_join_t, 1);
		g_hash_table_insert (self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_RADIUS,
				(gpointer) join);
	}
	
	return join;
}

ccss_border_join_t *
ccss_block_new_border_bottom_left_radius (ccss_block_t *self)
{
	ccss_border_join_t *join;

	g_return_val_if_fail (self && self->properties, NULL);

	join = (ccss_border_join_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_BOTTOM_LEFT_RADIUS);
	if (!join) {
		join = g_new0 (ccss_border_join_t, 1);
		g_hash_table_insert (self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_BOTTOM_LEFT_RADIUS,
				(gpointer) join);
	}
	
	return join;
}

ccss_border_join_t *
ccss_block_new_border_bottom_right_radius (ccss_block_t *self)
{
	ccss_border_join_t *join;

	g_return_val_if_fail (self && self->properties, NULL);

	join = (ccss_border_join_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_BOTTOM_RIGHT_RADIUS);
	if (!join) {
		join = g_new0 (ccss_border_join_t, 1);
		g_hash_table_insert (self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_BOTTOM_RIGHT_RADIUS,
				(gpointer) join);
	}
	
	return join;
}

ccss_border_join_t *
ccss_block_new_border_top_left_radius (ccss_block_t *self)
{
	ccss_border_join_t *join;

	g_return_val_if_fail (self && self->properties, NULL);

	join = (ccss_border_join_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_TOP_LEFT_RADIUS);
	if (!join) {
		join = g_new0 (ccss_border_join_t, 1);
		g_hash_table_insert (self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_TOP_LEFT_RADIUS,
				(gpointer) join);
	}
	
	return join;
}

ccss_border_join_t *
ccss_block_new_border_top_right_radius (ccss_block_t *self)
{
	ccss_border_join_t *join;

	g_return_val_if_fail (self && self->properties, NULL);

	join = (ccss_border_join_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_TOP_RIGHT_RADIUS);
	if (!join) {
		join = g_new0 (ccss_border_join_t, 1);
		g_hash_table_insert (self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_TOP_RIGHT_RADIUS,
				(gpointer) join);
	}
	
	return join;
}

ccss_border_image_t *
ccss_block_new_border_image (ccss_block_t *self)
{
	ccss_border_image_t *border_image;

	g_return_val_if_fail (self && self->properties, NULL);

	border_image = (ccss_border_image_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_IMAGE);
	if (!border_image) {
		border_image = g_new0 (ccss_border_image_t, 1);
		g_hash_table_insert (self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_IMAGE,
				(gpointer) border_image);
	}
	
	return border_image;
}

ccss_color_t *
ccss_block_new_color (ccss_block_t *self)
{
	ccss_color_t *color;

	g_return_val_if_fail (self && self->properties, NULL);

	color = (ccss_color_t *) g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_COLOR);
	if (!color) {
		color = g_new0 (ccss_color_t, 1);
		g_hash_table_insert (self->properties,
				(gpointer) CCSS_PROPERTY_COLOR,
				(gpointer) color);
	}
	
	return color;
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

/* Generic property. */
ccss_property_t const *
ccss_block_get_property (ccss_block_t const	*self,
			 char const		*property_name)
{
	GQuark property_id;

	g_return_val_if_fail (self && property_name && self->properties, NULL);

	property_id = g_quark_try_string (property_name);
	if (0 == property_id) {
		/* Maybe this is legal, or will be some time, but let's warn for now. */
		g_warning ("Unknown property `%s'", property_name);
		return NULL;
	}

	return (ccss_property_t const *) g_hash_table_lookup (
			self->properties,
			(gpointer) property_id);

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

ccss_color_t const *
ccss_block_get_border_color (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_color_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_COLOR);
}

ccss_border_style_t const *
ccss_block_get_border_style (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_border_style_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_STYLE);
}

ccss_border_width_t const *
ccss_block_get_border_width (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_border_width_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_WIDTH);
}

ccss_color_t const *
ccss_block_get_border_bottom_color (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_color_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_BOTTOM_COLOR);
}

ccss_border_style_t const *
ccss_block_get_border_bottom_style (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_border_style_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_BOTTOM_STYLE);
}

ccss_border_width_t const *
ccss_block_get_border_bottom_width (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_border_width_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_BOTTOM_WIDTH);
}

ccss_color_t const *
ccss_block_get_border_left_color (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_color_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_LEFT_COLOR);
}

ccss_border_style_t const *
ccss_block_get_border_left_style (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_border_style_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_LEFT_STYLE);
}

ccss_border_width_t const *
ccss_block_get_border_left_width (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_border_width_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_LEFT_WIDTH);
}

ccss_color_t const *
ccss_block_get_border_right_color (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_color_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_RIGHT_COLOR);
}

ccss_border_style_t const *
ccss_block_get_border_right_style (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_border_style_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_RIGHT_STYLE);
}

ccss_border_width_t const *
ccss_block_get_border_right_width (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_border_width_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_RIGHT_WIDTH);
}

ccss_color_t const *
ccss_block_get_border_top_color (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_color_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_TOP_COLOR);
}

ccss_border_style_t const *
ccss_block_get_border_top_style (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_border_style_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_TOP_STYLE);
}

ccss_border_width_t const *
ccss_block_get_border_top_width (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_border_width_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_TOP_WIDTH);
}

ccss_border_join_t const *
ccss_block_get_border_radius (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_border_join_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_RADIUS);
}

ccss_border_join_t const *
ccss_block_get_border_bottom_left_radius (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_border_join_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_BOTTOM_LEFT_RADIUS);
}

ccss_border_join_t const *
ccss_block_get_border_bottom_right_radius (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_border_join_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_BOTTOM_RIGHT_RADIUS);
}

ccss_border_join_t const *
ccss_block_get_border_top_left_radius (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_border_join_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_TOP_LEFT_RADIUS);
}

ccss_border_join_t const *
ccss_block_get_border_top_right_radius (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_border_join_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_TOP_RIGHT_RADIUS);
}

ccss_border_image_t const *
ccss_block_get_border_image (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_border_image_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_BORDER_IMAGE);
}

ccss_color_t const *
ccss_block_get_color (ccss_block_t const *self)
{
	g_return_val_if_fail (self && self->properties, NULL);

	return (ccss_color_t const *) 
			g_hash_table_lookup (
				self->properties,
				(gpointer) CCSS_PROPERTY_COLOR);
}

#ifdef CCSS_DEBUG

void
ccss_block_dump (ccss_block_t const *self)
{
	GHashTableIter	 iter;
	GQuark		 property_id;
	void const	*property;
	char		*strval;

	g_hash_table_iter_init (&iter, self->properties);
	while (g_hash_table_iter_next (&iter, (gpointer *) &property_id, (gpointer *) &property))  {

		if (ccss_property_convert (property, property_id, 
		    CCSS_PROPERTY_TYPE_STRING, &strval)) {
			printf ("%s: %s;\n", g_quark_to_string (property_id), strval);
			g_free (strval), strval = NULL;
		} else {
			g_message ("Failed to serialise property `%s'", g_quark_to_string (property_id));
		}
	}
}

#endif /* CCSS_DEBUG */

