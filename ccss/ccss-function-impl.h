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
 * SECTION:function-impl
 * @short_description: internal functionality associated to ccss_function_t.
 * @see_also: #ccss_function_t
 * @stability: Unstable
 * @include: ccss/ccss-function-impl.h
 *
 * Declarations for the implementation of custom functions.
 * This declarations are not to be considered part of the stable
 * ccss interface. Use with care.
 **/

#ifndef CCSS_FUNCTION_IMPL_H
#define CCSS_FUNCTION_IMPL_H

#include <libcroco/libcroco.h>
#include <ccss/ccss-function.h>
#include <ccss/ccss-macros.h>

CCSS_BEGIN_DECLS

/* Avoid circular dependencies. */
struct ccss_grammar_;

/**
 * ccss_function_t:
 * @name:		identifier of the function, as used in CSS.
 * @function:		handler, see #ccss_function_f.
 * @reference_count:	the reference count.
 *
 * This datastructure represents one line in the libccss' consumers vtable.
 **/
struct ccss_function_ {
	/*< private >*/
	CCSS_DEPRECATED (char const		*name);
	CCSS_DEPRECATED (ccss_function_f	 function);
	CCSS_DEPRECATED (unsigned int		 reference_count);
};

void
ccss_grammar_add_functions	(struct ccss_grammar_		*self,
				 ccss_function_t		*functions);

char *
ccss_grammar_invoke_function	(struct ccss_grammar_ const	 *self,
				 char const			 *function_name,
				 CRTerm const			 *values,
				 void				 *user_data);

CCSS_END_DECLS

#endif /* CCSS_FUNCTION_IMPL_H */

