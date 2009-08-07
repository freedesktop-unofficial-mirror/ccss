/* vim: set ts=8 sw=8 noexpandtab: */

#include <stdio.h>
#include <stdlib.h>
#include <ccss/ccss.h>
#include "config.h"

static char const _css_1[] = "		\
	a { 				\
		bar: 1;			\
	}				\
";

static char const _css_2[] = "		\
	a { 				\
		baz: 2;			\
	}				\
	b { 				\
		frob: 3;		\
	}				\
";

int
main (int	  argc,
      char	**argv)
{
	ccss_grammar_t		*grammar;
	ccss_stylesheet_t	*stylesheet;
	unsigned int		 descriptor;

	grammar = ccss_grammar_create_css ();
	stylesheet = ccss_grammar_create_stylesheet (grammar);
	ccss_grammar_destroy (grammar);

	g_debug ("Empty stylesheet");
	ccss_stylesheet_dump (stylesheet);

	descriptor = ccss_stylesheet_add_from_buffer (stylesheet,
						      _css_1, sizeof (_css_1),
						      CCSS_STYLESHEET_AUTHOR,
						      NULL);
	g_debug ("With buffer 1 ('%d')", descriptor);
	ccss_stylesheet_dump (stylesheet);

	descriptor = ccss_stylesheet_add_from_buffer (stylesheet,
						      _css_2, sizeof (_css_2),
						      CCSS_STYLESHEET_AUTHOR,
						      NULL);
	g_debug ("With buffer 2 ('%d')", descriptor);
	ccss_stylesheet_dump (stylesheet);

	ccss_stylesheet_unload (stylesheet, descriptor);
	g_debug ("Back to just buffer 1");
	ccss_stylesheet_dump (stylesheet);

	ccss_stylesheet_destroy (stylesheet);

	return EXIT_SUCCESS;
}

