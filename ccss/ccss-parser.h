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
#include <glib.h>
#include <ccss/ccss-features.h>

G_BEGIN_DECLS

GSList * ccss_parser_parse_buffer (char const *buffer, size_t size, 
				  GHashTable *type_rules, 
				  GHashTable *class_rules, GHashTable *id_rules);

GSList * ccss_parser_parse_file (char const *css_file, GHashTable *type_rules, 
				GHashTable *class_rules, GHashTable *id_rules);

G_END_DECLS

#endif /* CCSS_PARSER_H */

