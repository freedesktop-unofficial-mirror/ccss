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

/**
 * SECTION:color-impl
 * @short_description: internal functionality associated to ccss_color_t.
 * @see_also: #ccss_color_t
 * @stability: Unstable
 * @include: ccss/ccss-color-impl.h
 *
 * Declarations for the implementation of custom colours.
 * This declarations are not to be considered part of the stable
 * ccss interface. Use with care.
 **/

#ifndef CCSS_COLOR_IMPL_H
#define CCSS_COLOR_IMPL_H

/* Allow standalone inclusion. */
#define CCSS_STANDALONE_HEADER

#include <stdbool.h>
#include <libcroco/libcroco.h>
#include <ccss/ccss-block.h>
#include <ccss/ccss-color.h>
#include <ccss/ccss-grammar.h>
#include <ccss/ccss-macros.h>

CCSS_BEGIN_DECLS

bool
ccss_color_parse (ccss_color_t			 *self,
		  ccss_grammar_t const		 *grammar,
		  void				 *user_data,
		  CRTerm const			**value);

bool
ccss_color_factory (ccss_grammar_t const	*grammar,
		    ccss_block_t		*self,
		    char const			*name,
		    CRTerm const		*values,
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

#endif /* CCSS_COLOR_IMPL_H */

