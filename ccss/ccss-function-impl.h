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

/*
 * Declarations for the implementation of custom functions.
 * This declarations are not to be considered part of the stable
 * ccss interface. Use with care.
 *
 * FIXME: split out to a section of its own in the docs.
 */

#ifndef CCSS_FUNCTION_IMPL_H
#define CCSS_FUNCTION_IMPL_H

#include <ccss/ccss-function.h>
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

/**
 * ccss_function_t:
 * @name:	identifier of the function, as used in CSS.
 * @function:	handler, see #ccss_function_f.
 * @user_data:	data to pass to the function handler.
 *
 * This datastructure represents one line in the libccss' consumers vtable.
 **/
struct ccss_function_ {
	CCSS_DEPRECATED (char const		*name);
	CCSS_DEPRECATED (ccss_function_f	 function);
	CCSS_DEPRECATED (void			*user_data);
};

CCSS_END_DECLS

#endif /* CCSS_FUNCTION_IMPL_H */

