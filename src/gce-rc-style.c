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

#include <gtk/gtk.h>
#include <string.h>
#include "gce-rc-style.h"
#include "gce-serialize.h"
#include "gce-style.h"

static ccss_stylesheet_t	*_stylesheet = NULL;
gpointer		*_stylesheet_owner = NULL;

ccss_stylesheet_t const *
gce_rc_style_get_stylesheet (void)
{
	return _stylesheet;
}

struct _GceRcStyle
{
	GtkRcStyle parent;
};

struct _GceRcStyleClass
{
	GtkRcStyleClass parent;
};

static GType            gce_rc_style_type = 0;
static GtkRcStyleClass *gce_rc_style_parent_class = NULL;

static GtkStyle*
create_style (GtkRcStyle *rc_style)
{
    return GTK_STYLE (g_object_new (GCE_TYPE_STYLE, NULL));
}

static guint 
parse (GtkRcStyle  *rc_style, 
       GtkSettings *settings, 
       GScanner    *scanner)
{
	static GQuark scope_id = 0;

	char	*gce_file;
	guint	 old_scope;
	guint	 token;
		
	if (!scope_id)
		scope_id = g_quark_from_string ("gce_engine");
	
	old_scope = g_scanner_set_scope (scanner, scope_id);
	
	token = g_scanner_peek_next_token (scanner);
	if (token != G_TOKEN_RIGHT_CURLY) {

		token = g_scanner_get_next_token (scanner);
		g_assert (token == G_TOKEN_IDENTIFIER && 
			  0 == strcmp ("href", scanner->value.v_identifier));

		token = g_scanner_get_next_token (scanner);
		g_assert (token == '=');

		token = g_scanner_get_next_token (scanner);
		g_assert (token == G_TOKEN_STRING);
		g_assert (_stylesheet == NULL);
		gce_file = gtk_rc_find_pixmap_in_path (gtk_settings_get_default (), 
				scanner, scanner->value.v_string);
		_stylesheet = ccss_stylesheet_new_from_file (gce_file);
#ifdef GCE_RAPID_DEVELOPMENT
		G_STMT_START
		char *rc_string;
		rc_string = gce_serialize (_stylesheet);
		if (rc_string) {
			gtk_rc_parse_string (rc_string);
			g_free (rc_string), rc_string = NULL;
		}
		G_STMT_END
#endif
		_stylesheet_owner = (gpointer) rc_style;
		g_free (gce_file), gce_file = NULL;
	}

	g_scanner_get_next_token (scanner);	
	g_scanner_set_scope (scanner, old_scope);

	return G_TOKEN_NONE;
}

static void 
merge (GtkRcStyle *dst, 
       GtkRcStyle *src)
{
	gce_rc_style_parent_class->merge (dst, src);
}

static void
finalize (GObject *instance)
{
	/*
	GceRcStyle *self;
	self = GCE_RC_STYLE (instance);
	*/

	if (_stylesheet_owner == (gpointer) instance) {
		ccss_stylesheet_free (_stylesheet);
	}

	G_OBJECT_CLASS (gce_rc_style_parent_class)->finalize (instance);
}

static void 
instance_init (GceRcStyle *self)
{
}

static void 
class_init (GceRcStyleClass *klass)
{
	GObjectClass	*go_class;
	GtkRcStyleClass *rc_style_class;

	gce_rc_style_parent_class = g_type_class_peek_parent (klass);

	go_class = G_OBJECT_CLASS (klass);
	go_class->finalize = finalize;

	rc_style_class = GTK_RC_STYLE_CLASS (klass);
	rc_style_class->create_style = create_style;
	rc_style_class->parse = parse;
	rc_style_class->merge = merge;
}

void 
gce_rc_style_register_type (GTypeModule *module)
{
	if (!gce_rc_style_type) {
		static const GTypeInfo info = {
			sizeof(GceRcStyleClass),
			NULL,
			NULL,
			(GClassInitFunc) class_init,
			NULL,                   /* class_finalize */
			NULL,                   /* class_data */
			sizeof (GceRcStyle),
			0,                      /* n_preallocs */
			(GInstanceInitFunc) instance_init,
		};

		gce_rc_style_type = g_type_module_register_type (module, 
								  GTK_TYPE_RC_STYLE, 
								  "GceRcStyle", 
								  &info, 0);
	}
}

GType
gce_rc_style_get_type (void) 
{
	return gce_rc_style_type;
}
