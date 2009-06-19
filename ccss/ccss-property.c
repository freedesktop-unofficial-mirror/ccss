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

#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "ccss-property.h"
#include "config.h"

/**
 * ccss_property_init:
 * @self:		a #ccss_property_base_t.
 * @property_class:	a #ccss_property_class_t vtable.
 *
 * Initializes @self, needs to be called before the property is
 * registered with ccss.
 **/
void
ccss_property_init (ccss_property_base_t	*self,
		    ccss_property_class_t const	*property_class)
{
	g_assert (self && property_class);

	self->property_class = property_class;
	self->state = CCSS_PROPERTY_STATE_INVALID;
}

/**
 * ccss_property_parse_state:
 * @value: iterater of type #CRTerm.
 *
 * Parse a libcroco term to see whether it contains any of the predefined values
 * `none' or `inherit'.
 *
 * Returns: a #ccss_property_state_t.
 **/
ccss_property_state_t
ccss_property_parse_state (CRTerm const **value)
{
	char const *str;

	if (!*value) {
		return CCSS_PROPERTY_STATE_INVALID;
	} else if (TERM_IDENT == (*value)->type) {
		str = cr_string_peek_raw_str ((*value)->content.str);
		if (0 == strcmp ("none", str)) {
			*value = (*value)->next;
			return CCSS_PROPERTY_STATE_NONE;
		} else if (0 == strcmp ("inherit", str)) {
			*value = (*value)->next;
			return CCSS_PROPERTY_STATE_INHERIT;
		}
	}

	return CCSS_PROPERTY_STATE_SET;
}

static const struct {
	ccss_property_state_t    property_state;
	char const		*name;
} _property_state_map[] = {
  { CCSS_PROPERTY_STATE_INVALID,	"invalid" },
  { CCSS_PROPERTY_STATE_NONE,		"none" },
  { CCSS_PROPERTY_STATE_INHERIT,	"inherit" },
  { CCSS_PROPERTY_STATE_SET,		"set" },
  { CCSS_PROPERTY_STATE_ERROR_OVERFLOW, "<invalid>" }
};

/**
 * ccss_property_state_serialize:
 * @self:       a #ccss_property_state_t.
 *
 * Convert a property state into a readable string.
 *
 * Returns: textual representation of @self.
 **/
const char *
ccss_property_state_serialize (ccss_property_state_t self)
{
	for (unsigned int i = 0; i < CCSS_PROPERTY_STATE_ERROR_OVERFLOW; i++) {
		if (self == _property_state_map[i].property_state) {
			return _property_state_map[i].name;
		}
	}

	return NULL;
}
