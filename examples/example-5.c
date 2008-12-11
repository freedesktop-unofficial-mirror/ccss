
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

static char const _css[] = "					\
	box { 							\
		border-image: url(example-5.svg#button-focused) 8 9 6 9;	\
	}							\
";

static char *
url (GSList const	*args,
     void		*user_data)
{
	char *cwd;
	char *path;
	char *uri;

	g_return_val_if_fail (args && args->data, NULL);

	cwd = g_get_current_dir ();
	path = g_build_filename (cwd, args->data, NULL);
	g_free (cwd), cwd = NULL;
	uri = g_strdup_printf ("file://%s", path);
	g_free (path), path = NULL;

	return uri;
}

static ccss_function_t const _functions[] = 
{
  { "url",	url,	NULL },
  { NULL }
};

int
main (int	  argc,
      char	**argv)
{
	ccss_grammar_t		*grammar;
	ccss_stylesheet_t	*stylesheet;
	ccss_style_t		*style;
	GtkWidget		*window;
	gboolean		 ret;

	gtk_init (&argc, &argv);

	grammar = ccss_cairo_grammar_create ();
	ccss_grammar_add_functions (grammar, _functions);
	stylesheet = ccss_grammar_create_stylesheet_from_buffer (grammar,
							_css, sizeof (_css),
							NULL);

	style = ccss_style_create ();
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

	ccss_style_destroy (style);
	ccss_stylesheet_destroy (stylesheet);
	ccss_grammar_destroy (grammar);

	return EXIT_SUCCESS;
}

