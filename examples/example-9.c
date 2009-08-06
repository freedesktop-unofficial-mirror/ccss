/* vim: set ts=8 sw=8 noexpandtab: */

#include <stdlib.h>
#include <cairo.h>
#include <ccss-cairo/ccss-cairo.h>
#include <gtk/gtk.h>
#include "config.h"

static gboolean
expose_cb (GtkWidget		*widget,
	   GdkEventExpose       *event,
	   ccss_stylesheet_t	*stylesheet)
{
	ccss_style_t	*style;
	cairo_t		*cr;
	uint32_t	 hash;

	style = ccss_stylesheet_query_type (stylesheet, "box");
	g_return_val_if_fail (style, FALSE);

	hash = ccss_style_hash (style);
	g_debug ("%s() style instance='%p' hash='%u'",
		 __FUNCTION__,
		 (void *) style,
		 hash);

	cr = gdk_cairo_create (widget->window);
	ccss_cairo_style_draw_rectangle (style, cr, 
					 widget->allocation.x + 10,
					 widget->allocation.y + 10,
					 widget->allocation.width - 20, 
					 widget->allocation.height - 20);

	cairo_destroy (cr);
	ccss_style_destroy (style);

	return FALSE;
}

static char const _css[] = "			\
	box { 					\
		border: 3px solid black;	\
		border-top: 3px dotted red;	\
		border-radius: 3px;		\
	}					\
";

int
main (int	  argc,
      char	**argv)
{
	ccss_grammar_t		*grammar;
	ccss_stylesheet_t	*stylesheet;
	GtkWidget		*window;
	GtkWidget		*area;

	gtk_init (&argc, &argv);

	grammar = ccss_cairo_grammar_create ();
	stylesheet = ccss_grammar_create_stylesheet_from_buffer (grammar,
							_css, sizeof (_css),
							NULL);

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size (GTK_WINDOW (window), 160, 90);
	g_signal_connect (G_OBJECT (window), "delete-event", 
			  G_CALLBACK (gtk_main_quit), NULL);

	area = gtk_drawing_area_new ();
	gtk_container_add (GTK_CONTAINER (window), area);
	g_signal_connect (G_OBJECT (area), "expose-event", 
			  G_CALLBACK (expose_cb), stylesheet);

	gtk_widget_show_all (window);
	gtk_main ();

	ccss_stylesheet_destroy (stylesheet);
	ccss_grammar_destroy (grammar);

	return EXIT_SUCCESS;
}

