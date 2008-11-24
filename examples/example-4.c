
#include <stdlib.h>
#include <cairo.h>
#include <ccss/ccss.h>
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
font_family_new (CRTerm const *values)
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

static ccss_property_class_t const _ptable[] = {
    {
	.name = "font-family",
	.property_new = (ccss_property_new_f) font_family_new,
	.property_free = (ccss_property_free_f) font_family_free,
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
	return &_ptable[0];
}

static gboolean
expose_cb (GtkWidget		*widget,
	   GdkEventExpose	*event,
	   ccss_style_t const	*style)
{
	cairo_t			*cr;
	font_family_t const	*font_family;
	//PangoLayout*

	cr = gdk_cairo_create (widget->window);

	font_family = NULL;
	ccss_style_get_property (style, "font-family", (void **) &font_family);
	if (font_family) {
		printf ("%s\n", font_family->font_family);
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
	ccss_stylesheet_t	*stylesheet;
	ccss_style_t		*style;
	GtkWidget		*window;
	gboolean		 ret;

	gtk_init (&argc, &argv);
	ccss_cairo_init ();
	ccss_add_properties (_ptable);

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

	ccss_shutdown ();

	return EXIT_SUCCESS;
}

