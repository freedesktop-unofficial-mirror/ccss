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

#ifndef CCSS_BLOCK_H
#define CCSS_BLOCK_H

#include <glib.h>
#include <cairo.h>
#include <ccss/ccss-features.h>
#include <ccss/ccss-color.h>
#include <ccss/ccss-background.h>
#include <ccss/ccss-border.h>
#include <ccss/ccss-property.h>

G_BEGIN_DECLS

typedef struct {
	/*< private >*/
	ccss_background_t	background;
	ccss_border_t		border;
	ccss_color_t		color;
} ccss_block_t;

ccss_block_t *	ccss_block_new	(void);
void		ccss_block_free	(ccss_block_t *self);

#ifdef CCSS_DEBUG
void ccss_block_dump (ccss_block_t const *self);
#endif

G_END_DECLS

#endif /* CCSS_BLOCK_H */

