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
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "ccss-block-priv.h"
#include "ccss-border-parser.h"
#include "ccss-property-priv.h"

#define INSERT_BORDER_COLOR(block_, property_name_, property_)		       \
	G_STMT_START {							       \
	ccss_color_t *color;						       \
	property_.base.property_class = peek_property_class (property_name_);  \
	color = g_new0 (ccss_color_t, 1);				       \
	*color = property_;						       \
	ccss_block_add_property (block_, property_name_,		       \
				 (ccss_property_base_t *) color);	       \
	} G_STMT_END

#define INSERT_BORDER_STYLE(block_, property_name_, property_)		       \
	G_STMT_START {							       \
	ccss_border_style_t *style;					       \
	property_.base.property_class = peek_property_class (property_name_);  \
	style = g_new0 (ccss_border_style_t, 1);			       \
	*style = property_;						       \
	ccss_block_add_property (block_, property_name_,		       \
				 (ccss_property_base_t *) style);	       \
	} G_STMT_END

#define INSERT_BORDER_WIDTH(block_, property_name_, property_)		       \
	G_STMT_START {							       \
	ccss_border_width_t *width;					       \
	property_.base.property_class = peek_property_class (property_name_);  \
	width = g_new0 (ccss_border_width_t, 1);			       \
	*width = property_;						       \
	ccss_block_add_property (block_, property_name_,		       \
				 (ccss_property_base_t *) width);	       \
	} G_STMT_END

#define INSERT_BORDER_RADIUS(block_, property_name_, property_)		       \
	G_STMT_START {							       \
	ccss_border_join_t *join;					       \
	property_.base.property_class = peek_property_class (property_name_);  \
	join = g_new0 (ccss_border_join_t, 1);			       \
	*join = property_;						       \
	ccss_block_add_property (block_, property_name_,		       \
				 (ccss_property_base_t *) join);	       \
	} G_STMT_END

/*!
 * Map between border style css string and internal value.
 */
static const struct {
	ccss_border_style_type_t border_style;
	char const *css;
} _border_style_map[] = {
	{ CCSS_BORDER_STYLE_HIDDEN,	"hidden" },
	{ CCSS_BORDER_STYLE_DOTTED,	"dotted" },
	{ CCSS_BORDER_STYLE_DASHED,	"dashed" },
	{ CCSS_BORDER_STYLE_SOLID,	"solid"  },
	{ CCSS_BORDER_STYLE_DOUBLE,	"double" },
	{ CCSS_BORDER_STYLE_GROOVE,	"groove" },
	{ CCSS_BORDER_STYLE_RIDGE,	"ridge"  },
	{ CCSS_BORDER_STYLE_INSET,	"inset"  },
	{ CCSS_BORDER_STYLE_OUTSET,	"outset" }
};

static ccss_property_class_t const *
peek_property_class (char const *property_name);

static bool
match_style (char const		*css_border_style,
	     ccss_border_style_type_t	*style)
{
	g_return_val_if_fail (css_border_style && *css_border_style, false);

	for (unsigned int i = 0; i < G_N_ELEMENTS (_border_style_map); i++) {
		if (0 == strcmp (_border_style_map[i].css, css_border_style)) {
			*style = _border_style_map[i].border_style;
			return true;
		}
	}

	return false;
}

static char const *
lookup_style (ccss_border_style_type_t border_style)
{
	for (unsigned int i = 0; i < G_N_ELEMENTS (_border_style_map); i++) {
		if (_border_style_map[i].border_style == border_style) {
			return _border_style_map[i].css;
		}
	}	

	return NULL;
}

static bool
parse_width (CRTerm const		**value,
	     ccss_border_width_t	 *width)
{
	width->base.state = ccss_property_parse_state (value);
	if (CCSS_PROPERTY_STATE_SET == width->base.state &&
	    *value && 
	    TERM_NUMBER == (*value)->type) {

		width->width = (*value)->content.num->val;
		(*value) = (*value)->next;
		width->base.state = CCSS_PROPERTY_STATE_SET;

		return true;
	}

	return false;
}

static bool
parse_style (CRTerm const		**value,
	     ccss_border_style_t	 *style)
{
	style->base.state = ccss_property_parse_state (value);
	style->style = CCSS_BORDER_STYLE_SOLID;
	if (CCSS_PROPERTY_STATE_SET == style->base.state &&
	    *value && 
	    TERM_IDENT == (*value)->type &&
	    match_style (cr_string_peek_raw_str ((*value)->content.str), &style->style)) {

		(*value) = (*value)->next;
		style->base.state = CCSS_PROPERTY_STATE_SET;

		return true;
	}

	return false;
}

static bool
parse_radius (CRTerm const		**value,
	      ccss_border_join_t	 *radius)
{
	radius->base.state = ccss_property_parse_state (value);
	if (CCSS_PROPERTY_STATE_SET == radius->base.state &&
	    *value && 
	    TERM_NUMBER == (*value)->type) {

		radius->radius = (*value)->content.num->val;
		(*value) = (*value)->next;
		radius->base.state = CCSS_PROPERTY_STATE_SET;

		return true;
	}

	return false;
}


static bool
border_color_factory (ccss_block_t	*self,
		      CRTerm const	*values)
{
	CRTerm const	*iter;
	ccss_color_t	 c0, c1, c2, c3;

	memset (&c0, 0, sizeof (c0));
	memset (&c1, 0, sizeof (c1));
	memset (&c2, 0, sizeof (c2));
	memset (&c3, 0, sizeof (c3));

	iter = values;
	if (iter) { ccss_color_parse (&c0, &iter); }
	if (iter) { ccss_color_parse (&c1, &iter); }
	if (iter) { ccss_color_parse (&c2, &iter); }
	if (iter) { ccss_color_parse (&c3, &iter); }

	if (CCSS_PROPERTY_STATE_UNSET == c0.base.state) {

		return false;

	} else if (CCSS_PROPERTY_STATE_UNSET == c1.base.state) {

		INSERT_BORDER_COLOR (self, "border-color",  c0);

	} else if (CCSS_PROPERTY_STATE_UNSET == c2.base.state) {

		INSERT_BORDER_COLOR (self, "border-bottom-color",  c0);
		INSERT_BORDER_COLOR (self, "border-top-color",  c0);

		INSERT_BORDER_COLOR (self, "border-left-color",  c1);
		INSERT_BORDER_COLOR (self, "border-right-color",  c1);

	} else if (CCSS_PROPERTY_STATE_UNSET == c3.base.state) {

		INSERT_BORDER_COLOR (self, "border-top-color",  c0);

		INSERT_BORDER_COLOR (self, "border-left-color",  c1);
		INSERT_BORDER_COLOR (self, "border-right-color",  c1);

		INSERT_BORDER_COLOR (self, "border-bottom-color",  c2);

	} else if (c3.base.state != CCSS_PROPERTY_STATE_UNSET) {

		INSERT_BORDER_COLOR (self, "border-top-color",  c0);
		INSERT_BORDER_COLOR (self, "border-right-color",  c1);
		INSERT_BORDER_COLOR (self, "border-bottom-color",  c2);
		INSERT_BORDER_COLOR (self, "border-left-color",  c3);
	}

	return true;
}

static bool
border_style_factory (ccss_block_t	*self,
		      CRTerm const	*values)
{
	CRTerm const		*iter;
	ccss_border_style_t	 s0, s1, s2, s3;

	memset (&s0, 0, sizeof (s0));
	memset (&s1, 0, sizeof (s1));
	memset (&s2, 0, sizeof (s2));
	memset (&s3, 0, sizeof (s3));

	iter = values;
	if (iter) { parse_style (&iter, &s0); }
	if (iter) { parse_style (&iter, &s1); }
	if (iter) { parse_style (&iter, &s2); }
	if (iter) { parse_style (&iter, &s3); }

	if (CCSS_PROPERTY_STATE_UNSET == s0.base.state) {

		return false;

	} else if (CCSS_PROPERTY_STATE_UNSET == s1.base.state) {

		INSERT_BORDER_STYLE (self, "border-style", s0);

	} else if (CCSS_PROPERTY_STATE_UNSET == s2.base.state) {

		INSERT_BORDER_STYLE (self, "border-bottom-style", s0);
		INSERT_BORDER_STYLE (self, "border-top-style", s0);

		INSERT_BORDER_STYLE (self, "border-left-style", s1);
		INSERT_BORDER_STYLE (self, "border-right-style", s1);

	} else if (CCSS_PROPERTY_STATE_UNSET == s3.base.state) {

		INSERT_BORDER_STYLE (self, "border-top-style", s0);

		INSERT_BORDER_STYLE (self, "border-left-style", s1);
		INSERT_BORDER_STYLE (self, "border-right-style", s1);

		INSERT_BORDER_STYLE (self, "border-bottom-style", s2);

	} else if (s3.base.state != CCSS_PROPERTY_STATE_UNSET) {

		INSERT_BORDER_STYLE (self, "border-top-style", s0);
		INSERT_BORDER_STYLE (self, "border-right-style", s1);
		INSERT_BORDER_STYLE (self, "border-bottom-style", s2);
		INSERT_BORDER_STYLE (self, "border-left-style", s3);
	}

	return true;
}

static bool
border_width_factory (ccss_block_t	*self,
		      CRTerm const	*values)
{
	CRTerm const		*iter;
	ccss_border_width_t	 w0, w1, w2, w3;

	memset (&w0, 0, sizeof (w0));
	memset (&w1, 0, sizeof (w1));
	memset (&w2, 0, sizeof (w2));
	memset (&w3, 0, sizeof (w3));

	iter = values;
	if (iter) { parse_width (&iter, &w0); }
	if (iter) { parse_width (&iter, &w1); }
	if (iter) { parse_width (&iter, &w2); }
	if (iter) { parse_width (&iter, &w3); }

	if (CCSS_PROPERTY_STATE_UNSET == w0.base.state) {

		return false;

	} else if (CCSS_PROPERTY_STATE_UNSET == w1.base.state) {

		INSERT_BORDER_WIDTH (self, "border-width", w0);

	} else if (CCSS_PROPERTY_STATE_UNSET == w2.base.state) {

		INSERT_BORDER_WIDTH (self, "border-bottom-width", w0);
		INSERT_BORDER_WIDTH (self, "border-top-width", w0);

		INSERT_BORDER_WIDTH (self, "border-left-width", w1);
		INSERT_BORDER_WIDTH (self, "border-right-width", w1);

	} else if (CCSS_PROPERTY_STATE_UNSET == w3.base.state) {

		INSERT_BORDER_WIDTH (self, "border-top-width", w0);

		INSERT_BORDER_WIDTH (self, "border-left-width", w1);
		INSERT_BORDER_WIDTH (self, "border-right-width", w1);

		INSERT_BORDER_WIDTH (self, "border-bottom-width", w2);

	} else if (w3.base.state != CCSS_PROPERTY_STATE_UNSET) {

		INSERT_BORDER_WIDTH (self, "border-top-width", w0);
		INSERT_BORDER_WIDTH (self, "border-right-width", w1);
		INSERT_BORDER_WIDTH (self, "border-bottom-width", w2);
		INSERT_BORDER_WIDTH (self, "border-left-width", w3);
	}

	return true;
}

/*
 * Parse properties of the form
 * - border: ;		# only to prevent errors
 * - border: 1px;
 * - border: solid;
 * - border: 1px solid;
 * - border: red;
 * - border: 1px red;
 * - border: 1px solid red;
 */
static bool
border_factory_impl (ccss_block_t	*self,
		     char const		*property_prefix,
		     CRTerm const	*values)
{
	CRTerm const		*iter;
	ccss_color_t		*color, c;
	ccss_border_style_t	*style, s;
	ccss_border_width_t	*width, w;
	char			*property_name;

	memset (&c, 0, sizeof (c));
	memset (&s, 0, sizeof (s));
	memset (&w, 0, sizeof (w));

	iter = values;
	parse_width (&iter, width);

	if (iter) {
		parse_style (&iter, style);		
	}

	if (iter) {
		ccss_color_parse (color, &iter);
	}

	if (c.base.state != CCSS_PROPERTY_STATE_UNSET) { 

		property_name = g_strdup_printf ("%s-color", property_prefix);
		INSERT_BORDER_COLOR (self, property_name, c);
		g_free (property_name), property_name = NULL;
	}

	if (s.base.state != CCSS_PROPERTY_STATE_UNSET) {

		property_name = g_strdup_printf ("%s-style", property_prefix);
		INSERT_BORDER_STYLE (self, property_name, s);
		g_free (property_name), property_name = NULL;
	}

	if (w.base.state != CCSS_PROPERTY_STATE_UNSET) {

		property_name = g_strdup_printf ("%s-width", property_prefix);
		INSERT_BORDER_WIDTH (self, property_name, w);
		g_free (property_name), property_name = NULL;
	}

	/* FIXME: detect as error if no property valid. */

	return true;
}

static bool
border_factory (ccss_block_t	*self,
		CRTerm const	*values)
{
	return border_factory_impl (self, "border", values);
}

static bool
border_left_factory (ccss_block_t	*self,
		     CRTerm const	*values)
{
	return border_factory_impl (self, "border-left", values);
}

static bool
border_top_factory (ccss_block_t	*self,
		    CRTerm const	*values)
{
	return border_factory_impl (self, "border-top", values);
}

static bool
border_right_factory (ccss_block_t	*self,
		      CRTerm const	*values)
{
	return border_factory_impl (self, "border-right", values);
}

static bool
border_bottom_factory (ccss_block_t	*self,
		       CRTerm const	*values)
{
	return border_factory_impl (self, "border-bottom", values);
}

static ccss_border_join_t *
border_radius_new (CRTerm const *value)
{
	ccss_border_join_t *radius, r;

	memset (&r, 0, sizeof (r));
	parse_radius (&value, &r);
	if (CCSS_PROPERTY_STATE_UNSET == r.base.state) {
		return NULL;
	}

	r.base.property_class = peek_property_class ("border-radius");
	radius = g_new0 (ccss_border_join_t, 1);
	*radius = r;
	return radius;
}

static bool
border_radius_factory (ccss_block_t	*self,
		       CRTerm const	*values)
{
	CRTerm const		*iter;
	ccss_border_join_t	 r0, r1, r2, r3;

	memset (&r0, 0, sizeof (r0));
	memset (&r1, 0, sizeof (r1));
	memset (&r2, 0, sizeof (r2));
	memset (&r3, 0, sizeof (r3));

	iter = values;
	if (iter) { parse_radius (&iter, &r0); }
	if (iter) { parse_radius (&iter, &r1); }
	if (iter) { parse_radius (&iter, &r2); }
	if (iter) { parse_radius (&iter, &r3); }

	if (CCSS_PROPERTY_STATE_UNSET == r0.base.state) {

		return false;

	} else if (CCSS_PROPERTY_STATE_UNSET == r1.base.state) {

		INSERT_BORDER_RADIUS (self, "border-radius", r0);

	} else if (CCSS_PROPERTY_STATE_UNSET == r2.base.state) {

		INSERT_BORDER_RADIUS (self, "border-top-left-radius", r0);
		INSERT_BORDER_RADIUS (self, "border-top-right-radius", r1);
		INSERT_BORDER_RADIUS (self, "border-bottom-right-radius", r0);
		INSERT_BORDER_RADIUS (self, "border-bottom-left-radius", r1);

	} else if (CCSS_PROPERTY_STATE_UNSET == r3.base.state) {

		INSERT_BORDER_RADIUS (self, "border-top-left-radius", r0);
		INSERT_BORDER_RADIUS (self, "border-top-right-radius", r1);
		INSERT_BORDER_RADIUS (self, "border-bottom-right-radius", r2);
		INSERT_BORDER_RADIUS (self, "border-bottom-left-radius", r1);

	} else if (r3.base.state != CCSS_PROPERTY_STATE_UNSET) {

		INSERT_BORDER_RADIUS (self, "border-top-left-radius", r0);
		INSERT_BORDER_RADIUS (self, "border-top-right-radius", r1);
		INSERT_BORDER_RADIUS (self, "border-bottom-right-radius", r2);
		INSERT_BORDER_RADIUS (self, "border-bottom-left-radius", r3);
	}

	return true;
}

static bool
border_radius_convert (ccss_border_join_t const	*property,
		       ccss_property_type_t	 target,
		       void			*value)
{
	g_return_val_if_fail (property && value, false);

	switch (target) {
	case CCSS_PROPERTY_TYPE_DOUBLE:
		* (double *) value = property->radius;
		return true;
	case CCSS_PROPERTY_TYPE_STRING:
		* (char **) value = g_strdup_printf ("%f", property->radius);
		return true;
	default:
		g_assert_not_reached ();
		return false;
	}

	return false;
}

static ccss_border_style_t *
border_style_new (CRTerm const *value)
{
	ccss_border_style_t *self, s;

	memset (&s, 0, sizeof (s));
	parse_style (&value, &s);
	if (CCSS_PROPERTY_STATE_UNSET == s.base.state) {
		return NULL;
	}

	s.base.property_class = peek_property_class ("border-style");
	self = g_new0 (ccss_border_style_t, 1);
	*self = s;
	return self;
}

static bool
border_style_convert (ccss_border_style_t const	*property,
		      ccss_property_type_t	 target,
		      void			*value)
{
	g_return_val_if_fail (property && value, false);

	if (CCSS_PROPERTY_TYPE_DOUBLE == target)
		return false;

	* (char **) value = g_strdup (lookup_style (property->style));

	return true;
}

static ccss_border_width_t *
border_width_new (CRTerm const *value)
{
	ccss_border_width_t *self, w;

	memset (&w, 0, sizeof (w));
	parse_width (&value, &w);
	if (CCSS_PROPERTY_STATE_UNSET == w.base.state) {
		return NULL;
	}

	w.base.property_class = peek_property_class ("border-width");
	self = g_new0 (ccss_border_width_t, 1);
	*self = w;
	return self;
}

static bool
border_width_convert (ccss_border_width_t const	*property,
		      ccss_property_type_t	 target,
		      void			*value)
{
	g_return_val_if_fail (property && value, false);

	switch (target) {
	case CCSS_PROPERTY_TYPE_DOUBLE:
		* (double *) value = property->width;
		return true;
	case CCSS_PROPERTY_TYPE_STRING:
		* (char **) value = g_strdup_printf ("%f", property->width);
		return true;
	default:
		g_assert_not_reached ();
		return false;
	}

	return false;
}

static ccss_property_class_t const _ptable[] = {
    {
	.name = "border-top-right-radius",
	.property_new = (ccss_property_new_f) border_radius_new,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = (ccss_property_convert_f) border_radius_convert,
	.property_factory = NULL
    }, {
	.name = "border-bottom-right-radius",
	.property_new = (ccss_property_new_f) border_radius_new,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = (ccss_property_convert_f) border_radius_convert,
	.property_factory = NULL
    }, {
	.name = "border-bottom-left-radius",
	.property_new = (ccss_property_new_f) border_radius_new,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = (ccss_property_convert_f) border_radius_convert,
	.property_factory = NULL
    }, {
	.name = "border-top-left-radius",
	.property_new = (ccss_property_new_f) border_radius_new,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = (ccss_property_convert_f) border_radius_convert,
	.property_factory = NULL
    }, {
	.name = "border-radius",
	.property_new = NULL,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = (ccss_property_convert_f) border_radius_convert,
	.property_factory = (ccss_property_factory_f) border_radius_factory
    }, {
	.name = "border-left-color",
	.property_new = (ccss_property_new_f) ccss_color_new,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = (ccss_property_convert_f) ccss_color_convert,
	.property_factory = NULL
    }, {
	.name = "border-left-style",
	.property_new = (ccss_property_new_f) border_style_new,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = (ccss_property_convert_f) border_style_convert,
	.property_factory = NULL
    }, {
	.name = "border-left-width",
	.property_new = (ccss_property_new_f) border_width_new,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = (ccss_property_convert_f) border_width_convert,
	.property_factory = NULL
    }, {
	.name = "border-top-color",
	.property_new = (ccss_property_new_f) ccss_color_new,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = (ccss_property_convert_f) ccss_color_convert,
	.property_factory = NULL
    }, {
	.name = "border-top-style",
	.property_new = (ccss_property_new_f) border_style_new,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = (ccss_property_convert_f) border_style_convert,
	.property_factory = NULL
    }, {
	.name = "border-top-width",
	.property_new = (ccss_property_new_f) border_width_new,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = (ccss_property_convert_f) border_width_convert,
	.property_factory = NULL
    }, {
	.name = "border-right-color",
	.property_new = (ccss_property_new_f) ccss_color_new,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = (ccss_property_convert_f) ccss_color_convert,
	.property_factory = NULL
    }, {
	.name = "border-right-style",
	.property_new = (ccss_property_new_f) border_style_new,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = (ccss_property_convert_f) border_style_convert,
	.property_factory = NULL
    }, {
	.name = "border-right-width",
	.property_new = (ccss_property_new_f) border_width_new,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = (ccss_property_convert_f) border_width_convert,
	.property_factory = NULL
    }, {
	.name = "border-bottom-color",
	.property_new = (ccss_property_new_f) ccss_color_new,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = (ccss_property_convert_f) ccss_color_convert,
	.property_factory = NULL
    }, {
	.name = "border-bottom-style",
	.property_new = (ccss_property_new_f) border_style_new,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = (ccss_property_convert_f) border_style_convert,
	.property_factory = NULL
    }, {
	.name = "border-bottom-width",
	.property_new = (ccss_property_new_f) border_width_new,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = (ccss_property_convert_f) border_width_convert,
	.property_factory = NULL
    }, {
	.name = "border-left",
	.property_new = NULL,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = NULL,
	.property_factory = (ccss_property_factory_f) border_left_factory
    }, {
	.name = "border-top",
	.property_new = NULL,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = NULL,
	.property_factory = (ccss_property_factory_f) border_top_factory
    }, {
	.name = "border-right",
	.property_new = NULL,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = NULL,
	.property_factory = (ccss_property_factory_f) border_right_factory
    }, {
	.name = "border-bottom",
	.property_new = NULL,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = NULL,
	.property_factory = (ccss_property_factory_f) border_bottom_factory
    }, {
	.name = "border-color",
	.property_new = NULL,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = NULL,
	.property_factory = (ccss_property_factory_f) border_color_factory
    }, {
	.name = "border-style",
	.property_new = NULL,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = NULL,
	.property_factory = (ccss_property_factory_f) border_style_factory
    }, {
	.name = "border-width",
	.property_new = NULL,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = NULL,
	.property_factory = (ccss_property_factory_f) border_width_factory
    }, {
	.name = "border",
	.property_new = NULL,
	.property_free = (ccss_property_free_f) g_free,
	.property_convert = NULL,
	.property_factory = (ccss_property_factory_f) border_factory
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
ccss_border_get_ptable (void)
{
	return _ptable;
}

