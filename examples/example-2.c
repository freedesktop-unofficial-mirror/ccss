
#include <stdlib.h>
#include <cairo.h>
#include <ccss-cairo/ccss-cairo.h>
#include <gtk/gtk.h>

typedef struct {
	ccss_node_t	 parent;
	char const	*type_name;
	ptrdiff_t	 instance;
	char const	*inline_css;
} node_t;

static char const *
get_type (node_t const *self)
{
	return self->type_name;
}

static ptrdiff_t
get_instance (node_t const *self)
{
	return self->instance;
}

static char const *
get_style (node_t const *self)
{
	return self->inline_css;
}

static ccss_node_class_t _node_class = {
	.get_type		= (ccss_node_get_type_f) get_type,
	.get_instance		= (ccss_node_get_instance_f) get_instance,
	.get_style		= (ccss_node_get_style_f) get_style
};

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
		background-color: grey;		\
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
	node_t				 node;
	GtkWidget			*window;
	gboolean			 ret;

	gtk_init (&argc, &argv);
	ccss_cairo_init ();

	stylesheet = ccss_stylesheet_new_from_buffer (_css, sizeof (_css));

	ccss_node_init ((ccss_node_t *) &node, &_node_class);
	node.type_name = "box";
	node.instance = 0xdeadbeef;
	node.inline_css = "background-color: yellow";

	style = ccss_style_new ();
	ret = ccss_stylesheet_query (stylesheet, (ccss_node_t const *) &node,
				       style);
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

