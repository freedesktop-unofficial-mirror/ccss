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

#include <glib.h>
#include <libcroco/libcroco.h>

G_BEGIN_DECLS

typedef char * (*ccss_function_f) (GSList const *args);

typedef struct {
	char const	*name;
	ccss_function_f	 function;
} ccss_function_t;

void ccss_function_set_vtable (ccss_function_t const *vtable);

char * ccss_function_invoke (char const *name, CRTerm const *args);

G_END_DECLS

#endif /* CCSS_FUNCTION_H */

