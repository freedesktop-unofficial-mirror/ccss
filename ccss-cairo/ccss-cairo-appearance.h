/* vim: set ts=8 sw=8 noexpandtab: */

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

#ifndef CCSS_CAIRO_APPEARANCE_H
#define CCSS_CAIRO_APPEARANCE_H

#ifndef CCSS_CAIRO_H
  #ifndef CCSS_CAIRO_BUILD
    #error "Only <ccss-cairo/ccss-cairo.h> can be included directly."
  #endif
#endif

#include <stdbool.h>
#include <cairo.h>
#include <ccss/ccss.h>
#include <ccss-cairo/ccss-cairo-style.h>

CCSS_BEGIN_DECLS

struct _GModule;

typedef bool
(*ccss_cairo_appearance_draw_f) (ccss_style_t const	*self,
				 cairo_t		*cr,
				 int			 x,
				 int			 y,
				 int			 width,
				 int			 height);

typedef struct {
	unsigned int		 reference_count;
	char			*module_path;
	struct _GModule		*module;
} ccss_cairo_appearance_module_t;

typedef struct {
	ccss_property_t			 base;
	char				*appearance;
	ccss_cairo_appearance_module_t	*module;
	ccss_cairo_appearance_draw_f	 draw_function;
} ccss_cairo_appearance_t;

CCSS_END_DECLS

#endif /* CCSS_CAIRO_APPEARANCE_H */

