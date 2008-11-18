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

#include <string.h>
#include <glib.h>
#include "ccss-border-image.h"
#include "ccss-border-parser.h"
#include "ccss-cairo-property.h"
#include "ccss-property.h"
#include "ccss-style-priv.h"

static const ccss_background_attachment_t const _bg_attachment = {
	.spec = CCSS_PROPERTY_SPEC_SET,
	.attachment = CCSS_BACKGROUND_SCROLL
};

static const ccss_color_t const _bg_color = {
	.spec = CCSS_PROPERTY_SPEC_SET,
	.red = 1.,
	.green = 1.,
	.blue = 1.
};

static const ccss_background_image_t const _bg_image = {
	.spec = CCSS_PROPERTY_SPEC_NONE
};

static const ccss_background_position_t const _bg_position = {
	.spec = CCSS_PROPERTY_SPEC_SET,
	.hpos = { CCSS_POSITION_LENGTH, 0 },
	.vpos = { CCSS_POSITION_LENGTH, 0 }
};

static const ccss_background_repeat_t const _bg_repeat = {
	.spec = CCSS_PROPERTY_SPEC_SET,
	.repeat = CCSS_BACKGROUND_REPEAT
};

static const ccss_background_size_t const _bg_size = {
	.spec = CCSS_PROPERTY_SPEC_SET,
	.width = { CCSS_POSITION_AUTO, 0 },
	.height = { CCSS_POSITION_AUTO, 0 }
};

/* FIXME: don't draw borders by default? */
static const ccss_border_style_t const _border_style = {
	.spec = CCSS_PROPERTY_SPEC_UNSET,
	.style = CCSS_BORDER_STYLE_SOLID
};

static const ccss_border_width_t const _border_width = {
	.spec = CCSS_PROPERTY_SPEC_SET,
	.width = 1.
};

static const ccss_border_join_t const _border_join = {
	.spec = CCSS_PROPERTY_SPEC_UNSET,
	.radius = 0
};

static const ccss_color_t const _color = {
	.spec = CCSS_PROPERTY_SPEC_SET,
	.red = 0.,
	.green = 0.,
	.blue = 0.
};

static const struct {

	ccss_background_attachment_t const	*bg_attachment;
	ccss_color_t const			*bg_color;
	ccss_background_image_t const		*bg_image;
	ccss_background_position_t const	*bg_position;
	ccss_background_repeat_t const		*bg_repeat;
	ccss_background_size_t const		*bg_size;

	ccss_color_t const			*bottom_color;
	ccss_border_style_t const		*bottom_style;
	ccss_border_width_t const		*bottom_width;

	ccss_color_t const			*left_color;
	ccss_border_style_t const		*left_style;
	ccss_border_width_t const		*left_width;

	ccss_color_t const			*right_color;
	ccss_border_style_t const		*right_style;
	ccss_border_width_t const		*right_width;

	ccss_color_t const			*top_color;
	ccss_border_style_t const		*top_style;
	ccss_border_width_t const		*top_width;

	ccss_border_join_t const		*bottom_left_radius;
	ccss_border_join_t const		*bottom_right_radius;
	ccss_border_join_t const		*top_left_radius;
	ccss_border_join_t const		*top_right_radius;

	ccss_color_t const			*color;	

} _default_style = {

	.bg_attachment = &_bg_attachment,
	.bg_color = &_bg_color,
	.bg_image = &_bg_image,
	.bg_position = &_bg_position,
	.bg_repeat = &_bg_repeat,
	.bg_size = &_bg_size,

	.bottom_color = &_color,
	.bottom_style = &_border_style,
	.bottom_width = &_border_width,

	.left_color = &_color,
	.left_style = &_border_style,
	.left_width = &_border_width,

	.right_color = &_color,
	.right_style = &_border_style,
	.right_width = &_border_width,

	.top_color = &_color,
	.top_style = &_border_style,
	.top_width = &_border_width,

	.bottom_left_radius = &_border_join,
	.bottom_right_radius = &_border_join,
	.top_left_radius = &_border_join,
	.top_right_radius = &_border_join,

	.color = &_color
};

/* FIXME: maybe they should also be in `block', where the generic conversion function is? */

static bool
convert_background_attachment (ccss_background_attachment_t const	*property,
			       ccss_property_type_t			 target,
			       void					*value)
{
	char *ret;

	g_return_val_if_fail (property && value, false);

	if (CCSS_PROPERTY_TYPE_DOUBLE == target)
		return false;

	switch (property->attachment) {
	case CCSS_BACKGROUND_SCROLL:
		ret = g_strdup ("scroll");
		break;
	case CCSS_BACKGROUND_FIXED:
		ret = g_strdup ("fixed");
		break;
	default:
		g_assert_not_reached ();
		return false;
	}

	* (char **) value = ret;

	return true;
}

static bool
convert_background_image (ccss_background_image_t const	*property,
			  ccss_property_type_t		 target,
			  void				*value)
{
	char *ret;

	g_return_val_if_fail (property && value, false);

	if (CCSS_PROPERTY_TYPE_DOUBLE == target)
		return false;

	ret = g_strdup (property->image.uri);

	* (char **) value = ret;

	return true;
}

static bool
convert_background_position (ccss_background_position_t const	*property,
			     ccss_property_type_t		 target,
			     void				*value)
{
	// FIXME: this needs 2 return values.
	g_return_val_if_fail (0, false);
	return false;
}

static bool
convert_background_repeat (ccss_background_repeat_t const	*property,
			   ccss_property_type_t			 target,
			   void					*value)
{
	char *ret;

	g_return_val_if_fail (property && value, false);

	if (CCSS_PROPERTY_TYPE_DOUBLE == target)
		return false;

	switch (property->repeat) {
	case CCSS_BACKGROUND_REPEAT:
		ret = g_strdup ("repeat");
		break;
	case CCSS_BACKGROUND_REPEAT_X:
		ret = g_strdup ("repeat-x");
		break;
	case CCSS_BACKGROUND_REPEAT_Y:
		ret = g_strdup ("repeat-y");
		break;
	case CCSS_BACKGROUND_NO_REPEAT:
		ret = g_strdup ("no-repeat");
		break;
	default:
		g_assert_not_reached ();
		return false;
	}

	* (char **) value = ret;

	return true;
}

static bool
convert_background_size (ccss_background_size_t const	*property,
			 ccss_property_type_t		 target,
			 void				*value)
{
	// FIXME: this needs 2 return values.
	g_return_val_if_fail (0, false);
	return false;
}

static bool
convert_border_style (ccss_border_style_t const	*property,
		      ccss_property_type_t	 target,
		      void			*value)
{
	g_return_val_if_fail (property && value, false);

	if (CCSS_PROPERTY_TYPE_DOUBLE == target)
		return false;

	* (char **) value = g_strdup (ccss_border_lookup_name (property->style));

	return true;
}

static bool
convert_border_width (ccss_border_width_t const	*property,
		      ccss_property_type_t	 target,
		      void			*value)
{
	g_return_val_if_fail (property && value, false);

	switch (target) {
	case CCSS_PROPERTY_TYPE_DOUBLE:
		* (double *) value = property->width;
		return true;
	case CCSS_PROPERTY_TYPE_STRING:
		* (char **) value = g_strdup_printf ("%f", property->width);
		return true;
	default:
		g_assert_not_reached ();
		return false;
	}

	return false;
}

static bool
convert_border_radius (ccss_border_join_t const	*property,
		       ccss_property_type_t	 target,
		       void			*value)
{
	g_return_val_if_fail (property && value, false);

	switch (target) {
	case CCSS_PROPERTY_TYPE_DOUBLE:
		* (double *) value = property->radius;
		return true;
	case CCSS_PROPERTY_TYPE_STRING:
		* (char **) value = g_strdup_printf ("%f", property->radius);
		return true;
	default:
		g_assert_not_reached ();
		return false;
	}

	return false;
}

static bool
convert_border_image (ccss_border_image_t const	*property,
		      ccss_property_type_t	 target,
		      void			*value)
{
	char *top, *right, *bottom, *left;
	char const *horizontal_tiling, *vertical_tiling;

	g_return_val_if_fail (property && value, false);

	if (CCSS_PROPERTY_TYPE_DOUBLE == target)
		return false;

	top = ccss_position_serialize (&property->top);
	right = ccss_position_serialize (&property->right);
	bottom = ccss_position_serialize (&property->bottom);
	left = ccss_position_serialize (&property->left);

	switch (property->top_middle_bottom_horizontal_tiling) {
	case CCSS_BORDER_IMAGE_TILING_REPEAT:
		horizontal_tiling = "repeat";
		break;
	case CCSS_BORDER_IMAGE_TILING_ROUND:
		horizontal_tiling = "round";
		break;
	case CCSS_BORDER_IMAGE_TILING_STRETCH:
		horizontal_tiling = "stretch";
		break;
	}

	switch (property->left_middle_right_vertical_tiling) {
	case CCSS_BORDER_IMAGE_TILING_REPEAT:
		vertical_tiling = "repeat";
		break;
	case CCSS_BORDER_IMAGE_TILING_ROUND:
		vertical_tiling = "round";
		break;
	case CCSS_BORDER_IMAGE_TILING_STRETCH:
		vertical_tiling = "stretch";
		break;
	}

	* (char **) value = g_strdup_printf ("url(%s) %s %s %s %s %s %s", 
				property->image.uri,
				top, right, bottom, left,
				horizontal_tiling, vertical_tiling);

	g_free (top), top = NULL;
	g_free (right), right = NULL;
	g_free (bottom), bottom = NULL;
	g_free (left), left = NULL;

	return true;
}

static bool
convert_color (ccss_color_t const	*property,
	       ccss_property_type_t	 target,
	       void			*value)
{
	g_return_val_if_fail (property && value, false);

	if (CCSS_PROPERTY_TYPE_DOUBLE == target)
		return false;

	* (char **) value = g_strdup_printf ("#%02x%02x%02x", 
						(int) (property->red * 255),
						(int) (property->green * 255),
						(int) (property->blue * 255));
	return true;
}

typedef struct {
	GQuark id;
	GQuark fallback;	
} fallback_map_t;

fallback_map_t *_fallback_map = NULL;

void
ccss_style_init (void)
{
	fallback_map_t fm[] = {
		{ CCSS_PROPERTY_BORDER_COLOR, CCSS_PROPERTY_COLOR },

		{ CCSS_PROPERTY_BORDER_BOTTOM_COLOR, CCSS_PROPERTY_BORDER_COLOR },
		{ CCSS_PROPERTY_BORDER_BOTTOM_STYLE, CCSS_PROPERTY_BORDER_STYLE },
		{ CCSS_PROPERTY_BORDER_BOTTOM_WIDTH, CCSS_PROPERTY_BORDER_WIDTH },

		{ CCSS_PROPERTY_BORDER_LEFT_COLOR, CCSS_PROPERTY_BORDER_COLOR },
		{ CCSS_PROPERTY_BORDER_LEFT_STYLE, CCSS_PROPERTY_BORDER_STYLE },
		{ CCSS_PROPERTY_BORDER_LEFT_WIDTH, CCSS_PROPERTY_BORDER_WIDTH },

		{ CCSS_PROPERTY_BORDER_RIGHT_COLOR, CCSS_PROPERTY_BORDER_COLOR },
		{ CCSS_PROPERTY_BORDER_RIGHT_STYLE, CCSS_PROPERTY_BORDER_STYLE },
		{ CCSS_PROPERTY_BORDER_RIGHT_WIDTH, CCSS_PROPERTY_BORDER_WIDTH },

		{ CCSS_PROPERTY_BORDER_TOP_COLOR, CCSS_PROPERTY_BORDER_COLOR },
		{ CCSS_PROPERTY_BORDER_TOP_STYLE, CCSS_PROPERTY_BORDER_STYLE },
		{ CCSS_PROPERTY_BORDER_TOP_WIDTH, CCSS_PROPERTY_BORDER_WIDTH },

		{ CCSS_PROPERTY_BORDER_TOP_LEFT_RADIUS,		CCSS_PROPERTY_BORDER_RADIUS },
		{ CCSS_PROPERTY_BORDER_TOP_RIGHT_RADIUS,	CCSS_PROPERTY_BORDER_RADIUS },
		{ CCSS_PROPERTY_BORDER_BOTTOM_RIGHT_RADIUS,	CCSS_PROPERTY_BORDER_RADIUS },
		{ CCSS_PROPERTY_BORDER_BOTTOM_LEFT_RADIUS,	CCSS_PROPERTY_BORDER_RADIUS },

		{ 0, 0 }
	};

	g_assert (NULL == _fallback_map);

	_fallback_map = g_memdup (&fm, sizeof (fm));

	ccss_property_register_conversion_function (CCSS_PROPERTY_BACKGROUND_ATTACHMENT, (ccss_property_convert_f) convert_background_attachment);
	ccss_property_register_conversion_function (CCSS_PROPERTY_BACKGROUND_COLOR, (ccss_property_convert_f) convert_color);
	ccss_property_register_conversion_function (CCSS_PROPERTY_BACKGROUND_IMAGE, (ccss_property_convert_f) convert_background_image);
	ccss_property_register_conversion_function (CCSS_PROPERTY_BACKGROUND_POSITION, (ccss_property_convert_f) convert_background_position);
	ccss_property_register_conversion_function (CCSS_PROPERTY_BACKGROUND_REPEAT, (ccss_property_convert_f) convert_background_repeat);
	ccss_property_register_conversion_function (CCSS_PROPERTY_BACKGROUND_SIZE, (ccss_property_convert_f) convert_background_size);

	ccss_property_register_conversion_function (CCSS_PROPERTY_BORDER_COLOR, (ccss_property_convert_f) convert_color);
	ccss_property_register_conversion_function (CCSS_PROPERTY_BORDER_STYLE, (ccss_property_convert_f) convert_border_style);
	ccss_property_register_conversion_function (CCSS_PROPERTY_BORDER_WIDTH, (ccss_property_convert_f) convert_border_width);

	ccss_property_register_conversion_function (CCSS_PROPERTY_BORDER_BOTTOM_COLOR, (ccss_property_convert_f) convert_color);
	ccss_property_register_conversion_function (CCSS_PROPERTY_BORDER_BOTTOM_STYLE, (ccss_property_convert_f) convert_border_style);
	ccss_property_register_conversion_function (CCSS_PROPERTY_BORDER_BOTTOM_WIDTH, (ccss_property_convert_f) convert_border_width);

	ccss_property_register_conversion_function (CCSS_PROPERTY_BORDER_LEFT_COLOR, (ccss_property_convert_f) convert_color);
	ccss_property_register_conversion_function (CCSS_PROPERTY_BORDER_LEFT_STYLE, (ccss_property_convert_f) convert_border_style);
	ccss_property_register_conversion_function (CCSS_PROPERTY_BORDER_LEFT_WIDTH, (ccss_property_convert_f) convert_border_width);

	ccss_property_register_conversion_function (CCSS_PROPERTY_BORDER_RIGHT_COLOR, (ccss_property_convert_f) convert_color);
	ccss_property_register_conversion_function (CCSS_PROPERTY_BORDER_RIGHT_STYLE, (ccss_property_convert_f) convert_border_style);
	ccss_property_register_conversion_function (CCSS_PROPERTY_BORDER_RIGHT_WIDTH, (ccss_property_convert_f) convert_border_width);

	ccss_property_register_conversion_function (CCSS_PROPERTY_BORDER_TOP_COLOR, (ccss_property_convert_f) convert_color);
	ccss_property_register_conversion_function (CCSS_PROPERTY_BORDER_TOP_STYLE, (ccss_property_convert_f) convert_border_style);
	ccss_property_register_conversion_function (CCSS_PROPERTY_BORDER_TOP_WIDTH, (ccss_property_convert_f) convert_border_width);

	ccss_property_register_conversion_function (CCSS_PROPERTY_BORDER_RADIUS, (ccss_property_convert_f) convert_border_radius);

	ccss_property_register_conversion_function (CCSS_PROPERTY_BORDER_TOP_LEFT_RADIUS, (ccss_property_convert_f) convert_border_radius);
	ccss_property_register_conversion_function (CCSS_PROPERTY_BORDER_TOP_RIGHT_RADIUS, (ccss_property_convert_f) convert_border_radius);
	ccss_property_register_conversion_function (CCSS_PROPERTY_BORDER_BOTTOM_RIGHT_RADIUS, (ccss_property_convert_f) convert_border_radius);
	ccss_property_register_conversion_function (CCSS_PROPERTY_BORDER_BOTTOM_LEFT_RADIUS, (ccss_property_convert_f) convert_border_radius);

	ccss_property_register_conversion_function (CCSS_PROPERTY_BORDER_IMAGE, (ccss_property_convert_f) convert_border_image);

	ccss_property_register_conversion_function (CCSS_PROPERTY_COLOR, (ccss_property_convert_f) convert_color);
}

void
ccss_style_shutdown (void)
{
	g_assert (_fallback_map != NULL);

	g_free (_fallback_map), _fallback_map = NULL;
}

/**
 * ccss_style_new:
 *
 * Create a new #ccss_style_t.
 **/
ccss_style_t *
ccss_style_new (void)
{
	ccss_style_t *self;

	self = g_new0 (ccss_style_t, 1);
	self->properties = g_hash_table_new ((GHashFunc) g_direct_hash, 
					     (GEqualFunc) g_direct_equal);

	return self;
}

/**
 * ccss_style_free:
 * @self: a #ccss_style_t.
 *
 * Frees the style an all associated resources.
 **/
void
ccss_style_free (ccss_style_t *self)
{
	g_return_if_fail (self && self->properties);

	g_hash_table_destroy (self->properties), self->properties = NULL;
	g_free (self);
}

/* Look up a property, take fallback into account. */
static void const *
lookup_property_r (ccss_style_t const	*self,
		   GQuark		 property_id)
{
	void const *property;

	property = g_hash_table_lookup (self->properties, (gpointer) property_id);
	if (property) {
		return property;
	}

	for (unsigned int i = 0; _fallback_map[i].id != 0; i++) {
		if (property_id == _fallback_map[i].id) {
			return lookup_property_r (self, _fallback_map[i].fallback);
		}
	}

	return NULL;
}

/**
 * ccss_style_get_double:
 * @self:		a #ccss_style_t.
 * @property_name:	name of the property.
 * @value:		location to store the converted property.
 *
 * Query a numeric property.
 *
 * Returns: %TRUE if the property was found and could be converted.
 **/
bool 
ccss_style_get_double (ccss_style_t const	*self,
		       char const		*property_name,
		       double			*value)
{
	GQuark		 property_id;
	void const	*property;

	g_return_val_if_fail (self && property_name && value, false);

	property_id = g_quark_try_string (property_name);
	if (0 == property_id) {
		g_warning ("Unknown property `%s'", property_name);
		return false;
	}

	property = lookup_property_r (self, property_id);
	if (NULL == property)
		return false;

	return ccss_property_convert (property, property_id, 
				      CCSS_PROPERTY_TYPE_DOUBLE, value);
}

/**
 * ccss_style_get_string:
 * @self:		a #ccss_style_t.
 * @property_name:	name of the property.
 * @value:		location to store the converted property.
 *
 * Query a string property.
 * 
 * Returns: %TRUE if the property was found and could be converted.
 **/
bool 
ccss_style_get_string (ccss_style_t const	 *self,
		       char const		 *property_name,
		       char			**value)
{
	GQuark		 property_id;
	void const	*property;

	g_return_val_if_fail (self && property_name && value, false);

	property_id = g_quark_try_string (property_name);
	if (0 == property_id) {
		g_warning ("Unknown property `%s'", property_name);
		return false;
	}

	property = lookup_property_r (self, property_id);
	if (NULL == property)
		return false;

	return ccss_property_convert (property, property_id, 
				      CCSS_PROPERTY_TYPE_STRING, value);
}

/**
 * ccss_style_get_property:
 * @self:		a #ccss_style_t.
 * @property_name:	name of the property.
 * @value:		location to store the raw property pointer.
 * 
 * Query a custom property.
 *
 * Returns: %TRUE if the property was found.
 **/
bool
ccss_style_get_property	(ccss_style_t const	 *self,
			 char const		 *property_name,
			 void			**value)
{
	GQuark		 property_id;

	g_return_val_if_fail (self && property_name && value, false);

	property_id = g_quark_try_string (property_name);
	if (0 == property_id) {
		g_warning ("Unknown property `%s'", property_name);
		return false;
	}

	return g_hash_table_lookup_extended (self->properties,
					     (gconstpointer) property_id, NULL,
					     value);
}

/**
 * ccss_style_draw_horizontal_line:
 * @self:	a ccss_style_t.
 * @cr:		the target to draw onto.
 * @x1:		the starting x coordinate.
 * @x2:		the ending x coordinate.
 * @y:		the y coordinate.
 *
 * Draw a horizontal line using this style instance. This takes the `top'
 * border from the CSS, if specified.
 **/
void
ccss_style_draw_line (ccss_style_t const	*self,
		     cairo_t		*cr, 
		     int32_t 		 x1,
		     int32_t 		 x2,
		     int32_t 		 y1,
		     int32_t		 y2)
{
	ccss_border_stroke_t		 stroke;
	double				 off;

	stroke.color = (ccss_color_t *) lookup_property_r (self,
						CCSS_PROPERTY_BORDER_COLOR);
	if (NULL == stroke.color) {
		stroke.color = _default_style.top_color;
	}

	stroke.style = (ccss_border_style_t *) lookup_property_r (self,
						CCSS_PROPERTY_BORDER_STYLE);
	if (NULL == stroke.style) {
		stroke.style = _default_style.top_style;
	}

	stroke.width = (ccss_border_width_t *) lookup_property_r (self,
						CCSS_PROPERTY_BORDER_WIDTH);
	if (NULL == stroke.width) {
		stroke.width = _default_style.top_width;
	}

	/* Unlike borders, lines are not drawn inside the box, 
	 * account for that. */
	off = stroke.width->width / 2.;

	if (y1 == y2) {
		ccss_border_draw (NULL, NULL, &stroke, NULL,
				 NULL, NULL, NULL, NULL,
				 CCSS_BORDER_VISIBILITY_SHOW_ALL,
				 cr, x1, y1 - off, x2 - x1, 0);
	} else {
		ccss_border_draw (&stroke, NULL, NULL, NULL,
				 NULL, NULL, NULL, NULL,
				 CCSS_BORDER_VISIBILITY_SHOW_ALL,
				 cr, x1 - off, y1, 0, y2 - y1);
	}
}

static void
gather_stroke (ccss_style_t const		*self,
	       GQuark				 color_prop,
	       ccss_color_t const		*color_fallback,
	       GQuark				 style_prop,
	       ccss_border_style_t const	*style_fallback,
	       GQuark				 width_prop,
	       ccss_border_width_t const	*width_fallback,
	       ccss_border_stroke_t		*stroke)
{
	stroke->color = (ccss_color_t *) lookup_property_r (self, color_prop);
	if (NULL == stroke->color) {
		stroke->color = color_fallback;
	}

	stroke->style = (ccss_border_style_t *) lookup_property_r (self, style_prop);
	if (NULL == stroke->style) {
		stroke->style = style_fallback;
	}

	stroke->width = (ccss_border_width_t *) lookup_property_r (self, width_prop);
	if (NULL == stroke->width) {
		stroke->width = width_fallback;
	}
}

static ccss_border_join_t const *
gather_join (ccss_style_t const		*self,
	     GQuark			 prop,
	     ccss_border_join_t const	*fallback)
{
	ccss_border_join_t const *join;

	join = (ccss_border_join_t *) lookup_property_r (self, prop);
	if (NULL == join) {
		join = fallback;
	}

	return join;
}

void
ccss_style_gather_outline (ccss_style_t const		 *self,
			   ccss_border_stroke_t		 *bottom,
			   ccss_border_stroke_t		 *left,
			   ccss_border_stroke_t		 *right,
			   ccss_border_stroke_t		 *top,
			   ccss_border_join_t const	**bottom_left,
			   ccss_border_join_t const	**bottom_right,
			   ccss_border_join_t const	**top_left,
			   ccss_border_join_t const	**top_right)
{
	gather_stroke (self, 
		CCSS_PROPERTY_BORDER_BOTTOM_COLOR, _default_style.bottom_color, 
		CCSS_PROPERTY_BORDER_BOTTOM_STYLE, _default_style.bottom_style, 
		CCSS_PROPERTY_BORDER_BOTTOM_WIDTH, _default_style.bottom_width, 
		bottom);

	gather_stroke (self, 
		CCSS_PROPERTY_BORDER_LEFT_COLOR, _default_style.left_color, 
		CCSS_PROPERTY_BORDER_LEFT_STYLE, _default_style.left_style, 
		CCSS_PROPERTY_BORDER_LEFT_WIDTH, _default_style.left_width, 
		left);

	gather_stroke (self, 
		CCSS_PROPERTY_BORDER_RIGHT_COLOR, _default_style.right_color, 
		CCSS_PROPERTY_BORDER_RIGHT_STYLE, _default_style.right_style, 
		CCSS_PROPERTY_BORDER_RIGHT_WIDTH, _default_style.right_width, 
		right);

	gather_stroke (self, 
		CCSS_PROPERTY_BORDER_TOP_COLOR, _default_style.top_color, 
		CCSS_PROPERTY_BORDER_TOP_STYLE, _default_style.top_style, 
		CCSS_PROPERTY_BORDER_TOP_WIDTH, _default_style.top_width, 
		top);

	*bottom_left = gather_join (self, CCSS_PROPERTY_BORDER_BOTTOM_LEFT_RADIUS,
				   _default_style.bottom_left_radius);

	*bottom_right = gather_join (self, CCSS_PROPERTY_BORDER_BOTTOM_RIGHT_RADIUS,
				    _default_style.bottom_right_radius);

	*top_left = gather_join (self, CCSS_PROPERTY_BORDER_TOP_LEFT_RADIUS,
			        _default_style.top_left_radius);

	*top_right = gather_join (self, CCSS_PROPERTY_BORDER_TOP_RIGHT_RADIUS,
				 _default_style.top_right_radius);

}

void
ccss_style_gather_background (ccss_style_t const			 *self,
			      ccss_background_attachment_t const	**bg_attachment, 
			      ccss_color_t const			**bg_color,
			      ccss_background_image_t const		**bg_image,
			      ccss_background_position_t const		**bg_position,
			      ccss_background_repeat_t const		**bg_repeat,
			      ccss_background_size_t const		**bg_size)
{
	*bg_attachment = (ccss_background_attachment_t const *) 
		g_hash_table_lookup (
			self->properties,
			(gpointer) CCSS_PROPERTY_BACKGROUND_ATTACHMENT);
	if (!*bg_attachment)
		*bg_attachment = _default_style.bg_attachment;

	*bg_color = (ccss_color_t const *)
		g_hash_table_lookup (
			self->properties,
			(gpointer) CCSS_PROPERTY_BACKGROUND_COLOR);
	if (!*bg_color)
		*bg_color = _default_style.bg_color;

	*bg_image = (ccss_background_image_t const *)
		g_hash_table_lookup (
			self->properties,
			(gpointer) CCSS_PROPERTY_BACKGROUND_IMAGE);
	if (!*bg_image)
		*bg_image = _default_style.bg_image;

	*bg_position = (ccss_background_position_t const *)
		g_hash_table_lookup (
			self->properties,
			(gpointer) CCSS_PROPERTY_BACKGROUND_POSITION);
	if (!*bg_position)
		*bg_position = _default_style.bg_position;

	*bg_repeat = (ccss_background_repeat_t const *)
		g_hash_table_lookup (
			self->properties,
			(gpointer) CCSS_PROPERTY_BACKGROUND_REPEAT);
	if (!*bg_repeat)
		*bg_repeat = _default_style.bg_repeat;

	*bg_size = (ccss_background_size_t const *)
		g_hash_table_lookup (
			self->properties,
			(gpointer) CCSS_PROPERTY_BACKGROUND_SIZE);
	if (!*bg_size)
		*bg_size = _default_style.bg_size;
}

/**
 * ccss_style_draw_outline:
 * @self:	a ccss_style_t.
 * @cr:		the target to draw onto.
 * @x:		the starting x coordinate.
 * @y:		the starting y coordinate.
 * @width:	width of the outline to draw.
 * @height:	height of the outline to draw.
 *
 * Draw an outline using this style instance. Information about how to draw
 * this style's background is diregarded.
 **/
void
ccss_style_draw_outline (ccss_style_t const	*self, 
			cairo_t			*cr, 
			int32_t			 x,
			int32_t			 y,
			int32_t			 width,
			int32_t			 height)
{
	ccss_border_stroke_t		 bottom, left, right, top;
	ccss_border_join_t const	*bottom_left;
	ccss_border_join_t const	*bottom_right;
	ccss_border_join_t const	*top_left;
	ccss_border_join_t const	*top_right;

	ccss_style_gather_outline (self, &bottom, &left, &right, &top,
				   &bottom_left, &bottom_right, &top_left, &top_right);

	ccss_border_draw (&left, top_left, 
			  &top, top_right,
			  &right, bottom_right,
			  &bottom, bottom_left,
			  CCSS_BORDER_VISIBILITY_SHOW_ALL,
			  cr, x, y, width, height);
}

/**
 * ccss_style_draw_rectangle:
 * @self:	a ccss_style_t.
 * @cr:		the target to draw onto.
 * @x:		the starting x coordinate.
 * @y:		the starting y coordinate.
 * @width:	width of the outline to draw.
 * @height:	height of the outline to draw.
 *
 * Draw a rectangle using this style instance.
 **/
void
ccss_style_draw_rectangle (ccss_style_t const	*self,
			  cairo_t		*cr, 
			  int32_t		 x,
			  int32_t		 y,
			  int32_t		 width,
			  int32_t		 height)
{
	ccss_border_stroke_t		 bottom, left, right, top;
	ccss_border_join_t const	*bottom_left;
	ccss_border_join_t const	*bottom_right;
	ccss_border_join_t const	*top_left;
	ccss_border_join_t const	*top_right;
	ccss_border_image_t const	*border_image;

	ccss_background_attachment_t const	*bg_attachment;
	ccss_color_t const			*bg_color;
	ccss_background_image_t const		*bg_image;
	ccss_background_position_t const	*bg_position;
	ccss_background_repeat_t const		*bg_repeat;
	ccss_background_size_t const		*bg_size;

	int32_t l, t, w, h;

	ccss_style_gather_outline (self, &bottom, &left, &right, &top,
				   &bottom_left, &bottom_right, &top_left, &top_right);

	ccss_style_gather_background (self, &bg_attachment, &bg_color, &bg_image, 
				      &bg_position, &bg_repeat, &bg_size);

	ccss_border_path (&left, top_left, 
			  &top, top_right,
			  &right, bottom_right,
			  &bottom, bottom_left,
			  cr, x, y, width, height);

	/* FIXME: background size is calculated against allocation
	 * when using `fixed'. */
	if (CCSS_BACKGROUND_FIXED == bg_attachment->attachment) {
		l = self->viewport_x;
		t = self->viewport_y;
		w = self->viewport_width;
		h = self->viewport_height;
	} else {
		l = x;
		t = y;
		w = width;
		h = height;
	}

	ccss_background_fill (bg_attachment, bg_color, bg_image, bg_position, 
			      bg_repeat, bg_size, cr, l, t, w, h);

	cairo_new_path (cr);

	/* PONDERING: should border-image vs. border be resolved at style application time, 
	 * i.e. should a higher-priority border override border-image? */
	border_image = 	(ccss_border_image_t const *) 
		g_hash_table_lookup (
			self->properties,
			(gpointer) CCSS_PROPERTY_BORDER_IMAGE);

	if (border_image) {

		ccss_border_image_draw (border_image, cr,
					x, y, width, height);

	} else {
		ccss_border_draw (&left, top_left, 
				  &top, top_right,
				  &right, bottom_right,
				  &bottom, bottom_left,
				  CCSS_BORDER_VISIBILITY_SHOW_ALL,
				  cr, x, y, width, height);
	}
}

#ifdef CCSS_DEBUG

/**
 * ccss_style_dump:
 * @self:	a ccss_style_t.
 *
 * Print informations about the internal state of this object.
 **/
void
ccss_style_dump (ccss_style_t const *self)
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

