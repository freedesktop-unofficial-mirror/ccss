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

#ifndef CCSS_GRAMMAR_PRIV_H
#define CCSS_GRAMMAR_PRIV_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <glib.h>
#include <libcroco/libcroco.h>
#include <ccss/ccss-grammar.h>
#include <ccss/ccss-macros.h>
#include <ccss/ccss-selector-group.h>
#include <ccss/ccss-stylesheet.h>

CCSS_BEGIN_DECLS

/**
 * ccss_grammar_t:
 *
 * Represents a context for the creation of stylesheets of similar kind.
 **/
struct ccss_grammar_ {
	/*< private >*/
	unsigned int	 reference_count;
	GHashTable	*properties;
	GHashTable	*functions;
};

enum CRStatus
ccss_grammar_parse_file (ccss_grammar_t const		*self,
			 char const			*css_file, 
			 ccss_stylesheet_precedence_t	 precedence,
			 unsigned int			 stylesheet_descriptor,
			 void				*user_data,
			 GHashTable			*groups,
			 GHashTable			*blocks);

enum CRStatus
ccss_grammar_parse_buffer (ccss_grammar_t const		*self,
			   char const			*buffer,
			   size_t			 buffer_size,
			   ccss_stylesheet_precedence_t	 precedence,
			   unsigned int			 stylesheet_descriptor,
			   void				*user_data,
			   GHashTable			*groups,
			   GHashTable			*blocks);

enum CRStatus
ccss_grammar_parse_inline (ccss_grammar_t const		*self,
			   char const			*buffer,
			   ccss_stylesheet_precedence_t	 precedence,
			   unsigned int			 stylesheet_descriptor,
			   ptrdiff_t			 instance,
			   void				*user_data,
			   ccss_selector_group_t	*result_group,
			   GHashTable			*blocks);

CCSS_END_DECLS

#endif /* CCSS_GRAMMAR_PRIV_H */

