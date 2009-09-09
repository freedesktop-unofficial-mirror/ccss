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

#ifndef CCSS_COLOR_H
#define CCSS_COLOR_H

#include <ccss/ccss-macros.h>

CCSS_BEGIN_DECLS

typedef struct ccss_color_ ccss_color_t;

double
ccss_color_get_red	(ccss_color_t const *self);

double
ccss_color_get_green	(ccss_color_t const *self);

double
ccss_color_get_blue	(ccss_color_t const *self);

double
ccss_color_get_alpha	(ccss_color_t const *self);

CCSS_END_DECLS

#endif /* CCSS_COLOR_H */

