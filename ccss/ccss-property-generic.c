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

#include <string.h>
#include "ccss-property-generic.h"

static ccss_property_class_t const *
peek_property_class (void);

static ccss_property_generic_t *
property_new (CRTerm const *values)
{
	ccss_property_generic_t	*prop, p;
	char const		*s;

	p.base.property_class = peek_property_class ();

	memset (&p, 0, sizeof (p));
	switch (values->type) {
	case TERM_NUMBER:
		p.base.state = CCSS_PROPERTY_STATE_SET;
		p.type = CCSS_PROPERTY_TYPE_DOUBLE;
		p.content.dval = values->content.num->val;
		break;
	case TERM_IDENT:
	case TERM_STRING:
		s = cr_string_peek_raw_str (values->content.str);
		if (0 == g_strcmp0 ("none", s)) {
			p.base.state = CCSS_PROPERTY_STATE_NONE;
		} else if (0 == g_strcmp0 ("inherit", s)) {
			p.base.state = CCSS_PROPERTY_STATE_INHERIT;
		} else {
			p.base.state = CCSS_PROPERTY_STATE_SET;
			p.type = CCSS_PROPERTY_TYPE_STRING;
			p.content.sval = g_strdup (s);
		}
		break;
	default:
		g_warning ("Unknown property type %d", values->type);
		return NULL;
	}

	/*  Coming here means success. */
	prop = g_new0 (ccss_property_generic_t, 1);
	*prop = p;
	return prop;
}

static bool
property_convert (ccss_property_generic_t	*self,
		  ccss_property_type_t		 target,
		  void				*value)
{
	g_return_val_if_fail (self && value, false);

	switch (target) {
	case CCSS_PROPERTY_TYPE_DOUBLE:
		switch (self->type) {
		case CCSS_PROPERTY_TYPE_DOUBLE:
			* (double *) value = self->content.dval;
			break;
		case CCSS_PROPERTY_TYPE_STRING:
			g_message ("Unable to convert `string' property to `double'");
			return false;
		default:
			g_assert_not_reached ();
			return false;
		}
		break;
	case CCSS_PROPERTY_TYPE_STRING:
		switch (self->type) {
		case CCSS_PROPERTY_TYPE_DOUBLE:
			* (char **) value = g_strdup_printf ("%f", self->content.dval);
			break;
		case CCSS_PROPERTY_TYPE_STRING:
			* (char **) value = g_strdup (self->content.sval);
			break;
		default:
			g_assert_not_reached ();
			return false;
		}
		break;
	default:
		g_assert_not_reached ();
		return false;
	}

	return true;
}

ccss_property_class_t const _ptable[] = {
  { 
	.name			= "*",
	.property_new		= (ccss_property_new_f) property_new,
	.property_free		= (ccss_property_free_f) g_free,
	.property_convert	= (ccss_property_convert_f) property_convert,
	.property_factory	= NULL
  },
  { 
	.name			= NULL,
	.property_new		= NULL,
	.property_free		= NULL,
	.property_convert	= NULL,
	.property_factory	= NULL
  }
};

static ccss_property_class_t const *
peek_property_class (void)
{
	return &_ptable[0];
}

ccss_property_class_t const *
ccss_property_generic_get_ptable (void)
{
	return _ptable;
}

