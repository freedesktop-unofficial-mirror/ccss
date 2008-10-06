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

#include <math.h>
#include <string.h>
#include "ccss-background.h"

static const struct {
	char const				*name;
	const enum ccss_background_repeat	 repeat;
} _repeat_map[] = {
  { "repeat",		CCSS_BACKGROUND_REPEAT		},
  { "repeat-x",		CCSS_BACKGROUND_REPEAT_X		},
  { "repeat-y",		CCSS_BACKGROUND_REPEAT_Y		},
  { "no-repeat",	CCSS_BACKGROUND_NO_REPEAT	}
};

ccss_background_t * 
ccss_background_new (void)
{
	return g_new0 (ccss_background_t, 1);
}

void
ccss_background_free (ccss_background_t *self)
{
	g_free (self);
}

static bool
bg_attachment_parse (ccss_background_attachment_t	 *self,
		     CRTerm const			**values)
{
	ccss_property_spec_t spec;

	if (!*values) {
		return false;
	}

	spec = ccss_property_parse_spec (values);
	if (CCSS_PROPERTY_SPEC_INHERIT == spec) {
		self->spec = spec;
		return true;
	}

	if (TERM_IDENT == (*values)->type) {
		char const *attachment;
		attachment = cr_string_peek_raw_str ((*values)->content.str);
		if (0 == g_ascii_strcasecmp ("scroll", attachment)) {
			self->attachment = CCSS_BACKGROUND_SCROLL;
			self->spec = CCSS_PROPERTY_SPEC_SET;
			*values = (*values)->next;
			return true;
		} else if (0 == g_ascii_strcasecmp ("fixed", attachment)) {
			self->attachment = CCSS_BACKGROUND_FIXED;
			self->spec = CCSS_PROPERTY_SPEC_SET;
			*values = (*values)->next;
			return true;
		}
	}

	return false;
}

static bool
bg_image_parse (ccss_background_image_t	 *self,
		CRTerm const		**values)
{
	if (!*values) {
		return false;
	}

	self->spec = ccss_image_parse (&self->image, values);
	return self->spec == CCSS_PROPERTY_SPEC_SET;
}

static bool
bg_position_parse (ccss_background_position_t	 *self,
		   CRTerm const			**values)
{
	ccss_property_spec_t	spec;
	uint32_t		flags;
	bool			have_hpos;
	bool			have_vpos;

	if (!*values) {
		return false;
	}

	spec = ccss_property_parse_spec (values);
	if (CCSS_PROPERTY_SPEC_INHERIT == spec) {
		/* Not implemented yet. */
		g_assert_not_reached ();
		self->spec = CCSS_PROPERTY_SPEC_UNSET;
		return false;
	}

	flags = CCSS_POSITION_MASK_NUMERIC | CCSS_POSITION_MASK_HORIZONTAL;
	have_hpos = ccss_position_parse (&self->hpos, flags, values);
	if (!have_hpos) {
		self->spec = CCSS_PROPERTY_SPEC_UNSET;
		return false;		
	}

	flags = CCSS_POSITION_MASK_NUMERIC | CCSS_POSITION_MASK_VERTICAL;
	have_vpos = ccss_position_parse (&self->vpos, flags, values);
	if (have_hpos && !have_vpos) {
		/* Fall back to `center' for vpos. 
		 * http://www.w3.org/TR/CSS21/colors.html#propdef-background-position */
		self->vpos.type = CCSS_POSITION_PERCENTAGE;
		self->vpos.value = 50;
	}
	
	/* A bit fuzzy, but let's say we're satisfied with `hpos' only. */
	if (have_hpos) {
		self->spec = CCSS_PROPERTY_SPEC_SET;
		return true;
	}

	return false;
}

static bool
bg_repeat_parse (ccss_background_repeat_t	 *self,
		 CRTerm const			**values)
{
	char const		*repeat;
	ccss_property_spec_t	 spec;

	if (!*values || (*values)->type != TERM_IDENT) {
		return false;
	}

	repeat = cr_string_peek_raw_str ((*values)->content.str);
	for (unsigned int i = 0; i < G_N_ELEMENTS (_repeat_map); i++) {
		if (0 == g_ascii_strcasecmp (_repeat_map[i].name, repeat)) {
			self->repeat = _repeat_map[i].repeat;
			self->spec = CCSS_PROPERTY_SPEC_SET;
			*values = (*values)->next;
			return true;
		}
	}

	/* Not found, maybe a generic property? 
	 * Only `inherit' is allowed anyway. */
	spec = ccss_property_parse_spec (values);
	if (spec == CCSS_PROPERTY_SPEC_INHERIT) {
		self->spec = spec;
		return true;
	} else {
		self->spec = CCSS_PROPERTY_SPEC_UNSET;
		return false;
	}
}

static bool
bg_size_parse (ccss_background_size_t	 *self,
	       CRTerm const		**values)
{
	uint32_t	flags;
	bool		ret;

	if (!*values) {
		return false;
	}

	flags = CCSS_POSITION_MASK_NUMERIC | CCSS_POSITION_MASK_AUTO;
	ret = ccss_position_parse (&self->width, flags, values);
	if (!ret) {
		return false;
	}

	if (CCSS_POSITION_CONTAIN == self->width.type || 
	    CCSS_POSITION_COVER == self->width.type) {
		self->height.type = self->width.type;
		self->spec = CCSS_PROPERTY_SPEC_SET;
		return true;
	}

	if (CCSS_POSITION_MASK_NUMERIC & self->width.type ||
	    CCSS_POSITION_AUTO == self->width.type) {
		flags = CCSS_POSITION_MASK_NUMERIC | CCSS_POSITION_MASK_AUTO;
		ret = ccss_position_parse (&self->height, flags, values);
		if (ret) {
			self->spec = CCSS_PROPERTY_SPEC_SET;
			return true;
		} else {
			self->height.type =  CCSS_POSITION_AUTO;
			self->spec = CCSS_PROPERTY_SPEC_SET;
			return true;
		}
	}

	return false;
}

bool
ccss_background_parse (ccss_background_t	*self,
		      char const	*property,
		      CRTerm const	*values)
{
	bool ret;

	ret = false;

	if (0 == strcmp ("background", property)) {

		/* TODO: also support `background-size' here, but let's stick
		 * to CSS2 for now. */
		ret = ccss_color_parse (&self->bg_color, &values);
		if (ret && values == NULL)
			return true;
		else if (!ret)
			return false;

		ret = bg_image_parse (&self->bg_image, &values);
		if (ret && values == NULL)
			return true;
		else if (!ret)
			return false;

		ret = bg_repeat_parse (&self->bg_repeat, &values);
		if (ret && values == NULL)
			return true;
		else if (!ret)
			return false;

		ret = bg_attachment_parse (&self->bg_attachment, &values);
		if (ret && values == NULL)
			return true;
		else if (!ret)
			return false;

		ret = bg_position_parse (&self->bg_position, &values);
		if (ret && values == NULL)
			return true;
		else if (!ret)
			return false;
	}

	if (0 == strcmp ("background-attachment", property)) {

		return bg_attachment_parse (&self->bg_attachment, &values);
	}

	if (0 == strcmp ("background-color", property)) {

		return ccss_color_parse (&self->bg_color, &values);
	}

	if (0 == strcmp ("background-image", property)) {

		return bg_image_parse (&self->bg_image, &values);
	}

	if (0 == strcmp ("background-position", property)) {

		return bg_position_parse (&self->bg_position, &values);
	}

	if (0 == strcmp ("background-repeat", property)) {

		return bg_repeat_parse (&self->bg_repeat, &values);
	}

	if (0 == strcmp ("background-size", property)) {

		return bg_size_parse (&self->bg_size, &values);
	}

	return false;
}

static cairo_pattern_t *
create_pattern (ccss_background_image_t const	*bg_image,
		double				 width,
		double				 height,
		double				 tile_width,
		double				 tile_height)
{
	cairo_t		*cr;
	cairo_surface_t	*surface;
	cairo_status_t	 status;
	cairo_pattern_t *pattern;

	/* Setup. */
	surface = NULL;
	status = cairo_pattern_get_surface (bg_image->image.pattern, &surface);
	if (status != CAIRO_STATUS_SUCCESS) {
		g_warning (cairo_status_to_string (status));
		return NULL;
	}

	surface = cairo_surface_create_similar (surface,
						CAIRO_CONTENT_COLOR_ALPHA,
						width, height);
	cr = cairo_create (surface);

	/* Drawing. */
	cairo_pattern_set_extend (bg_image->image.pattern, CAIRO_EXTEND_REPEAT);
	cairo_set_source (cr, bg_image->image.pattern);
	cairo_paint (cr);

	/* Cleanup. */
	pattern = cairo_pattern_create_for_surface (surface);
	cairo_destroy (cr), cr = NULL;
	cairo_surface_destroy (surface), surface = NULL;

	return pattern;
}

static void
repeat (ccss_background_image_t const	*bg_image,
	cairo_t				*cr,
	int32_t				 width,
	int32_t				 height,
	double				 tile_width,
	double				 tile_height)
{
	cairo_pattern_t	*pattern;

	/* Create pattern for (width + tile_width, height + tile_height)
	 * so we can account for `background-position'. */
	pattern = create_pattern (bg_image,
				  width + tile_width, height + tile_height,
				  tile_width, tile_height);

	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_NONE);
	cairo_set_source (cr, pattern);
	cairo_pattern_destroy (pattern), pattern = NULL;
	cairo_fill_preserve (cr);
}

static void
repeat_x (ccss_background_image_t const	*bg_image,
	  cairo_t			*cr,
	  int32_t			 width,
	  int32_t			 height,
	  double			 tile_width,
	  double			 tile_height)
{
	cairo_pattern_t	*pattern;

	/* Create pattern for (width + tile_width, tile_height)
	 * so we can account for `background-position'. */
	pattern = create_pattern (bg_image,
				  width + tile_width, tile_height,
				  tile_width, tile_height);

	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_NONE);
	cairo_set_source (cr, pattern);
	cairo_pattern_destroy (pattern), pattern = NULL;
	cairo_fill_preserve (cr);
}

static void
repeat_y (ccss_background_image_t const	*bg_image,
	  cairo_t			*cr,
	  int32_t			 width,
	  int32_t			 height,
	  double			 tile_width,
	  double			 tile_height)
{
	cairo_pattern_t	*pattern;

	/* Create pattern for (tile_width, height + tile_height)
	 * so we can account for `background-position'. */
	pattern = create_pattern (bg_image,
				  tile_width, height + tile_height,
				  tile_width, tile_height);

	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_NONE);
	cairo_set_source (cr, pattern);
	cairo_pattern_destroy (pattern), pattern = NULL;
	cairo_fill_preserve (cr);
}

static void
no_repeat (ccss_background_image_t const	*bg_image,
	   cairo_t			*cr,
	   int32_t			 width,
	   int32_t			 height)
{
	cairo_pattern_set_extend (bg_image->image.pattern, CAIRO_EXTEND_NONE);
	cairo_set_source (cr, bg_image->image.pattern);
	cairo_fill_preserve (cr);
}

/**
 * ccss_background_fill:
 *
 * This function requires that a closed path exists, which is filled with 
 * the background. The path is not modified.
 **/
void
ccss_background_fill (ccss_background_attachment_t const	*bg_attachment, 
		     ccss_color_t const			*bg_color,
		     ccss_background_image_t const	*bg_image,
		     ccss_background_position_t const	*bg_position,
		     ccss_background_repeat_t const	*bg_repeat,
		     ccss_background_size_t const	*bg_size,
		     cairo_t				*cr,
		     int32_t				 x,
		     int32_t				 y, 
		     int32_t				 width,
		     int32_t				 height)
{
	cairo_status_t	status;
	double		dx;
	double		dy;

	g_return_if_fail (bg_attachment);
	/* FIXME, we need "transparent" color g_return_if_fail (bg_color); */
	g_return_if_fail (bg_image);
	g_return_if_fail (bg_position);
	g_return_if_fail (bg_repeat);
	g_return_if_fail (bg_size);

	cairo_save (cr);

	if (bg_color && ccss_property_is_set (bg_color->spec)) {

		cairo_set_source_rgb (cr, bg_color->red, 
					  bg_color->green, 
					  bg_color->blue);
		cairo_fill_preserve (cr);
	}

	if (ccss_property_is_set (bg_image->spec)) {

		double tile_width;
		double tile_height;
		double xoff;
		double yoff;

		tile_width = ccss_position_get_hsize (&bg_size->width, 
						     width, height,
						     bg_image->image.width,
						     bg_image->image.height);
		tile_height = ccss_position_get_vsize (&bg_size->height,
						      width, height,
						      bg_image->image.width,
						      bg_image->image.height);

		xoff = ccss_position_get_pos (&bg_position->hpos,
					     width, tile_width);
		yoff = ccss_position_get_pos (&bg_position->vpos,
					     height, tile_height);

		dx = tile_width / bg_image->image.width;
		dy = tile_height / bg_image->image.height;

		switch (bg_repeat->repeat) {
		case CCSS_BACKGROUND_REPEAT:
			/* Normalise offsets, we don't create an infinite
			 * background pattern. */
			xoff = fmod (xoff, tile_width) - tile_width;
			yoff = fmod (yoff, tile_height) - tile_height;
			/* FIXME: not rounding can make the background edges
			 * blurry, but when rounding we might be 1px off.
			cairo_translate (cr, x + lround (xoff), y + lround (yoff)); */
			cairo_translate (cr, x + xoff, y + yoff);
			cairo_scale (cr, dx, dy);
			repeat (bg_image, cr, width / dx, height / dy,
				tile_width / dx, tile_height / dy);
			break;
		case CCSS_BACKGROUND_REPEAT_X:
			/* Normalise vertical offset, we don't create an infinite
			 * background pattern. */
			xoff = fmod (xoff, tile_width) - tile_width;
			/* FIXME: not rounding can make the background edges
			 * blurry, but when rounding we might be 1px off.
			cairo_translate (cr, x + lround (xoff), y + lround (yoff)); */
			cairo_translate (cr, x + xoff, y + yoff);
			cairo_scale (cr, dx, dy);
			repeat_x (bg_image, cr, width / dx, height / dy,
				  tile_width / dx, tile_height / dy);
			break;
		case CCSS_BACKGROUND_REPEAT_Y:
			/* Normalise horizontal offset, we don't create an infinite
			 * background pattern. */
			yoff = fmod (yoff, tile_height) - tile_height;
			/* FIXME: not rounding can make the background edges
			 * blurry, but when rounding we might be 1px off.
			cairo_translate (cr, x + lround (xoff), y + lround (yoff)); */
			cairo_translate (cr, x + xoff, y + yoff);
			cairo_scale (cr, dx, dy);
			repeat_y (bg_image, cr, width / dx, height / dy,
				  tile_width / dx, tile_height / dy);
			break;
		case CCSS_BACKGROUND_NO_REPEAT:
			/* FIXME: not rounding can make the background edges
			 * blurry, but when rounding we might be 1px off.
			cairo_translate (cr, x + lround (xoff), y + lround (yoff)); */
			cairo_translate (cr, x + xoff, y + yoff);
			cairo_scale (cr, dx, dy);
			no_repeat (bg_image, cr, width / dx, height / dy);
			break;
		default:
			g_assert_not_reached ();
			/* Need some code here when building w/o assertions. */
			break;
		}

		status = cairo_status (cr);
		if (status != CAIRO_STATUS_SUCCESS) {
			g_warning (cairo_status_to_string (status));
		}
	}

	cairo_restore (cr);
}

#ifdef CCSS_DEBUG

void
ccss_background_dump (ccss_background_t const *self)
{
	printf (CCSS_PROPERTY_DUMP_PREFIX "background-color: ");
	ccss_color_dump (&self->bg_color);
	printf (";\n");

	printf (CCSS_PROPERTY_DUMP_PREFIX "background-image: ");
	ccss_background_image_dump (&self->bg_image);
	printf (";\n");
}

// TODO dump functions

void
ccss_background_image_dump (ccss_background_image_t const *self)
{
	switch (self->spec) {
	case CCSS_PROPERTY_SPEC_UNSET:
	case CCSS_PROPERTY_SPEC_NONE:
		ccss_property_spec_dump (self->spec);
		break;
	case CCSS_PROPERTY_SPEC_INHERIT:
		ccss_property_spec_dump (self->spec);
		/* fall thru */
	case CCSS_PROPERTY_SPEC_SET:
		ccss_image_dump (&self->image);
		break;
	}
}

#endif /* CCSS_DEBUG */

