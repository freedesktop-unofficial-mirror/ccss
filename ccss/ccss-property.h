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

#ifndef CCSS_PROPERTY_H
#define CCSS_PROPERTY_H

#include <stdbool.h>
#include <libcroco/libcroco.h>
#include <ccss/ccss-features.h>
#include <ccss/ccss-macros.h>

CCSS_BEGIN_DECLS

typedef enum {
	CCSS_PROPERTY_SPEC_UNSET = 0,
	CCSS_PROPERTY_SPEC_NONE,
	CCSS_PROPERTY_SPEC_INHERIT,
	CCSS_PROPERTY_SPEC_SET
} ccss_property_spec_t;

typedef enum {
	CCSS_PROPERTY_TYPE_DOUBLE,
	CCSS_PROPERTY_TYPE_STRING
} ccss_property_type_t;

typedef struct {
	ccss_property_spec_t	spec;
	ccss_property_type_t	type;
	union {
		double	 dval;
		char	*sval;
	}			content;
} ccss_property_t;

typedef bool (*ccss_property_convert_f)	(void const		*property,
					 ccss_property_type_t	 target,
					 void			*value);

void
ccss_property_subsystem_init		(void);

void
ccss_property_subsystem_shutdown	(void);


void
ccss_property_register_conversion_function	(GQuark				property,
						 ccss_property_convert_f	function);

bool
ccss_property_convert				(void const			*property,
						 GQuark				 property_id,
						 ccss_property_type_t		 target,
						 void				*value);

ccss_property_spec_t
ccss_property_parse_spec			(CRTerm const			**value);

CCSS_END_DECLS

#endif /* CCSS_PROPERTY_H */

