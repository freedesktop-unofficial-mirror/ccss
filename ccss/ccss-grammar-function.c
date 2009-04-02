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
#include "ccss-grammar-priv.h"
#include "config.h"

static GSList *
parse_args_r (GSList		 *args,
	      CRTerm const	**values)
{
	char		*val;
	char const	*unit;
	CRTerm const	*args_term;

	while (*values) {
		val = NULL;
		switch ((*values)->type) {
		case TERM_NUMBER:
			switch ((*values)->content.num->type) {
			case NUM_GENERIC:
				unit = "";
				break;
			case NUM_LENGTH_PX:
				unit = "px";
				break;
			case NUM_PERCENTAGE:
				unit = "%";
				break;
			default:
				g_assert_not_reached ();
				unit = "";
			}
			val = g_strdup_printf ("%f%s", (*values)->content.num->val, unit);
			args = g_slist_prepend (args, val);
			break;
		case TERM_STRING:
		case TERM_IDENT:
		case TERM_URI:
			val = g_strdup_printf ("%s", cr_string_peek_raw_str ((*values)->content.str));
			args = g_slist_prepend (args, val);
			break;
		case TERM_HASH:
			val = g_strdup_printf ("#%s", cr_string_peek_raw_str ((*values)->content.str));
			args = g_slist_prepend (args, val);			
			break;
		case TERM_FUNCTION:

			val = g_strdup_printf ("%s(", cr_string_peek_raw_str ((*values)->content.str));
			args = g_slist_prepend (args, val);

			args_term = (*values)->ext_content.func_param;
			args = parse_args_r (args, &args_term);

			val = g_strdup (")");
			args = g_slist_prepend (args, val);

			break;
		case TERM_RGB:

			val = g_strdup ("rgb(");
			args = g_slist_prepend (args, val);

			val = g_strdup_printf ("%f", (*values)->content.rgb->red / 255.);
			args = g_slist_prepend (args, val);

			val = g_strdup (",");
			args = g_slist_prepend (args, val);

			val = g_strdup_printf ("%f", (*values)->content.rgb->green / 255.);
			args = g_slist_prepend (args, val);

			val = g_strdup (",");
			args = g_slist_prepend (args, val);

			val = g_strdup_printf ("%f", (*values)->content.rgb->blue / 255.);
			args = g_slist_prepend (args, val);

			val = g_strdup (")");
			args = g_slist_prepend (args, val);

			break;
		case TERM_NO_TYPE:
		case TERM_UNICODERANGE:
		default:
			g_assert_not_reached ();
			*values = (*values)->next;
			continue;
		}

		if ((*values)->next) {
			val = g_strdup (",");
			args = g_slist_prepend (args, val);
		}

		*values = (*values)->next;
	}

	return args;
}
/**
 * ccss_grammar_invoke_function:
 * @self:		a #ccss_grammar_t.
 * @function_name:	name of the function to invoke, e.g. `url'.
 * @args:		arguments passed to the function handler.
 * @user_data:		user-data passed to the function handler.
 *			Overrides the user-data assigned in the function handler's definition.
 *
 * Invoke a registerd function handler. This API is meant to be used by property
 * implementations, like when parsing properties like `background-image: url(foo.png)'.
 *
 * Returns: string value passed back by the ccss API consumer.
 **/
char *
ccss_grammar_invoke_function (ccss_grammar_t const	*self,
			      char const		*function_name,
			      CRTerm const		*values,
			      void			*user_data)
{
	ccss_function_t const	*handler;
	GSList			*args;
	void			*actual_user_data;
	char			*ret;

	g_return_val_if_fail (self && function_name, NULL);

	handler = (ccss_function_t const *)
			g_hash_table_lookup (self->functions, function_name);

	if (!handler) {
		g_warning ("Function `%s' could not be resolved.", function_name);
		return NULL;
	}

	/* parse args */
	args = parse_args_r (NULL, &values);
	args = g_slist_reverse (args);

	/* dispatch */
	if (user_data) {
		actual_user_data = user_data;
	} else {
		actual_user_data = handler->user_data;
	}
	ret = handler->function (args, actual_user_data);

	/* free args */
	while (args) {
		g_free (args->data);
		args = g_slist_delete_link (args, args);
	}

	return ret;
}

