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
#include "ccss-property-priv.h"

static ccss_property_class_t const *
peek_property_class (char const *property_name);

static const struct {
	char const				*name;
	const enum ccss_background_repeat	 repeat;
} _repeat_map[] = {
  { "repeat",		CCSS_BACKGROUND_REPEAT		},
  { "repeat-x",		CCSS_BACKGROUND_REPEAT_X	},
  { "repeat-y",		CCSS_BACKGROUND_REPEAT_Y	},
  { "no-repeat",	CCSS_BACKGROUND_NO_REPEAT	}
};

static bool
bg_attachment_parse (ccss_background_attachment_t	 *self,
		     CRTerm const			**values)
{
	ccss_property_state_t state;

	if (!*values) {
		return false;
	}

	state = ccss_property_parse_state (values);
	if (CCSS_PROPERTY_STATE_INHERIT == state) {
		self->base.state = state;
		return true;
	}

	if (TERM_IDENT == (*values)->type) {
		char const *attachment;
		attachment = cr_string_peek_raw_str ((*values)->content.str);
		if (0 == g_strcmp0 ("scroll", attachment)) {
			self->attachment = CCSS_BACKGROUND_SCROLL;
			self->base.state = CCSS_PROPERTY_STATE_SET;
			*values = (*values)->next;
			return true;
		} else if (0 == g_strcmp0 ("fixed", attachment)) {
			self->attachment = CCSS_BACKGROUND_FIXED;
			self->base.state = CCSS_PROPERTY_STATE_SET;
			*values = (*values)->next;
			return true;
		}
	}

	return false;
}

static bool
bg_image_parse (ccss_background_image_t	 *image,
		CRTerm const		**values)
{
	if (!*values) {
		return false;
	}

	image->base.state = ccss_image_parse (&image->image, values);
	return image->base.state == CCSS_PROPERTY_STATE_SET;
}

static bool
bg_position_parse (ccss_background_position_t	 *self,
		   CRTerm const			**values)
{
	ccss_property_state_t	state;
	uint32_t		flags;
	bool			have_hpos;
	bool			have_vpos;

	if (!*values) {
		return false;
	}

	state = ccss_property_parse_state (values);
	if (CCSS_PROPERTY_STATE_INHERIT == state) {
		self->base.state = CCSS_PROPERTY_STATE_INHERIT;
		return true;
	}

	flags = CCSS_POSITION_MASK_NUMERIC | CCSS_POSITION_MASK_HORIZONTAL;
	have_hpos = ccss_position_parse (&self->hpos, flags, values);
	if (!have_hpos) {
		self->base.state = CCSS_PROPERTY_STATE_UNSET;
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
		self->base.state = CCSS_PROPERTY_STATE_SET;
		return true;
	}

	return false;
}

static bool
bg_repeat_parse (ccss_background_repeat_t	 *self,
		 CRTerm const			**values)
{
	char const		*repeat;
	ccss_property_state_t	 state;

	if (!*values || (*values)->type != TERM_IDENT) {
		return false;
	}

	repeat = cr_string_peek_raw_str ((*values)->content.str);
	for (unsigned int i = 0; i < G_N_ELEMENTS (_repeat_map); i++) {
		if (0 == g_strcmp0 (_repeat_map[i].name, repeat)) {
			self->repeat = _repeat_map[i].repeat;
			self->base.state = CCSS_PROPERTY_STATE_SET;
			*values = (*values)->next;
			return true;
		}
	}

	/* Not found, maybe a generic property? 
	 * Only `inherit' is allowed anyway. */
	state = ccss_property_parse_state (values);
	if (state == CCSS_PROPERTY_STATE_INHERIT) {
		self->base.state = state;
		return true;
	} else {
		self->base.state = CCSS_PROPERTY_STATE_UNSET;
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
		self->base.state = CCSS_PROPERTY_STATE_SET;
		return true;
	}

	if (CCSS_POSITION_MASK_NUMERIC & self->width.type ||
	    CCSS_POSITION_AUTO == self->width.type) {
		flags = CCSS_POSITION_MASK_NUMERIC | CCSS_POSITION_MASK_AUTO;
		ret = ccss_position_parse (&self->height, flags, values);
		if (ret) {
			self->base.state = CCSS_PROPERTY_STATE_SET;
			return true;
		} else {
			self->height.type =  CCSS_POSITION_AUTO;
			self->base.state = CCSS_PROPERTY_STATE_SET;
			return true;
		}
	}

	return false;
}

static bool
background_factory (ccss_block_t		*self,
		    CRTerm const		*values)
{
	ccss_background_attachment_t	*bg_attachment,	bga;
	ccss_color_t			*bg_color,	bgc;
	ccss_background_image_t		*bg_image,	bgi;
	ccss_background_position_t	*bg_position,	bgp;
	ccss_background_repeat_t	*bg_repeat,	bgr;
	bool				 ret;

	/* FIXME: also support `background-size' here, but let's stick
	 * to CSS2 for now. */
	ret = ccss_color_parse (&bgc, &values);
	if (ret) {
		bgc.base.property_class = peek_property_class ("background-color");
		bg_color = g_new0 (ccss_color_t, 1);
		*bg_color = bgc;
		ccss_block_add_property (self, "background-color", 
					 (ccss_property_base_t *) bg_color);
		if (NULL == values)
			return true;
	} else {
		return false;
	}

	ret = bg_image_parse (&bgi, &values);
	if (ret) {
		bgi.base.property_class = peek_property_class ("background-image");
		bg_image = g_new0 (ccss_background_image_t, 1);
		*bg_image = bgi;
		ccss_block_add_property (self, "background-image", 
					    (ccss_property_base_t *) bg_image);
		if (NULL == values)
			return true;
	} else {
		return false;
	}

	ret = bg_repeat_parse (&bgr, &values);
	if (ret) {
		bgr.base.property_class = peek_property_class ("background-repeat");
		bg_repeat = g_new0 (ccss_background_repeat_t, 1);
		*bg_repeat = bgr;
		ccss_block_add_property (self, "background-repeat", 
					 (ccss_property_base_t *) bg_repeat);
		if (NULL == values)
			return true;
	} else {
		return false;
	}

	ret = bg_attachment_parse (&bga, &values);
	if (ret) {
		bga.base.property_class = peek_property_class ("background-attachment");
		bg_attachment = g_new0 (ccss_background_attachment_t, 1);
		*bg_attachment = bga;
		ccss_block_add_property (self, "background-attachment", 
					 (ccss_property_base_t *) bg_attachment);
		if (NULL == values)
			return true;
	} else {
		return false;
	}

	ret = bg_position_parse (&bgp, &values);
	if (ret) {
		bgp.base.property_class = peek_property_class ("background-position");
		bg_position = g_new0 (ccss_background_position_t, 1);
		*bg_position = bgp;
		ccss_block_add_property (self, "background-position", 
					 (ccss_property_base_t *) bg_position);
		if (NULL == values)
			return true;
	} else {
		return false;
	}

	/* All sub-properties have been parsed correctly. */
	return true;
}

static ccss_background_attachment_t *
background_attachment_new (CRTerm const *values)
{
	ccss_background_attachment_t	*self;
	bool				 ret;

	g_return_val_if_fail (values, NULL);

	self = g_new0 (ccss_background_attachment_t, 1);
	ret = bg_attachment_parse (self, &values);
	if (!ret) {
		g_free (self), self = NULL;
	}

	return self;
}

static bool
background_attachment_convert (ccss_background_attachment_t const	*property,
			       ccss_property_type_t			 target,
			       void					*value)
{
	char *ret;

	g_return_val_if_fail (property && value, false);

	if (CCSS_PROPERTY_TYPE_DOUBLE == target)
		return false;

	switch (property->attachment) {
	case CCSS_BACKGROUND_SCROLL:
		ret = g_strdup ("scroll");
		break;
	case CCSS_BACKGROUND_FIXED:
		ret = g_strdup ("fixed");
		break;
	default:
		g_assert_not_reached ();
		return false;
	}

	* (char **) value = ret;

	return true;
}

static ccss_background_image_t *
background_image_new (CRTerm const *values)
{
	ccss_background_image_t	*self;
	bool			 ret;

	g_return_val_if_fail (values, NULL);

	self = g_new0 (ccss_background_image_t, 1);
	ret = bg_image_parse (self, &values);
	if (!ret) {
		g_free (self), self = NULL;
	}

	return self;
}

static bool
background_image_convert (ccss_background_image_t const	*property,
			  ccss_property_type_t		 target,
			  void				*value)
{
	char *ret;

	g_return_val_if_fail (property && value, false);

	if (CCSS_PROPERTY_TYPE_DOUBLE == target)
		return false;

	ret = g_strdup (property->image.uri);

	* (char **) value = ret;

	return true;
}

static ccss_background_position_t *
background_position_new (CRTerm const *values)
{
	ccss_background_position_t	*self;
	bool				 ret;

	g_return_val_if_fail (values, NULL);

	self = g_new0 (ccss_background_position_t, 1);
	ret = bg_position_parse (self, &values);
	if (!ret) {
		g_free (self), self = NULL;
	}

	return self;
}

static bool
background_position_convert (ccss_background_position_t const	*property,
			     ccss_property_type_t		 target,
			     void				*value)
{
	// FIXME: this needs 2 return values.
	g_return_val_if_fail (0, false);
	return false;
}

static ccss_background_repeat_t *
background_repeat_new (CRTerm const *values)
{
	ccss_background_repeat_t	*self;
	bool				 ret;

	g_return_val_if_fail (values, NULL);

	self = g_new0 (ccss_background_repeat_t, 1);
	ret = bg_repeat_parse (self, &values);
	if (!ret) {
		g_free (self), self = NULL;
	}

	return self;
}

static bool
background_repeat_convert (ccss_background_repeat_t const	*property,
			   ccss_property_type_t			 target,
			   void					*value)
{
	char *ret;

	g_return_val_if_fail (property && value, false);

	if (CCSS_PROPERTY_TYPE_DOUBLE == target)
		return false;

	switch (property->repeat) {
	case CCSS_BACKGROUND_REPEAT:
		ret = g_strdup ("repeat");
		break;
	case CCSS_BACKGROUND_REPEAT_X:
		ret = g_strdup ("repeat-x");
		break;
	case CCSS_BACKGROUND_REPEAT_Y:
		ret = g_strdup ("repeat-y");
		break;
	case CCSS_BACKGROUND_NO_REPEAT:
		ret = g_strdup ("no-repeat");
		break;
	default:
		g_assert_not_reached ();
		return false;
	}

	* (char **) value = ret;

	return true;
}

static ccss_background_size_t *
background_size_new (CRTerm const *values)
{
	ccss_background_size_t	*self;
	bool			 ret;

	g_return_val_if_fail (values, NULL);

	self = g_new0 (ccss_background_size_t, 1);
	ret = bg_size_parse (self, &values);
	if (!ret) {
		g_free (self), self = NULL;
	}

	return self;
}

static bool
background_size_convert (ccss_background_size_t const	*property,
			 ccss_property_type_t		 target,
			 void				*value)
{
	// FIXME: this needs 2 return values.
	g_return_val_if_fail (0, false);
	return false;
}

static ccss_property_class_t const _ptable[] = {
    {
	.name = "background-attachment",
	.property_new = (ccss_property_new_f) background_attachment_new,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = (ccss_property_convert_f) background_attachment_convert,
	.property_factory = NULL
    }, {
	.name = "background-image",
	.property_new = (ccss_property_new_f) background_image_new,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = (ccss_property_convert_f) background_image_convert,
	.property_factory = NULL
    }, {
	.name = "background-position",
	.property_new = (ccss_property_new_f) background_position_new,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = (ccss_property_convert_f) background_position_convert,
	.property_factory = NULL
    }, {
	.name = "background-repeat",
	.property_new = (ccss_property_new_f) background_repeat_new,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = (ccss_property_convert_f) background_repeat_convert,
	.property_factory = NULL
    }, {
	.name = "background-size",
	.property_new = (ccss_property_new_f) background_size_new,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = (ccss_property_convert_f) background_size_convert,
	.property_factory = NULL
    }, {
	.name = "background",
	.property_new = NULL,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = NULL,
	.property_factory = background_factory
    }, {
	.name = NULL,
	.property_new = NULL,
	.property_free = NULL,
	.property_convert = NULL,
	.property_factory = NULL
    }
};

static ccss_property_class_t const *
peek_property_class (char const *property_name)
{
	for (unsigned int i = 0; i < G_N_ELEMENTS (_ptable); i++) {
		if (0 == g_strcmp0 (property_name, _ptable[i].name))
			return &_ptable[i];
	}

	g_return_val_if_fail (0, NULL);

	return NULL;
}

ccss_property_class_t const *
ccss_background_get_ptable (void)
{
	return _ptable;
}

