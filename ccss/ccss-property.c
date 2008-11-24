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
	self->state = CCSS_PROPERTY_STATE_UNSET;
}

// TODO document
ccss_property_state_t
ccss_property_parse_state (CRTerm const **value)
{
	char const *str;

	if (!*value) {
		return CCSS_PROPERTY_STATE_UNSET;
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

