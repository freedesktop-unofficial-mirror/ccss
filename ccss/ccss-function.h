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

#ifndef CCSS_FUNCTION_H
#define CCSS_FUNCTION_H

#include <ccss/ccss-block.h>
#include <ccss/ccss-macros.h>

CCSS_BEGIN_DECLS

/* Let's just forward declare this, so we don't have to pull in <glib.h>. */
struct _GSList;

/**
 * ccss_function_f:
 * @block:		the CSS block context of the invoked function.
 * @property_name:	property that the function is associated with, e.g. `background-image'.
 * @function_name:	name of the function, e.g. `url'.
 * @args:		argument-list passed to the function.
 * 
 * Prototype for a custom `CSS function' handler.
 *
 * Returns: the function's result as a string.
 **/
typedef char * (*ccss_function_f) (struct _GSList const	*args);

/**
 * ccss_function_t:
 * @name:	identifier of the function, as used in CSS.
 * @function:	handler, see #ccss_function_f.
 *
 * This datastructure represents one line in the libccss' consumers vtable.
 **/
typedef struct {
	char const	*name;
	ccss_function_f	 function;
} ccss_function_t;

// TODO document
char *
ccss_function_invoke (char const	*function_name,
		      CRTerm const	*args);

CCSS_END_DECLS

#endif /* CCSS_FUNCTION_H */

