/* vim: set ts=8 sw=8 noexpandtab: */

#include <stdlib.h>
#include <ccss/ccss.h>
#include <glib.h>
#include <glib/gprintf.h>

#define ccss_assert_float_equal(n1, n2)					      \
	do {								      \
		long double __n1 = (n1), __n2 = (n2);			      \
		if (__n1 <= __n2 + FLT_EPSILON &&			      \
		    __n1 >= __n2 - FLT_EPSILON)				      \
			;						      \
		else							      \
			g_assertion_message_cmpnum (G_LOG_DOMAIN, __FILE__,   \
						    __LINE__, G_STRFUNC,      \
						    #n1 " ~ " #n2,            \
						    __n1, "~", __n2, 'f');    \
	} while (0)

static void
test_color (void)
{
	static char const *_properties[] = {
		"named-color-rgb",
		"named-color-rgba",
		"hash-color-rgb",
		"hash-color-rgba",
		"hash-color-rrggbb",
		"hash-color-rrggbbaa",
		"absolute-color-rgb",
//		"absolute-color-rgba",  /* FIXME libcroco? */
		"relative-color-rgb",
//		"relative-color-rgba"   /* FIXME libcroco? */
	};
	static char const _css[] =
		"named-color-rgb	{ color: hotpink;	           }\n"
		"named-color-rgba	{ color: transparent;		   }\n"
		"hash-color-rgb		{ color: #def;			   }\n"
		"hash-color-rgba	{ color: #def1;			   }\n"
		"hash-color-rrggbb	{ color: #1a2b3c;		   }\n"
		"hash-color-rrggbbaa	{ color: #1a2b3c4d;		   }\n"
		"absolute-color-rgb	{ color: rgb(11, 22, 33);	   }\n"
//		"absolute-color-rgba	{ color: rgba(11, 22, 33, 0.5);    }\n"
		"relative-color-rgb	{ color: rgb(11%, 22%, 33%);	   }\n";
//		"relative-color-rgba	{ color: rgba(11%, 22%, 33%, 0.5); }\n"
	/* Color values are 0..1, negative means undefined here */
	static double _values[][4] = {
	       /* red		green		blue		alpha */
		{ 0xff/255.,    0x69/255.,      0xb4/255.,      1.	     },
		{ -1.,		-1.,		-1.,		0.	     },
		{ 0xd/15.,      0xe/15.,	0xf/15.,	1.	     },
		{ 0xd/15.,      0xe/15.,	0xf/15.,	0x1/15.	     },
		{ 0x1a/255.,    0x2b/255.,      0x3c/255.,      1.	     },
		{ 0x1a/255.,    0x2b/255.,      0x3c/255.,      0x4d/255.    },
		{ 11/255.,      22/255.,	33/255.,	1.	     },
//		{ 11/255.,      22/255.,	33/255.,	.5	     },
		{ 11/100.,      22/100.,	33/100.,	1.	     },
//		{ 11/100.,      22/100.,	33/100.,	.5	     }
	};
	ccss_grammar_t		*grammar;
	ccss_stylesheet_t	*stylesheet;
	ccss_style_t		*style;
	ccss_color_t const	*color;
	bool			 ret;

	if (g_test_verbose ()) g_printf ("testing '%s'\n", _css);

	grammar = ccss_grammar_create_css ();
	g_assert (grammar);

	if (g_test_verbose ()) g_print ("parsing");
	stylesheet = ccss_grammar_create_stylesheet_from_buffer (grammar,
							_css, sizeof (_css) - 1,
							NULL);
	g_assert (stylesheet);

	for (unsigned int i = 0; i < G_N_ELEMENTS (_properties); i++) {

		if (g_test_verbose ()) g_printf ("querying '%s'\n", _properties[i]);
		style = ccss_stylesheet_query_type (stylesheet, _properties[i]);
		g_assert (style);

		color = NULL;
		ret = ccss_style_get_property (style,
					       "color",
					       (ccss_property_base_t const **) &color);
		g_assert (ret);
		/* Only compare defined values (i.e. > 0). */
		if (_values[i][0] >= 0.)
			ccss_assert_float_equal (color->red, _values[i][0]);
		if (_values[i][1] >= 0.)
			ccss_assert_float_equal (color->green, _values[i][1]);
		if (_values[i][2] >= 0.)
			ccss_assert_float_equal (color->blue, _values[i][2]);
		if (_values[i][3] >= 0.)
			ccss_assert_float_equal (color->alpha, _values[i][3]);

		ccss_style_destroy (style);
	}

	ccss_stylesheet_destroy (stylesheet);
	ccss_grammar_destroy (grammar);
}

static void
test_generic_property (void)
{
	static char const        _css[] = "foo { bar: baz; }";
	ccss_grammar_t		*grammar;
	ccss_stylesheet_t	*stylesheet;
	ccss_style_t		*style;
	/*double		 dval;*/
	char			*sval;
	bool			 ret;

	if (g_test_verbose ()) g_printf ("testing '%s'\n", _css);

	grammar = ccss_grammar_create_generic ();
	g_assert (grammar);

	if (g_test_verbose ()) g_print ("parsing");
	stylesheet = ccss_grammar_create_stylesheet_from_buffer (grammar,
							_css, sizeof (_css) - 1,
							NULL);
	g_assert (stylesheet);

	if (g_test_verbose ()) g_print ("querying");
	style = ccss_stylesheet_query_type (stylesheet, "foo");
	g_assert (style);

	/* TODO: find out how to rund a test that's supposed to fail.
	 * this issues a warning which aborts
	if (g_test_verbose ()) g_print ("converting to double");
	ret = ccss_style_get_double (style, "bar", &dval);
	g_assert (!ret);
	*/

	if (g_test_verbose ()) g_print ("converting to string");
	sval = NULL;
	ret = ccss_style_get_string (style, "bar", &sval);
	g_assert (ret);
	g_assert_cmpstr (sval, ==, "baz");

	ccss_style_destroy (style);
	ccss_stylesheet_destroy (stylesheet);
	ccss_grammar_destroy (grammar);

}

int
main (int	  argc,
      char	**argv)
{
	g_test_init (&argc, &argv, NULL);

	g_test_add_func ("/ccss-parser/color", test_color);
	g_test_add_func ("/ccss-parser/generic-property", test_generic_property);

	return g_test_run ();
}

