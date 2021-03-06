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

#ifndef CCSS_STYLESHEET_H
#define CCSS_STYLESHEET_H

#include <stdbool.h>
#include <ccss/ccss-node.h>
#include <ccss/ccss-macros.h>
#include <ccss/ccss-style.h>

CCSS_BEGIN_DECLS

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

void
ccss_stylesheet_destroy		(ccss_stylesheet_t		*self);

ccss_stylesheet_t *
ccss_stylesheet_reference (ccss_stylesheet_t *self);

unsigned int
ccss_stylesheet_get_reference_count	(ccss_stylesheet_t const *self);

/* Somewhat hackish */
struct ccss_grammar_ *
ccss_stylesheet_get_grammar		(ccss_stylesheet_t const *self);

unsigned int
ccss_stylesheet_add_from_file	(ccss_stylesheet_t		*self,
				 char const			*css_file,
				 ccss_stylesheet_precedence_t	 precedence,
				 void				*user_data);

unsigned int
ccss_stylesheet_add_from_buffer	(ccss_stylesheet_t		*self,
				 char const			*buffer,
				 size_t				 size,
				 ccss_stylesheet_precedence_t	 precedence,
				 void				*user_data);

bool
ccss_stylesheet_unload		(ccss_stylesheet_t		*self,
				 unsigned int			 descriptor);

ccss_style_t *
ccss_stylesheet_query_type	(ccss_stylesheet_t 		*self,
				 char const			*type_name);

ccss_style_t *
ccss_stylesheet_query		(ccss_stylesheet_t 		*self,
				 ccss_node_t			*node);

/**
 * ccss_stylesheet_iterator_f:
 * @self:	a #ccss_stylesheet_t.
 * @type_name:	node type name selectors are available for, e.g. `div'.
 * @user_data:	user data passed to #ccss_stylesheet_foreach.
 *
 * Specifies the type of the function passed to ccss_stylesheet_foreach().
 **/
typedef void (*ccss_stylesheet_iterator_f) (ccss_stylesheet_t 	*self,
					    char const		*type_name,
					    void		*user_data);

void
ccss_stylesheet_foreach (ccss_stylesheet_t		*self,
			 ccss_stylesheet_iterator_f	 func,
			 void				*user_data);

void
ccss_stylesheet_dump (ccss_stylesheet_t const *self);

CCSS_END_DECLS

#endif /* CCSS_STYLESHEET_H */

