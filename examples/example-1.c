
#include <stdlib.h>
#include <cairo.h>
#include <ccss-cairo/ccss-cairo.h>
#include <gtk/gtk.h>

static gboolean
expose_cb (GtkWidget		*widget,
	   GdkEventExpose	*event,
	   ccss_style_t const	*style)
{
	cairo_t *cr;

	cr = gdk_cairo_create (widget->window);

	ccss_cairo_style_draw_rectangle (style, cr, 
					 widget->allocation.x + 10,
					 widget->allocation.y + 10,
					 widget->allocation.width - 20, 
					 widget->allocation.height - 20);

	cairo_destroy (cr);

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
	ccss_stylesheet_t		*stylesheet;
	ccss_style_t			*style;
	GtkWidget			*window;
	gboolean			 ret;

	gtk_init (&argc, &argv);
	ccss_cairo_init ();

	stylesheet = ccss_stylesheet_new_from_buffer (_css, sizeof (_css));
	/* stylesheet = ccss_stylesheet_new_from_file ("example-1.css"); */

	style = ccss_style_new ();
	ret = ccss_stylesheet_query_type (stylesheet, "box", style);
	g_assert (ret);

#ifdef CCSS_DEBUG
	ccss_style_dump (style);
#endif
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size (GTK_WINDOW (window), 160, 90);
	gtk_widget_set_app_paintable (window, TRUE);
	g_signal_connect (G_OBJECT (window), "delete-event", 
			  G_CALLBACK (gtk_main_quit), NULL);
	g_signal_connect (G_OBJECT (window), "expose-event", 
			  G_CALLBACK (expose_cb), style);

	gtk_widget_show_all (window);
	gtk_main ();

	ccss_style_free (style);
	ccss_stylesheet_free (stylesheet);

	ccss_cairo_shutdown ();

	return EXIT_SUCCESS;
}

