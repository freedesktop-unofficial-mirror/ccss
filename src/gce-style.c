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
#include "gce-maps.h"
#include "gce-node.h"
#include "gce-rc-style.h"
#include "gce-style.h"

struct GceStyle_
{
	GtkStyle parent;
};

struct GceStyleClass_
{
	GtkStyleClass parent;
};

static GType          gce_style_type = 0;
static GtkStyleClass *gce_style_parent_class = NULL;

static ccss_selector_group_t *
setup (GceNode const *node, 
       GceNode const *base)
{
	ccss_stylesheet_t const	*stylesheet;
	ccss_selector_group_t	*group;
	gboolean		 status;

	stylesheet = gce_rc_style_get_stylesheet ();
	group = ccss_selector_group_new ();
	status = FALSE;

	/* query for widget theming */
	status |= ccss_stylesheet_query_collect (stylesheet, (ccss_node_t const *) node, group, FALSE);

	/* query for primitive theming */
	status |= ccss_stylesheet_query_collect (stylesheet, (ccss_node_t const *) base, group, TRUE);

	if (!status) {
		g_warning ("Un-themed widget `%s', primitive `%s'.", 
			   G_OBJECT_TYPE_NAME (G_OBJECT (gce_node_get_widget (node))), gce_node_get_primitive (base));
		ccss_selector_group_free (group);
		group = NULL;
	}

	return group;
}

static void
rectangle (GtkStyle		*self, 
	   GceNode const	*node,
	   GceNode const	*base,
	   GdkWindow		*window, 
	   GdkRectangle		*area, 
	   GtkWidget		*widget, 
	   gint			 x, 
	   gint			 y, 
	   gint			 width, 
	   gint			 height,
	   gboolean		 fill)
{
	ccss_selector_group_t	*group;
	ccss_style_t		 style;
	cairo_t			*cr;

	group = setup (node, base);
	if (group) {
		cr = gdk_cairo_create (window);
		ccss_style_init (&style);

		if (area) {
			gdk_cairo_rectangle (cr, area);
			cairo_clip (cr);
		}

		/* sanitise */
		if (width == -1 || height == -1) {
			gint w, h;
			gdk_drawable_get_size (GDK_DRAWABLE (window), &w, &h);
			width = width == -1 ? w : width;
			height = height == -1 ? h : height;
		}

		ccss_selector_group_apply (group, &style);

		/* Hackishly support "background-attachment: fixed". */
		ccss_style_set_viewport (&style,
			widget->allocation.x, widget->allocation.y,
			widget->allocation.width, widget->allocation.height);

		if (fill) {
			ccss_style_draw_rectangle (&style, cr, x, y, width, height);
		} else {
			ccss_style_draw_outline (&style, cr, x, y, width, height);
		}

		cairo_destroy (cr);
		ccss_selector_group_free (group);
	}
}

static void
gap (GtkStyle		*self, 
     GceNode const	*node,
     GceNode const	*base,
     GdkWindow		*window, 
     GdkRectangle	*area, 
     GtkWidget		*widget, 
     gint		 x, 
     gint		 y, 
     gint		 width, 
     gint		 height,
     GtkPositionType	 gap_side,
     gint		 gap_start,
     gint		 gap_width)
{
	ccss_selector_group_t	*group;
	ccss_style_t		 style;
	cairo_t			*cr;

	group = setup (node, base);
	if (group) {
		cr = gdk_cairo_create (window);
		ccss_style_init (&style);

		if (area) {
			gdk_cairo_rectangle (cr, area);
			cairo_clip (cr);
		}

		/* sanitise */
		if (width == -1 || height == -1) {
			gint w, h;
			gdk_drawable_get_size (GDK_DRAWABLE (window), &w, &h);
			width = width == -1 ? w : width;
			height = height == -1 ? h : height;
		}

		ccss_selector_group_apply (group, &style);

		/* Hackishly support "background-attachment: fixed". */
		ccss_style_set_viewport (&style,
			widget->allocation.x, widget->allocation.y,
			widget->allocation.width, widget->allocation.height);

		ccss_style_draw_gap (&style, cr, x, y, width, height, gap_side, gap_start, gap_width);

		cairo_destroy (cr);
		ccss_selector_group_free (group);
	}
}

static void
line (GtkStyle		*self, 
      GceNode		*node,
      GceNode		*base,
      GdkWindow		*window, 
      GdkRectangle	*area, 
      GtkWidget		*widget, 
      gint		 x1, 
      gint		 x2, 
      gint		 y1, 
      gint		 y2)
{
	ccss_selector_group_t	*group;
	ccss_style_t		 style;
	cairo_t			*cr;

	group = setup (node, base);
	if (group) {
		cr = gdk_cairo_create (window);
		ccss_style_init (&style);

		if (area) {
			gdk_cairo_rectangle (cr, area);
			cairo_clip (cr);
		}

		ccss_selector_group_apply (group, &style);

		/* Hackishly support "background-attachment: fixed". */
		ccss_style_set_viewport (&style,
			widget->allocation.x, widget->allocation.y,
			widget->allocation.width, widget->allocation.height);

		ccss_style_draw_line (&style, cr, x1, x2, y1, y2);

		cairo_destroy (cr);
		ccss_selector_group_free (group);
	}
}

static void 
draw_hline (GtkStyle		*self, 
	    GdkWindow		*window, 
	    GtkStateType	 state, 
	    GdkRectangle	*area, 
	    GtkWidget		*widget, 
	    char const		*detail, 
	    gint		 x1, 
	    gint		 x2, 
	    gint		 y)
{
	GceNode *node;
	GceNode *base;

	base = gce_node_cache_fetch_primitive_node (widget, 
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			NULL, NULL, NULL, NULL, NULL,
			"hline");

	node = gce_node_cache_fetch_node (widget,
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			NULL, NULL, NULL, NULL, NULL);

	line (self, node, base, window, area, widget, x1, x2, y, y);

	gce_node_cache_release_node (node);
	gce_node_cache_release_node (base);
}

static void 
draw_vline (GtkStyle		*self, 
	    GdkWindow		*window, 
	    GtkStateType	 state, 
	    GdkRectangle	*area, 
	    GtkWidget		*widget, 
	    char const		*detail, 
	    gint		 y1, 
	    gint		 y2, 
	    gint		 x)
{
	GceNode *node;
	GceNode *base;

	base = gce_node_cache_fetch_primitive_node (widget, 
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			NULL, NULL, NULL, NULL, NULL,
			"vline");

	node = gce_node_cache_fetch_node (widget,
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			NULL, NULL, NULL, NULL, NULL);

	line (self, node, base, window, area, widget, x, x, y1, y2);

	gce_node_cache_release_node (node);
	gce_node_cache_release_node (base);

}

static void 
draw_shadow (GtkStyle		*self, 
	     GdkWindow		*window, 
	     GtkStateType	 state, 
	     GtkShadowType	 shadow, 
	     GdkRectangle	*area, 
	     GtkWidget		*widget, 
	     char const		*detail, 
	     gint		 x, 
	     gint		 y, 
	     gint		 width, 
	     gint		 height)
{
	GceNode *node;
	GceNode *base;

	base = gce_node_cache_fetch_primitive_node (widget, 
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			gce_maps_get_shadow (shadow),
			NULL, NULL, NULL, NULL,
			"shadow");

	node = gce_node_cache_fetch_node (widget, 
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			gce_maps_get_shadow (shadow),
			NULL, NULL, NULL, NULL);

	rectangle (self, node, base, window, area, widget, 
		   x, y, width, height, TRUE);

	gce_node_cache_release_node (node);
	gce_node_cache_release_node (base);
}

static void 
draw_box (GtkStyle		*self, 
	  GdkWindow		*window, 
	  GtkStateType		 state, 
	  GtkShadowType		 shadow,
	  GdkRectangle		*area,
	  GtkWidget		*widget,
	  char const		*detail,
	  gint			 x,
	  gint			 y,
	  gint			 width,
	  gint			 height)
{
	GceNode *node;
	GceNode *base;

	base = gce_node_cache_fetch_primitive_node (widget, 
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			gce_maps_get_shadow (shadow),
			NULL, NULL, NULL, NULL,
			"box");

	node = gce_node_cache_fetch_node (widget, 
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			gce_maps_get_shadow (shadow),
			NULL, NULL, NULL, NULL);

	rectangle (self, node, base, window, area, widget,
		   x, y, width, height, TRUE);

	gce_node_cache_release_node (node);
	gce_node_cache_release_node (base);
}

static void 
draw_flat_box (GtkStyle		*self, 
	       GdkWindow	*window, 
	       GtkStateType	 state, 
	       GtkShadowType	 shadow,
	       GdkRectangle	*area,
	       GtkWidget	*widget,
	       char const	*detail,
	       gint		 x,
	       gint		 y,
	       gint		 width,
	       gint		 height)
{
	GceNode *node;
	GceNode *base;

	base = gce_node_cache_fetch_primitive_node (widget, 
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			gce_maps_get_shadow (shadow),
			NULL, NULL, NULL, NULL,
			"flatbox");

	node = gce_node_cache_fetch_node (widget, 
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			gce_maps_get_shadow (shadow),
			NULL, NULL, NULL, NULL);

	rectangle (self, node, base, window, area, widget, 
		   x, y, width, height, TRUE);

	gce_node_cache_release_node (node);
	gce_node_cache_release_node (base);
}

static void 
draw_check (GtkStyle		*self,
	    GdkWindow		*window,
	    GtkStateType	 state,
	    GtkShadowType	 shadow,
	    GdkRectangle	*area,
	    GtkWidget		*widget,
	    char const		*detail,
	    gint		 x,
	    gint		 y,
	    gint		 width,
	    gint		 height)
{
	GceNode *node;
	GceNode *base;

	base = gce_node_cache_fetch_primitive_node (widget, 
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			gce_maps_get_shadow (shadow),
			NULL, NULL, NULL, NULL,
			"check");

	node = gce_node_cache_fetch_node (widget, 
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			gce_maps_get_shadow (shadow),
			NULL, NULL, NULL, NULL);

	rectangle (self, node, base, window, area, widget, 
		   x, y, width, height, TRUE);

	gce_node_cache_release_node (node);
	gce_node_cache_release_node (base);
}

static void 
draw_option (GtkStyle		 *self,
	     GdkWindow		 *window,
	     GtkStateType	 state,
	     GtkShadowType	 shadow,
	     GdkRectangle	 *area,
	     GtkWidget		 *widget,
	     char const		 *detail,
	     gint		 x,
	     gint		 y,
	     gint		 width,
	     gint		 height)
{
	GceNode *node;
	GceNode *base;

	base = gce_node_cache_fetch_primitive_node (widget, 
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			gce_maps_get_shadow (shadow),
			NULL, NULL, NULL, NULL,
			"option");

	node = gce_node_cache_fetch_node (widget, 
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			gce_maps_get_shadow (shadow),
			NULL, NULL, NULL, NULL);

	rectangle (self, node, base, window, area, widget, 
		   x, y, width, height, TRUE);

	gce_node_cache_release_node (node);
	gce_node_cache_release_node (base);
}

static void 
draw_shadow_gap (GtkStyle		*self,
		 GdkWindow		*window,
		 GtkStateType		 state,
		 GtkShadowType		 shadow,
		 GdkRectangle		*area,
		 GtkWidget		*widget,
		 char const		*detail,
		 gint			 x,
		 gint			 y,
		 gint			 width,
		 gint			 height,
		 GtkPositionType	 gap_side,
		 gint			 gap_start,
		 gint			 gap_width)
{
	GceNode *node;
	GceNode *base;

	base = gce_node_cache_fetch_primitive_node (widget, 
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			gce_maps_get_shadow (shadow),
			NULL, gce_maps_get_position (gap_side), NULL, NULL,
			"shadow");

	node = gce_node_cache_fetch_node (widget, 
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			gce_maps_get_shadow (shadow),
			NULL, gce_maps_get_position (gap_side), NULL, NULL);

	gap (self, node, base, window, area, widget, 
	     x, y, width, height, gap_side, gap_start, gap_width);

	gce_node_cache_release_node (node);
	gce_node_cache_release_node (base);
}

static void 
draw_box_gap (GtkStyle			*self,
	      GdkWindow			*window,
	      GtkStateType		 state,
	      GtkShadowType		 shadow,
	      GdkRectangle		*area,
	      GtkWidget			*widget,
	      char const		*detail,
	      gint			 x,
	      gint			 y,
	      gint			 width,
	      gint			 height,
	      GtkPositionType		 gap_side,
	      gint			 gap_start,
	      gint			 gap_width)
{
	GceNode *node;
	GceNode *base;

	base = gce_node_cache_fetch_primitive_node (widget, 
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			gce_maps_get_shadow (shadow),
			NULL, gce_maps_get_position (gap_side), NULL, NULL,
			"box");

	node = gce_node_cache_fetch_node (widget, 
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			gce_maps_get_shadow (shadow),
			NULL, gce_maps_get_position (gap_side), NULL, NULL);

	gap (self, node, base, window, area, widget, 
	     x, y, width, height, gap_side, gap_start, gap_width);

	gce_node_cache_release_node (node);
	gce_node_cache_release_node (base);
}

static void 
draw_extension (GtkStyle	*self,
		GdkWindow	*window,
		GtkStateType	 state,
		GtkShadowType	 shadow,
		GdkRectangle	*area,
		GtkWidget	*widget,
		char const	*detail,
		gint		 x,
		gint		 y,
		gint		 width,
		gint		 height,
		GtkPositionType	 gap_side)
{
	GceNode *node;
	GceNode *base;
	gint	 gap_width;

	base = gce_node_cache_fetch_primitive_node (widget, 
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			gce_maps_get_shadow (shadow),
			NULL, gce_maps_get_position (gap_side), NULL, NULL,
			"extension");

	node = gce_node_cache_fetch_node (widget, 
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			gce_maps_get_shadow (shadow),
			NULL, gce_maps_get_position (gap_side), NULL, NULL);

	switch (gap_side) {
	case GTK_POS_LEFT:
	case GTK_POS_RIGHT:
		gap_width = height;
		break;
	case GTK_POS_TOP:
	case GTK_POS_BOTTOM:
		gap_width = width;
		break;
	}

	gap (self, node, base, window, area, widget, 
	     x, y, width, height, gap_side, 0, gap_width);

	gce_node_cache_release_node (node);
	gce_node_cache_release_node (base);
}

static void 
draw_slider (GtkStyle		*self,
	     GdkWindow		*window,
	     GtkStateType	 state,
	     GtkShadowType	 shadow,
	     GdkRectangle	*area,
	     GtkWidget		*widget,
	     char const		*detail,
	     gint		 x,
	     gint		 y,
	     gint		 width,
	     gint		 height,
	     GtkOrientation	 orientation)
{
	GceNode *node;
	GceNode *base;

	base = gce_node_cache_fetch_primitive_node (widget,
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			gce_maps_get_shadow (shadow),
			gce_maps_get_orientation (orientation),
			NULL, NULL, NULL,
			"slider");

	node = gce_node_cache_fetch_node (widget, 
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			gce_maps_get_shadow (shadow),
			gce_maps_get_orientation (orientation),
			NULL, NULL, NULL);

	rectangle (self, node, base, window, area, widget, 
		   x, y, width, height, TRUE);

	gce_node_cache_release_node (node);
	gce_node_cache_release_node (base);
}

static void 
draw_handle (GtkStyle		*self,
	     GdkWindow		*window,
	     GtkStateType	 state,
	     GtkShadowType	 shadow,
	     GdkRectangle	*area,
	     GtkWidget		*widget,
	     char const		*detail,
	     gint		 x,
	     gint		 y,
	     gint		 width,
	     gint		 height,
	     GtkOrientation	 orientation)
{
	GceNode *node;
	GceNode *base;

	base = gce_node_cache_fetch_primitive_node (widget,
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			gce_maps_get_shadow (shadow),
			gce_maps_get_orientation (orientation),
			NULL, NULL, NULL,
			"handle");

	node = gce_node_cache_fetch_node (widget, 
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			gce_maps_get_shadow (shadow),
			gce_maps_get_orientation (orientation),
			NULL, NULL, NULL);

	rectangle (self, node, base, window, area, widget, 
		   x, y, width, height, TRUE);

	gce_node_cache_release_node (node);
	gce_node_cache_release_node (base);
}

static void 
draw_resize_grip (GtkStyle		*self,
		  GdkWindow		*window,
		  GtkStateType		 state,
		  GdkRectangle		*area,
		  GtkWidget		*widget,
		  char const		*detail,
		  GdkWindowEdge		 edge,
		  gint			 x,
		  gint			 y,
		  gint			 width,
		  gint			 height)
{
	GceNode *node;
	GceNode *base;

	base = gce_node_cache_fetch_primitive_node (widget,
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			NULL, NULL, NULL, gce_maps_get_window_edge (edge), NULL,
			"resizegrip");

	node = gce_node_cache_fetch_node (widget, 
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			NULL, NULL, NULL, gce_maps_get_window_edge (edge), NULL);

	rectangle (self, node, base, window, area, widget, 
		   x, y, width, height, TRUE);

	gce_node_cache_release_node (node);
	gce_node_cache_release_node (base);
}

static void 
draw_focus (GtkStyle		*self,
	    GdkWindow		*window,
	    GtkStateType	 state,
	    GdkRectangle	*area,
	    GtkWidget		*widget,
	    char const		*detail,
	    gint		 x,
	    gint		 y,
	    gint		 width,
	    gint		 height)
{
	GceNode *node;
	GceNode *base;

	base = gce_node_cache_fetch_primitive_node (widget,
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			NULL, NULL, NULL, NULL, NULL,
			"focus");

	node = gce_node_cache_fetch_node (widget, 
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			NULL, NULL, NULL, NULL, NULL);

	rectangle (self, node, base, window, area, widget, 
		   x, y, width, height, FALSE);

	gce_node_cache_release_node (node);
	gce_node_cache_release_node (base);
}

static void
draw_expander (GtkStyle		*self,
	       GdkWindow	*window,
	       GtkStateType	 state,
	       GdkRectangle	*area,
	       GtkWidget	*widget,
	       char const	*detail,
	       gint		 x,
	       gint		 y,
	       GtkExpanderStyle	 style)
{
#define DEFAULT_EXPANDER_SIZE 12
	GceNode *node;
	GceNode *base;
	gint	 expander_size;

	if (widget &&
	    gtk_widget_class_find_style_property (GTK_WIDGET_GET_CLASS (widget),
						  "expander-size")) {
		gtk_widget_style_get (widget,
				      "expander-size", &expander_size,
				      NULL);
	} else {
		expander_size = DEFAULT_EXPANDER_SIZE;
	}

	base = gce_node_cache_fetch_primitive_node (widget,
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			NULL, NULL, NULL, NULL, gce_maps_get_expander_style (style),
			"expander");

	node = gce_node_cache_fetch_node (widget, 
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			NULL, NULL, NULL, NULL, gce_maps_get_expander_style (style));

	rectangle (self, node, base, window, area, widget, 
		   x, y, expander_size, expander_size, TRUE);

	gce_node_cache_release_node (node);
	gce_node_cache_release_node (base);
#undef DEFAULT_EXPANDER_SIZE
}

static void
draw_diamond (GtkStyle		*self,
	      GdkWindow		*window,
	      GtkStateType	 state,
	      GtkShadowType	 shadow,
	      GdkRectangle	*area,
	      GtkWidget		*widget,
	      char const	*detail,
	      gint		 x,
	      gint		 y,
	      gint		 width,
	      gint		 height)
{
	GceNode *node;
	GceNode *base;

	base = gce_node_cache_fetch_primitive_node (widget,
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			gce_maps_get_shadow (shadow),
			NULL, NULL, NULL, NULL,
			"diamond");

	node = gce_node_cache_fetch_node (widget, 
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			gce_maps_get_shadow (shadow),
			NULL, NULL, NULL, NULL);

	rectangle (self, node, base, window, area, widget, 
		   x, y, width, height, TRUE);

	gce_node_cache_release_node (node);
	gce_node_cache_release_node (base);
}

/*
 * TODO: handle `fill'?
 */
static void
draw_arrow (GtkStyle		*self,
	    GdkWindow		*window,
	    GtkStateType	 state,
	    GtkShadowType	 shadow,
	    GdkRectangle	*area,
	    GtkWidget		*widget,
	    char const		*detail,
	    GtkArrowType	 arrow,
	    gboolean		 fill,
	    gint		 x,
	    gint		 y,
	    gint		 width,
	    gint		 height)
{
	GceNode *node;
	GceNode *base;

	base = gce_node_cache_fetch_primitive_node (widget,
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			gce_maps_get_shadow (shadow),
			gce_maps_get_arrow (arrow),
			NULL, NULL, NULL,
			"arrow");

	node = gce_node_cache_fetch_node (widget, 
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			gce_maps_get_shadow (shadow),
			gce_maps_get_arrow (arrow),
			NULL, NULL, NULL);

	rectangle (self, node, base, window, area, widget, 
		   x, y, width, height, TRUE);

	gce_node_cache_release_node (node);
	gce_node_cache_release_node (base);
}

static void
draw_tab (GtkStyle		*self,
	  GdkWindow		*window,
	  GtkStateType		 state,
	  GtkShadowType		 shadow,
	  GdkRectangle		*area,
	  GtkWidget		*widget,
	  char const		*detail,
	  gint			 x,
	  gint			 y,
	  gint			 width,
	  gint			 height)
{
	GceNode *node;
	GceNode *base;

	base = gce_node_cache_fetch_primitive_node (widget,
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			gce_maps_get_shadow (shadow),
			NULL, NULL, NULL, NULL,
			"tab");

	node = gce_node_cache_fetch_node (widget, 
			gce_maps_get_detail (detail),
			gce_maps_get_state (state),
			gce_maps_get_shadow (shadow),
			NULL, NULL, NULL, NULL);

	rectangle (self, node, base, window, area, widget, 
		   x, y, width, height, TRUE);

	gce_node_cache_release_node (node);
	gce_node_cache_release_node (base);
}

static void 
instance_init (GceStyle *self)
{

}

static void 
class_init (GceStyleClass *klass)
{
	GtkStyleClass *style_class = GTK_STYLE_CLASS (klass);

	gce_style_parent_class = g_type_class_peek_parent (klass);

	style_class->draw_hline = draw_hline;
	style_class->draw_vline = draw_vline;
	style_class->draw_shadow = draw_shadow;
	/*
	draw_polygon
	*/
	style_class->draw_arrow = draw_arrow;
	style_class->draw_diamond = draw_diamond;
	style_class->draw_box = draw_box;
	style_class->draw_flat_box = draw_flat_box;
	style_class->draw_check = draw_check;
	style_class->draw_option = draw_option;
	style_class->draw_tab = draw_tab;
	style_class->draw_shadow_gap = draw_shadow_gap;
	style_class->draw_box_gap = draw_box_gap;
	style_class->draw_extension = draw_extension;
	style_class->draw_focus = draw_focus;
	style_class->draw_slider = draw_slider;
	style_class->draw_handle = draw_handle;
	style_class->draw_expander = draw_expander;
/*
draw_layout
*/
	style_class->draw_resize_grip = draw_resize_grip;
}

void
gce_style_register_type (GTypeModule *module)
{
	if (!gce_style_type) {
		static const GTypeInfo info = {
			sizeof (GceStyleClass),
			NULL,
			NULL,
			(GClassInitFunc) class_init,
			NULL,                   /* class_finalize */
			NULL,                   /* class_data */
			sizeof (GceStyle),
			0,                      /* n_preallocs */
			(GInstanceInitFunc) instance_init,
		};

		gce_style_type = g_type_module_register_type (module, 
							       GTK_TYPE_STYLE, 
							       "GceStyle", 
							       &info, 0);
	}
}

GType
gce_style_get_type (void) 
{
	return gce_style_type;
}
