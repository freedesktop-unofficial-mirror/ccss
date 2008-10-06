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

#ifndef GCE_RC_STYLE_H
#define GCE_RC_STYLE_H

#include <ccss/ccss.h>
#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define GCE_TYPE_RC_STYLE              (gce_rc_style_get_type ())
#define GCE_RC_STYLE(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), GCE_TYPE_RC_STYLE, GceRcStyle))
#define GCE_RC_STYLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), GCE_TYPE_RC_STYLE, GceRcStyleClass))
#define GCE_IS_RC_STYLE(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), GCE_TYPE_RC_STYLE))
#define GCE_IS_RC_STYLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GCE_TYPE_RC_STYLE))
#define GCE_RC_STYLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), GCE_TYPE_RC_STYLE, GceRcStyleClass))

typedef struct _GceRcStyle GceRcStyle;
typedef struct _GceRcStyleClass GceRcStyleClass;

void  gce_rc_style_register_type (GTypeModule *module);
GType gce_rc_style_get_type (void) G_GNUC_CONST;

ccss_stylesheet_t const * gce_rc_style_get_stylesheet (void);

G_END_DECLS

#endif /* GCE_RC_STYLE_H */
