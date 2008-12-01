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
#include "config.h"

#define PROPERTY_SET(property_)					\
	(CCSS_PROPERTY_STATE_NONE == property_->state ||	\
	 CCSS_PROPERTY_STATE_SET == property_->state)

#define PROPERTY_INHERIT(property_)				\
	(CCSS_PROPERTY_STATE_INHERIT == property_->state)

/* Dummy property to hook up the inheritance function. */
typedef struct {
	ccss_property_base_t			 base;
	ccss_background_attachment_t const	*bg_attachment;
	ccss_background_image_t const		*bg_image;
	ccss_background_position_t const	*bg_position;
	ccss_background_repeat_t const		*bg_repeat;
	ccss_color_t const			*bg_color;
} background_property_t;

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
		ccss_grammar_t const	 *grammar,
		CRTerm const		**values)
{
	if (!*values) {
		return false;
	}

	image->base.state = ccss_image_parse (&image->image, grammar, values);
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
		self->base.state = CCSS_PROPERTY_STATE_INVALID;
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
		self->base.state = CCSS_PROPERTY_STATE_INVALID;
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
background_factory (ccss_grammar_t const	*grammar,
		    ccss_block_t		*self,
		    CRTerm const		*values)
{
	ccss_background_attachment_t	*bg_attachment,	bga;
	ccss_color_t			*bg_color,	bgc;
	ccss_background_image_t		*bg_image,	bgi;
	ccss_background_position_t	*bg_position,	bgp;
	ccss_background_repeat_t	*bg_repeat,	bgr;
	background_property_t		*background,	bg;
	bool				 have_bg_properties;
	bool				 ret;

	/* If `background: inherit;' then insert a dummy */
	memset (&bg, 0, sizeof (bg));
	ccss_property_init (&bg.base, peek_property_class ("background"));
	bg.base.state = ccss_property_parse_state (&values);
	if (bg.base.state == CCSS_PROPERTY_STATE_INHERIT) {
		background = g_new0 (background_property_t, 1);
		*background = bg;
		ccss_block_add_property (self, "background", &background->base);
		return true;
	}

	have_bg_properties = false;

	/* PONDERING: also support `background-size' here, but let's stick
	 * to CSS2 for now. */
	ccss_property_init (&bgc.base, peek_property_class ("background-color"));
	ret = ccss_color_parse (&bgc, grammar, &values);
	if (ret) {
		bg_color = g_new0 (ccss_color_t, 1);
		*bg_color = bgc;
		ccss_block_add_property (self, "background-color", &bg_color->base);
		have_bg_properties = true;
		bg.bg_color = bg_color;
	}

	ccss_property_init (&bgi.base, peek_property_class ("background-image"));
	ret = bg_image_parse (&bgi, grammar, &values);
	if (ret) {
		bg_image = g_new0 (ccss_background_image_t, 1);
		*bg_image = bgi;
		ccss_block_add_property (self, "background-image", &bg_image->base);
		have_bg_properties = true;
		bg.bg_image = bg_image;
	}

	ccss_property_init (&bgr.base, peek_property_class ("background-repeat"));
	ret = bg_repeat_parse (&bgr, &values);
	if (ret) {
		bg_repeat = g_new0 (ccss_background_repeat_t, 1);
		*bg_repeat = bgr;
		ccss_block_add_property (self, "background-repeat", &bg_repeat->base);
		have_bg_properties = true;
		bg.bg_repeat = bg_repeat;
	}

	ccss_property_init (&bga.base, peek_property_class ("background-attachment"));
	ret = bg_attachment_parse (&bga, &values);
	if (ret) {
		bg_attachment = g_new0 (ccss_background_attachment_t, 1);
		*bg_attachment = bga;
		ccss_block_add_property (self, "background-attachment", &bg_attachment->base);
		have_bg_properties = true;
		bg.bg_attachment = bg_attachment;
	}

	ccss_property_init (&bgp.base, peek_property_class ("background-position"));
	ret = bg_position_parse (&bgp, &values);
	if (ret) {
		bg_position = g_new0 (ccss_background_position_t, 1);
		*bg_position = bgp;
		ccss_block_add_property (self, "background-position", &bg_position->base);
		have_bg_properties = true;
		bg.bg_position = bg_position;
	}

	/* Any valid properties? */
	if (have_bg_properties) {
		background = g_new0 (background_property_t, 1);
		*background = bg;
		ccss_block_add_property (self, "background", &background->base);
		return true;
	}

	return false;
}

static bool
background_inherit (ccss_style_t const	*container_style,
		    ccss_style_t	*style)
{
	ccss_property_base_t const	*property;
	bool				 ret;

	ret = false;

	if (ccss_style_get_property (container_style,
				"background", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"background", property);
			ret = true;
		}
	}
	if (ccss_style_get_property (container_style,
				"background-attachment", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"background-attachment", property);
		} else if (PROPERTY_INHERIT (property)) {
			/* Need to resolve further. */
			ret = false;			
		}
	}
	if (ccss_style_get_property (container_style,
				"background-color", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"background-color", property);
		} else if (PROPERTY_INHERIT (property)) {
			/* Need to resolve further. */
			ret = false;			
		}
	}
	if (ccss_style_get_property (container_style,
				"background-image", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"background-image", property);
		} else if (PROPERTY_INHERIT (property)) {
			/* Need to resolve further. */
			ret = false;			
		}
	}
	if (ccss_style_get_property (container_style,
				"background-position", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"background-position", property);
		} else if (PROPERTY_INHERIT (property)) {
			/* Need to resolve further. */
			ret = false;			
		}
	}
	if (ccss_style_get_property (container_style,
				"background-repeat", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"background-repeat", property);
		} else if (PROPERTY_INHERIT (property)) {
			/* Need to resolve further. */
			ret = false;			
		}
	}
	/* PONDERING support CSS2 `background' for now.
	if (ccss_style_get_property (container_style, "background-size",
				     &property)) {
		ccss_style_set_property (style, "background-size", property);
	} */

	return ret;
}

static ccss_property_base_t *
background_attachment_create (ccss_grammar_t const	*grammar,
			      CRTerm const		*values)
{
	ccss_background_attachment_t	*self;
	bool				 ret;

	g_return_val_if_fail (values, NULL);

	self = g_new0 (ccss_background_attachment_t, 1);
	ccss_property_init (&self->base, peek_property_class ("background-attachment"));
	ret = bg_attachment_parse (self, &values);
	if (!ret) {
		g_free (self), self = NULL;
	}

	return &self->base;
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

static ccss_property_base_t *
background_image_create (ccss_grammar_t const	*grammar,
			 CRTerm const		*values)
{
	ccss_background_image_t	*self;
	bool			 ret;

	g_return_val_if_fail (values, NULL);

	self = g_new0 (ccss_background_image_t, 1);
	ccss_property_init (&self->base, peek_property_class ("background-image"));
	ret = bg_image_parse (self, grammar, &values);
	if (!ret) {
		g_free (self), self = NULL;
	}

	return &self->base;
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

static ccss_property_base_t *
background_position_create (ccss_grammar_t const	*grammar,
			    CRTerm const		*values)
{
	ccss_background_position_t	*self;
	bool				 ret;

	g_return_val_if_fail (values, NULL);

	self = g_new0 (ccss_background_position_t, 1);
	ccss_property_init (&self->base, peek_property_class ("background-position"));
	ret = bg_position_parse (self, &values);
	if (!ret) {
		g_free (self), self = NULL;
	}

	return &self->base;
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

static ccss_property_base_t *
background_repeat_create (ccss_grammar_t const	*grammar,
			  CRTerm const		*values)
{
	ccss_background_repeat_t	*self;
	bool				 ret;

	g_return_val_if_fail (values, NULL);

	self = g_new0 (ccss_background_repeat_t, 1);
	ccss_property_init (&self->base, peek_property_class ("background-repeat"));
	ret = bg_repeat_parse (self, &values);
	if (!ret) {
		g_free (self), self = NULL;
	}

	return &self->base;
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

static ccss_property_base_t *
background_size_create (ccss_grammar_t const	*grammar,
			CRTerm const		*values)
{
	ccss_background_size_t	*self;
	bool			 ret;

	g_return_val_if_fail (values, NULL);

	self = g_new0 (ccss_background_size_t, 1);
	ccss_property_init (&self->base, peek_property_class ("background-size"));
	ret = bg_size_parse (self, &values);
	if (!ret) {
		g_free (self), self = NULL;
	}

	return &self->base;
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
	.property_create = background_attachment_create,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) background_attachment_convert,
	.property_factory = NULL,
	.property_inherit = NULL
    }, {
	.name = "background-color",
	.property_create = ccss_color_create,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) ccss_color_convert,
	.property_factory = NULL,
	.property_inherit = NULL
    }, {
	.name = "background-image",
	.property_create = background_image_create,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) background_image_convert,
	.property_factory = NULL,
	.property_inherit = NULL
    }, {
	.name = "background-position",
	.property_create = background_position_create,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) background_position_convert,
	.property_factory = NULL,
	.property_inherit = NULL
    }, {
	.name = "background-repeat",
	.property_create = background_repeat_create,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) background_repeat_convert,
	.property_factory = NULL,
	.property_inherit = NULL
    }, {
	.name = "background-size",
	.property_create = background_size_create,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) background_size_convert,
	.property_factory = NULL,
	.property_inherit = NULL
    }, {
	.name = "background",
	.property_create = NULL,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = NULL,
	.property_factory = background_factory,
	.property_inherit = background_inherit
    }, {
	.name = NULL
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

