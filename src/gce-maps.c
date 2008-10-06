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

#include <string.h>
#include "gce-maps.h"

/**
 * Map between detail strings in gtk and css format.
 */
static const struct {
	char const *gtk_detail;
	char const *css_detail;
} _detail_map[] = {
  { "add-mode",				"add-mode" },
  { "arrow",				"arrow" },
  { "bar",				"bar" },
  { "base",				"base" },
  { "button",				"button" },
  { "buttondefault",			"buttondefault" },
  { "calendar",				"calendar" },
  { "calendar-day",			"calendar-day" },
  { "cellcheck",			"cellcheck" },
  { "cell_even",			"cell-even" },
  { "cell_even_end",			"cell-even-end" },
  { "cell_even_middle",			"cell-even-middle" },
  { "cell_even_ruled",			"cell-even-ruled" },
  { "cell_even_ruled_end",		"cell-even-ruled-end" },
  { "cell_even_ruled_middle",		"cell-even-ruled-middle" },
  { "cell_even_ruled_sorted",		"cell-even-ruled-sorted" },
  { "cell_even_ruled_sorted_end",	"cell-even-ruled-sorted-end" },
  { "cell_even_ruled_sorted_middle",	"cell-even-ruled-sorted-middle" },
  { "cell_even_ruled_sorted_start",	"cell-even-ruled-sorted-start" },
  { "cell_even_ruled_start",		"cell-even-ruled-start" },
  { "cell_even_sorted",			"cell-even-sorted" },
  { "cell_even_sorted_end",		"cell-even-sorted-end" },
  { "cell_even_sorted_middle",		"cell-even-sorted-middle" },
  { "cell_even_sorted_start",		"cell-even-sorted-start" },
  { "cell_even_start",			"cell-even-start" },
  { "cell_odd",				"cell-odd" },
  { "cell_odd_end",			"cell-odd-end" },
  { "cell_odd_middle",			"cell-odd-middle" },
  { "cell_odd_ruled",			"cell-odd-ruled" },
  { "cell_odd_ruled_end",		"cell-odd-ruled-end" },
  { "cell_odd_ruled_middle",		"cell-odd-ruled-middle" },
  { "cell_odd_ruled_sorted",		"cell-odd-ruled-sorted" },
  { "cell_odd_ruled_sorted_end",	"cell-odd-ruled-sorted-end" },
  { "cell_odd_ruled_sorted_middle",	"cell-odd-ruled-sorted-middle" },
  { "cell_odd_ruled_sorted_start",	"cell-odd-ruled-sorted-start" },
  { "cell_odd_ruled_start",		"cell-odd-ruled-start" },
  { "cell_odd_sorted",			"cell-odd-sorted" },
  { "cell_odd_sorted_end",		"cell-odd-sorted-end" },
  { "cell_odd_sorted_middle",		"cell-odd-sorted-middle" },
  { "cell_odd_sorted_start",		"cell-odd-sorted-start" },
  { "cell_odd_start",			"cell-odd-start" },
  { "cellradio",			"cellradio" },
  { "check",				"check" },
  { "checkbutton",			"checkbutton" },
  { "colorwheel_dark",			"colorwheel-dark" },
  { "colorwheel_light",			"colorwheel-light" },
  { "combobox",				"combobox" },
  { "curve_bg",				"curve-bg" },
  { "dnd",				"dnd" },
  { "entry",				"entry" },
  { "entry_bg",				"entry-bg" },
  { "eventbox",				"eventbox" },
  { "expander",				"expander" },
  { "frame",				"frame" },
  { "handle",				"handle" },
  { "handlebox",			"handlebox" },
  { "handlebox_bin",			"handlebox-bin" },
  { "hruler",				"hruler" },
  { "hscale",				"hscale" },
  { "hscrollbar",			"hscrollbar" },
  { "hseparator",			"hseparator" },
  { "icon_view",			"icon-view" },
  { "iconview-drop-indicator",		"iconview-drop-indicator" },
  { "listitem",				"listitem" },
  { "menu",				"menu" },
  { "menubar",				"menubar" },
  { "menuitem",				"menuitem" },
  { "menu_scroll_arrow_down",		"menu-scroll-arrow-down" },
  { "menu_scroll_arrow_up",		"menu-scroll-arrow-up" },
  { "notebook",				"notebook" },
  { "option",				"option" },
  { "optionmenu",			"optionmenu" },
  { "optionmenutab",			"optionmenutab" },
  { "paned",				"paned" },
  { "radiobutton",			"radiobutton" },
  { "scrolled_window",			"scrolled-window" },
  { "spinbutton",			"spinbutton" },
  { "spinbutton_down",			"spinbutton-down" },
  { "spinbutton_up",			"spinbutton-up" },
  { "statusbar",			"statusbar" },
  { "tab",				"tab" },
  { "tearoffmenuitem",			"tearoffmenuitem" },
  { "text",				"text" },
  { "textview",				"textview" },
  { "through",				"through" },
  { "toolbar",				"toolbar" },
  { "tooltip",				"tooltip" },
  { "tray-icon",			"tray-icon" },
  { "treeitem",				"treeitem" },
  { "treeview",				"treeview" },
  { "treeview-drop-indicator",		"treeview-drop-indicator" },
  { "treeview-drop-indicator-left",	"treeview-drop-indicator-left" },
  { "treeview-drop-indicator-middle",	"treeview-drop-indicator-middle" },
  { "treeview-drop-indicator-right",	"treeview-drop-indicator-right" },
  { "treeview-left",			"treeview-left" },
  { "treeview-middle",			"treeview-middle" },
  { "treeview-right",			"treeview-right" },
  { "trough",				"trough" },
  { "trough-fill-level",		"trough-fill-level" },
  { "trough-fill-level-full",		"trough-fill-level-full" },
  { "trough-lower",			"trough-lower" },
  { "trough-upper",			"trough-upper" },
  { "viewport",				"viewport" },
  { "viewportbin",			"viewportbin" },
  { "vruler",				"vruler" },
  { "vscale",				"vscale" },
  { "vscrollbar",			"vscrollbar" },
  { "vseparator",			"vseparator" }
};

/**
 * Find gtk detail string for a given css detail.
 *
 * \note This could be sped up by manually comparing char by char. First 
 * traversing 'til the first one matches, then goin on with the second one ...
 * If it would buy us anyting is another question.
 */
char const *
gce_maps_match_detail (char const *css_detail)
{
	if (!css_detail) {
		return NULL;
	}

	for (unsigned int i = 0; i < G_N_ELEMENTS (_detail_map); i++) {
		if (0 == strcmp (_detail_map[i].css_detail, css_detail)) {
			return _detail_map[i].gtk_detail;
		}
	}

	return NULL;
}

char const *
gce_maps_get_detail (char const *gtk_detail)
{
	if (!gtk_detail) {
		return NULL;
	}

	for (unsigned int i = 0; i < G_N_ELEMENTS (_detail_map); i++) {
		if (0 == strcmp (_detail_map[i].gtk_detail, gtk_detail)) {
			return _detail_map[i].css_detail;
		}
	}

	return NULL;
}

/**
 * Map between states in gtk and css format.
 */
static const struct {
	GtkStateType	 gtk_state;
	char const	*css_state;
} _state_map[] = {
  { GTK_STATE_NORMAL,		"normal" },
  { GTK_STATE_ACTIVE,		"active" },
  { GTK_STATE_PRELIGHT,		"prelight" },
  { GTK_STATE_SELECTED,		"selected" },
  { GTK_STATE_INSENSITIVE,	"insensitive" }
};

/**
 * Default is GTK_STATE_NORMAL.
 */
GtkStateType
gce_maps_match_state (char const *css_state)
{
	g_return_val_if_fail (css_state, GTK_STATE_NORMAL);

	for (unsigned int i = 0; i < G_N_ELEMENTS (_state_map); i++) {
		if (0 == strcmp (_state_map[i].css_state, css_state)) {
			return _state_map[i].gtk_state;
		}
	}

	return GTK_STATE_NORMAL;
}

char const *
gce_maps_get_state (GtkStateType gtk_state)
{
	for (unsigned int i = 0; i < G_N_ELEMENTS (_state_map); i++) {
		if (_state_map[i].gtk_state == gtk_state) {
			return _state_map[i].css_state;
		}
	}

	return NULL;
}

/**
 * Map between shadow types in gtk and css format.
 */
static const struct {
	GtkShadowType	 gtk_shadow;
	char const	*css_shadow;
} _shadow_map[] = {
  { GTK_SHADOW_NONE,		"none" },
  { GTK_SHADOW_IN,		"in" },
  { GTK_SHADOW_OUT,		"out" },
  { GTK_SHADOW_ETCHED_IN,	"etched-in" },
  { GTK_SHADOW_ETCHED_OUT,	"etched-out" }
};

/**
 * Default is GTK_SHADOW_NONE.
 */
GtkShadowType
gce_maps_match_shadow (char const *css_shadow)
{
	g_return_val_if_fail (css_shadow, GTK_SHADOW_NONE);

	for (unsigned int i = 0; i < G_N_ELEMENTS (_shadow_map); i++) {
		if (0 == strcmp (_shadow_map[i].css_shadow, css_shadow)) {
			return _shadow_map[i].gtk_shadow;
		}
	}

	return GTK_SHADOW_NONE;
}

char const *
gce_maps_get_shadow (GtkShadowType gtk_shadow)
{
	for (unsigned int i = 0; i < G_N_ELEMENTS (_shadow_map); i++) {
		if (_shadow_map[i].gtk_shadow == gtk_shadow) {
			return _shadow_map[i].css_shadow;
		}
	}

	return NULL;
}

/**
 * Map between arrow types in gtk and css format.
 */
static const struct {
	GtkArrowType	 gtk_arrow;
	char const	*css_arrow;
} _arrow_map[] = {
  { GTK_ARROW_UP,	"up" },
  { GTK_ARROW_DOWN,	"down" },
  { GTK_ARROW_LEFT,	"left" },
  { GTK_ARROW_RIGHT,	"right" },
  { GTK_ARROW_NONE,	"none" }
};

/**
 * Default is GTK_ARROW_NONE.
 */
GtkArrowType
gce_maps_match_arrow (char const *css_arrow)
{
	g_return_val_if_fail (css_arrow, GTK_ARROW_NONE);

	for (unsigned int i = 0; i < G_N_ELEMENTS (_arrow_map); i++) {
		if (0 == strcmp (_arrow_map[i].css_arrow, css_arrow)) {
			return _arrow_map[i].gtk_arrow;
		}
	}

	return GTK_ARROW_NONE;
}

char const *
gce_maps_get_arrow (GtkArrowType gtk_arrow)
{
	for (unsigned int i = 0; i < G_N_ELEMENTS (_arrow_map); i++) {
		if (_arrow_map[i].gtk_arrow == gtk_arrow) {
			return _arrow_map[i].css_arrow;
		}
	}

	return NULL;
}

/**
 * Map between position types in gtk and css format.
 */
static const struct {
	GtkPositionType	 gtk_position;
	char const	*css_position;
} _position_map[] = {
  { GTK_POS_LEFT,	"left" },
  { GTK_POS_RIGHT,	"right" },
  { GTK_POS_TOP,	"top" },
  { GTK_POS_BOTTOM,	"bottom" },
};

/**
 * Default is GTK_POS_LEFT.
 */
GtkPositionType
gce_maps_match_position (char const *css_position)
{
	g_return_val_if_fail (css_position, GTK_POS_LEFT);

	for (unsigned int i = 0; i < G_N_ELEMENTS (_position_map); i++) {
		if (0 == strcmp (_position_map[i].css_position, css_position)) {
			return _position_map[i].gtk_position;
		}
	}

	return GTK_POS_LEFT;
}

char const *
gce_maps_get_position (GtkPositionType gtk_position)
{
	for (unsigned int i = 0; i < G_N_ELEMENTS (_position_map); i++) {
		if (_position_map[i].gtk_position == gtk_position) {
			return _position_map[i].css_position;
		}
	}

	return NULL;
}

/**
 * Map between edge types in gdk and css format.
 */
static const struct {
	GdkWindowEdge	 gdk_window_edge;
	char const	*css_window_edge;
} _window_edge_map[] = {
  { GDK_WINDOW_EDGE_NORTH_WEST,	"north-west" },
  { GDK_WINDOW_EDGE_NORTH,	"north" },
  { GDK_WINDOW_EDGE_NORTH_EAST,	"north-east" },
  { GDK_WINDOW_EDGE_WEST,	"west" },
  { GDK_WINDOW_EDGE_EAST,	"east" },
  { GDK_WINDOW_EDGE_SOUTH_WEST,	"south-west" },
  { GDK_WINDOW_EDGE_SOUTH,	"south" },
  { GDK_WINDOW_EDGE_SOUTH_EAST,	"south-east" },
};

/**
 * Default is GDK_WINDOW_EDGE_SOUTH_EAST.
 */
GdkWindowEdge
gce_maps_match_window_edge (char const *css_window_edge)
{
	g_return_val_if_fail (css_window_edge, GDK_WINDOW_EDGE_SOUTH_EAST);

	for (unsigned int i = 0; i < G_N_ELEMENTS (_window_edge_map); i++) {
		if (0 == strcmp (_window_edge_map[i].css_window_edge, css_window_edge)) {
			return _window_edge_map[i].gdk_window_edge;
		}
	}

	return GDK_WINDOW_EDGE_SOUTH_EAST;
}

char const *
gce_maps_get_window_edge (GdkWindowEdge gdk_window_edge)
{
	for (unsigned int i = 0; i < G_N_ELEMENTS (_window_edge_map); i++) {
		if (_window_edge_map[i].gdk_window_edge == gdk_window_edge) {
			return _window_edge_map[i].css_window_edge;
		}
	}

	return NULL;
}

/**
 * Map between orientation types in gtk and css format.
 */
static const struct {
	GtkOrientation	 gtk_orientation;
	char const	*css_orientation;
} _orientation_map[] = {
  { GTK_ORIENTATION_HORIZONTAL,	"horizontal" },
  { GTK_ORIENTATION_VERTICAL,	"vertical" }
};

/**
 * Default is GTK_ORIENTATION_HORIZONTAL.
 */
GtkOrientation
gce_maps_match_orientation (char const *css_orientation)
{
	g_return_val_if_fail (css_orientation, GTK_ORIENTATION_HORIZONTAL);

	for (unsigned int i = 0; i < G_N_ELEMENTS (_orientation_map); i++) {
		if (0 == strcmp (_orientation_map[i].css_orientation, css_orientation)) {
			return _orientation_map[i].gtk_orientation;
		}
	}

	return GTK_ORIENTATION_HORIZONTAL;
}

char const *
gce_maps_get_orientation (GtkOrientation gtk_orientation)
{
	for (unsigned int i = 0; i < G_N_ELEMENTS (_orientation_map); i++) {
		if (_orientation_map[i].gtk_orientation == gtk_orientation) {
			return _orientation_map[i].css_orientation;
		}
	}

	return NULL;
}







/**
 * Map between expander style types in gtk and css format.
 */
static const struct {
	GtkExpanderStyle	 gtk_expander_style;
	char const		*css_expander_style;
} _expander_style_map[] = {
  { GTK_EXPANDER_COLLAPSED,		"collapsed" },
  { GTK_EXPANDER_SEMI_COLLAPSED,	"semi-collapsed" },
  { GTK_EXPANDER_SEMI_EXPANDED,		"semi-expanded" },
  { GTK_EXPANDER_EXPANDED,		"expanded" }
};

/**
 * Default is GTK_EXPANDER_COLLAPSED.
 */
GtkExpanderStyle
gce_maps_match_expander_style (char const *css_expander_style)
{
	g_return_val_if_fail (css_expander_style, GTK_EXPANDER_COLLAPSED);

	for (unsigned int i = 0; i < G_N_ELEMENTS (_expander_style_map); i++) {
		if (0 == strcmp (_expander_style_map[i].css_expander_style, css_expander_style)) {
			return _expander_style_map[i].gtk_expander_style;
		}
	}

	return GTK_EXPANDER_COLLAPSED;
}

char const *
gce_maps_get_expander_style (GtkExpanderStyle gtk_expander_style)
{
	for (unsigned int i = 0; i < G_N_ELEMENTS (_expander_style_map); i++) {
		if (_expander_style_map[i].gtk_expander_style == gtk_expander_style) {
			return _expander_style_map[i].css_expander_style;
		}
	}

	return NULL;
}

