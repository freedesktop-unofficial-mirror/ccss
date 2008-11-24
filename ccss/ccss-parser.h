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

#ifndef CCSS_PARSER_H
#define CCSS_PARSER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <glib.h>
#include <libcroco/libcroco.h>
#include <ccss/ccss-macros.h>
#include <ccss/ccss-selector-group.h>
#include <ccss/ccss-stylesheet.h>

CCSS_BEGIN_DECLS

void
ccss_parser_subsystem_add_properties (ccss_property_class_t const *properties);

void
ccss_parser_subsystem_shutdown (void);

enum CRStatus
ccss_parser_parse_file (char const			*css_file, 
			ccss_stylesheet_precedence_t	 precedence,
			GHashTable			*groups,
			GHashTable			*blocks);

enum CRStatus
ccss_parser_parse_buffer (char const			*buffer,
			  size_t			 buffer_size,
			  ccss_stylesheet_precedence_t	 precedence,
			  GHashTable			*groups,
			  GHashTable			*blocks);

enum CRStatus
ccss_parser_parse_inline (char const			*buffer,
			  ccss_stylesheet_precedence_t	 precedence,
			  ptrdiff_t			 instance,
			  ccss_selector_group_t		*result_group,
			  GHashTable			*blocks);

CCSS_END_DECLS

#endif /* CCSS_PARSER_H */

