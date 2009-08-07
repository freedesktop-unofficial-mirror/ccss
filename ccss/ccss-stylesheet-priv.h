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

#ifndef CCSS_STYLESHEET_PRIV_H
#define CCSS_STYLESHEET_PRIV_H

#include <ccss/ccss-grammar.h>
#include <ccss/ccss-macros.h>
#include <ccss/ccss-stylesheet.h>

CCSS_BEGIN_DECLS

/**
 * ccss_stylesheet_t:
 * @reference_count:	reference count.
 * @grammar:		The grammar for this stylesheet.
 * @blocks:		List owning all blocks parsed from the stylesheet.
 * @groups:		Associates type names with all applying selectors.
 * @current_descriptor: descriptor of the recently loaded CSS file or buffer.
 *
 * Represents a parsed instance of a stylesheet.
 **/
struct ccss_stylesheet_ {
	/*< private >*/
	unsigned int	 reference_count;
	ccss_grammar_t	*grammar;
	GHashTable	*blocks;
	GHashTable	*groups;
	unsigned int     current_descriptor;
};

ccss_stylesheet_t *
ccss_stylesheet_create (void);

void
ccss_stylesheet_fix_dangling_selectors (ccss_stylesheet_t *self);

CCSS_END_DECLS

#endif /* CCSS_STYLESHEET_PRIV_H */

