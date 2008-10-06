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
#include "ccss-property.h"

ccss_property_spec_t
ccss_property_parse_spec (CRTerm const **value)
{
	char const *str;

	if (!*value) {
		return CCSS_PROPERTY_SPEC_UNSET;
	} else if (TERM_IDENT == (*value)->type) {
		str = cr_string_peek_raw_str ((*value)->content.str);
		if (0 == strcmp ("none", str)) {
			*value = (*value)->next;
			return CCSS_PROPERTY_SPEC_NONE;
		} else if (0 == strcmp ("inherit", str)) {
			g_warning ("Property `inherit' not supported yet.");
			*value = (*value)->next;
			return CCSS_PROPERTY_SPEC_INHERIT;
		}
	}

	return CCSS_PROPERTY_SPEC_SET;
}

bool
ccss_property_is_set (ccss_property_spec_t property)
{
	return CCSS_PROPERTY_SPEC_SET == property || 
	       CCSS_PROPERTY_SPEC_INHERIT == property;
}


#ifdef CCSS_DEBUG

void
ccss_property_spec_dump (ccss_property_spec_t const spec)
{
	switch (spec) {
	case CCSS_PROPERTY_SPEC_UNSET:
		printf ("<unset>");
		break;
	case CCSS_PROPERTY_SPEC_NONE:
		printf ("none");
		break;
	case CCSS_PROPERTY_SPEC_INHERIT:
		printf ("<inherit>");
		break;
	case CCSS_PROPERTY_SPEC_SET:
		/* Needs custom printing. */
		break;
	}
}

#endif /* CCSS_DEBUG */

