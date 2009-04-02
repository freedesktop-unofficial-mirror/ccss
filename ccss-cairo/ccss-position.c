/* vim: set ts=8 sw=8 noexpandtab: */

/* The Cairo CSS Drawing Library.
 * Copyright (C) 2008 Robert Staudinger
 *
 * This  library is free  software; you can  redistribute it and/or
 * modify it  under  the terms  of the  GNU Lesser  General  Public
 * License  as published  by the Free  Software  Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed  in the hope that it will be useful,
 * but  WITHOUT ANY WARRANTY; without even  the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License  along  with  this library;  if not,  write to  the Free
 * Software Foundation, Inc., 51  Franklin St, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include <glib.h>
#include "ccss-position.h"
#include "config.h"

static const struct {
	char const		*name;
	ccss_position_type_t	 type;
	const double		 percentage;
} _position_map[] = {
  { "left",	CCSS_POSITION_LEFT,	0	},
  { "top",	CCSS_POSITION_TOP,	0	},
  { "right",	CCSS_POSITION_RIGHT,	100	},
  { "bottom",	CCSS_POSITION_BOTTOM,	100	},
  { "center",	CCSS_POSITION_CENTER,	50	},

  { "auto",	CCSS_POSITION_AUTO,	-1	},
  { "contain",	CCSS_POSITION_CONTAIN,	-1	},
  { "cover",	CCSS_POSITION_COVER,	-1	}
};

bool
ccss_position_parse (ccss_position_t	 *self,
		     uint32_t		  flags,
		     CRTerm const	**values)
{
	char const *name;

	if (!*values) {
		return false;
	}

	switch ((*values)->type) {
	case TERM_IDENT:
		name = (char const *) cr_string_peek_raw_str ((*values)->content.str);
		for (unsigned int i = 0; i < G_N_ELEMENTS (_position_map); i++) {
			if (_position_map[i].type & flags &&
			    0 == g_strcmp0 (_position_map[i].name, name)) {
				if (_position_map[i].percentage > -1) {
					self->type = CCSS_POSITION_PERCENTAGE;
					self->value = _position_map[i].percentage;
				} else {
					self->type =  _position_map[i].type;
					self->value = -1;
				}
				*values = (*values)->next;
				return true;
			}
		}
		break;
	case TERM_NUMBER:
		if (CCSS_POSITION_PERCENTAGE & flags &&
		    NUM_PERCENTAGE == (*values)->content.num->type) {
			self->type = CCSS_POSITION_PERCENTAGE;
			self->value = (*values)->content.num->val;
			*values = (*values)->next;
			return true;
		} else if (CCSS_POSITION_LENGTH & flags &&
			   NUM_GENERIC == (*values)->content.num->type) {
			self->type = CCSS_POSITION_LENGTH;
			self->value = (*values)->content.num->val;
			*values = (*values)->next;
			return true;
		} else {
			return false;
		}
		break;
	default:
		g_assert_not_reached ();
		/* Need some code here when building w/o assertions. */
		return false;
	}

	return false;
}

double
ccss_position_get_pos (ccss_position_t const	*self,
		       double			 extent,
		       double			 size)
{
	switch (self->type) {
	case CCSS_POSITION_LENGTH:
		return self->value;
	case CCSS_POSITION_PERCENTAGE:
		return self->value / 100. * (extent - size);
	case CCSS_POSITION_CONTAIN:
	case CCSS_POSITION_COVER:
	case CCSS_POSITION_AUTO:
	default:
		g_assert_not_reached ();
		/* Need some code here when building w/o assertions. */
		return 0;
	}

	return 0;
}

double
ccss_position_get_size (ccss_position_t const	*self,
		        double			 extent)
{
	switch (self->type) {
	case CCSS_POSITION_LENGTH:
		return self->value;
	case CCSS_POSITION_PERCENTAGE:
		return self->value / 100. * extent;
	case CCSS_POSITION_CONTAIN:
	case CCSS_POSITION_COVER:
	case CCSS_POSITION_AUTO:
	default:
		g_assert_not_reached ();
		/* Need some code here when building w/o assertions. */
		return 0;
	}

	return 0;
}

static void
contain (double	 extent_x,
	 double	 extent_y,
	 double	 width,
	 double	 height,
	 double	*x,
	 double	*y)
{
	/* Try to use full width. */
	*y = extent_x * height / width;
	if (*y <= extent_y) {
		*x = extent_x;
		return;
	}

	/* Use full height. */
	*x = extent_y * width / height;
	*y = extent_y;
}

static void
cover (double	 extent_x,
       double	 extent_y,
       double	 width,
       double	 height,
       double	*x,
       double	*y)
{
	/* Try to use full width. */
	*y = extent_x * height / width;
	if (*y >= extent_y) {
		*x = extent_x;
		return;
	}

	/* Use full height. */
	*x = extent_y * width / height;
	*y = extent_y;
}

double
ccss_position_get_hsize (ccss_position_t const	*self,
			 double			 extent_x,
			 double			 extent_y,
			 double			 width,
			 double			 height)
{
	double x;
	double y;

	switch (self->type) {
	case CCSS_POSITION_LENGTH:
		return self->value;
	case CCSS_POSITION_PERCENTAGE:
		return self->value * extent_x / 100.;
	case CCSS_POSITION_CONTAIN:
		contain (extent_x, extent_y, width, height, &x, &y);
		return x;
	case CCSS_POSITION_COVER:
		cover (extent_x, extent_y, width, height, &x, &y);
		return x;
	case CCSS_POSITION_AUTO:
		return width;
	default:
		g_assert_not_reached ();
		/* Need some code here when building w/o assertions. */
		return 0;
	}

	return 0;
}

double
ccss_position_get_vsize (ccss_position_t const	*self,
			 double			 extent_x,
			 double			 extent_y,
			 double			 width,
			 double			 height)
{
	double x;
	double y;

	switch (self->type) {
	case CCSS_POSITION_LENGTH:
		return self->value;
	case CCSS_POSITION_PERCENTAGE:
		return self->value * extent_y / 100.;
	case CCSS_POSITION_CONTAIN:
		contain (extent_x, extent_y, width, height, &x, &y);
		return y;
	case CCSS_POSITION_COVER:
		cover (extent_x, extent_y, width, height, &x, &y);
		return y;
	case CCSS_POSITION_AUTO:
		return height;
	default:
		g_assert_not_reached ();
		/* Need some code here when building w/o assertions. */
		return 0;
	}

	return 0;
}


char *
ccss_position_serialize (ccss_position_t const *self)
{
	g_return_val_if_fail (self, NULL);

	switch (self->type) {
	case CCSS_POSITION_LENGTH:
		return g_strdup_printf ("%.3f", self->value);
	case CCSS_POSITION_PERCENTAGE:
		return g_strdup_printf ("%.3fpx", self->value);
	case CCSS_POSITION_LEFT:
		return g_strdup ("left");
	case CCSS_POSITION_TOP:
		return g_strdup ("top");
	case CCSS_POSITION_RIGHT:
		return g_strdup ("right");
	case CCSS_POSITION_BOTTOM:
		return g_strdup ("bottom");
	case CCSS_POSITION_CENTER:
		return g_strdup ("center");
	case CCSS_POSITION_AUTO:
		return g_strdup ("auto");
	case CCSS_POSITION_CONTAIN:
		return g_strdup ("containe");
	case CCSS_POSITION_COVER:
		return g_strdup ("cover");
	case CCSS_POSITION_MASK_NUMERIC:
	case CCSS_POSITION_MASK_HORIZONTAL:
	case CCSS_POSITION_MASK_VERTICAL:
	case CCSS_POSITION_MASK_AUTO:
		/* Fall thru. */
		break;
	}

	return NULL;
}

