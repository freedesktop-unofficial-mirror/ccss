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

#ifndef CCSS_COLOR_H
#define CCSS_COLOR_H

#include <stdbool.h>
#include <libcroco/libcroco.h>
#include <ccss/ccss.h>

CCSS_BEGIN_DECLS

#define CCSS_COLOR_ASSIGN(lhs_, rhs_) {		\
	(lhs_).state = (rhs_).state;		\
	(lhs_).red = (rhs_).red;		\
	(lhs_).green = (rhs_).green;		\
	(lhs_).blue = (rhs_).blue;		\
}

typedef struct {
	ccss_property_base_t	base;

	double			red;
	double			green;
	double			blue;
} ccss_color_t;

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

bool
ccss_color_parse (ccss_color_t		 *self,
		  ccss_grammar_t const	 *grammar,
		  void			 *user_data,
		  CRTerm const		**values);

ccss_property_class_t const *
ccss_color_get_ptable (void);

CCSS_END_DECLS

#endif /* CCSS_COLOR_H */

