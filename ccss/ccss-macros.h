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

#ifndef CCSS_MACROS_H
#define CCSS_MACROS_H

/* Every header needs this one, so force single includes here. */
#if !defined(CCSS_H) && !defined(CCSS_BUILD)
  #ifdef CCSS_STANDALONE_HEADER
    #error "Toplevel header <ccss/ccss.h> needs to be included before standalone header."
  #else
    #error "Only <ccss/ccss.h> can be included directly."
  #endif
#endif

#ifdef  __cplusplus
  #define CCSS_BEGIN_DECLS  extern "C" {
  #define CCSS_END_DECLS    }
#else
  #define CCSS_BEGIN_DECLS
  #define CCSS_END_DECLS
#endif

/* Internally we'd like to use the struct fields directly. */
#ifndef CCSS_DEPRECATED
  #define CCSS_DEPRECATED(sym) sym
#endif

#endif /* CCSS_MACROS_H */

