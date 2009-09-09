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

#ifndef CCSS_BACKGROUND_PRIV_H
#define CCSS_BACKGROUND_PRIV_H

#include <ccss/ccss-macros.h>
#include <ccss/ccss-background.h>
#include <ccss/ccss-color-impl.h>
#include <ccss/ccss-position-priv.h>
#include <ccss/ccss-property-impl.h>

CCSS_BEGIN_DECLS

/*
 * Background attachment.
 */

struct ccss_background_attachment_ {
	/*< private >*/
	CCSS_DEPRECATED (ccss_property_t			base);

	CCSS_DEPRECATED (ccss_background_attachment_type_t	attachment);
};

/*
 * Background image.
 */

struct ccss_background_image_ {
	/*< private >*/
	CCSS_DEPRECATED (ccss_property_t	 base);

	CCSS_DEPRECATED (char			*uri);
};

/*
 * Background position.
 */

struct ccss_background_position_ {
	/*< private >*/
	CCSS_DEPRECATED (ccss_property_t	base);

	CCSS_DEPRECATED (ccss_position_t	hpos);
	CCSS_DEPRECATED (ccss_position_t	vpos);
};

/*
 * Background repeat.
 */

struct ccss_background_repeat_ {
	/*< private >*/
	CCSS_DEPRECATED (ccss_property_t		base);

	CCSS_DEPRECATED (ccss_background_repeat_type_t  repeat);
};

/*
 * Background size.
 */

struct ccss_background_size_ {
	/*< private >*/
	CCSS_DEPRECATED (ccss_property_t	base);

	CCSS_DEPRECATED (ccss_position_t	width);
	CCSS_DEPRECATED (ccss_position_t	height);
};

/*
 * Background.
 */

struct ccss_background_ {
	/*< private >*/
	CCSS_DEPRECATED (ccss_property_t		base);

	CCSS_DEPRECATED (ccss_background_attachment_t	bg_attachment);
	CCSS_DEPRECATED (ccss_color_t			bg_color);
	CCSS_DEPRECATED (ccss_background_image_t	bg_image);
	CCSS_DEPRECATED (ccss_background_position_t	bg_position);
	CCSS_DEPRECATED (ccss_background_repeat_t	bg_repeat);
	CCSS_DEPRECATED (ccss_background_size_t		bg_size);
};

CCSS_END_DECLS

#endif /* CCSS_BACKGROUND_PRIV_H */

