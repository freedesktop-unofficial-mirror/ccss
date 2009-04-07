/* vim: set ts=8 sw=8 noexpandtab: */

#include <stdio.h>
#include <stdlib.h>
#include <ccss/ccss.h>

static char const _css[] = "		\
	box { 				\
		padding: 2px;		\
		padding-left: 3px;	\
	}				\
";

int
main (int	  argc,
      char	**argv)
{
	ccss_grammar_t		*grammar;
	ccss_stylesheet_t	*stylesheet;
	ccss_style_t		*style;
	double			 padding;
	gboolean		 ret;

	grammar = ccss_grammar_create_css ();
	stylesheet = ccss_grammar_create_stylesheet_from_buffer (grammar,
							_css, sizeof (_css),
							NULL);

	style = ccss_stylesheet_query_type (stylesheet, "box");
	g_assert (style);

#ifdef CCSS_DEBUG
	ccss_style_dump (style);
#endif

	padding = -1;
	ret = ccss_style_get_double (style, "padding-left", &padding);
	printf ("padding-left: %.3f (%d)\n", padding, ret);

	padding = -1;
	ret = ccss_style_get_double (style, "padding-right", &padding);
	printf ("padding-right: %.3f (%d)\n", padding, ret);

	ccss_style_destroy (style);
	ccss_stylesheet_destroy (stylesheet);
	ccss_grammar_destroy (grammar);

	return EXIT_SUCCESS;
}

