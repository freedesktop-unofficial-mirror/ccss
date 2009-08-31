/* vim: set ts=8 sw=8 noexpandtab: */

/* The `C' CSS Library.
 * Copyright (C) 2009 Intel Corporation.
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

#ifndef CCSS_PADDING_H
#define CCSS_PADDING_H

#include <ccss/ccss-macros.h>
#include <ccss/ccss-property.h>

CCSS_BEGIN_DECLS

typedef struct {
	/*< private >*/
	CCSS_DEPRECATED (ccss_property_t	base);

	CCSS_DEPRECATED (double			padding);
} ccss_padding_t;

double
ccss_padding_get_padding (ccss_padding_t const *self);

CCSS_END_DECLS

#endif /* CCSS_PADDING_H */

