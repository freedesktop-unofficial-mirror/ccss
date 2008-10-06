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
#include <gmodule.h>
#include <gtk/gtk.h>

#include "gce-functions.h"
#include "gce-node.h"
#include "gce-style.h"
#include "gce-rc-style.h"

/* This prototypes silence some warnings. */
G_MODULE_EXPORT void		theme_init		(GTypeModule *module);
G_MODULE_EXPORT void		theme_exit		(void);
G_MODULE_EXPORT GtkRcStyle *	theme_create_rc_style	(void);
G_MODULE_EXPORT const gchar *	g_module_check_init	(GModule *module);

G_MODULE_EXPORT void 
theme_init (GTypeModule *module)
{
	ccss_function_t const	*vtable;

	gce_rc_style_register_type (module);
	gce_style_register_type (module);

	vtable = gce_functions_get_vtable ();
	ccss_init (vtable);
}

G_MODULE_EXPORT void 
theme_exit (void)
{
	ccss_shutdown ();
}

G_MODULE_EXPORT GtkRcStyle*
theme_create_rc_style (void)
{
	return GTK_RC_STYLE (g_object_new (GCE_TYPE_RC_STYLE, NULL));
}

G_MODULE_EXPORT const gchar*
g_module_check_init (GModule *module)
{
	return gtk_check_version (GTK_MAJOR_VERSION, 
				  GTK_MINOR_VERSION, 
				  GTK_MICRO_VERSION - GTK_INTERFACE_AGE);
}

