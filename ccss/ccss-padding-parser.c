/* vim: set ts=8 sw=8 noexpandtab: */

/* The `C' CSS Library.
 * Copyright (C) 2009 Intel Corporation.
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

#include <string.h>
#include "ccss-padding.h"
#include "ccss-padding-parser.h"
#include "ccss-grammar.h"
#include "config.h"

#define INSERT_PADDING(block_, prop_name_, prop_, prop_ptr_)		       \
	G_STMT_START {							       \
	prop_.base.property_class = peek_property_class (prop_name_);	       \
	prop_ptr_ = g_new0 (ccss_padding_t, 1);				       \
	*prop_ptr_ = prop_;						       \
	ccss_block_add_property (block_, prop_name_, &prop_ptr_->base);	       \
	} G_STMT_END

#define PROPERTY_SET(property_)					\
	(CCSS_PROPERTY_STATE_NONE == property_->state ||	\
	 CCSS_PROPERTY_STATE_SET == property_->state)

#define PROPERTY_INHERIT(property_)				\
	(CCSS_PROPERTY_STATE_INHERIT == property_->state)

static ccss_property_class_t const *
peek_property_class (char const *property_name);

static bool
parse_padding (CRTerm const	**value,
	       ccss_padding_t	 *padding)
{
	padding->base.state = ccss_property_parse_state (value);
	if (CCSS_PROPERTY_STATE_SET == padding->base.state &&
	    *value &&
	    TERM_NUMBER == (*value)->type) {

		padding->padding = (*value)->content.num->val;
		(*value) = (*value)->next;
		padding->base.state = CCSS_PROPERTY_STATE_SET;

		return true;
	}

	return false;
}

static ccss_property_base_t *
padding_create (ccss_grammar_t const	*grammar,
		CRTerm const		*value,
		void			*user_data)
{
	ccss_padding_t *self, p;

	memset (&p, 0, sizeof (p));
	ccss_property_init (&p.base, peek_property_class ("padding"));

	parse_padding (&value, &p);
	if (CCSS_PROPERTY_STATE_INVALID == p.base.state) {
		return NULL;
	}

	self = g_new0 (ccss_padding_t, 1);
	*self = p;
	return &self->base;
}

static bool
padding_convert (ccss_padding_t const	*property,
		 ccss_property_type_t	 target,
		 void			*value)
{
	g_return_val_if_fail (property && value, false);

	switch (target) {
	case CCSS_PROPERTY_TYPE_DOUBLE:
		* (double *) value = property->padding;
		return true;
	case CCSS_PROPERTY_TYPE_STRING:
		* (char **) value = g_strdup_printf ("%f", property->padding);
		return true;
	default:
		g_assert_not_reached ();
		return false;
	}

	return false;
}

static bool
padding_factory (ccss_grammar_t const	*grammar,
		 ccss_block_t		*self,
		 char const		*name,
		 CRTerm const		*values,
		 void			*user_data)
{
	CRTerm const	*iter;
	ccss_padding_t	 p, p0, p1, p2, p3;
	ccss_padding_t	*padding;

	/* If `padding: inherit;' then insert a dummy. */
	memset (&p, 0, sizeof (p));
	ccss_property_init (&p.base, peek_property_class ("padding"));
	p.base.state = ccss_property_parse_state (&values);
	if (p.base.state == CCSS_PROPERTY_STATE_INHERIT) {
		padding = g_new0 (ccss_padding_t, 1);
		*padding = p;
		ccss_block_add_property (self, "padding", &padding->base);
		return true;
	}

	memset (&p0, 0, sizeof (p0));
	memset (&p1, 0, sizeof (p1));
	memset (&p2, 0, sizeof (p2));
	memset (&p3, 0, sizeof (p3));

	iter = values;
	if (iter) { parse_padding (&iter, &p0); }
	if (iter) { parse_padding (&iter, &p1); }
	if (iter) { parse_padding (&iter, &p2); }
	if (iter) { parse_padding (&iter, &p3); }

	if (CCSS_PROPERTY_STATE_INVALID == p0.base.state) {

		return false;

	} else if (CCSS_PROPERTY_STATE_INVALID == p1.base.state) {

		INSERT_PADDING (self, "padding", p0, padding);

		INSERT_PADDING (self, "padding-top", p0, padding);
		INSERT_PADDING (self, "padding-right", p0, padding);
		INSERT_PADDING (self, "padding-bottom", p0, padding);
		INSERT_PADDING (self, "padding-left", p0, padding);

	} else if (CCSS_PROPERTY_STATE_INVALID == p2.base.state) {

		INSERT_PADDING (self, "padding-bottom", p0, padding);
		INSERT_PADDING (self, "padding-top", p0, padding);

		INSERT_PADDING (self, "padding-left", p1, padding);
		INSERT_PADDING (self, "padding-right", p1, padding);

	} else if (CCSS_PROPERTY_STATE_INVALID == p3.base.state) {

		INSERT_PADDING (self, "padding-top", p0, padding);

		INSERT_PADDING (self, "padding-left", p1, padding);
		INSERT_PADDING (self, "padding-right", p1, padding);

		INSERT_PADDING (self, "padding-bottom", p2, padding);

	} else if (p3.base.state != CCSS_PROPERTY_STATE_INVALID) {

		INSERT_PADDING (self, "padding-top", p0, padding);
		INSERT_PADDING (self, "padding-right", p1, padding);
		INSERT_PADDING (self, "padding-bottom", p2, padding);
		INSERT_PADDING (self, "padding-left", p3, padding);
	}

	return true;
}

static bool
padding_inherit (ccss_style_t const	*container_style,
		 ccss_style_t		*style)
{
	ccss_property_base_t const	*property;
	bool				 ret;

	if (ccss_style_get_property (container_style,
				"padding", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"padding", property);
			ret = true;
		}
	}
	if (ccss_style_get_property (container_style,
				"padding-left", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"padding-left", property);
		} else if (PROPERTY_INHERIT (property)) {
			/* Need to resolve further. */
			ret = false;
		}
	}
	if (ccss_style_get_property (container_style,
				"padding-top", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"padding-top", property);
		} else if (PROPERTY_INHERIT (property)) {
			/* Need to resolve further. */
			ret = false;
		}
	}
	if (ccss_style_get_property (container_style,
				"padding-right", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"padding-right", property);
		} else if (PROPERTY_INHERIT (property)) {
			/* Need to resolve further. */
			ret = false;
		}
	}
	if (ccss_style_get_property (container_style,
				"padding-bottom", &property)) {
		if (PROPERTY_SET (property)) {
			ccss_style_set_property (style,
				"padding-bottom", property);
		} else if (PROPERTY_INHERIT (property)) {
			/* Need to resolve further. */
			ret = false;
		}
	}

	return ret;
}

static ccss_property_class_t const _ptable[] = {
    {
	.name = "padding-top",
	.create = padding_create,
	.destroy = (ccss_property_destroy_f) g_free,
	.convert = (ccss_property_convert_f) padding_convert,
	.factory = NULL,
	.inherit = NULL,
	.serialize = NULL
    }, {
	.name = "padding-right",
	.create = padding_create,
	.destroy = (ccss_property_destroy_f) g_free,
	.convert = (ccss_property_convert_f) padding_convert,
	.factory = NULL,
	.inherit = NULL,
	.serialize = NULL
    }, {
	.name = "padding-bottom",
	.create = padding_create,
	.destroy = (ccss_property_destroy_f) g_free,
	.convert = (ccss_property_convert_f) padding_convert,
	.factory = NULL,
	.inherit = NULL,
	.serialize = NULL
    }, {
	.name = "padding-left",
	.create = padding_create,
	.destroy = (ccss_property_destroy_f) g_free,
	.convert = (ccss_property_convert_f) padding_convert,
	.factory = NULL,
	.inherit = NULL,
	.serialize = NULL
    }, {
	.name = "padding",
	.create = NULL,
	.destroy = (ccss_property_destroy_f) g_free,
	.convert = (ccss_property_convert_f) padding_convert,
	.factory = padding_factory,
	.inherit = padding_inherit,
	.serialize = NULL
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
ccss_padding_parser_get_property_classes (void)
{
	return _ptable;
}

