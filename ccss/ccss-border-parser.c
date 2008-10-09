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
#include "ccss-border-parser.h"


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

static ccss_property_spec_t
parse_width (CRTerm const		**value,
	     double			 *width)
{
	ccss_property_spec_t spec;

	spec = ccss_property_parse_spec (value);
	if (CCSS_PROPERTY_SPEC_SET == spec &&
	    *value && 
	    TERM_NUMBER == (*value)->type) {

		*width = (*value)->content.num->val;
		(*value) = (*value)->next;
		spec = CCSS_PROPERTY_SPEC_SET;
	}

	return spec;
}

static ccss_property_spec_t
parse_style (CRTerm const		**value,
	     ccss_border_style_type_t		 *style)
{
	ccss_property_spec_t spec;

	spec = ccss_property_parse_spec (value);
	*style = CCSS_BORDER_STYLE_SOLID;
	if (CCSS_PROPERTY_SPEC_SET == spec &&
	    *value && 
	    TERM_IDENT == (*value)->type &&
	    match_style (cr_string_peek_raw_str ((*value)->content.str), style)) {

		(*value) = (*value)->next;
		spec = CCSS_PROPERTY_SPEC_SET;
	}

	return spec;
}

static ccss_property_spec_t
parse_color (CRTerm const		**value,
	     ccss_color_t		 *color)
{
	ccss_color_parse (color, value);

	return color->spec;
}

static ccss_property_spec_t
parse_radius (CRTerm const		**value,
	      double			 *radius)
{
	ccss_property_spec_t spec;

	spec = ccss_property_parse_spec (value);
	if (CCSS_PROPERTY_SPEC_SET == spec &&
	    *value && 
	    TERM_NUMBER == (*value)->type) {

		*radius = (*value)->content.num->val;
		(*value) = (*value)->next;
		spec = CCSS_PROPERTY_SPEC_SET;
	}

	return spec;
}

static bool
parse_stroke_property (ccss_color_t		*color,
		       ccss_border_style_t	*style,
		       ccss_border_width_t	*width,
		       char const		*property,
		       CRTerm const		*value)
{
	if (g_str_has_suffix ("width", property)) {

		ccss_border_width_t w;

		w.spec = parse_width (&value, &w.width);
		if (CCSS_PROPERTY_SPEC_UNSET == w.spec) {
			return false;
		}
		*width = w;
		color->spec = CCSS_PROPERTY_SPEC_UNSET;
		style->spec = CCSS_PROPERTY_SPEC_UNSET;

	} else if (g_str_has_suffix ("style", property)) {

		ccss_border_style_t s;

		s.spec = parse_style (&value, &s.style);
		if (CCSS_PROPERTY_SPEC_UNSET == s.spec) {
			return false;
		}
		*style = s;
		color->spec = CCSS_PROPERTY_SPEC_UNSET;
		width->spec = CCSS_PROPERTY_SPEC_UNSET;

	} else if (g_str_has_suffix ("color", property)) {

		ccss_color_t c;

		c.spec = parse_color (&value, &c);
		if (CCSS_PROPERTY_SPEC_UNSET == c.spec) {
			return false;
		}
		*color = c;
		style->spec = CCSS_PROPERTY_SPEC_UNSET;
		width->spec = CCSS_PROPERTY_SPEC_UNSET;

	} else {
		g_assert_not_reached ();
		g_warning ("Unknown property `%s'", property);
		color->spec = CCSS_PROPERTY_SPEC_UNSET;
		style->spec = CCSS_PROPERTY_SPEC_UNSET;
		width->spec = CCSS_PROPERTY_SPEC_UNSET;
		return false;
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
bool
ccss_block_parse_border (ccss_block_t	*self,
			 char const	*property,
			 CRTerm const	*values)
{
	CRTerm const		*iter;
	ccss_color_t		*color, c;
	ccss_border_style_t	*style, s;
	ccss_border_width_t	*width, w;
	bool			 ret;

	/* Radius */
	iter = values;
	if (0 == strcmp ("border-top-right-radius", property)) {

		ccss_border_join_t *radius, r;

		memset (&r, 0, sizeof (r));
		r.spec = parse_radius (&iter, &r.radius);
		if (CCSS_PROPERTY_SPEC_UNSET == r.spec) {
			return false;
		}
		radius = ccss_block_new_border_top_right_radius (self);
		*radius = r;
		return true;

	} else if (0 == strcmp ("border-bottom-right-radius", property)) {

		ccss_border_join_t *radius, r;

		memset (&r, 0, sizeof (r));
		r.spec = parse_radius (&iter, &r.radius);
		if (CCSS_PROPERTY_SPEC_UNSET == r.spec) {
			return false;
		}
		radius = ccss_block_new_border_bottom_right_radius (self);
		*radius = r;
		return true;

	} else if (0 == strcmp ("border-bottom-left-radius", property)) {

		ccss_border_join_t *radius, r;

		memset (&r, 0, sizeof (r));
		r.spec = parse_radius (&iter, &r.radius);
		if (CCSS_PROPERTY_SPEC_UNSET == r.spec) {
			return false;
		}
		radius = ccss_block_new_border_bottom_left_radius (self);
		*radius = r;
		return true;

	} else if (0 == strcmp ("border-top-left-radius", property)) {

		ccss_border_join_t *radius, r;

		memset (&r, 0, sizeof (r));
		r.spec = parse_radius (&iter, &r.radius);
		if (CCSS_PROPERTY_SPEC_UNSET == r.spec) {
			return false;
		}
		radius = ccss_block_new_border_top_left_radius (self);
		*radius = r;
		return true;

	} else if (0 == strcmp ("border-radius", property)) {

		ccss_border_join_t *radius, r;

		memset (&r, 0, sizeof (r));
		r.spec = parse_radius (&iter, &r.radius);
		if (CCSS_PROPERTY_SPEC_UNSET == r.spec) {
			return false;
		}
		radius = ccss_block_new_border_bottom_left_radius (self);
		*radius = r;
		radius = ccss_block_new_border_bottom_right_radius (self);
		*radius = r;
		radius = ccss_block_new_border_top_left_radius (self);
		*radius = r;
		radius = ccss_block_new_border_top_right_radius (self);
		*radius = r;
		return true;
	}

	/* Test for specific properties first. */
	if (g_str_has_prefix (property, "border-left-")) {

		ret = parse_stroke_property (&c, &s, &w, property, values);
		if (c.spec != CCSS_PROPERTY_SPEC_UNSET) { color = ccss_block_new_border_left_color (self); *color = c; }
		if (s.spec != CCSS_PROPERTY_SPEC_UNSET) { style = ccss_block_new_border_left_style (self); *style = s; }
		if (w.spec != CCSS_PROPERTY_SPEC_UNSET) { width = ccss_block_new_border_left_width (self); *width = w; }
		return ret;

	} else if (g_str_has_prefix (property, "border-top-")) {

		ret = parse_stroke_property (&c, &s, &w, property, values);
		if (c.spec != CCSS_PROPERTY_SPEC_UNSET) { color = ccss_block_new_border_top_color (self); *color = c; }
		if (s.spec != CCSS_PROPERTY_SPEC_UNSET) { style = ccss_block_new_border_top_style (self); *style = s; }
		if (w.spec != CCSS_PROPERTY_SPEC_UNSET) { width = ccss_block_new_border_top_width (self); *width = w; }
		return ret;

	} else if (g_str_has_prefix (property, "border-right-")) {

		ret = parse_stroke_property (&c, &s, &w, property, values);
		if (c.spec != CCSS_PROPERTY_SPEC_UNSET) { color = ccss_block_new_border_right_color (self); *color = c; }
		if (s.spec != CCSS_PROPERTY_SPEC_UNSET) { style = ccss_block_new_border_right_style (self); *style = s; }
		if (w.spec != CCSS_PROPERTY_SPEC_UNSET) { width = ccss_block_new_border_right_width (self); *width = w; }
		return ret;

	} else if (g_str_has_prefix (property, "border-bottom-")) {

		ret = parse_stroke_property (&c, &s, &w, property, values);
		if (c.spec != CCSS_PROPERTY_SPEC_UNSET) { color = ccss_block_new_border_bottom_color (self); *color = c; }
		if (s.spec != CCSS_PROPERTY_SPEC_UNSET) { style = ccss_block_new_border_bottom_style (self); *style = s; }
		if (w.spec != CCSS_PROPERTY_SPEC_UNSET) { width = ccss_block_new_border_bottom_width (self); *width = w; }
		return ret;
	}

	/* Now try to parse multi-value properties. */
	iter = values;
	w.spec = parse_width (&iter, &w.width);
	if (CCSS_PROPERTY_SPEC_NONE == w.spec && !iter) {
		s.spec = CCSS_PROPERTY_SPEC_NONE;
		c.spec = CCSS_PROPERTY_SPEC_NONE;
	} else {
		s.spec = parse_style (&iter, &s.style);
		parse_color (&iter, &c);
	}

	if (0 == strcmp ("border", property)) {

		if (c.spec != CCSS_PROPERTY_SPEC_UNSET) { color = ccss_block_new_border_bottom_color (self); *color = c; }
		if (s.spec != CCSS_PROPERTY_SPEC_UNSET) { style = ccss_block_new_border_bottom_style (self); *style = s; }
		if (w.spec != CCSS_PROPERTY_SPEC_UNSET) { width = ccss_block_new_border_bottom_width (self); *width = w; }

		if (c.spec != CCSS_PROPERTY_SPEC_UNSET) { color = ccss_block_new_border_left_color (self); *color = c; }
		if (s.spec != CCSS_PROPERTY_SPEC_UNSET) { style = ccss_block_new_border_left_style (self); *style = s; }
		if (w.spec != CCSS_PROPERTY_SPEC_UNSET) { width = ccss_block_new_border_left_width (self); *width = w; }

		if (c.spec != CCSS_PROPERTY_SPEC_UNSET) { color = ccss_block_new_border_right_color (self); *color = c; }
		if (s.spec != CCSS_PROPERTY_SPEC_UNSET) { style = ccss_block_new_border_right_style (self); *style = s; }
		if (w.spec != CCSS_PROPERTY_SPEC_UNSET) { width = ccss_block_new_border_right_width (self); *width = w; }

		if (c.spec != CCSS_PROPERTY_SPEC_UNSET) { color = ccss_block_new_border_top_color (self); *color = c; }
		if (s.spec != CCSS_PROPERTY_SPEC_UNSET) { style = ccss_block_new_border_top_style (self); *style = s; }
		if (w.spec != CCSS_PROPERTY_SPEC_UNSET) { width = ccss_block_new_border_top_width (self); *width = w; }

	} else if (0 == strcmp ("border-left", property)) {

		if (c.spec != CCSS_PROPERTY_SPEC_UNSET) { color = ccss_block_new_border_left_color (self); *color = c; }
		if (s.spec != CCSS_PROPERTY_SPEC_UNSET) { style = ccss_block_new_border_left_style (self); *style = s; }
		if (w.spec != CCSS_PROPERTY_SPEC_UNSET) { width = ccss_block_new_border_left_width (self); *width = w; }

	} else if (0 == strcmp ("border-top", property)) {

		if (c.spec != CCSS_PROPERTY_SPEC_UNSET) { color = ccss_block_new_border_top_color (self); *color = c; }
		if (s.spec != CCSS_PROPERTY_SPEC_UNSET) { style = ccss_block_new_border_top_style (self); *style = s; }
		if (w.spec != CCSS_PROPERTY_SPEC_UNSET) { width = ccss_block_new_border_top_width (self); *width = w; }

	} else if (0 == strcmp ("border-right", property)) {

		if (c.spec != CCSS_PROPERTY_SPEC_UNSET) { color = ccss_block_new_border_right_color (self); *color = c; }
		if (s.spec != CCSS_PROPERTY_SPEC_UNSET) { style = ccss_block_new_border_right_style (self); *style = s; }
		if (w.spec != CCSS_PROPERTY_SPEC_UNSET) { width = ccss_block_new_border_right_width (self); *width = w; }

	} else if (0 == strcmp ("border-bottom", property)) {

		if (c.spec != CCSS_PROPERTY_SPEC_UNSET) { color = ccss_block_new_border_bottom_color (self); *color = c; }
		if (s.spec != CCSS_PROPERTY_SPEC_UNSET) { style = ccss_block_new_border_bottom_style (self); *style = s; }
		if (w.spec != CCSS_PROPERTY_SPEC_UNSET) { width = ccss_block_new_border_bottom_width (self); *width = w; }

	} else {
		return false;
	}

	return true;
}

#ifdef CCSS_DEBUG

static char const *
lookup_name (ccss_border_style_type_t border_style)
{
	for (unsigned int i = 0; i < G_N_ELEMENTS (_border_style_map); i++) {
		if (_border_style_map[i].border_style == border_style) {
			return _border_style_map[i].css;
		}
	}	

	return NULL;
}

#endif /* CCSS_DEBUG */
