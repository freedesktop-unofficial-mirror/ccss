/* vim: set ts=8 sw=8 noexpandtab: */

#include <stdlib.h>
#include <cairo.h>
#include <ccss-cairo/ccss-cairo.h>
#include <gtk/gtk.h>
#include "config.h"

typedef struct {
	char const	*type_name;
	ptrdiff_t	 instance;
	char const	*inline_css;
} nodeinfo_t;

static char const *
get_type (ccss_node_t const *self)
{
	nodeinfo_t *info = ccss_node_get_user_data (self);

	return info->type_name;
}

static ptrdiff_t
get_instance (ccss_node_t const *self)
{
	nodeinfo_t *info = ccss_node_get_user_data (self);

	return info->instance;
}

static char const *
get_style (ccss_node_t const	*self,
	   unsigned int		 descriptor)
{
	nodeinfo_t *info = ccss_node_get_user_data (self);

	return info->inline_css;
}

static ccss_node_class_t _node_class = {
	.is_a			= NULL,
	.get_container		= NULL,
	.get_base_style		= NULL,
	.get_instance		= get_instance,
	.get_id			= NULL,
	.get_type		= get_type,
	.get_classes		= NULL,
	.get_pseudo_classes	= NULL,
	.get_attribute		= NULL,
	.get_style		= get_style,
	.get_viewport		= NULL,
	.release		= NULL
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
	ccss_grammar_t		*grammar;
	ccss_stylesheet_t	*stylesheet;
	ccss_style_t		*style;
	ccss_node_t		*node;
	GtkWidget		*window;
	GtkWidget		*area;
	nodeinfo_t		 info;

	gtk_init (&argc, &argv);

	grammar = ccss_cairo_grammar_create ();
	stylesheet = ccss_grammar_create_stylesheet_from_buffer (grammar,
							_css, sizeof (_css),
							NULL);
	info.type_name = "box";
	info.instance = 0xdeadbeef;
	info.inline_css = "background-color: yellow";
	node = ccss_node_create (&_node_class,
				 CCSS_NODE_CLASS_N_METHODS (_node_class),
				 &info);
	style = ccss_stylesheet_query (stylesheet, node);
	ccss_node_destroy (node);
	g_assert (style);

#ifdef CCSS_DEBUG
	ccss_style_dump (style);
#endif

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size (GTK_WINDOW (window), 160, 90);
	g_signal_connect (G_OBJECT (window), "delete-event", 
			  G_CALLBACK (gtk_main_quit), NULL);

	area = gtk_drawing_area_new ();
	gtk_container_add (GTK_CONTAINER (window), area);
	g_signal_connect (G_OBJECT (area), "expose-event",
			  G_CALLBACK (expose_cb), style);

	gtk_widget_show_all (window);
	gtk_main ();

	ccss_style_destroy (style);
	ccss_stylesheet_destroy (stylesheet);
	ccss_grammar_destroy (grammar);

	return EXIT_SUCCESS;
}

