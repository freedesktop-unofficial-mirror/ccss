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

#ifndef CCSS_FUNCTION_H
#define CCSS_FUNCTION_H

#include <ccss/ccss-macros.h>

CCSS_BEGIN_DECLS

/* Let's just forward declare this, so we don't have to pull in <glib.h>. */
struct _GSList;

/**
 * ccss_function_f:
 * @args:	argument-list passed to the function.
 * @user_data:	user data associated to the function handler.
 *
 * Prototype for a custom `CSS function' handler.
 *
 * Returns: the function's result as a string.
 **/
typedef char * (*ccss_function_f) (struct _GSList const	*args,
				   void			*user_data);

typedef struct ccss_function_ ccss_function_t;

ccss_function_t *
ccss_function_create			(char const		*name,
					 ccss_function_f	 function);

void
ccss_function_destroy			(ccss_function_t	*self);

ccss_function_t *
ccss_function_reference			(ccss_function_t	*self);

unsigned int
ccss_function_get_reference_count       (ccss_function_t const  *self);

CCSS_END_DECLS

#endif /* CCSS_FUNCTION_H */

