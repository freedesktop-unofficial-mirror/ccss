/* vim: set ts=8 sw=8 noexpandtab: */

/* The `C' CSS Library.
 * Copyright (C) 2008 Robert Staudinger
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

#ifndef CCSS_BORDER_H
#define CCSS_BORDER_H

#include <ccss/ccss-color.h>
#include <ccss/ccss-macros.h>

CCSS_BEGIN_DECLS

/*
 * Border style.
 */

typedef enum {
	CCSS_BORDER_STYLE_HIDDEN,
	CCSS_BORDER_STYLE_DOTTED,
	CCSS_BORDER_STYLE_DASHED,
	CCSS_BORDER_STYLE_SOLID,
	CCSS_BORDER_STYLE_DOUBLE,
	CCSS_BORDER_STYLE_GROOVE,
	CCSS_BORDER_STYLE_RIDGE,
	CCSS_BORDER_STYLE_INSET,
	CCSS_BORDER_STYLE_OUTSET
} ccss_border_style_type_t;

typedef struct ccss_border_style_ ccss_border_style_t;

ccss_border_style_type_t
ccss_border_style_get_style (ccss_border_style_t const *self);

/*
 * Border width.
 */

typedef struct ccss_border_width_ ccss_border_width_t;

double
ccss_border_width_get_width (ccss_border_width_t const *self);

/*
 * Border spacing.
 * FIXME: introduce generic "double" value struct.
 */

typedef struct ccss_border_width_ ccss_border_spacing_t;

double
ccss_border_spacing_get_spacing (ccss_border_spacing_t const *self);

/*
 * Border join.
 * FIXME: rename to `border-radius'?
 */

typedef struct ccss_border_join_ ccss_border_join_t;

double
ccss_border_join_get_radius (ccss_border_join_t const *self);

CCSS_END_DECLS

#endif /* CCSS_BORDER_H */

