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

#ifndef CCSS_PROPERTY_H
#define CCSS_PROPERTY_H

#include <stdbool.h>
#include <ccss/ccss-macros.h>

CCSS_BEGIN_DECLS

/**
 * ccss_property_state_t:
 * @CCSS_PROPERTY_STATE_INVALID:	error state, invalid property.
 * @CCSS_PROPERTY_STATE_NONE:		property set to `none', switched off.
 * @CCSS_PROPERTY_STATE_INHERIT:	inherit property from container.
 * @CCSS_PROPERTY_STATE_SET:		property is valid and set.
 * @CCSS_PROPERTY_STATE_ERROR_OVERFLOW: error state, used for bounds checking.
 *
 * This enum must be embedded as first field in every property implementation.
 *
 * <!-- PONDERING: turn into flags and add CCSS_PROPERTY_STATE_UNRESOLVED. -->
 **/
typedef enum {
	CCSS_PROPERTY_STATE_INVALID = 0,
	CCSS_PROPERTY_STATE_NONE,
	CCSS_PROPERTY_STATE_INHERIT,
	CCSS_PROPERTY_STATE_SET,
	CCSS_PROPERTY_STATE_ERROR_OVERFLOW
} ccss_property_state_t;

const char *
ccss_property_state_serialize (ccss_property_state_t self);

/**
 * ccss_property_type_t:
 * @CCSS_PROPERTY_TYPE_DOUBLE:	property represented by a floating point number.
 * @CCSS_PROPERTY_TYPE_STRING:	property represented by a string.
 *
 * Type descriptions for generic properties.
 **/
typedef enum {
	CCSS_PROPERTY_TYPE_DOUBLE,
	CCSS_PROPERTY_TYPE_STRING
} ccss_property_type_t;

typedef struct ccss_property_		ccss_property_t;
typedef struct ccss_property_class_     ccss_property_class_t;

ccss_property_state_t
ccss_property_get_state (ccss_property_t const *self);

CCSS_END_DECLS

#endif /* CCSS_PROPERTY_H */

