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

#ifndef CCSS_BLOCK_H
#define CCSS_BLOCK_H

#include <ccss/ccss-macros.h>
#include <ccss/ccss-property.h>

CCSS_BEGIN_DECLS

typedef struct ccss_block_ ccss_block_t;

void
ccss_block_add_property (ccss_block_t		*self,
			 char const		*property_name,
			 ccss_property_base_t	*property);

CCSS_END_DECLS

#endif /* CCSS_BLOCK_H */

