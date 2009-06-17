/* vim: set ts=8 sw=8 noexpandtab: */

/* The Gtk+ CSS Drawing Library.
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

#ifndef CCSS_GTK_FUNCTIONS_H
#define CCSS_GTK_FUNCTIONS_H

#ifndef CCSS_GTK_H
  #ifndef CCSS_GTK_BUILD
    #error "Only <ccss-gtk/ccss-gtk.h> can be included directly."
  #endif
#endif

#include <ccss/ccss.h>

CCSS_BEGIN_DECLS

ccss_function_t const * ccss_gtk_functions_get_vtable (void);

CCSS_END_DECLS

#endif /* CCSS_GTK_FUNCTIONS_H */

