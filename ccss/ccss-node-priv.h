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

#ifndef CCSS_NODE_PRIV_H
#define CCSS_NODE_PRIV_H

#include <stdbool.h>
#include <stddef.h>
#include <ccss/ccss-macros.h>
#include <ccss/ccss-node.h>

CCSS_BEGIN_DECLS

/**
 * ccss_node_t:
 *
 * Represents a DOM node object.
 **/
struct ccss_node_ {
	/*< private >*/
	CCSS_DEPRECATED (ccss_node_class_t const node_class);

	CCSS_DEPRECATED (void		 *user_data);

	/* Cache */
	CCSS_DEPRECATED (ptrdiff_t	  instance);
	CCSS_DEPRECATED (char const	 *id);
	CCSS_DEPRECATED (char const	 *type_name);
	CCSS_DEPRECATED (char const	**css_classes);
	CCSS_DEPRECATED (char const	**pseudo_classes);
	CCSS_DEPRECATED (char const	 *inline_style);
};

bool
ccss_node_is_a			(ccss_node_t		*self,
				 char const		*type_name);

ccss_node_t *
ccss_node_get_container		(ccss_node_t		*self);

ccss_node_t *
ccss_node_get_base_style	(ccss_node_t		*self);

char const *
ccss_node_get_type		(ccss_node_t		*self);

ptrdiff_t
ccss_node_get_instance		(ccss_node_t 		*self);

char const *
ccss_node_get_id		(ccss_node_t 		*self);

const char **
ccss_node_get_classes           (ccss_node_t 		*self);

const char **
ccss_node_get_pseudo_classes    (ccss_node_t 		*self);

char *
ccss_node_get_attribute		(ccss_node_t const	*self,
				 char const		*name);

char const *
ccss_node_get_style		(ccss_node_t 		*self,
				 unsigned int		 descriptor);

bool
ccss_node_get_viewport		(ccss_node_t const	*self,
				 double			*x,
				 double			*y,
				 double			*width,
				 double			*height);

void
ccss_node_release		(ccss_node_t		*self);

CCSS_END_DECLS

#endif /* CCSS_NODE_PRIV_H */

