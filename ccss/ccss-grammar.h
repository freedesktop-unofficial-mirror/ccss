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

#ifndef CCSS_GRAMMAR_H
#define CCSS_GRAMMAR_H

#include <ccss/ccss-function.h>
#include <ccss/ccss-macros.h>
#include <ccss/ccss-property.h>
#include <ccss/ccss-stylesheet.h>

CCSS_BEGIN_DECLS

typedef struct ccss_grammar_ ccss_grammar_t;

ccss_grammar_t *
ccss_grammar_create_generic (void);

ccss_grammar_t *
ccss_grammar_create_css (void);

void
ccss_grammar_destroy (ccss_grammar_t *self);

ccss_grammar_t *
ccss_grammar_reference (ccss_grammar_t *self);

unsigned int
ccss_grammar_get_reference_count (ccss_grammar_t const *self);

void
ccss_grammar_add_properties	(ccss_grammar_t			*self,
				 ccss_property_class_t const	*properties);

ccss_property_class_t const *
ccss_grammar_lookup_property	(ccss_grammar_t const		*self,
				 char const			*name);

void
ccss_grammar_add_function	(ccss_grammar_t			*self,
				 ccss_function_t		*function);

ccss_function_t const *
ccss_grammar_lookup_function	(ccss_grammar_t const		*self,
				 char const			*name);

ccss_stylesheet_t *
ccss_grammar_create_stylesheet			(ccss_grammar_t	*self);

ccss_stylesheet_t *
ccss_grammar_create_stylesheet_from_buffer	(ccss_grammar_t	*self,
						 char const	*buffer,
						 size_t		 size,
						 void		*user_data);
ccss_stylesheet_t *
ccss_grammar_create_stylesheet_from_file	(ccss_grammar_t	*self,
						 char const	*css_file,
						 void		*user_data);

CCSS_END_DECLS

#endif /* CCSS_GRAMMAR_H */

