/* vim: set ts=8 sw=8 noexpandtab: */

/* The `C' CSS Library.
 * Copyright (C) 2008 Robert Staudinger
 * Copyright (C) 2009 Intel Corporation
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

#ifndef CCSS_PROPERTY_GENERIC_H
#define CCSS_PROPERTY_GENERIC_H

#include <libcroco/libcroco.h>
#include <ccss/ccss-macros.h>
#include <ccss/ccss-property.h>

CCSS_BEGIN_DECLS

/**
 * ccss_property_generic_t:
 * @base:	base property.
 * @name:	name of the property, e.g. %color.
 * @values:	linked list of values.
 *
 * Implementation of a generic, single-value property.
 **/
typedef struct {
	ccss_property_t		 base;

	char			*name;
	CRTerm			*values;
} ccss_property_generic_t;

CCSS_END_DECLS

#endif /* CCSS_PROPERTY_GENERIC_H */

