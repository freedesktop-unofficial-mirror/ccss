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
#include "ccss-border-image.h"
#include "ccss-border-image-parser.h"

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

bool
ccss_block_parse_border_image (ccss_block_t	*self,
			       char const	*property, 
			       CRTerm const	*values)
{
	ccss_border_image_t	*border_image;
	ccss_border_image_t	 bimg;
	CRTerm const		*iter;
	ccss_position_t		*borders[4];
	unsigned int		 n_borders;
	bool			 ret;

	g_return_val_if_fail (0 == g_strcmp0 ("border-image", property), false);

	memset (&bimg, 0, sizeof (bimg));
	iter = values;

	/* Image */
	bimg.spec = ccss_image_parse (&bimg.image, &iter);
	if (CCSS_PROPERTY_SPEC_NONE == bimg.spec) {
		border_image = ccss_block_new_border_image (self);
		*border_image = bimg;
		return true;
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
		return false;
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
	bimg.vertical_tiling = CCSS_BORDER_IMAGE_TILING_STRETCH;
	bimg.horizontal_tiling = CCSS_BORDER_IMAGE_TILING_STRETCH;

	if (values) {
		/* vertical */
		ret = parse_tiling (&values, &bimg.vertical_tiling);
		if (!ret)
			return false;
	}

	if (!values) {
		/* Only one tiling param, so both are equal. */
		bimg.horizontal_tiling = bimg.vertical_tiling;
	}
		

	if (values) {
		/* horizontal */
		ret = parse_tiling (&values, &bimg.horizontal_tiling);
		if (!ret)
			return false;
	}

	/* Valid border-image. */
	border_image = ccss_block_new_border_image (self);
	*border_image = bimg;
	return true;
}

