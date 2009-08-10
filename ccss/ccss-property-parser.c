/* vim: set ts=8 sw=8 noexpandtab: */

/* The `C' CSS Library.
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
#include <glib.h>
#include <libcroco/libcroco.h>
#include "ccss-grammar.h"
#include "ccss-property-generic.h"
#include "ccss-property-parser.h"
#include "config.h"

static ccss_property_class_t const *
peek_property_class (void);

static bool
property_factory (ccss_grammar_t const	*grammar,
		  ccss_block_t		*block,
		  char const		*name,
		  CRTerm const		*values,
		  void			*user_data)
{
	ccss_property_generic_t	*property, p;

	memset (&p, 0, sizeof (p));

	p.base.property_class = peek_property_class ();
	p.base.state = ccss_property_parse_state (&values);
	p.name = g_strdup (name);
	if (p.base.state == CCSS_PROPERTY_STATE_SET) {
		p.values = (CRTerm *) values;
		cr_term_ref (p.values);
	}

	property = g_new0 (ccss_property_generic_t, 1);
	*property = p;
	ccss_block_add_property (block, name, &property->base);

	return true;
}

static void
property_destroy (ccss_property_generic_t *self)
{
	g_return_if_fail (self);

	g_free (self->name);

	if (self->values) {
		cr_term_unref (self->values);
	}

	g_free (self);
}

static bool
property_convert (ccss_property_generic_t	*self,
		  ccss_property_type_t		 target,
		  void				*value)
{
	char const	*str;
	double		 num;

	g_return_val_if_fail (self && value, false);

	switch (target) {
	case CCSS_PROPERTY_TYPE_DOUBLE:
		if (self->values->type == TERM_NUMBER) {
			* (double *) value = self->values->content.num->val;
			return true;
		}
		g_warning (G_STRLOC "Cannot convert property '%s' to 'double'",
			   self->name);
		return false;
		break;
	case CCSS_PROPERTY_TYPE_STRING:
		switch (self->base.state) {
		case CCSS_PROPERTY_STATE_NONE:
			* (char **) value = g_strdup ("none");
			return true;
		case CCSS_PROPERTY_STATE_INHERIT:
			* (char **) value = g_strdup ("inherit");
			return true;
		case CCSS_PROPERTY_STATE_SET:
			if (self->values == NULL) {
				g_warning (G_STRLOC "NULL property '%s'",
					   self->name);
				return false;			
			}
			break;
		case CCSS_PROPERTY_STATE_INVALID:
		default:
			g_warning (G_STRLOC "Invalid property '%s'",
				   self->name);
			return false;
		}

		switch (self->values->type) {
		case TERM_NUMBER:
			num = self->values->content.num->val;
			* (char **) value = g_strdup_printf ("%f", num);
			return true;
		case TERM_HASH:
			str = cr_string_peek_raw_str (self->values->content.str);
			* (char **) value = g_strdup_printf ("#%s", str);
			return true;
		case TERM_IDENT: /* Fall thru. */
		case TERM_STRING:
			str = cr_string_peek_raw_str (self->values->content.str);
			* (char **) value = g_strdup (str);
			return true;
		default:
			g_warning (G_STRLOC "Unhandled property type '%d'",
				   self->values->type);
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
	return &_ptable[0];
}

ccss_property_class_t const *
ccss_property_parser_get_property_classes (void)
{
	return _ptable;
}

