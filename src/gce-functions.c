/* The CSS Theme Engine for Gtk+.
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

#include <ccss/ccss.h>

/* Need the ccss include above for the feature defs. */
#ifdef CCSS_WITH_SOUP
#include <libsoup/soup.h>
#endif

#include "gce-functions.h"
#include "gce-color.h"

/*
 * TODO make this bullet proof wrt uri scheme.
 */
static char *
url (GSList const *args)
{
	char		*given_path;
	char		*resolved_path;
	char		*ret;
#ifdef CCSS_WITH_SOUP
	SoupURI		*uri;

	g_return_val_if_fail (args, NULL);

	given_path = g_strdup_printf ("file:///%s", (char const *) args->data);
	uri = soup_uri_new (given_path);
	g_free (given_path), given_path = NULL;

	resolved_path = gtk_rc_find_pixmap_in_path (gtk_settings_get_default (), NULL, uri->path);
	soup_uri_set_path (uri, resolved_path);
	g_free (resolved_path), resolved_path = NULL;

	ret = soup_uri_to_string (uri, FALSE);
	soup_uri_free (uri), uri = NULL;
#else
	g_return_val_if_fail (args, NULL);

	given_path = (char const *) args->data;
	resolved_path = gtk_rc_find_pixmap_in_path (gtk_settings_get_default (), NULL, given_path);
	ret = resolved_path;
#endif

	return ret;
}

static gboolean
color_walk_r (GdkColor		 *color,
	     GSList const	**args,
	     char const		 *function)
{
	double		 factor;
	char const	*token;
	gboolean	 ret;

	g_return_val_if_fail (*args && (*args)->data, FALSE);

	if (function)
		token = function;
	else
		token = (char const *) (*args)->data;

	if (0 == g_strcmp0 ("rgb(", token)) {

		double red, green, blue;

		if (!function) { *args = (*args)->next; if (!*args) return FALSE; }

		/* red */
		red = g_ascii_strtod ((char const *) (*args)->data, NULL);
		*args = (*args)->next; if (!*args) return FALSE;
		
		/* "," */
		if (0 != g_strcmp0(",", (char const *) (*args)->data)) return FALSE;
		*args = (*args)->next; if (!*args) return FALSE;

		/* green */
		green = g_ascii_strtod ((char const *) (*args)->data, NULL);
		*args = (*args)->next; if (!*args) return FALSE;
		
		/* "," */
		if (0 != g_strcmp0(",", (char const *) (*args)->data)) return FALSE;
		*args = (*args)->next; if (!*args) return FALSE;

		/* blue */
		blue = g_ascii_strtod ((char const *) (*args)->data, NULL);
		
		if (!function) {
			/* ")" */
			*args = (*args)->next; if (!*args) return FALSE;
			if (0 != g_strcmp0(")", (char const *) (*args)->data)) return FALSE;
		}

		color->red = (guint16) (red * 65535.0);
		color->green = (guint16) (green * 65535.0);
		color->blue = (guint16) (blue * 65535.0);

		return TRUE;

	} else if (0 == g_strcmp0 ("gtk-color(", token)) {

		if (!function) { *args = (*args)->next; if (!*args) return FALSE; }

		/* color */
		ret = gce_color_lookup ((char const *) (*args)->data, color);

		if (!function) {
			/* ")" */
			*args = (*args)->next; if (!*args) return FALSE;
			if (0 != g_strcmp0(")", (char const *) (*args)->data)) return FALSE;
		}

		return ret;

	} else if (0 == g_strcmp0 ("gtk-mix(", token)) {

		GdkColor color1;
		GdkColor color2;

		if (!function) { *args = (*args)->next; if (!*args) return FALSE; }

		/* factor */
		factor = g_ascii_strtod ((char const *) (*args)->data, NULL);
		*args = (*args)->next; if (!*args) return FALSE;
		
		/* "," */
		if (0 != g_strcmp0(",", (char const *) (*args)->data)) return FALSE;
		*args = (*args)->next; if (!*args) return FALSE;

		/* color1 */
		if (!color_walk_r (&color1, args, NULL)) return FALSE;

		/* "," */
		if (0 != g_strcmp0(",", (char const *) (*args)->data)) return FALSE;
		*args = (*args)->next; if (!*args) return FALSE;

		/* color2 */
		if (!color_walk_r (&color2, args, NULL)) return FALSE;

		if (!function) {
			/* ")" */
			*args = (*args)->next; if (!*args) return FALSE;
			if (0 != g_strcmp0(")", (char const *) (*args)->data)) return FALSE;
		}

		return gce_color_mix (factor, &color1, &color2, color);

	} else if (0 == g_strcmp0 ("gtk-shade(", token)) {

		if (!function) { *args = (*args)->next; if (!*args) return FALSE; }

		/* factor */
		factor = g_ascii_strtod ((char const *) (*args)->data, NULL);
		*args = (*args)->next; if (!*args) return FALSE;
		
		/* "," */
		if (0 != g_strcmp0(",", (char const *) (*args)->data)) return FALSE;
		*args = (*args)->next; if (!*args) return FALSE;

		/* color */
		if (!color_walk_r (color, args, NULL)) return FALSE;

		if (!function) {
			/* ")" */
			*args = (*args)->next; if (!*args) return FALSE;
			if (0 != g_strcmp0(")", (char const *) (*args)->data)) return FALSE;
		}

		return gce_color_shade (factor, color);

	} else if (0 == g_strcmp0 ("gtk-darker(", token)) {

		if (!function) { *args = (*args)->next; if (!*args) return FALSE; }

		/* color */
		if (!color_walk_r (color, args, NULL)) return FALSE;

		if (!function) {
			/* ")" */
			*args = (*args)->next; if (!*args) return FALSE;
			if (0 != g_strcmp0(")", (char const *) (*args)->data)) return FALSE;
		}

		return gce_color_shade (0.7, color);

	} else if (0 == g_strcmp0 ("gtk-lighter(", token)) {

		if (!function) { *args = (*args)->next; if (!*args) return FALSE; }

		/* color */
		if (!color_walk_r (color, args, NULL)) return FALSE;

		if (!function) {
			/* ")" */
			*args = (*args)->next; if (!*args) return FALSE;
			if (0 != g_strcmp0(")", (char const *) (*args)->data)) return FALSE;
		}

		return gce_color_shade (1.3, color);

	} else {

		/* Color. */
		ret = gce_color_lookup (token, color);
		*args = (*args)->next;
		return ret;
	}

	return FALSE;
}

static char *
dump (GSList const *args)
{
	GSList const	*iter;
	GString		*str;

	str = g_string_new (NULL);

	for ( iter = args; iter != NULL; iter = iter->next) {
		g_string_append (str, (char const *) iter->data);
	}

	return g_string_free (str, FALSE);
}

static char *
color (GSList const *args)
{
	GSList const	*iter;
	GdkColor	 result;
	gboolean	 ret;

	g_return_val_if_fail (args && args->data, NULL);

	ret = color_walk_r (&result, &args, "gtk-color(");
	if (!ret) {
		char *data;
		data = dump (args);
		g_warning ("Color could not be resolved: `gtk-color(%s)'", data);
		return NULL;
	}

	return g_strdup_printf ("rgb(%f,%f,%f)", result.red / 65535.0, 
						 result.green / 65535.0,
						 result.blue / 65535.0);
}

static char *
mix (GSList const *args)
{
	GSList const	*iter;
	GdkColor	 result;
	gboolean	 ret;

	g_return_val_if_fail (args && args->data, NULL);

	iter = args;
	ret = color_walk_r (&result, &iter, "gtk-mix(");
	if (!ret) {
		char *data;
		data = dump (args);
		g_warning ("Color could not be resolved: `gtk-mix(%s)'", data);
		return NULL;
	}

	return g_strdup_printf ("rgb(%f,%f,%f)", result.red / 65535.0, 
						 result.green / 65535.0,
						 result.blue / 65535.0);
}

static char *
shade (GSList const *args)
{
	GSList const	*iter;
	GdkColor	 result;
	gboolean	 ret;

	g_return_val_if_fail (args && args->data, NULL);

	iter = args;
	ret = color_walk_r (&result, &iter, "gtk-shade(");
	if (!ret) {
		char *data;
		data = dump (args);
		g_warning ("Color could not be resolved: `gtk-shade(%s)'", data);
		return NULL;
	}

	return g_strdup_printf ("rgb(%f,%f,%f)", result.red / 65535.0, 
						 result.green / 65535.0,
						 result.blue / 65535.0);
}

static char *
lighter (GSList const *args)
{
	GSList const	*iter;
	GdkColor	 result;
	gboolean	 ret;

	g_return_val_if_fail (args && args->data, NULL);

	iter = args;
	ret = color_walk_r (&result, &iter, "gtk-lighter(");
	if (!ret) {
		char *data;
		data = dump (args);
		g_warning ("Color could not be resolved: `gtk-lighter(%s)'", data);
		return NULL;
	}

	return g_strdup_printf ("rgb(%f,%f,%f)", result.red / 65535.0, 
						 result.green / 65535.0,
						 result.blue / 65535.0);
}

static char *
darker (GSList const *args)
{
	GSList const	*iter;
	GdkColor	 result;
	gboolean	 ret;

	g_return_val_if_fail (args && args->data, NULL);

	iter = args;
	ret = color_walk_r (&result, &iter, "gtk-darker(");
	if (!ret) {
		char *data;
		data = dump (args);
		g_warning ("Color could not be resolved: `gtk-darker(%s)'", data);
		return NULL;
	}

	return g_strdup_printf ("rgb(%f,%f,%f)", result.red / 65535.0, 
						 result.green / 65535.0,
						 result.blue / 65535.0);
}


static ccss_function_t const _functions[] = 
{
  { "url",		url	},
  { "gtk-color",	color	},
  { "gtk-mix",		mix	},
  { "gtk-shade",	shade	},
  { "gtk-lighter",	lighter	},
  { "gtk-darker",	darker	},
  { NULL }
};

ccss_function_t const *
gce_functions_get_vtable (void)
{
	return _functions;
}

