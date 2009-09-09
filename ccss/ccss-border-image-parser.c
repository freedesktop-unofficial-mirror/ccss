/* vim: set ts=8 sw=8 noexpandtab: */

/* The `C' CSS Library.
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
#include "ccss-border-image-priv.h"
#include "ccss-border-image-parser.h"
#include "ccss-function-impl.h"
#include "ccss-grammar.h"
#include "ccss-position-parser.h"
#include "ccss-property-impl.h"
#include "config.h"

static ccss_property_class_t const *
peek_property_class (void);

static bool
parse_tiling (CRTerm const			**value,
	      ccss_border_image_tiling_t	 *tiling)
{
	g_return_val_if_fail (tiling && *value, false);

	if (TERM_IDENT == (*value)->type) {
		char const *ident;
		ident = cr_string_peek_raw_str ((*value)->content.str);
		if (0 == g_strcmp0 ("repeat", ident)) {
			*tiling = CCSS_BORDER_IMAGE_TILING_REPEAT;
			*value = (*value)->next;
			return true;
		} else if (0 == g_strcmp0 ("round", ident)) {
			*tiling = CCSS_BORDER_IMAGE_TILING_ROUND;
			*value = (*value)->next;
			return true;
		} else if (0 == g_strcmp0 ("stretch", ident)) {
			*tiling = CCSS_BORDER_IMAGE_TILING_STRETCH;
			*value = (*value)->next;
			return true;
		}
	}

	return false;
}

/* FIXME: consolidate with background-image parsing code. */
static bool
border_image_parse (ccss_border_image_t		 *image,
		    ccss_grammar_t const	 *grammar,
		    void			 *user_data,
		    CRTerm const		**values)
{
	if (!*values) {
		return false;
	}

	switch ((*values)->type) {
	case TERM_IDENT:
		image->base.state = ccss_property_parse_state (values);
		break;
	case TERM_URI:
		/* FIXME: use raw uri if no url hook set. */
		image->uri = ccss_grammar_invoke_function (grammar, "url",
							  *values, user_data);
		if (image->uri) {
			image->base.state = CCSS_PROPERTY_STATE_SET;
			*values = (*values)->next;
		}
		break;
	default:
		image->base.state = CCSS_PROPERTY_STATE_INVALID;
	}

	return image->base.state != CCSS_PROPERTY_STATE_INVALID;
}

/*
 * FIXME: create all the border-image tiles here and save some time while painting.
 */
static ccss_property_t *
border_image_create (ccss_grammar_t const	*grammar,
		     CRTerm const		*values,
		     void			*user_data)
{
	ccss_border_image_t	*border_image;
	ccss_border_image_t	 bimg;
	CRTerm const		*iter;
	ccss_position_t		*borders[4];
	unsigned int		 n_borders;
	bool			 ret;

	memset (&bimg, 0, sizeof (bimg));
	bimg.base.vtable = peek_property_class ();

	iter = values;

	/* Image */
	border_image_parse (&bimg, grammar, user_data, &iter);
	if (CCSS_PROPERTY_STATE_NONE == bimg.base.state ||
	    CCSS_PROPERTY_STATE_INHERIT == bimg.base.state) {
		border_image = g_new0 (ccss_border_image_t, 1);
		*border_image = bimg;
		return &border_image->base;
	}

	/* Border sizes. */
	borders[0] = &bimg.top;
	borders[1] = &bimg.right;
	borders[2] = &bimg.bottom;
	borders[3] = &bimg.left;
	n_borders = 0;
	for (unsigned int i = 0; i < G_N_ELEMENTS (borders); i++) {
		ret = ccss_position_parse (borders[i], 
					   CCSS_POSITION_MASK_NUMERIC,
					   &iter);
		if (ret)
			n_borders++;
		else
			break;
	}

	switch (n_borders) {
	case 0:
		/* No valid size. */
		return NULL;
	case 1:
		/* One valid size. */
		*borders[1] = *borders[0];
		/* Fall thru. */
	case 2:
		/* Two valid sizes. */
		*borders[2] = *borders[0];
		/* Fall thru. */
	case 3:
		/* Three valid sizes. */
		*borders[3] = *borders[1];
	}

	/* Tiling. */
	bimg.top_middle_bottom_horizontal_tiling = CCSS_BORDER_IMAGE_TILING_STRETCH;
	bimg.left_middle_right_vertical_tiling = CCSS_BORDER_IMAGE_TILING_STRETCH;

	if (iter) {
		/* vertical */
		ret = parse_tiling (&iter, &bimg.top_middle_bottom_horizontal_tiling);
		if (!ret)
			return NULL;
	}

	if (!iter) {
		/* Only one tiling param, so both are equal. */
		bimg.left_middle_right_vertical_tiling = bimg.top_middle_bottom_horizontal_tiling;
	}
		

	if (iter) {
		/* horizontal */
		ret = parse_tiling (&iter, &bimg.left_middle_right_vertical_tiling);
		if (!ret)
			return NULL;
	}

	/* Valid border-image. */
	border_image = g_new0 (ccss_border_image_t, 1);
	*border_image = bimg;

	return &border_image->base;
}

static void
border_image_destroy (ccss_border_image_t *self)
{
	g_free (self->uri);
	g_free (self);
}

static bool
border_image_convert (ccss_border_image_t const	*property,
		      ccss_property_type_t	 target,
		      void			*value)
{
	char *top, *right, *bottom, *left;
	char const *horizontal_tiling, *vertical_tiling;

	g_return_val_if_fail (property && value, false);

	if (CCSS_PROPERTY_TYPE_DOUBLE == target)
		return false;

	if (property->uri == NULL) {
		* (char **) value = g_strdup ("none");
		return true;
	}

	top = ccss_position_serialize (&property->top);
	right = ccss_position_serialize (&property->right);
	bottom = ccss_position_serialize (&property->bottom);
	left = ccss_position_serialize (&property->left);

	horizontal_tiling = NULL;
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

	vertical_tiling = NULL;
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
				property->uri,
				top, right, bottom, left,
				horizontal_tiling, vertical_tiling);

	g_free (top), top = NULL;
	g_free (right), right = NULL;
	g_free (bottom), bottom = NULL;
	g_free (left), left = NULL;

	return true;
}

static ccss_property_class_t const _ptable[] = {
    {
	.name = "border-image",
	.create = border_image_create,
	.destroy = (ccss_property_destroy_f) border_image_destroy,
	.convert = (ccss_property_convert_f) border_image_convert,
	.factory = NULL,
	.inherit = NULL,
	.serialize = NULL
    }, {
	.name = NULL
    }
};

static ccss_property_class_t const *
peek_property_class (void)
{
	return &_ptable[0];
}

ccss_property_class_t const *
ccss_border_image_parser_get_property_classes (void)
{
	return _ptable;
}

