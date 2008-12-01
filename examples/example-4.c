
#include <stdlib.h>
#include <string.h>
#include <cairo.h>
#include <ccss-cairo/ccss-cairo.h>
#include <gtk/gtk.h>

static ccss_property_class_t const *
peek_property_class (void);

/*
 * Custom property implementation start.
 */

typedef struct {
	ccss_property_base_t	 base;
	char			*font_family;
} font_family_t;

static ccss_property_base_t *
font_family_new (ccss_grammar_t	*grammar,
		 CRTerm const	*values)
{
	font_family_t *self;

	g_return_val_if_fail (values, NULL);

	self = NULL;

	switch (values->type) {
	case TERM_IDENT:
	case TERM_STRING:
		self = g_new0 (font_family_t, 1);
		ccss_property_init ((ccss_property_base_t *) self, peek_property_class ());
		self->font_family = g_strdup (cr_string_peek_raw_str (values->content.str));
		break;
	default:
		/* Unhandled, fall thru. */
		;
	}

	return (ccss_property_base_t *) self;
}

static void
font_family_free (font_family_t *self)
{
	g_free (self->font_family);
	g_free (self);
}

static bool
font_family_convert (font_family_t const	*self,
		     ccss_property_type_t	 target,
		     void			*value)
{
	/* Only conversion to string is supported. */

	if (CCSS_PROPERTY_TYPE_STRING == target) {
		* (char **) value = g_strdup (self->font_family);
		return true;
	}

	return false;
}

static ccss_property_class_t const _properties[] = {
    {
	.name = "font-family",
	.property_create = (ccss_property_create_f) font_family_new,
	.property_destroy = (ccss_property_destroy_f) font_family_free,
	.property_convert = (ccss_property_convert_f) font_family_convert,
	.property_factory = NULL
    }, {
	.name = NULL
    }
};

/*
 * Custom property implementation end.
 */

static ccss_property_class_t const *
peek_property_class (void)
{
	return &_properties[0];
}

static gboolean
expose_cb (GtkWidget		*widget,
	   GdkEventExpose	*event,
	   ccss_style_t const	*style)
{
	cairo_t				 *cr;
	ccss_property_base_t const	 *property;
	PangoContext			 *context;
	PangoLayout			 *layout;

	cr = gdk_cairo_create (widget->window);

	property = NULL;
	ccss_style_get_property (style, "font-family", &property);
	if (property) {
		font_family_t const *font_family = (font_family_t const *) property;
		context = gtk_widget_get_pango_context (widget);
		layout = pango_layout_new (context);
		pango_layout_set_text (layout, font_family->font_family,
				       strlen (font_family->font_family));
		pango_cairo_show_layout (cr, layout);
		g_object_unref (G_OBJECT (layout)), layout = NULL;
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
	gboolean		 ret;

	gtk_init (&argc, &argv);
	ccss_cairo_init ();

	grammar = ccss_cairo_grammar_create ();
	ccss_grammar_add_properties (grammar, _properties);
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

	ccss_cairo_shutdown ();

	return EXIT_SUCCESS;
}

