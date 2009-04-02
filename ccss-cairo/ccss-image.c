/* vim: set ts=8 sw=8 noexpandtab: */

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

#include <stdbool.h>
#include <string.h>
#include <glib.h>
#include "ccss-image.h"
#include "config.h"

#ifdef CCSS_WITH_SOUP
#include <libsoup/soup.h>
#endif

#if CCSS_WITH_RSVG
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>
#endif

void
ccss_image_discard (ccss_image_t *self)
{
	if (self->uri) {
		g_free (self->uri);
		self->uri = NULL;
	}

	if (self->pattern) {
		cairo_pattern_destroy (self->pattern);
		self->pattern = NULL;
	}
}

#if CCSS_WITH_RSVG

static bool
load_svg (ccss_image_t	*self,
	  char const	*uri,
	  char const	*id)
{
	RsvgHandle		*handle;
	GError			*error;
	RsvgDimensionData	 dimensions;
	cairo_t			*cr;
	cairo_surface_t		*surface;
	cairo_status_t		 status;
	bool			 ret;

	error = NULL;
	handle = rsvg_handle_new_from_file (uri, &error);
	if (error) {
		g_warning ("%s", error->message);
		g_warning ("Could not load `%s'", uri);
		g_error_free (error);
		return false;
	}

#if CCSS_WITH_SOUP
	if (id) {
		char			*fragment;
		RsvgPositionData	 position;

		fragment = g_strdup_printf ("#%s", id);

		rsvg_handle_get_dimensions_sub (handle, &dimensions, fragment);
		rsvg_handle_get_position_sub (handle, &position, fragment);
		self->width = dimensions.width;
		self->height = dimensions.height;
		surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 
						      self->width, self->height);
		cr = cairo_create (surface);
		cairo_translate (cr, -1 * position.x, -1 * position.y);
		ret = rsvg_handle_render_cairo (handle, cr);
		/* ret = rsvg_handle_render_cairo_sub (handle, cr, fragment); */
		status = cairo_status (cr);
		if (status != CAIRO_STATUS_SUCCESS) {
			g_warning ("%s", cairo_status_to_string (status));
		}

		g_free (fragment), fragment = NULL;
	} else {
#endif
		rsvg_handle_get_dimensions (handle, &dimensions);
		self->width = dimensions.width;
		self->height = dimensions.height;
		surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 
						      self->width, self->height);
		cr = cairo_create (surface);
		ret = rsvg_handle_render_cairo (handle, cr);
		status = cairo_status (cr);
		if (status != CAIRO_STATUS_SUCCESS) {
			g_warning ("%s", cairo_status_to_string (status));
		}
#if CCSS_WITH_SOUP
	}
#endif
	self->pattern = cairo_pattern_create_for_surface (surface);
	cairo_pattern_reference (self->pattern);
	cairo_surface_destroy (surface), surface = NULL;
	cairo_destroy (cr), cr = NULL;

	g_object_unref (G_OBJECT (handle)), handle = NULL;

	return true;
}

#endif /*  CCSS_WITH_RSVG */

static bool
load_png (ccss_image_t	*self,
	  char const	*path)
{
	cairo_surface_t	*surface;
	cairo_status_t	 status;

	surface = cairo_image_surface_create_from_png (path);
	status = cairo_surface_status (surface);
	if (status != CAIRO_STATUS_SUCCESS) {
		g_warning ("%s", cairo_status_to_string (status));
		cairo_surface_destroy (surface);
		return false;
	}

	self->width = cairo_image_surface_get_width (surface);
	self->height = cairo_image_surface_get_height (surface);

	self->pattern = cairo_pattern_create_for_surface (surface);
	cairo_pattern_reference (self->pattern);
	cairo_surface_destroy (surface), surface = NULL;

	return true;
}

static bool
load_image (ccss_image_t *self)
{
	cairo_status_t	 status;
	bool		 matched;
	char		*path;
	char const	*fragment;
#if CCSS_WITH_SOUP
	SoupURI		*uri;

	g_return_val_if_fail (self && self->uri, false);
	uri = soup_uri_new (self->uri);
	g_return_val_if_fail (uri, NULL);
	path = uri->path;
	fragment = uri->fragment;
#else
	g_return_val_if_fail (self && self->uri, false);
	path = g_filename_from_uri (self->uri, NULL, NULL);
	fragment = NULL;
#endif

	matched = false;

#if CCSS_WITH_RSVG
	if (!matched &&
	    g_str_has_suffix (path, ".svg")) {
		matched = load_svg (self, path, fragment);
	}
#endif

	if (!matched) {
		if (fragment)
			g_warning ("`%s' ignoring fragment", self->uri);
		matched = load_png (self, path);
	}

#if CCSS_WITH_SOUP
	soup_uri_free (uri), uri = NULL;
	path = NULL;
#else
	g_free (path), path = NULL;
#endif

	status = self->pattern ? cairo_pattern_status (self->pattern) : CAIRO_STATUS_SUCCESS;
	if (status != CAIRO_STATUS_SUCCESS) {
		g_warning ("%s", cairo_status_to_string (status));
	}

	return matched && self->pattern;
}

ccss_property_state_t
ccss_image_parse (ccss_image_t		 *self,
		  ccss_grammar_t const	 *grammar,
		  void			 *user_data,
		  CRTerm const		**value)
{
	switch ((*value)->type) {
	case TERM_IDENT:
		return ccss_property_parse_state (value);
	case TERM_URI:
		self->uri = ccss_grammar_invoke_function (grammar, "url", *value, user_data);
		*value = (*value)->next;
		return load_image (self) ? 
			CCSS_PROPERTY_STATE_SET :
			CCSS_PROPERTY_STATE_INVALID;
	default:
		return CCSS_PROPERTY_STATE_INVALID;
	}
}

