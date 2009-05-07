/* vim: set ts=8 sw=8 noexpandtab: */

#include <stdlib.h>
#include <ccss/ccss.h>
#include <glib.h>		
#include <glib/gprintf.h>

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

	g_test_add_func ("/ccss-parser/generic-property", test_generic_property);

	return g_test_run ();
}

