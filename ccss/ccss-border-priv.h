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

#ifndef CCSS_BORDER_PRIV_H
#define CCSS_BORDER_PRIV_H

#include <ccss/ccss-macros.h>
#include <ccss/ccss-border.h>
#include <ccss/ccss-property-impl.h>

CCSS_BEGIN_DECLS

/*
 * Border style.
 */

struct ccss_border_style_ {
	/*< private >*/
	CCSS_DEPRECATED (ccss_property_t		base);

	CCSS_DEPRECATED (ccss_border_style_type_t	style);
};

/*
 * Border width.
 */

struct ccss_border_width_ {
	/*< private >*/
	CCSS_DEPRECATED (ccss_property_t	base);

	CCSS_DEPRECATED (double			width);
};

/*
 * Border join.
 */

struct ccss_border_join_ {
	/*< private >*/
	CCSS_DEPRECATED (ccss_property_t	base);

	CCSS_DEPRECATED (double			radius);
};

CCSS_END_DECLS

#endif /* CCSS_BORDER_PRIV_H */

