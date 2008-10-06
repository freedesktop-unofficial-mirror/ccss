/* The Cairo CSS Drawing Library.
 * Copyright (C) 2008 Robert Staudinger
 *
 * Portions of  this file are derived  from gtkrc.c, licensed under
 * the GNU Lesser General  Public License; either version  2 of the
 * License,  or (at your option) any  later version.  Copyright (C)
 * the GTK+ Team, see the ChangeLog at ftp://ftp.gtk.org/pub/gtk/.
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
#include "gce-color.h"

gboolean
gce_color_lookup (char const	*spec,
		  GdkColor	*color)
{
	GtkSettings	*settings;
	GHashTable	*colors;
	GdkColor	*system_color;

	g_return_val_if_fail (spec, NULL);

	settings = gtk_settings_get_default ();
	colors = NULL;
	g_object_get (G_OBJECT (settings), "color-hash", &colors, NULL);
	g_return_val_if_fail (colors, NULL);

	system_color = (GdkColor *) g_hash_table_lookup (colors, spec);
	if (system_color) {
		color->red = system_color->red;
		color->green = system_color->green;
		color->blue = system_color->blue;
		return TRUE;
	}

	/* Try to parse the color. */
	return gdk_color_parse (spec, color);
}

static void
rgb_to_hls (gdouble *r,
            gdouble *g,
            gdouble *b)
{
  gdouble min;
  gdouble max;
  gdouble red;
  gdouble green;
  gdouble blue;
  gdouble h, l, s;
  gdouble delta;
  
  red = *r;
  green = *g;
  blue = *b;
  
  if (red > green)
    {
      if (red > blue)
        max = red;
      else
        max = blue;
      
      if (green < blue)
        min = green;
      else
        min = blue;
    }
  else
    {
      if (green > blue)
        max = green;
      else
        max = blue;
      
      if (red < blue)
        min = red;
      else
        min = blue;
    }
  
  l = (max + min) / 2;
  s = 0;
  h = 0;
  
  if (max != min)
    {
      if (l <= 0.5)
        s = (max - min) / (max + min);
      else
        s = (max - min) / (2 - max - min);
      
      delta = max -min;
      if (red == max)
        h = (green - blue) / delta;
      else if (green == max)
        h = 2 + (blue - red) / delta;
      else if (blue == max)
        h = 4 + (red - green) / delta;
      
      h *= 60;
      if (h < 0.0)
        h += 360;
    }
  
  *r = h;
  *g = l;
  *b = s;
}

static void
hls_to_rgb (gdouble *h,
            gdouble *l,
            gdouble *s)
{
  gdouble hue;
  gdouble lightness;
  gdouble saturation;
  gdouble m1, m2;
  gdouble r, g, b;
  
  lightness = *l;
  saturation = *s;
  
  if (lightness <= 0.5)
    m2 = lightness * (1 + saturation);
  else
    m2 = lightness + saturation - lightness * saturation;
  m1 = 2 * lightness - m2;
  
  if (saturation == 0)
    {
      *h = lightness;
      *l = lightness;
      *s = lightness;
    }
  else
    {
      hue = *h + 120;
      while (hue > 360)
        hue -= 360;
      while (hue < 0)
        hue += 360;
      
      if (hue < 60)
        r = m1 + (m2 - m1) * hue / 60;
      else if (hue < 180)
        r = m2;
      else if (hue < 240)
        r = m1 + (m2 - m1) * (240 - hue) / 60;
      else
        r = m1;
      
      hue = *h;
      while (hue > 360)
        hue -= 360;
      while (hue < 0)
        hue += 360;
      
      if (hue < 60)
        g = m1 + (m2 - m1) * hue / 60;
      else if (hue < 180)
        g = m2;
      else if (hue < 240)
        g = m1 + (m2 - m1) * (240 - hue) / 60;
      else
        g = m1;
      
      hue = *h - 120;
      while (hue > 360)
        hue -= 360;
      while (hue < 0)
        hue += 360;
      
      if (hue < 60)
        b = m1 + (m2 - m1) * hue / 60;
      else if (hue < 180)
        b = m2;
      else if (hue < 240)
        b = m1 + (m2 - m1) * (240 - hue) / 60;
      else
        b = m1;
      
      *h = r;
      *l = g;
      *s = b;
    }
}

gboolean
gce_color_mix (double		 factor,
	       GdkColor const	*color1,
	       GdkColor const	*color2,
	       GdkColor		*result)
{
	result->red   = factor * color1->red   + (1.0 - factor) * color2->red;
	result->green = factor * color1->green + (1.0 - factor) * color2->green;
	result->blue  = factor * color1->blue  + (1.0 - factor) * color2->blue;

	return TRUE;
}

gboolean
gce_color_shade (double		 factor,
		 GdkColor	*color)
{
  gdouble red;
  gdouble green;
  gdouble blue;
  
  red = (gdouble) color->red / 65535.0;
  green = (gdouble) color->green / 65535.0;
  blue = (gdouble) color->blue / 65535.0;
  
  rgb_to_hls (&red, &green, &blue);
  
  green *= factor;
  if (green > 1.0)
    green = 1.0;
  else if (green < 0.0)
    green = 0.0;
  
  blue *= factor;
  if (blue > 1.0)
    blue = 1.0;
  else if (blue < 0.0)
    blue = 0.0;
  
  hls_to_rgb (&red, &green, &blue);

  color->red = (guint16) (red * 65535.0);
  color->green = (guint16) (green * 65535.0);
  color->blue = (guint16) (blue * 65535.0);

  return TRUE;
}

