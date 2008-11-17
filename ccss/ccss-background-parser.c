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

#include <math.h>
#include <string.h>
#include <glib.h>
#include "ccss-background-parser.h"
#include "ccss-background.h"
#include "ccss-block-priv.h"

static const struct {
	char const				*name;
	const enum ccss_background_repeat	 repeat;
} _repeat_map[] = {
  { "repeat",		CCSS_BACKGROUND_REPEAT		},
  { "repeat-x",		CCSS_BACKGROUND_REPEAT_X		},
  { "repeat-y",		CCSS_BACKGROUND_REPEAT_Y		},
  { "no-repeat",	CCSS_BACKGROUND_NO_REPEAT	}
};

static bool
bg_attachment_parse (ccss_background_attachment_t	 *self,
		     CRTerm const			**values)
{
	ccss_property_spec_t spec;

	if (!*values) {
		return false;
	}

	spec = ccss_property_parse_spec (values);
	if (CCSS_PROPERTY_SPEC_INHERIT == spec) {
		self->spec = spec;
		return true;
	}

	if (TERM_IDENT == (*values)->type) {
		char const *attachment;
		attachment = cr_string_peek_raw_str ((*values)->content.str);
		if (0 == g_strcmp0 ("scroll", attachment)) {
			self->attachment = CCSS_BACKGROUND_SCROLL;
			self->spec = CCSS_PROPERTY_SPEC_SET;
			*values = (*values)->next;
			return true;
		} else if (0 == g_strcmp0 ("fixed", attachment)) {
			self->attachment = CCSS_BACKGROUND_FIXED;
			self->spec = CCSS_PROPERTY_SPEC_SET;
			*values = (*values)->next;
			return true;
		}
	}

	return false;
}

static bool
bg_image_parse (ccss_background_image_t	 *self,
		char const		 *property_name,
		CRTerm const		**values)
{
	if (!*values) {
		return false;
	}

	self->spec = ccss_image_parse (&self->image, property_name, values);
	return self->spec == CCSS_PROPERTY_SPEC_SET;
}

static bool
bg_position_parse (ccss_background_position_t	 *self,
		   CRTerm const			**values)
{
	ccss_property_spec_t	spec;
	uint32_t		flags;
	bool			have_hpos;
	bool			have_vpos;

	if (!*values) {
		return false;
	}

	spec = ccss_property_parse_spec (values);
	if (CCSS_PROPERTY_SPEC_INHERIT == spec) {
		self->spec = CCSS_PROPERTY_SPEC_INHERIT;
		return true;
	}

	flags = CCSS_POSITION_MASK_NUMERIC | CCSS_POSITION_MASK_HORIZONTAL;
	have_hpos = ccss_position_parse (&self->hpos, flags, values);
	if (!have_hpos) {
		self->spec = CCSS_PROPERTY_SPEC_UNSET;
		return false;		
	}

	flags = CCSS_POSITION_MASK_NUMERIC | CCSS_POSITION_MASK_VERTICAL;
	have_vpos = ccss_position_parse (&self->vpos, flags, values);
	if (have_hpos && !have_vpos) {
		/* Fall back to `center' for vpos. 
		 * http://www.w3.org/TR/CSS21/colors.html#propdef-background-position */
		self->vpos.type = CCSS_POSITION_PERCENTAGE;
		self->vpos.value = 50;
	}
	
	/* A bit fuzzy, but let's say we're satisfied with `hpos' only. */
	if (have_hpos) {
		self->spec = CCSS_PROPERTY_SPEC_SET;
		return true;
	}

	return false;
}

static bool
bg_repeat_parse (ccss_background_repeat_t	 *self,
		 CRTerm const			**values)
{
	char const		*repeat;
	ccss_property_spec_t	 spec;

	if (!*values || (*values)->type != TERM_IDENT) {
		return false;
	}

	repeat = cr_string_peek_raw_str ((*values)->content.str);
	for (unsigned int i = 0; i < G_N_ELEMENTS (_repeat_map); i++) {
		if (0 == g_strcmp0 (_repeat_map[i].name, repeat)) {
			self->repeat = _repeat_map[i].repeat;
			self->spec = CCSS_PROPERTY_SPEC_SET;
			*values = (*values)->next;
			return true;
		}
	}

	/* Not found, maybe a generic property? 
	 * Only `inherit' is allowed anyway. */
	spec = ccss_property_parse_spec (values);
	if (spec == CCSS_PROPERTY_SPEC_INHERIT) {
		self->spec = spec;
		return true;
	} else {
		self->spec = CCSS_PROPERTY_SPEC_UNSET;
		return false;
	}
}

static bool
bg_size_parse (ccss_background_size_t	 *self,
	       CRTerm const		**values)
{
	uint32_t	flags;
	bool		ret;

	if (!*values) {
		return false;
	}

	flags = CCSS_POSITION_MASK_NUMERIC | CCSS_POSITION_MASK_AUTO;
	ret = ccss_position_parse (&self->width, flags, values);
	if (!ret) {
		return false;
	}

	if (CCSS_POSITION_CONTAIN == self->width.type || 
	    CCSS_POSITION_COVER == self->width.type) {
		self->height.type = self->width.type;
		self->spec = CCSS_PROPERTY_SPEC_SET;
		return true;
	}

	if (CCSS_POSITION_MASK_NUMERIC & self->width.type ||
	    CCSS_POSITION_AUTO == self->width.type) {
		flags = CCSS_POSITION_MASK_NUMERIC | CCSS_POSITION_MASK_AUTO;
		ret = ccss_position_parse (&self->height, flags, values);
		if (ret) {
			self->spec = CCSS_PROPERTY_SPEC_SET;
			return true;
		} else {
			self->height.type =  CCSS_POSITION_AUTO;
			self->spec = CCSS_PROPERTY_SPEC_SET;
			return true;
		}
	}

	return false;
}

bool
ccss_block_parse_background (ccss_block_t	*self,
			     char const		*property,
			     CRTerm const	*values)
{
	ccss_background_attachment_t	*bg_attachment,	bga;
	ccss_color_t			*bg_color,	bgc;
	ccss_background_image_t		*bg_image,	bgi;
	ccss_background_position_t	*bg_position,	bgp;
	ccss_background_repeat_t	*bg_repeat,	bgr;
	ccss_background_size_t		*bg_size,	bgs;

	bool ret;

	ret = false;

	if (0 == strcmp ("background", property)) {

		/* FIXME: also support `background-size' here, but let's stick
		 * to CSS2 for now. */
		ret = ccss_color_parse (&bgc, property, &values);
		if (ret) {
			bg_color = ccss_block_new_background_color (self);
			*bg_color = bgc;
			if (NULL == values)
				return true;
		} else {
			return false;
		}

		ret = bg_image_parse (&bgi, property, &values);
		if (ret) {
			bg_image = ccss_block_new_background_image (self);
			*bg_image = bgi;
			if (NULL == values)
				return true;
		} else {
			return false;
		}

		ret = bg_repeat_parse (&bgr, &values);
		if (ret) {
			bg_repeat = ccss_block_new_background_repeat (self);
			*bg_repeat = bgr;
			if (NULL == values)
				return true;
		} else {
			return false;
		}

		ret = bg_attachment_parse (&bga, &values);
		if (ret) {
			bg_attachment = ccss_block_new_background_attachment (self);
			*bg_attachment = bga;
			if (NULL == values)
				return true;
		} else {
			return false;
		}

		ret = bg_position_parse (&bgp, &values);
		if (ret) {
			bg_position = ccss_block_new_background_position (self);
			*bg_position = bgp;
			if (NULL == values)
				return true;
		} else {
			return false;
		}
	}

	if (0 == strcmp ("background-attachment", property)) {

		ret = bg_attachment_parse (&bga, &values);
		if (ret) {
			bg_attachment = ccss_block_new_background_attachment (self);
			*bg_attachment = bga;
			if (NULL == values)
				return true;
		} else {
			return false;
		}
	}

	if (0 == strcmp ("background-color", property)) {

		ret = ccss_color_parse (&bgc, property, &values);
		if (ret) {
			bg_color = ccss_block_new_background_color (self);
			*bg_color = bgc;
			if (NULL == values)
				return true;
		} else {
			return false;
		}
	}

	if (0 == strcmp ("background-image", property)) {

		ret = bg_image_parse (&bgi, property, &values);
		if (ret) {
			bg_image = ccss_block_new_background_image (self);
			*bg_image = bgi;
			if (NULL == values)
				return true;
		} else {
			return false;
		}
	}

	if (0 == strcmp ("background-position", property)) {

		ret = bg_position_parse (&bgp, &values);
		if (ret) {
			bg_position = ccss_block_new_background_position (self);
			*bg_position = bgp;
			if (NULL == values)
				return true;
		} else {
			return false;
		}
	}

	if (0 == strcmp ("background-repeat", property)) {

		ret = bg_repeat_parse (&bgr, &values);
		if (ret) {
			bg_repeat = ccss_block_new_background_repeat (self);
			*bg_repeat = bgr;
			if (NULL == values)
				return true;
		} else {
			return false;
		}
	}

	if (0 == strcmp ("background-size", property)) {

		ret = bg_size_parse (&bgs, &values);
		if (ret) {
			bg_size = ccss_block_new_background_size (self);
			*bg_size = bgs;
			if (NULL == values)
				return true;
		} else {
			return false;
		}
	}

	g_warning ("Unknown property `%s'", property);
	return false;
}

