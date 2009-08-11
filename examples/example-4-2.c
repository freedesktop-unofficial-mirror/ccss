/* vim: set ts=8 sw=8 noexpandtab: */

#include <stdlib.h>
#include <string.h>
#include <cairo.h>
#include <ccss-cairo/ccss-cairo.h>
#include <gtk/gtk.h>
#include "config.h"

typedef struct {
	ccss_property_t	 base;
	char const	*font_family;   /* Owned by the stylesheet. */
} font_family_t;

static ccss_property_t *
font_family_new (ccss_grammar_t	*grammar,
		 CRTerm const	*values,
		 void		*user_data)
{
	font_family_t *self;

	g_return_val_if_fail (values, NULL);

	self = NULL;

	switch (values->type) {
	case TERM_IDENT:
	case TERM_STRING:
		self = g_new0 (font_family_t, 1);
		self->font_family = cr_string_peek_raw_str (values->content.str);
		break;
	default:
		/* Unhandled, fall thru. */
		;
	}

	return (ccss_property_t *) self;
}

static gboolean
expose_cb (GtkWidget		*widget,
	   GdkEventExpose	*event,
	   ccss_style_t const	*style)
{
	cairo_t		*cr;
	font_family_t	*property;

	cr = gdk_cairo_create (widget->window);

	property = NULL;
	ccss_style_interpret_property (style, "font-family",
				       (ccss_property_create_f) font_family_new,
				       NULL,
				       (ccss_property_t **) &property);
	if (property) {
		PangoContext    *context;
		PangoLayout	*layout;

		context = gtk_widget_get_pango_context (widget);
		layout = pango_layout_new (context);
		pango_layout_set_text (layout, property->font_family,
				       strlen (property->font_family));
		pango_cairo_show_layout (cr, layout);
		g_object_unref (G_OBJECT (layout)), layout = NULL;

		g_free (property);
	}

	cairo_destroy (cr);

	return FALSE;
}

static char const _css[] = "			\
	box { 					\
		font-family: Helvetica;		\
	}					\
";

int
main (int	  argc,
      char	**argv)
{
	ccss_grammar_t		*grammar;
	ccss_stylesheet_t	*stylesheet;
	ccss_style_t		*style;
	GtkWidget		*window;
	GtkWidget		*area;

	gtk_init (&argc, &argv);

	grammar = ccss_cairo_grammar_create ();
	stylesheet = ccss_grammar_create_stylesheet_from_buffer (grammar,
							_css, sizeof (_css),
							NULL);

	style = ccss_stylesheet_query_type (stylesheet, "box");
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

