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

#ifndef CCSS_POSITION_H
#define CCSS_POSITION_H

#include <stdbool.h>
#include <stdint.h>
#include <libcroco/libcroco.h>
#include <ccss/ccss-features.h>
#include <ccss/ccss-macros.h>

CCSS_BEGIN_DECLS

typedef enum {
	/* Remember to revisit all locations where a mask is used when it's 
	 * being extended. */
	CCSS_POSITION_LENGTH		= 1 << 0,
	CCSS_POSITION_PERCENTAGE		= 1 << 1,

	CCSS_POSITION_MASK_NUMERIC	= CCSS_POSITION_LENGTH | 
					  CCSS_POSITION_PERCENTAGE,

	CCSS_POSITION_LEFT		= 1 << 2,
	CCSS_POSITION_TOP		= 1 << 3,
	CCSS_POSITION_RIGHT		= 1 << 4,
	CCSS_POSITION_BOTTOM		= 1 << 5,
	CCSS_POSITION_CENTER		= 1 << 6,

	CCSS_POSITION_MASK_HORIZONTAL	= CCSS_POSITION_LEFT |
					  CCSS_POSITION_RIGHT |
					  CCSS_POSITION_CENTER,

	CCSS_POSITION_MASK_VERTICAL	= CCSS_POSITION_TOP |
					  CCSS_POSITION_BOTTOM |
					  CCSS_POSITION_CENTER,

	CCSS_POSITION_AUTO		= 1 << 7,
	CCSS_POSITION_CONTAIN		= 1 << 8,
	CCSS_POSITION_COVER		= 1 << 9,
	CCSS_POSITION_MASK_AUTO		= CCSS_POSITION_AUTO |
					  CCSS_POSITION_CONTAIN |
					  CCSS_POSITION_COVER
} ccss_position_type_t;

typedef struct {
	ccss_position_type_t	type;
	double			value;
} ccss_position_t;

bool ccss_position_parse	(ccss_position_t *self, uint32_t flags, 
			 CRTerm const **value);

double ccss_position_get_pos (ccss_position_t const *self,
			     double extent, double size);

double ccss_position_get_vpos (ccss_position_t const *self,
			      double extent_x, double extent_y,
			      double width, double height);

double ccss_position_get_hsize (ccss_position_t const *self,
			       double extent_x, double extent_y,
			       double width, double height);

double ccss_position_get_vsize (ccss_position_t const *self,
			       double extent_x, double extent_y,
			       double width, double height);

#ifdef CCSS_DEBUG
void ccss_position_dump (ccss_position_t const *self);
#endif

CCSS_END_DECLS

#endif /* CCSS_POSITION_H */

