/* vim: set ts=8 sw=8 noexpandtab: */

/* The Gtk+ CSS Drawing Library.
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
#include "ccss-gtk-property.h"

static ccss_property_class_t const *
peek_property_class (void);

static ccss_property_class_t const *_fallback_property_class = NULL;

static bool
parse_gboolean (CRTerm const		*values,
		ccss_gtk_property_t	*property)
{
	char const	*str;
	bool		 ret;

	g_return_val_if_fail (values && property, false);

	ret = false;
	switch (values->type) {
	case TERM_IDENT:
	case TERM_STRING:
		ret = true;
		str = cr_string_peek_raw_str (values->content.str);
		if (0 == g_ascii_strcasecmp ("true", str))
			property->content.gboolean_val = true;
		else if (0 == g_ascii_strcasecmp ("false", str))
			property->content.gboolean_val = false;
		else
			ret = false;
		break;
	default:
		/* Shut up compiler. */
		ret = false;
	}

	return ret;
}

static bool
parse_gchararray (CRTerm const		*values,
		  ccss_gtk_property_t	*property)
{
	char const	*str;
	bool		 ret;

	g_return_val_if_fail (values && property, false);

	ret = false;
	switch (values->type) {
	case TERM_IDENT:
	case TERM_STRING:
		str = cr_string_peek_raw_str (values->content.str);
		property->content.gchararray_val = g_strdup (str);
		ret = true;
		break;
	default:
		/* Shut up compiler. */
		ret = false;
	}

	return ret;
}

static bool
parse_gfloat (CRTerm const		*values,
	      ccss_gtk_property_t	*property)
{
	bool ret;

	g_return_val_if_fail (values && property, false);

	ret = false;
	switch (values->type) {
	case TERM_NUMBER:
		property->content.gfloat_val = values->content.num->val;
		ret = true;
		break;
	default:
		/* Shut up compiler. */
		ret = false;
	}

	return ret;
}

static bool
parse_gint (CRTerm const	*values,
	    ccss_gtk_property_t	*property)
{
	bool ret;

	g_return_val_if_fail (values && property, false);

	ret = false;
	switch (values->type) {
	case TERM_NUMBER:
		/* Be strict about fractionals. */
		if (lround (values->content.num->val) == values->content.num->val) {
			property->content.gint_val = (int) values->content.num->val;
			ret = true;
		}
		break;
	default:
		/* Shut up compiler. */
		ret = false;
	}

	return ret;
}

static bool
parse_guchar (CRTerm const		*values,
	      ccss_gtk_property_t	*property)
{
	bool ret;

	g_return_val_if_fail (values && property, false);

	ret = false;
	switch (values->type) {
	case TERM_NUMBER:
		/* Be strict about fractionals. */
		if (lround (values->content.num->val) == values->content.num->val) {
			if (values->content.num->val > G_MAXUINT8)
				property->content.guchar_val = G_MAXUINT8;
			else if (values->content.num->val < 0)
				property->content.guchar_val = 0;
			else
				property->content.guchar_val = (guchar) values->content.num->val;
			ret = true;
		}
		break;
	default:
		/* Shut up compiler. */
		ret = false;
	}

	return ret;
}

static bool
parse_gdk_color (ccss_grammar_t const	*grammar,
		 CRTerm const		*values,
		 void			*user_data,
		 ccss_gtk_property_t	*property)
{
	ccss_color_t	color;
	bool		ret;

	g_return_val_if_fail (values && property, false);

	memset (&color, 0, sizeof (color));
	ret = ccss_color_parse (&color, grammar, user_data, &values);
	if (ret) {
		property->content.gdkcolor_val.red = color.red * 65535;
		property->content.gdkcolor_val.green = color.green * 65535;
		property->content.gdkcolor_val.blue = color.blue * 65535;
	}

	return ret;
}

static bool
parse_gtk_border (CRTerm const		*values,
		  ccss_gtk_property_t	*property)
{
	gint		 *border_ptr;
	int		  i;

	g_return_val_if_fail (values && property, false);

	border_ptr = (gint *) &property->content.gtkborder_val;
	for (i = 0; i < 4; i++) {

		if (!values) {
			break;
		}
		
		/* Be strict about fractionals. */
		if (TERM_NUMBER == values->type &&
		    lround (values->content.num->val) == values->content.num->val) {

			border_ptr[i] = (gint) values->content.num->val;
			values = values->next;
		}
	}

	return i == 4;
}

static bool
parse_gtk_relief_style (CRTerm const		*values,
			ccss_gtk_property_t	*property)
{
	char const	*str;
	bool		 ret;

	g_return_val_if_fail (values && property, false);

	ret = false;
	switch (values->type) {
	case TERM_IDENT:
	case TERM_STRING:
		ret = true;
		str = cr_string_peek_raw_str (values->content.str);
		if (0 == g_ascii_strcasecmp ("normal", str))
			property->content.gtkreliefstyle_val = GTK_RELIEF_NORMAL;
		else if (0 == g_ascii_strcasecmp ("half", str))
			property->content.gtkreliefstyle_val = GTK_RELIEF_HALF;
		else if (0 == g_ascii_strcasecmp ("none", str))
			property->content.gtkreliefstyle_val = GTK_RELIEF_NONE;
		else
			ret = false;
		break;
	default:
		/* Shut up compiler. */
		ret = false;
	}

	return ret;
}

static bool
parse_gtk_requisition (CRTerm const		*values,
		       ccss_gtk_property_t	*property)
{
	gint		 *border_ptr;
	int		  i;

	g_return_val_if_fail (values && property, false);

	border_ptr = (gint *) &property->content.gtkrequisition_val;
	for (i = 0; i < 2; i++) {

		if (!values) {
			break;
		}
		
		/* Be strict about fractionals. */
		if (TERM_NUMBER == values->type &&
		    lround (values->content.num->val) == values->content.num->val) {

			border_ptr[i] = (gint) values->content.num->val;
			values = values->next;
		}
	}

	return i == 2;
}

static bool
parse_gtk_shadow_type (CRTerm const		*values,
		       ccss_gtk_property_t	*property)
{
	char const	*str;
	bool		 ret;

	g_return_val_if_fail (values && property, false);

	ret = false;
	switch (values->type) {
	case TERM_IDENT:
	case TERM_STRING:
		ret = true;
		str = cr_string_peek_raw_str (values->content.str);
		if (0 == g_ascii_strcasecmp ("none", str))
			property->content.gtkshadowtype_val = GTK_SHADOW_NONE;
		else if (0 == g_ascii_strcasecmp ("in", str))
			property->content.gtkreliefstyle_val = GTK_SHADOW_IN;
		else if (0 == g_ascii_strcasecmp ("out", str))
			property->content.gtkreliefstyle_val = GTK_SHADOW_OUT;
		else if (0 == g_ascii_strcasecmp ("etched-in", str))
			property->content.gtkreliefstyle_val = GTK_SHADOW_ETCHED_IN;
		else if (0 == g_ascii_strcasecmp ("etched-out", str))
			property->content.gtkreliefstyle_val = GTK_SHADOW_ETCHED_OUT;
		else
			ret = false;
		break;
	default:
		/* Shut up compiler. */
		ret = false;
	}

	return ret;
}

static bool
parse_gtk_toolbar_space_style (CRTerm const		*values,
			       ccss_gtk_property_t	*property)
{
	char const	*str;
	bool		 ret;

	g_return_val_if_fail (values && property, false);

	ret = false;
	switch (values->type) {
	case TERM_IDENT:
	case TERM_STRING:
		ret = true;
		str = cr_string_peek_raw_str (values->content.str);
		if (0 == g_ascii_strcasecmp ("empty", str))
			property->content.gtktoolbarspacestyle_val = GTK_TOOLBAR_SPACE_EMPTY;
		else if (0 == g_ascii_strcasecmp ("line", str))
			property->content.gtktoolbarspacestyle_val = GTK_TOOLBAR_SPACE_LINE;
		else
			ret = false;
		break;
	default:
		/* Shut up compiler. */
		ret = false;
	}

	return ret;
}

static bool
parse_gtk_style_property (ccss_grammar_t const	*grammar,
			  char const		*name,
			  CRTerm const		*values,
			  void			*user_data,
			  ccss_gtk_property_t	*property)
{
	char			**tokens;
	char			 *class_name = NULL;
	char			 *property_name = NULL;
	GtkBuilder		 *builder = NULL;
	GType			  type = 0;
	GtkWidget		 *widget = NULL;
	GParamSpec		 *pspec = NULL;

	/* Parse property name. */
	tokens = g_strsplit (name, "-", 2);
	if (tokens && tokens[0]) {
		class_name = tokens[0];
	} else {
		goto bail;
	}
	if (tokens && tokens[1]) {
		property_name = tokens[1];
	} else {
		goto bail;
	}

	/* Introspect property type. */
	builder = gtk_builder_new ();
	type = gtk_builder_get_type_from_name (builder, class_name);
	if (0 == type)
		goto bail;

	widget = GTK_WIDGET (g_object_new (type, NULL));
	if (NULL == widget)
		goto bail;

	pspec = gtk_widget_class_find_style_property (GTK_WIDGET_GET_CLASS (widget),
						      property_name);
	if (NULL == pspec)
		goto bail;

	/* Parse value. */
	property->gtype = 0;
	if (G_TYPE_BOOLEAN == pspec->value_type) {
		if (parse_gboolean (values, property)) {
			property->gtype = pspec->value_type;
		}
	} else if (G_TYPE_STRING == pspec->value_type) {
		if (parse_gchararray (values, property)) {
			property->gtype = pspec->value_type;
		}
	} else if (G_TYPE_FLOAT == pspec->value_type) {
		if (parse_gfloat (values, property)) {
			property->gtype = pspec->value_type;
		}
	} else if (G_TYPE_INT == pspec->value_type) {
		if (parse_gint (values, property)) {
			property->gtype = pspec->value_type;
		}
	} else if (G_TYPE_UCHAR == pspec->value_type) {
		if (parse_guchar (values, property)) {
			property->gtype = pspec->value_type;
		}
	} else if (GDK_TYPE_COLOR == pspec->value_type) {
		if (parse_gdk_color (grammar, values, user_data, property)) {
			property->gtype = pspec->value_type;
		}
	} else if (GTK_TYPE_BORDER == pspec->value_type) {
		if (parse_gtk_border (values, property)) {
			property->gtype = pspec->value_type;
		}
	} else if (GTK_TYPE_RELIEF_STYLE == pspec->value_type) {
		if (parse_gtk_relief_style (values, property)) {
			property->gtype = pspec->value_type;
		}
	} else if (GTK_TYPE_REQUISITION == pspec->value_type) {
		if (parse_gtk_requisition (values, property)) {
			property->gtype = pspec->value_type;
		}
	} else if (GTK_TYPE_SHADOW_TYPE == pspec->value_type) {
		if (parse_gtk_shadow_type (values, property)) {
			property->gtype = pspec->value_type;
		}
	} else if (GTK_TYPE_TOOLBAR_SPACE_STYLE == pspec->value_type) {
		if (parse_gtk_toolbar_space_style (values, property)) {
			property->gtype = pspec->value_type;
		}
	} else {
		g_assert_not_reached ();
	}

	if (property->gtype) {
		property->class_name = class_name, class_name = NULL;
		property->property_name = property_name, property_name = NULL;
	}

bail:
	if (tokens)
		g_free (tokens);
	if (class_name)
		g_free (class_name);
	if (property_name)
		g_free (property_name);
	if (builder)
		g_object_unref (G_OBJECT (builder));
	if (widget)
		gtk_widget_destroy (widget);

	return (bool) property->gtype;
}

static void 
property_destroy (ccss_gtk_property_t *self)
{
	g_return_if_fail (self);

	if (G_TYPE_STRING == self->gtype && self->content.gchararray_val) {
		g_free (self->content.gchararray_val);
	}
	g_free (self);
}

static bool
property_convert (ccss_gtk_property_t const	*self,
		  ccss_property_type_t		 target,
		  void				*value)
{

	return false;
}

static bool
property_factory (ccss_grammar_t const	*grammar,
		  ccss_block_t		*block,
		  char const		*name,
		  CRTerm const		*values,
		  void			*user_data)
{
	ccss_gtk_property_t s, *self;

	g_return_val_if_fail (grammar, false);
	g_return_val_if_fail (block, false);
	g_return_val_if_fail (name, false);
	g_return_val_if_fail (values, false);

	/* PONDERING: Handle things like GtkSettings `gtk-button-images' in the future? */

	memset (&s, 0, sizeof (s));
	ccss_property_init (&s.base, peek_property_class ());

	self = NULL;
	if (0 == g_strcmp0 (name, "xthickness")) {
		if (parse_gint (values, &s)) {
			s.class_name = NULL;
			s.property_name = g_strdup ("xthickness");
			self = g_new0 (ccss_gtk_property_t, 1);
			*self = s;
			ccss_block_add_property (block, "xthickness", &self->base);
		}
		return (bool) self;
	} else if (0 == g_strcmp0 (name, "ythickness")) {
		if (parse_gint (values, &s)) {
			s.class_name = NULL;
			s.property_name = g_strdup ("ythickness");
			self = g_new0 (ccss_gtk_property_t, 1);
			*self = s;
			ccss_block_add_property (block, "ythickness", &self->base);
		}
		return (bool) self;
	} else if (g_str_has_prefix (name, "Gtk")) {
		if (parse_gtk_style_property (grammar, name, values, user_data, &s)) {
			self = g_new0 (ccss_gtk_property_t, 1);
			*self = s;
			ccss_block_add_property (block, name, &self->base);
		}
		return (bool) self;
	} else if (g_ascii_isupper (name[0])) {
		// TODO introspect style property and feed back to gtk directly.
		// Non-gtk style properties (wnck, nautilus, ...) can't be
		// resolved offline because css2gtkrc doesn't link against them.
		// May cause some breakage, let's see how it goes.
		// TODO link css2gtkrc against gtk.
	}


	/* Fall back. */
	g_return_val_if_fail (_fallback_property_class, false);
	g_return_val_if_fail (_fallback_property_class->factory, false);
	return _fallback_property_class->factory (grammar, block, name,
						  values, user_data);
}

static ccss_property_class_t const _properties[] = {
    {
	.name = "*",
	.create = NULL,
	.destroy = (ccss_property_destroy_f) property_destroy,
	.convert = (ccss_property_convert_f) property_convert,
	.factory = property_factory,
	.inherit = NULL
    }, {
	.name = NULL
    }
};

static ccss_property_class_t const *
peek_property_class (void)
{
	return &_properties[0];
}

ccss_property_class_t const *
ccss_gtk_property_get_property_classes (void)
{
	return _properties;
}

void
ccss_gtk_property_set_fallback_class (ccss_property_class_t const *property_class)
{
	g_return_if_fail (property_class);

	_fallback_property_class = property_class;
}

bool
ccss_gtk_property_is_style_property (ccss_property_base_t const *self)
{
	return self->property_class->destroy ==
			(ccss_property_destroy_f) property_destroy;
}

