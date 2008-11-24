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
#include "ccss-block-priv.h"
#include "ccss-border-image.h"
#include "ccss-border-image-parser.h"
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

/*
 * FIXME: create all the border-image tiles here and save some time while painting.
 */
static ccss_border_image_t *
border_image_new (CRTerm const	*values)
{
	ccss_border_image_t	*border_image;
	ccss_border_image_t	 bimg;
	CRTerm const		*iter;
	ccss_position_t		*borders[4];
	unsigned int		 n_borders;
	bool			 ret;

	memset (&bimg, 0, sizeof (bimg));
	bimg.base.property_class = peek_property_class ();

	iter = values;

	/* Image */
	bimg.base.state = ccss_image_parse (&bimg.image, &iter);
	if (CCSS_PROPERTY_STATE_NONE == bimg.base.state) {
		border_image = g_new0 (ccss_border_image_t, 1);
		*border_image = bimg;
		return border_image;
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
	return border_image;
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

static ccss_property_class_t const _ptable[] = {
    {
	.name = "border-image",
	.property_new = (ccss_property_new_f) border_image_new,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = (ccss_property_convert_f) border_image_convert,
	.property_factory = NULL
    }, {
	.name = NULL,
	.property_new = NULL,
	.property_free = NULL,
	.property_convert = NULL,
	.property_factory = NULL
    }
};

static ccss_property_class_t const *
peek_property_class (void)
{
	return &_ptable[0];
}

ccss_property_class_t const *
ccss_border_image_get_ptable (void)
{
	return _ptable;
}

