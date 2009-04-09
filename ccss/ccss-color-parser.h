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

#ifndef CCSS_COLOR_PARSER_H
#define CCSS_COLOR_PARSER_H

#include <stdbool.h>
#include <libcroco/libcroco.h>
#include <ccss/ccss-color.h>
#include <ccss/ccss-grammar.h>
#include <ccss/ccss-macros.h>

CCSS_BEGIN_DECLS

ccss_property_base_t *
ccss_color_create (ccss_grammar_t const	*grammar,
		   CRTerm const		*value,
		   void			*user_data);

void
ccss_color_destroy (ccss_color_t *self);

bool
ccss_color_convert (ccss_color_t const		*property,
		    ccss_property_type_t	 target,
		    void			*value);

ccss_property_class_t const *
ccss_color_parser_get_property_classes (void);

CCSS_END_DECLS

#endif /* CCSS_COLOR_PARSER_H */

