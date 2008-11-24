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
#include <glib.h>
#include "ccss-function-priv.h"

static GHashTable *_function_handlers = NULL;

void
ccss_function_subsystem_add_functions (ccss_function_t const *functions)
{
	g_return_if_fail (functions);

	if (NULL == _function_handlers) {
		_function_handlers = g_hash_table_new (g_str_hash, g_str_equal);
	}

	for (unsigned int i = 0; functions[i].name != NULL; i++) {

		/* Handler already exists? */
		g_warn_if_fail (NULL == g_hash_table_lookup (_function_handlers, functions[i].name));

		g_hash_table_insert (_function_handlers,
				     (gpointer) functions[i].name,
				     (gpointer) &functions[i]);
	}
}

void
ccss_function_subsystem_shutdown (void)
{
	if (_function_handlers) {
		g_hash_table_destroy (_function_handlers);
		_function_handlers = NULL;
	}
}

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

char *
ccss_function_invoke (char const	*function_name,
		      CRTerm const	*values)
{
	ccss_function_t const	*handler;
	GSList			*args;
	char			*ret;

	g_return_val_if_fail (_function_handlers && function_name, NULL);

	handler = (ccss_function_t const *)
			g_hash_table_lookup (_function_handlers,
					     function_name);

	if (!handler) {
		g_warning ("Function `%s' could not be resolved.", function_name);
		return NULL;
	}

	/* parse args */
	args = parse_args_r (NULL, &values);
	args = g_slist_reverse (args);

	/* dispatch */
	ret = handler->function (args);

	/* free args */
	while (args) {
		g_free (args->data);
		args = g_slist_delete_link (args, args);
	}

	return ret;
}

