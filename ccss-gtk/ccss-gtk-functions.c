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

#include <ccss/ccss.h>
#include <ccss/ccss-function-impl.h>
#include <gtk/gtk.h>
#include "ccss-gtk-functions.h"
#include "ccss-gtk-color.h"
#include "config.h"

#ifdef CCSS_WITH_SOUP
#include <libsoup/soup.h>
#endif

/*
 * FIXME: make this bullet proof wrt uri scheme.
 */
static char *
url (GSList const	*args,
     void		*user_data)
{
	char		*resolved_path;
	char		*ret;
#ifdef CCSS_WITH_SOUP
	char		*given_path;
	SoupURI		*uri;

	g_return_val_if_fail (args, NULL);

	given_path = g_strdup_printf ("file:///%s", (char const *) args->data);
	uri = soup_uri_new (given_path);
	g_free (given_path), given_path = NULL;

	resolved_path = gtk_rc_find_pixmap_in_path (gtk_settings_get_default (), NULL, uri->path);
	soup_uri_set_path (uri, resolved_path);
	g_free (resolved_path), resolved_path = NULL;

	ret = soup_uri_to_string (uri, false);
	soup_uri_free (uri), uri = NULL;
#else
	char const *given_path;

	g_return_val_if_fail (args, NULL);

	given_path = (char const *) args->data;
	resolved_path = gtk_rc_find_pixmap_in_path (gtk_settings_get_default (), NULL, given_path);
	ret = g_strdup_printf ("file:///%s", resolved_path);
	g_free (resolved_path), resolved_path = NULL;
#endif

	return ret;
}

static gboolean
color_walk_r (GdkColor		 *color,
	      GSList const	**args,
	      char const	 *function)
{
	double		 factor;
	char const	*token;
	gboolean	 ret;

	g_return_val_if_fail (*args && (*args)->data, false);

	if (function)
		token = function;
	else
		token = (char const *) (*args)->data;

	if (0 == g_strcmp0 ("rgb(", token)) {

		double red, green, blue;

		if (!function) { *args = (*args)->next; if (!*args) return false; }

		/* red */
		red = g_ascii_strtod ((char const *) (*args)->data, NULL);
		*args = (*args)->next; if (!*args) return false;

		/* "," */
		if (0 != g_strcmp0(",", (char const *) (*args)->data)) return false;
		*args = (*args)->next; if (!*args) return false;

		/* green */
		green = g_ascii_strtod ((char const *) (*args)->data, NULL);
		*args = (*args)->next; if (!*args) return false;

		/* "," */
		if (0 != g_strcmp0(",", (char const *) (*args)->data)) return false;
		*args = (*args)->next; if (!*args) return false;

		/* blue */
		blue = g_ascii_strtod ((char const *) (*args)->data, NULL);

		if (!function) {
			/* ")" */
			*args = (*args)->next; if (!*args) return false;
			if (0 != g_strcmp0(")", (char const *) (*args)->data)) return false;
		}

		color->red = (guint16) (red * 65535.0);
		color->green = (guint16) (green * 65535.0);
		color->blue = (guint16) (blue * 65535.0);

		return true;

	} else if (0 == g_strcmp0 ("gtk-color(", token)) {

		if (!function) { *args = (*args)->next; if (!*args) return false; }

		/* color */
		ret = ccss_gtk_color_lookup ((char const *) (*args)->data, color);

		if (!function) {
			/* ")" */
			*args = (*args)->next; if (!*args) return false;
			if (0 != g_strcmp0(")", (char const *) (*args)->data)) return false;
		}

		return ret;

	} else if (0 == g_strcmp0 ("gtk-mix(", token)) {

		GdkColor color1;
		GdkColor color2;

		if (!function) { *args = (*args)->next; if (!*args) return false; }

		/* factor */
		factor = g_ascii_strtod ((char const *) (*args)->data, NULL);
		*args = (*args)->next; if (!*args) return false;

		/* "," */
		if (0 != g_strcmp0(",", (char const *) (*args)->data)) return false;
		*args = (*args)->next; if (!*args) return false;

		/* color1 */
		if (!color_walk_r (&color1, args, NULL)) return false;

		/* "," */
		if (0 != g_strcmp0(",", (char const *) (*args)->data)) return false;
		*args = (*args)->next; if (!*args) return false;

		/* color2 */
		if (!color_walk_r (&color2, args, NULL)) return false;

		if (!function) {
			/* ")" */
			*args = (*args)->next; if (!*args) return false;
			if (0 != g_strcmp0(")", (char const *) (*args)->data)) return false;
		}

		return ccss_gtk_color_mix (factor, &color1, &color2, color);

	} else if (0 == g_strcmp0 ("gtk-shade(", token)) {

		if (!function) { *args = (*args)->next; if (!*args) return false; }

		/* factor */
		factor = g_ascii_strtod ((char const *) (*args)->data, NULL);
		*args = (*args)->next; if (!*args) return false;

		/* "," */
		if (0 != g_strcmp0(",", (char const *) (*args)->data)) return false;
		*args = (*args)->next; if (!*args) return false;

		/* color */
		if (!color_walk_r (color, args, NULL)) return false;

		if (!function) {
			/* ")" */
			*args = (*args)->next; if (!*args) return false;
			if (0 != g_strcmp0(")", (char const *) (*args)->data)) return false;
		}

		return ccss_gtk_color_shade (factor, color);

	} else if (0 == g_strcmp0 ("gtk-darker(", token)) {

		if (!function) { *args = (*args)->next; if (!*args) return false; }

		/* color */
		if (!color_walk_r (color, args, NULL)) return false;

		if (!function) {
			/* ")" */
			*args = (*args)->next; if (!*args) return false;
			if (0 != g_strcmp0(")", (char const *) (*args)->data)) return false;
		}

		return ccss_gtk_color_shade (0.7, color);

	} else if (0 == g_strcmp0 ("gtk-lighter(", token)) {

		if (!function) { *args = (*args)->next; if (!*args) return false; }

		/* color */
		if (!color_walk_r (color, args, NULL)) return false;

		if (!function) {
			/* ")" */
			*args = (*args)->next; if (!*args) return false;
			if (0 != g_strcmp0(")", (char const *) (*args)->data)) return false;
		}

		return ccss_gtk_color_shade (1.3, color);

	} else {

		/* Color. */
		ret = ccss_gtk_color_lookup (token, color);
		*args = (*args)->next;
		return ret;
	}

	return false;
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

	return g_string_free (str, false);
}

static char *
color (GSList const	*args,
       void		*user_data)
{
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

	return g_strdup_printf ("rgba(%f,%f,%f,1.0)", result.red / 65535.0,
						      result.green / 65535.0,
						      result.blue / 65535.0);
}

static char *
mix (GSList const	*args,
     void		*user_data)
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

	return g_strdup_printf ("rgba(%f,%f,%f,1.0)", result.red / 65535.0,
						      result.green / 65535.0,
						      result.blue / 65535.0);
}

static char *
shade (GSList const	*args,
       void		*user_data)
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

	return g_strdup_printf ("rgba(%f,%f,%f,1.0)", result.red / 65535.0,
						      result.green / 65535.0,
						      result.blue / 65535.0);
}

static char *
lighter (GSList const	*args,
	 void		*user_data)
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

	return g_strdup_printf ("rgba(%f,%f,%f,1.0)", result.red / 65535.0,
						      result.green / 65535.0,
						      result.blue / 65535.0);
}

static char *
darker (GSList const	*args,
	void		*user_data)
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

	return g_strdup_printf ("rgba(%f,%f,%f,1.0)", result.red / 65535.0,
						      result.green / 65535.0,
						      result.blue / 65535.0);
}

static ccss_function_t const _functions[] =
{
  { "url",		url,		NULL },
  { "gtk-color",	color,		NULL },
  { "gtk-mix",		mix,		NULL },
  { "gtk-shade",	shade,		NULL },
  { "gtk-lighter",	lighter,	NULL },
  { "gtk-darker",	darker,		NULL },
  { NULL }
};

ccss_function_t const *
ccss_gtk_functions_get_vtable (void)
{
	return _functions;
}

