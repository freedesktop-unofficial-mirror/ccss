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
#include "ccss-border.h"
#include "ccss-border-parser.h"
#include "ccss-cairo-color-priv.h"
#include "config.h"

#define INSERT_BORDER_COLOR(block_, prop_name_, prop_, prop_ptr_)	       \
	G_STMT_START {							       \
	prop_.base.property_class = peek_property_class (prop_name_);	       \
	prop_ptr_ = g_new0 (ccss_cairo_color_t, 1);				       \
	*prop_ptr_ = prop_;						       \
	ccss_block_add_property (block_, prop_name_, &prop_ptr_->base);	       \
	} G_STMT_END

#define INSERT_BORDER_STYLE(block_, prop_name_, prop_, prop_ptr_)	       \
	G_STMT_START {							       \
	prop_.base.property_class = peek_property_class (prop_name_);	       \
	prop_ptr_ = g_new0 (ccss_border_style_t, 1);			       \
	*prop_ptr_ = prop_;						       \
	ccss_block_add_property (block_, prop_name_, &prop_ptr_->base);	       \
	} G_STMT_END

#define INSERT_BORDER_WIDTH(block_, prop_name_, prop_, prop_ptr_)	       \
	G_STMT_START {							       \
	prop_.base.property_class = peek_property_class (prop_name_);	       \
	prop_ptr_ = g_new0 (ccss_border_width_t, 1);			       \
	*prop_ptr_ = prop_;						       \
	ccss_block_add_property (block_, prop_name_, &prop_ptr_->base);	       \
	} G_STMT_END

#define INSERT_BORDER_RADIUS(block_, prop_name_, prop_, prop_ptr_)	       \
	G_STMT_START {							       \
	prop_.base.property_class = peek_property_class (prop_name_);	       \
	prop_ptr_ = g_new0 (ccss_border_join_t, 1);			       \
	*prop_ptr_ = prop_;						       \
	ccss_block_add_property (block_, prop_name_, &prop_ptr_->base);	       \
	} G_STMT_END

#define PROPERTY_SET(property_)					\
	(CCSS_PROPERTY_STATE_NONE == property_->state ||	\
	 CCSS_PROPERTY_STATE_SET == property_->state)

#define PROPERTY_INHERIT(property_)				\
	(CCSS_PROPERTY_STATE_INHERIT == property_->state)

typedef struct {
	ccss_property_base_t	 base;
	ccss_border_width_t	*width;
	ccss_border_style_t	*style;
	ccss_cairo_color_t const	*color;
} border_property_t;

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
border_color_factory (ccss_grammar_t const	*grammar,
		      ccss_block_t		*self,
		      char const		*name,
		      CRTerm const		*values,
		      void			*user_data)
{
	CRTerm const	*iter;
	ccss_cairo_color_t	 c, c0, c1, c2, c3;
	ccss_cairo_color_t	*color;

	/* If `border-color: inherit;' then insert a dummy. */
	memset (&c, 0, sizeof (c));
	ccss_property_init (&c.base, peek_property_class ("border-color"));
	c.base.state = ccss_property_parse_state (&values);
	if (c.base.state == CCSS_PROPERTY_STATE_INHERIT) {
		color = g_new0 (ccss_cairo_color_t, 1);
		*color = c;
		ccss_block_add_property (self, "border-color", &color->base);
		return true;
	}

	memset (&c0, 0, sizeof (c0));
	memset (&c1, 0, sizeof (c1));
	memset (&c2, 0, sizeof (c2));
	memset (&c3, 0, sizeof (c3));

	iter = values;
	if (iter) { ccss_cairo_color_parse (&c0, grammar, user_data, &iter); }
	if (iter) { ccss_cairo_color_parse (&c1, grammar, user_data, &iter); }
	if (iter) { ccss_cairo_color_parse (&c2, grammar, user_data, &iter); }
	if (iter) { ccss_cairo_color_parse (&c3, grammar, user_data, &iter); }

	if (CCSS_PROPERTY_STATE_INVALID == c0.base.state) {

		return false;

	} else if (CCSS_PROPERTY_STATE_INVALID == c1.base.state) {

		INSERT_BORDER_COLOR (self, "border-color",  c0, color);

	} else if (CCSS_PROPERTY_STATE_INVALID == c2.base.state) {

		INSERT_BORDER_COLOR (self, "border-bottom-color",  c0, color);
		INSERT_BORDER_COLOR (self, "border-top-color",  c0, color);

		INSERT_BORDER_COLOR (self, "border-left-color",  c1, color);
		INSERT_BORDER_COLOR (self, "border-right-color",  c1, color);

	} else if (CCSS_PROPERTY_STATE_INVALID == c3.base.state) {

		INSERT_BORDER_COLOR (self, "border-top-color",  c0, color);

		INSERT_BORDER_COLOR (self, "border-left-color",  c1, color);
		INSERT_BORDER_COLOR (self, "border-right-color",  c1, color);

		INSERT_BORDER_COLOR (self, "border-bottom-color",  c2, color);

	} else if (c3.base.state != CCSS_PROPERTY_STATE_INVALID) {

		INSERT_BORDER_COLOR (self, "border-top-color",  c0, color);
		INSERT_BORDER_COLOR (self, "border-right-color",  c1, color);
		INSERT_BORDER_COLOR (self, "border-bottom-color",  c2, color);
		INSERT_BORDER_COLOR (self, "border-left-color",  c3, color);
	}

	return true;
}

static bool 
border_color_inherit (ccss_style_t const	*container_style,
		      ccss_style_t		*style)
{
	ccss_property_base_t const	*property;
	bool				 ret;

	ret = false;

	if (ccss_style_get_property (container_style,
				"border-color", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"border-color", property);
			ret = true;
		}
	}
	if (ccss_style_get_property (container_style, 
				"border-left-color", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"border-left-color", property);
		} else if (PROPERTY_INHERIT (property)) {
			/* Need to resolve further. */
			ret = false;			
		}
	}
	if (ccss_style_get_property (container_style,
				"border-top-color", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"border-top-color", property);
		} else if (PROPERTY_INHERIT (property)) {
			/* Need to resolve further. */
			ret = false;			
		}
	}
	if (ccss_style_get_property (container_style,
				"border-right-color", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"border-right-color", property);
		} else if (PROPERTY_INHERIT (property)) {
			/* Need to resolve further. */
			ret = false;			
		}
	}
	if (ccss_style_get_property (container_style,
				"border-bottom-color", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"border-bottom-color", property);
		} else if (PROPERTY_INHERIT (property)) {
			/* Need to resolve further. */
			ret = false;			
		}
	}

	return ret;
}

static bool
border_style_factory (ccss_grammar_t const	*grammar,
		      ccss_block_t		*self,
		      CRTerm const		*values,
		      void			*user_data)
{
	CRTerm const		*iter;
	ccss_border_style_t	 s, s0, s1, s2, s3;
	ccss_border_style_t	*style;

	/* If `border-style: inherit;' then insert a dummy */
	memset (&s, 0, sizeof (s));
	ccss_property_init (&s.base, peek_property_class ("border-style"));
	s.base.state = ccss_property_parse_state (&values);
	if (s.base.state == CCSS_PROPERTY_STATE_INHERIT) {
		style = g_new0 (ccss_border_style_t, 1);
		*style = s;
		ccss_block_add_property (self, "border-style", &style->base);
		return true;
	}

	memset (&s0, 0, sizeof (s0));
	memset (&s1, 0, sizeof (s1));
	memset (&s2, 0, sizeof (s2));
	memset (&s3, 0, sizeof (s3));

	iter = values;
	if (iter) { parse_style (&iter, &s0); }
	if (iter) { parse_style (&iter, &s1); }
	if (iter) { parse_style (&iter, &s2); }
	if (iter) { parse_style (&iter, &s3); }

	if (CCSS_PROPERTY_STATE_INVALID == s0.base.state) {

		return false;

	} else if (CCSS_PROPERTY_STATE_INVALID == s1.base.state) {

		INSERT_BORDER_STYLE (self, "border-style", s0, style);

	} else if (CCSS_PROPERTY_STATE_INVALID == s2.base.state) {

		INSERT_BORDER_STYLE (self, "border-bottom-style", s0, style);
		INSERT_BORDER_STYLE (self, "border-top-style", s0, style);

		INSERT_BORDER_STYLE (self, "border-left-style", s1, style);
		INSERT_BORDER_STYLE (self, "border-right-style", s1, style);

	} else if (CCSS_PROPERTY_STATE_INVALID == s3.base.state) {

		INSERT_BORDER_STYLE (self, "border-top-style", s0, style);

		INSERT_BORDER_STYLE (self, "border-left-style", s1, style);
		INSERT_BORDER_STYLE (self, "border-right-style", s1, style);

		INSERT_BORDER_STYLE (self, "border-bottom-style", s2, style);

	} else if (s3.base.state != CCSS_PROPERTY_STATE_INVALID) {

		INSERT_BORDER_STYLE (self, "border-top-style", s0, style);
		INSERT_BORDER_STYLE (self, "border-right-style", s1, style);
		INSERT_BORDER_STYLE (self, "border-bottom-style", s2, style);
		INSERT_BORDER_STYLE (self, "border-left-style", s3, style);
	}

	return true;
}

static bool 
border_style_inherit (ccss_style_t const	*container_style,
		      ccss_style_t		*style)
{
	ccss_property_base_t const	*property;
	bool				 ret;

	ret = false;

	if (ccss_style_get_property (container_style,
				"border-style", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"border-style", property);
			ret = true;
		}
	}
	if (ccss_style_get_property (container_style,
				"border-left-style", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"border-left-style", property);
		} else if (PROPERTY_INHERIT (property)) {
			/* Need to resolve further. */
			ret = false;
		}
	}
	if (ccss_style_get_property (container_style,
				"border-top-style", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"border-top-style", property);
		} else if (PROPERTY_INHERIT (property)) {
			/* Need to resolve further. */
			ret = false;
		}
	}
	if (ccss_style_get_property (container_style,
				"border-right-style", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"border-right-style", property);
		} else if (PROPERTY_INHERIT (property)) {
			/* Need to resolve further. */
			ret = false;
		}
	}
	if (ccss_style_get_property (container_style,
				"border-bottom-style", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"border-bottom-style", property);
		} else if (PROPERTY_INHERIT (property)) {
			/* Need to resolve further. */
			ret = false;
		}
	}

	return ret;
}

static bool
border_width_factory (ccss_grammar_t const	*grammar,
		      ccss_block_t		*self,
		      CRTerm const		*values,
		      void			*user_data)
{
	CRTerm const		*iter;
	ccss_border_width_t	 w, w0, w1, w2, w3;
	ccss_border_width_t	*width;

	/* If `border-style: inherit;' then insert a dummy. */
	memset (&w, 0, sizeof (w));
	ccss_property_init (&w.base, peek_property_class ("border-width"));
	w.base.state = ccss_property_parse_state (&values);
	if (w.base.state == CCSS_PROPERTY_STATE_INHERIT) {
		width = g_new0 (ccss_border_width_t, 1);
		*width = w;
		ccss_block_add_property (self, "border-width", &width->base);
		return true;
	}

	memset (&w0, 0, sizeof (w0));
	memset (&w1, 0, sizeof (w1));
	memset (&w2, 0, sizeof (w2));
	memset (&w3, 0, sizeof (w3));

	iter = values;
	if (iter) { parse_width (&iter, &w0); }
	if (iter) { parse_width (&iter, &w1); }
	if (iter) { parse_width (&iter, &w2); }
	if (iter) { parse_width (&iter, &w3); }

	if (CCSS_PROPERTY_STATE_INVALID == w0.base.state) {

		return false;

	} else if (CCSS_PROPERTY_STATE_INVALID == w1.base.state) {

		INSERT_BORDER_WIDTH (self, "border-width", w0, width);

	} else if (CCSS_PROPERTY_STATE_INVALID == w2.base.state) {

		INSERT_BORDER_WIDTH (self, "border-bottom-width", w0, width);
		INSERT_BORDER_WIDTH (self, "border-top-width", w0, width);

		INSERT_BORDER_WIDTH (self, "border-left-width", w1, width);
		INSERT_BORDER_WIDTH (self, "border-right-width", w1, width);

	} else if (CCSS_PROPERTY_STATE_INVALID == w3.base.state) {

		INSERT_BORDER_WIDTH (self, "border-top-width", w0, width);

		INSERT_BORDER_WIDTH (self, "border-left-width", w1, width);
		INSERT_BORDER_WIDTH (self, "border-right-width", w1, width);

		INSERT_BORDER_WIDTH (self, "border-bottom-width", w2, width);

	} else if (w3.base.state != CCSS_PROPERTY_STATE_INVALID) {

		INSERT_BORDER_WIDTH (self, "border-top-width", w0, width);
		INSERT_BORDER_WIDTH (self, "border-right-width", w1, width);
		INSERT_BORDER_WIDTH (self, "border-bottom-width", w2, width);
		INSERT_BORDER_WIDTH (self, "border-left-width", w3, width);
	}

	return true;
}

static bool 
border_width_inherit (ccss_style_t const	*container_style,
		      ccss_style_t		*style)
{
	ccss_property_base_t const	*property;
	bool				 ret;

	if (ccss_style_get_property (container_style,
				"border-width", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"border-width", property);
			ret = true;
		}
	}
	if (ccss_style_get_property (container_style,
				"border-left-width", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"border-left-width", property);
		} else if (PROPERTY_INHERIT (property)) {
			/* Need to resolve further. */
			ret = false;
		}
	}
	if (ccss_style_get_property (container_style,
				"border-top-width", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"border-top-width", property);
		} else if (PROPERTY_INHERIT (property)) {
			/* Need to resolve further. */
			ret = false;
		}
	}
	if (ccss_style_get_property (container_style,
				"border-right-width", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"border-right-width", property);
		} else if (PROPERTY_INHERIT (property)) {
			/* Need to resolve further. */
			ret = false;
		}
	}
	if (ccss_style_get_property (container_style,
				"border-bottom-width", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"border-bottom-width", property);
		} else if (PROPERTY_INHERIT (property)) {
			/* Need to resolve further. */
			ret = false;
		}
	}

	return ret;
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
border_factory_impl (ccss_grammar_t const	*grammar,
		     ccss_block_t		*self,
		     char const			*property_prefix,
		     CRTerm const		*values,
		     void			*user_data)
{
	CRTerm const		*iter;
	border_property_t	*border, b;
	ccss_cairo_color_t		 c;
	ccss_border_style_t	 s;
	ccss_border_width_t	 w;
	char			*property_name;

	/* If `border: inherit;' then insert a dummy. */
	memset (&b, 0, sizeof (b));
	ccss_property_init (&b.base, peek_property_class ("border"));
	b.base.state = ccss_property_parse_state (&values);
	if (b.base.state == CCSS_PROPERTY_STATE_INHERIT) {
		border = g_new0 (border_property_t, 1);
		*border = b;
		ccss_block_add_property (self, "border", &border->base);
		return true;
	}

	memset (&c, 0, sizeof (c));
	memset (&s, 0, sizeof (s));
	memset (&w, 0, sizeof (w));

	iter = values;
	parse_width (&iter, &w);

	if (iter) {
		parse_style (&iter, &s);
	}

	if (iter) {
		ccss_cairo_color_parse (&c, grammar, user_data, &iter);
	}

	if (c.base.state != CCSS_PROPERTY_STATE_INVALID) { 

		ccss_cairo_color_t *color = NULL;
		property_name = g_strdup_printf ("%s-color", property_prefix);
		INSERT_BORDER_COLOR (self, property_name, c, color);
		g_free (property_name), property_name = NULL;
		b.color = color;
	}

	if (s.base.state != CCSS_PROPERTY_STATE_INVALID) {

		ccss_border_style_t *style = NULL;
		property_name = g_strdup_printf ("%s-style", property_prefix);
		INSERT_BORDER_STYLE (self, property_name, s, style);
		g_free (property_name), property_name = NULL;
		b.style = style;
	}

	if (w.base.state != CCSS_PROPERTY_STATE_INVALID) {

		ccss_border_width_t *width = NULL;
		property_name = g_strdup_printf ("%s-width", property_prefix);
		INSERT_BORDER_WIDTH (self, property_name, w, width);
		g_free (property_name), property_name = NULL;
		b.width = width;
	}

	if (b.color || b.style || b.width) {

		border = g_new0 (border_property_t, 1);
		*border = b;
		ccss_block_add_property (self, "border", &border->base);
		return true;
	}

	return false;
}

/* FIXME: make use of property names being passed now. */

static bool
border_factory (ccss_grammar_t const	*grammar,
		ccss_block_t		*self,
		char const		*name,
		CRTerm const		*values,
		void			*user_data)
{
	return border_factory_impl (grammar, self, "border",
				    values, user_data);
}

static bool
border_left_factory (ccss_grammar_t const	*grammar,
		     ccss_block_t		*self,
		     char const			*name,
		     CRTerm const		*values,
		     void			*user_data)
{
	return border_factory_impl (grammar, self, "border-left",
				    values, user_data);
}

static bool
border_top_factory (ccss_grammar_t const	*grammar,
		    ccss_block_t		*self,
		    char const			*name,
		    CRTerm const		*values,
		    void			*user_data)
{
	return border_factory_impl (grammar, self, "border-top",
				    values, user_data);
}

static bool
border_right_factory (ccss_grammar_t const	*grammar,
		      ccss_block_t		*self,
		      char const		*name,
		      CRTerm const		*values,
		      void			*user_data)
{
	return border_factory_impl (grammar, self, "border-right",
				    values, user_data);
}

static bool
border_bottom_factory (ccss_grammar_t const	*grammar,
		       ccss_block_t		*self,
		       char const		*name,
		       CRTerm const		*values,
		       void			*user_data)
{
	return border_factory_impl (grammar, self, "border-bottom",
				    values, user_data);
}

static bool 
border_side_inherit_impl (ccss_style_t const	*container_style,
			  char const		*side,
			  ccss_style_t		*style)
{
	ccss_property_base_t const	*property;
	char				*property_name;
	bool				 ret;

	g_return_val_if_fail (container_style && side && style, false);

	ret = true;

	property_name = g_strdup_printf ("border-%s-color", side);
	if (ccss_style_get_property (container_style,
				     property_name, &property) &&
	    PROPERTY_SET (property)) {

		ccss_style_set_property (style, property_name, property);

	} else if (ccss_style_get_property (container_style,
					    "border-color", &property) &&
		   PROPERTY_SET (property)) {

		/* Inherit general property. */
		ccss_style_set_property (style, property_name, property);
	} else {
		ret = false;
	}
	g_free (property_name), property_name = NULL;

	property_name = g_strdup_printf ("border-%s-style", side);
	if (ccss_style_get_property (container_style,
				     property_name, &property) &&
	    PROPERTY_SET (property)) {

		ccss_style_set_property (style, property_name, property);

	} else if (ccss_style_get_property (container_style,
					    "border-style", &property) &&
		   PROPERTY_SET (property)) {

		/* Inherit general property. */
		ccss_style_set_property (style, property_name, property);
	} else {
		ret = false;
	}
	g_free (property_name), property_name = NULL;

	property_name = g_strdup_printf ("border-%s-width", side);
	if (ccss_style_get_property (container_style,
				     property_name, &property) &&
	    PROPERTY_SET (property)) {

		ccss_style_set_property (style, property_name, property);

	} else if (ccss_style_get_property (container_style,
					    "border-width", &property) &&
		   PROPERTY_SET (property)) {

		/* Inherit general property. */
		ccss_style_set_property (style, property_name, property);
	} else {
		ret = false;
	}
	g_free (property_name), property_name = NULL;

	return ret;
}

static bool 
border_left_inherit (ccss_style_t const	*container_style,
		     ccss_style_t	*style)
{
	return border_side_inherit_impl (container_style, "left", style);
}

static bool 
border_top_inherit (ccss_style_t const	*container_style,
		    ccss_style_t	*style)
{
	return border_side_inherit_impl (container_style, "top", style);
}

static bool 
border_right_inherit (ccss_style_t const	*container_style,
		      ccss_style_t		*style)
{
	return border_side_inherit_impl (container_style, "right", style);
}

static bool 
border_bottom_inherit (ccss_style_t const	*container_style,
		       ccss_style_t		*style)
{
	return border_side_inherit_impl (container_style, "bottom", style);
}

static bool 
border_inherit (ccss_style_t const	*container_style,
		ccss_style_t		*style)
{
	bool ret;

	ret = true;

	ret &= border_left_inherit (container_style, style);	
	ret &= border_top_inherit (container_style, style);	
	ret &= border_right_inherit (container_style, style);	
	ret &= border_bottom_inherit (container_style, style);	

	return ret;
}

static ccss_property_base_t *
border_radius_create (ccss_grammar_t const	*grammar,
		      CRTerm const		*value,
		      void			*user_data)
{
	ccss_border_join_t *radius, r;

	memset (&r, 0, sizeof (r));
	ccss_property_init (&r.base, peek_property_class ("border-radius"));

	parse_radius (&value, &r);
	if (CCSS_PROPERTY_STATE_INVALID == r.base.state) {
		return NULL;
	}

	radius = g_new0 (ccss_border_join_t, 1);
	*radius = r;
	return &radius->base;
}

static bool
border_radius_factory (ccss_grammar_t const	*grammar,
		       ccss_block_t		*self,
		       char const		*name,
		       CRTerm const		*values,
		       void			*user_data)
{
	CRTerm const		*iter;
	ccss_border_join_t	 r, r0, r1, r2, r3;
	ccss_border_join_t	*radius;

	/* If `border-radius: inherit;' then insert a dummy. */
	memset (&r, 0, sizeof (r));
	ccss_property_init (&r.base, peek_property_class ("border-radius"));
	r.base.state = ccss_property_parse_state (&values);
	if (r.base.state == CCSS_PROPERTY_STATE_INHERIT) {
		radius = g_new0 (ccss_border_join_t, 1);
		*radius = r;
		ccss_block_add_property (self, "border-radius", &radius->base);
		return true;
	}

	memset (&r0, 0, sizeof (r0));
	memset (&r1, 0, sizeof (r1));
	memset (&r2, 0, sizeof (r2));
	memset (&r3, 0, sizeof (r3));

	iter = values;
	if (iter) { parse_radius (&iter, &r0); }
	if (iter) { parse_radius (&iter, &r1); }
	if (iter) { parse_radius (&iter, &r2); }
	if (iter) { parse_radius (&iter, &r3); }

	if (CCSS_PROPERTY_STATE_INVALID == r0.base.state) {

		return false;

	} else if (CCSS_PROPERTY_STATE_INVALID == r1.base.state) {

		INSERT_BORDER_RADIUS (self, "border-radius", r0, radius);

	} else if (CCSS_PROPERTY_STATE_INVALID == r2.base.state) {

		INSERT_BORDER_RADIUS (self, "border-top-left-radius", r0, radius);
		INSERT_BORDER_RADIUS (self, "border-top-right-radius", r1, radius);
		INSERT_BORDER_RADIUS (self, "border-bottom-right-radius", r0, radius);
		INSERT_BORDER_RADIUS (self, "border-bottom-left-radius", r1, radius);

	} else if (CCSS_PROPERTY_STATE_INVALID == r3.base.state) {

		INSERT_BORDER_RADIUS (self, "border-top-left-radius", r0, radius);
		INSERT_BORDER_RADIUS (self, "border-top-right-radius", r1, radius);
		INSERT_BORDER_RADIUS (self, "border-bottom-right-radius", r2, radius);
		INSERT_BORDER_RADIUS (self, "border-bottom-left-radius", r1, radius);

	} else if (r3.base.state != CCSS_PROPERTY_STATE_INVALID) {

		INSERT_BORDER_RADIUS (self, "border-top-left-radius", r0, radius);
		INSERT_BORDER_RADIUS (self, "border-top-right-radius", r1, radius);
		INSERT_BORDER_RADIUS (self, "border-bottom-right-radius", r2, radius);
		INSERT_BORDER_RADIUS (self, "border-bottom-left-radius", r3, radius);
	}

	return true;
}

static bool 
border_radius_inherit (ccss_style_t const	*container_style,
		       ccss_style_t		*style)
{
	ccss_property_base_t const	*property;
	bool				 ret;

	ret = false;

	if (ccss_style_get_property (container_style,
				"border-radius", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"border-radius", property);
			ret = true;
		}
	}
	if (ccss_style_get_property (container_style,
				"border-top-left-radius", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"border-top-left-radius", property);
		} else if (PROPERTY_INHERIT (property)) {
			/* Need to resolve further. */
			ret = false;
		}
	}
	if (ccss_style_get_property (container_style,
				"border-top-right-radius", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"border-top-right-radius", property);
		} else if (PROPERTY_INHERIT (property)) {
			/* Need to resolve further. */
			ret = false;
		}
	}
	if (ccss_style_get_property (container_style,
				"border-bottom-left-radius", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"border-bottom-left-radius", property);
		} else if (PROPERTY_INHERIT (property)) {
			/* Need to resolve further. */
			ret = false;
		}
	}
	if (ccss_style_get_property (container_style,
				"border-bottom-right-radius", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"border-bottom-right-radius", property);
		} else if (PROPERTY_INHERIT (property)) {
			/* Need to resolve further. */
			ret = false;
		}
	}

	return ret;
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

static ccss_property_base_t *
border_style_create (ccss_grammar_t const	*grammar,
		     CRTerm const		*value,
		     void			*user_data)
{
	ccss_border_style_t *self, s;

	memset (&s, 0, sizeof (s));
	ccss_property_init (&s.base, peek_property_class ("border-style"));

	parse_style (&value, &s);
	if (CCSS_PROPERTY_STATE_INVALID == s.base.state) {
		return NULL;
	}

	self = g_new0 (ccss_border_style_t, 1);
	*self = s;
	return &self->base;
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

static ccss_property_base_t *
border_width_create (ccss_grammar_t const	*grammar,
		     CRTerm const		*value,
		     void			*user_data)
{
	ccss_border_width_t *self, w;

	memset (&w, 0, sizeof (w));
	ccss_property_init (&w.base, peek_property_class ("border-width"));

	parse_width (&value, &w);
	if (CCSS_PROPERTY_STATE_INVALID == w.base.state) {
		return NULL;
	}

	self = g_new0 (ccss_border_width_t, 1);
	*self = w;
	return &self->base;
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
	.property_create = border_radius_create,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) border_radius_convert,
	.property_factory = NULL,
	.property_inherit = NULL
    }, {
	.name = "border-bottom-right-radius",
	.property_create = border_radius_create,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) border_radius_convert,
	.property_factory = NULL,
	.property_inherit = NULL
    }, {
	.name = "border-bottom-left-radius",
	.property_create = border_radius_create,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) border_radius_convert,
	.property_factory = NULL,
	.property_inherit = NULL
    }, {
	.name = "border-top-left-radius",
	.property_create = border_radius_create,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) border_radius_convert,
	.property_factory = NULL,
	.property_inherit = NULL
    }, {
	.name = "border-radius",
	.property_create = NULL,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) border_radius_convert,
	.property_factory = (ccss_property_factory_f) border_radius_factory,
	.property_inherit = border_radius_inherit
    }, {
	.name = "border-left-color",
	.property_create = ccss_cairo_color_create,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) ccss_cairo_color_convert,
	.property_factory = NULL,
	.property_inherit = NULL
    }, {
	.name = "border-left-style",
	.property_create = border_style_create,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) border_style_convert,
	.property_factory = NULL,
	.property_inherit = NULL
    }, {
	.name = "border-left-width",
	.property_create = border_width_create,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) border_width_convert,
	.property_factory = NULL,
	.property_inherit = NULL
    }, {
	.name = "border-top-color",
	.property_create = ccss_cairo_color_create,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) ccss_cairo_color_convert,
	.property_factory = NULL,
	.property_inherit = NULL
    }, {
	.name = "border-top-style",
	.property_create = border_style_create,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) border_style_convert,
	.property_factory = NULL,
	.property_inherit = NULL
    }, {
	.name = "border-top-width",
	.property_create = border_width_create,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) border_width_convert,
	.property_factory = NULL,
	.property_inherit = NULL
    }, {
	.name = "border-right-color",
	.property_create = ccss_cairo_color_create,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) ccss_cairo_color_convert,
	.property_factory = NULL,
	.property_inherit = NULL
    }, {
	.name = "border-right-style",
	.property_create = border_style_create,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) border_style_convert,
	.property_factory = NULL,
	.property_inherit = NULL
    }, {
	.name = "border-right-width",
	.property_create = border_width_create,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) border_width_convert,
	.property_factory = NULL,
	.property_inherit = NULL
    }, {
	.name = "border-bottom-color",
	.property_create = ccss_cairo_color_create,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) ccss_cairo_color_convert,
	.property_factory = NULL,
	.property_inherit = NULL
    }, {
	.name = "border-bottom-style",
	.property_create = border_style_create,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) border_style_convert,
	.property_factory = NULL,
	.property_inherit = NULL
    }, {
	.name = "border-bottom-width",
	.property_create = border_width_create,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) border_width_convert,
	.property_factory = NULL,
	.property_inherit = NULL
    }, {
	.name = "border-left",
	.property_create = NULL,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = NULL,
	.property_factory = (ccss_property_factory_f) border_left_factory,
	.property_inherit = border_left_inherit
    }, {
	.name = "border-top",
	.property_create = NULL,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = NULL,
	.property_factory = (ccss_property_factory_f) border_top_factory,
	.property_inherit = border_top_inherit
    }, {
	.name = "border-right",
	.property_create = NULL,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = NULL,
	.property_factory = (ccss_property_factory_f) border_right_factory,
	.property_inherit = border_right_inherit
    }, {
	.name = "border-bottom",
	.property_create = NULL,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = NULL,
	.property_factory = (ccss_property_factory_f) border_bottom_factory,
	.property_inherit = border_bottom_inherit
    }, {
	.name = "border-color",
	.property_create = NULL,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) ccss_cairo_color_convert,
	.property_factory = (ccss_property_factory_f) border_color_factory,
	.property_inherit = border_color_inherit
    }, {
	.name = "border-style",
	.property_create = NULL,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) border_style_convert,
	.property_factory = (ccss_property_factory_f) border_style_factory,
	.property_inherit = border_style_inherit
    }, {
	.name = "border-width",
	.property_create = NULL,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = (ccss_property_convert_f) border_width_convert,
	.property_factory = (ccss_property_factory_f) border_width_factory,
	.property_inherit = border_width_inherit
    }, {
	.name = "border",
	.property_create = NULL,
	.property_destroy = (ccss_property_destroy_f) g_free,
	.property_convert = NULL,
	.property_factory = (ccss_property_factory_f) border_factory,
	.property_inherit = border_inherit
    }, {
	.name = NULL,
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

