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

#ifndef CCSS_SELECTOR_GROUP_PRIV_H
#define CCSS_SELECTOR_GROUP_PRIV_H

#include <stdbool.h>
#include <glib.h>
#include <ccss/ccss-features.h>
#include <ccss/ccss-node.h>
#include <ccss/ccss-selector-group.h>
#include <ccss/ccss-selector.h>
#include <ccss/ccss-style.h>

G_BEGIN_DECLS

void ccss_selector_group_add_selector	(ccss_selector_group_t *self, 
					 ccss_selector_t *selector);
void ccss_selector_group_merge		(ccss_selector_group_t *self,
					 ccss_selector_group_t const *group);
void ccss_selector_group_merge_base	(ccss_selector_group_t *self,
					 ccss_selector_group_t const *group);

GSList const *	ccss_selector_group_get_dangling_selectors	(ccss_selector_group_t const *self);
void		ccss_selector_group_clear_dangling_selectors	(ccss_selector_group_t *self);

bool ccss_selector_group_query_collect (ccss_selector_group_t const *self, 
				       ccss_node_t const *node, 
				       ccss_selector_group_t *result_group,
				       bool as_base);
bool ccss_selector_group_query_apply (ccss_selector_group_t const *self, 
			             ccss_node_t const *node, ccss_style_t *style);

G_END_DECLS

#endif /* CCSS_SELECTOR_GROUP_PRIV_H */

