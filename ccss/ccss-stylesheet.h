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

#ifndef CCSS_STYLESHEET_H
#define CCSS_STYLESHEET_H

#include <stdbool.h>
#include <glib.h>
#include <ccss/ccss-features.h>
#include <ccss/ccss-selector-group.h>
#include <ccss/ccss-style.h>

G_BEGIN_DECLS

/**
 * ccss_stylesheet_precedence_t:
 * @CCSS_STYLESHEET_USER_AGENT:	the application's intrinsic CSS rules.
 * @CCSS_STYLESHEET_USER:	CSS rules provided by the user.
 * @CCSS_STYLESHEET_AUTHOR:	CSS rules provided by the author.
 *
 * See <ulink url="http://www.w3.org/TR/CSS21/cascade.html">http://www.w3.org/TR/CSS21/cascade.html</ulink>.
 **/
typedef enum {
	CCSS_STYLESHEET_USER_AGENT = 0,
	CCSS_STYLESHEET_USER,
	CCSS_STYLESHEET_AUTHOR
} ccss_stylesheet_precedence_t;

typedef struct ccss_stylesheet_ ccss_stylesheet_t;

ccss_stylesheet_t *	ccss_stylesheet_new_from_buffer	(char const *buffer,
							 size_t size);
ccss_stylesheet_t *	ccss_stylesheet_new_from_file	(char const *css_file);

ccss_stylesheet_t *	ccss_stylesheet_load_from_file	(ccss_stylesheet_t *self,
							 char const *css_file,
							 ccss_stylesheet_precedence_t precedence);

void			ccss_stylesheet_free		(ccss_stylesheet_t *self);

ccss_selector_group_t const * ccss_stylesheet_query_type (ccss_stylesheet_t const *self,
							char const *type_name);

bool
ccss_stylesheet_query (ccss_stylesheet_t const	*self,
		       ccss_node_t const	*node, 
		       ccss_style_t		*style);

/**
 * ccss_stylesheet_iter_t:
 *
 * Stack-allocatable iterator for walking a stylesheet.
 **/
typedef GHashTableIter ccss_stylesheet_iter_t;

void ccss_stylesheet_iter_init (ccss_stylesheet_iter_t *self,
			       ccss_stylesheet_t const *stylesheet);

bool ccss_stylesheet_iter_next (ccss_stylesheet_iter_t *self,
			       char const **type_name,
			       ccss_selector_group_t const **group);

#ifdef CCSS_DEBUG
void ccss_stylesheet_dump (ccss_stylesheet_t const *self);
#endif

G_END_DECLS

#endif /* CCSS_STYLESHEET_H */

