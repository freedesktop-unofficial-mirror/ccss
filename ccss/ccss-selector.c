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

#include <stdio.h>
#include <string.h>
#include "ccss-node.h"
#include "ccss-selector.h"
#include "ccss-style.h"
#include "ccss-stylesheet.h"

typedef enum {
	CCSS_SELECTOR_MODALITY_UNIVERSAL,	/* Universal selector. */
	CCSS_SELECTOR_MODALITY_TYPE,		/* By element type. */
	CCSS_SELECTOR_MODALITY_BASE_TYPE,	/* By element type. */
	CCSS_SELECTOR_MODALITY_CLASS,		/* By element class. */
	CCSS_SELECTOR_MODALITY_ID,		/* By element ID. */
	CCSS_SELECTOR_MODALITY_ATTRIBUTE,	/* By element attribute. */
	CCSS_SELECTOR_MODALITY_PSEUDO_CLASS	/* By pseudo class. */
} ccss_selector_modality_t;

/*
 * Abstract base selector.
 * See http://www.w3.org/TR/CSS21/cascade.html#specificity for an explantion 
 * of the meaning of the fields `a', `b', `c' and `d'. Field `e' (lowest priority)
 * is used for merging base styles. Field `a' is not used currently.
 */
struct ccss_selector_ {
	ccss_selector_modality_t	 modality;
	uint8_t			 a;
	uint8_t			 b;
	uint8_t			 c;
	uint8_t			 d;
	uint8_t			 e;
	struct ccss_selector_	*refinement;
	struct ccss_selector_	*container;
	struct ccss_selector_	*antecessor;
	ccss_block_t const	*block;
};

static void
selector_sync (ccss_selector_t const	*self,
	       ccss_selector_t		*to)
{
	g_assert (self && to);

	to->modality = self->modality;
	to->a = self->a;
	to->b = self->b;
	to->c = self->c;
	to->d = self->d;
	to->e = self->e;
	to->refinement = NULL;
	to->container = NULL;
	to->antecessor = NULL;
	to->block = self->block;
}

/*
 * Universal selector.
 */
typedef ccss_selector_t ccss_universal_selector_t;

ccss_selector_t * 
ccss_universal_selector_new (void)
{
	ccss_universal_selector_t *self;

	self = g_new0 (ccss_universal_selector_t, 1);
	self->modality = CCSS_SELECTOR_MODALITY_UNIVERSAL;

	return (ccss_selector_t *) self;
}

static ccss_selector_t *
universal_selector_dup (ccss_universal_selector_t const *original)
{
	ccss_universal_selector_t *self;

	self = g_new0 (ccss_universal_selector_t, 1);
	selector_sync ((ccss_selector_t const *) original, self);

	return (ccss_selector_t *) self;
}

static void
universal_selector_free (ccss_universal_selector_t *self)
{
	g_assert (self);

	g_free (self);
}

#ifdef CCSS_DEBUG

static void
universal_selector_dump (ccss_universal_selector_t const *self)
{
	printf (" *");
}

#endif /* CCSS_DEBUG */

/*
 * Select by element type.
 */
typedef struct {
	ccss_selector_t	 parent;
	char		*type_name;
} ccss_type_selector_t;

ccss_selector_t *
ccss_type_selector_new (char const *type_name)
{
	ccss_type_selector_t *self;

	g_assert (type_name);

	self = g_new0 (ccss_type_selector_t, 1);
	self->parent.modality = CCSS_SELECTOR_MODALITY_TYPE;
	self->parent.d = 1;
	self->type_name = g_strdup (type_name);

	return (ccss_selector_t *) self;
}

static ccss_selector_t *
type_selector_dup (ccss_type_selector_t const *original)
{
	ccss_type_selector_t *self;

	self = g_new0 (ccss_type_selector_t, 1);
	selector_sync ((ccss_selector_t const *) original, &self->parent);
	self->type_name = g_strdup (original->type_name);

	return (ccss_selector_t *) self;
}

static void
type_selector_free (ccss_type_selector_t *self)
{
	g_assert (self);

	g_free (self->type_name);
	g_free (self);
}

#ifdef CCSS_DEBUG

static void
type_selector_dump (ccss_type_selector_t const *self)
{
	printf (" %s", self->type_name);
}

#endif /* CCSS_DEBUG */

/*
 * Select by element's base type.
 * Derived from the type selector
 */
ccss_selector_t * 
ccss_base_type_selector_new (char const		*type_name,
			    unsigned int	 specificity_e)
{
	ccss_selector_t *self;

	self = ccss_type_selector_new (type_name);
	self->modality = CCSS_SELECTOR_MODALITY_BASE_TYPE;
	self->a = 0;
	self->b = 0;
	self->c = 0;
	self->d = 0;
	self->e = specificity_e;

	return self;
}

/*
 * Select by element class.
 */
typedef struct {
	ccss_selector_t	 parent;
	char		*class_name;
} ccss_class_selector_t;

ccss_selector_t *
ccss_class_selector_new (char const *class_name)
{
	ccss_class_selector_t *self;

	g_assert (class_name);

	self = g_new0 (ccss_class_selector_t, 1);
	self->parent.modality = CCSS_SELECTOR_MODALITY_CLASS;
	self->parent.c = 1;
	self->class_name = g_strdup (class_name);

	return (ccss_selector_t *) self;
}

static ccss_selector_t *
class_selector_dup (ccss_class_selector_t const *original)
{
	ccss_class_selector_t *self;

	self = g_new0 (ccss_class_selector_t, 1);
	selector_sync ((ccss_selector_t const *) original, &self->parent);
	self->class_name = g_strdup (original->class_name);

	return (ccss_selector_t *) self;
}

static void
class_selector_free (ccss_class_selector_t *self)
{
	g_assert (self);

	g_free (self->class_name);
	g_free (self);
}

#ifdef CCSS_DEBUG

static void
class_selector_dump (ccss_class_selector_t const *self)
{
	printf (".%s", self->class_name);
}

#endif /* CCSS_DEBUG */

/*
 * Select by element ID.
 */
typedef struct {
	ccss_selector_t	 parent;
	char		*id;
} ccss_id_selector_t;

ccss_selector_t *
ccss_id_selector_new (char const *id)
{
	ccss_id_selector_t *self;

	g_assert (id);

	self = g_new0 (ccss_id_selector_t, 1);
	self->parent.modality = CCSS_SELECTOR_MODALITY_ID;
	self->parent.b = 1;
	self->id = g_strdup (id);

	return (ccss_selector_t *) self;
}

static ccss_selector_t *
id_selector_dup (ccss_id_selector_t const *original)
{
	ccss_id_selector_t *self;

	self = g_new0 (ccss_id_selector_t, 1);
	selector_sync ((ccss_selector_t const *) original, &self->parent);
	self->id = g_strdup (original->id);

	return (ccss_selector_t *) self;
}

static void
id_selector_free (ccss_id_selector_t *self)
{
	g_assert (self);

	g_free (self->id);
	g_free (self);
}

#ifdef CCSS_DEBUG

static void
id_selector_dump (ccss_id_selector_t const *self)
{
	printf ("#%s", self->id);
}

#endif /* CCSS_DEBUG */

/*
 * Select by attribute.
 */
typedef struct {
	ccss_selector_t			 parent;
	char				*name;
	char				*value;
	ccss_attribute_selector_match_t	 match;
} ccss_attribute_selector_t;

ccss_selector_t *
ccss_attribute_selector_new (char const				*name,
			    char const				*value,
			    ccss_attribute_selector_match_t	 match)
{
	ccss_attribute_selector_t *self;

	g_assert (name && value);

	self = g_new0 (ccss_attribute_selector_t, 1);
	self->parent.modality = CCSS_SELECTOR_MODALITY_ATTRIBUTE;
	self->parent.c = 1;
	self->name = g_strdup (name);
	self->value = g_strdup (value);
	self->match = match;

	return (ccss_selector_t *) self;
}

static ccss_selector_t *
attribute_selector_dup (ccss_attribute_selector_t const *original)
{
	ccss_attribute_selector_t *self;

	self = g_new0 (ccss_attribute_selector_t, 1);
	selector_sync ((ccss_selector_t const *) original, &self->parent);
	self->name = g_strdup (original->name);
	self->value = g_strdup (original->value);
	self->match = original->match;

	return (ccss_selector_t *) self;
}

static void
attribute_selector_free (ccss_attribute_selector_t *self)
{
	g_assert (self);

	g_free (self->name);
	g_free (self->value);
	g_free (self);
}

#ifdef CCSS_DEBUG

static void
attribute_selector_dump (ccss_attribute_selector_t const *self)
{
	switch (self->match) {
	case CCSS_ATTRIBUTE_SELECTOR_MATCH_EXISTS:
		printf ("[%s]", self->name);
		break;
	case CCSS_ATTRIBUTE_SELECTOR_MATCH_EQUALS:
		printf ("[%s=%s]", self->name, self->value);
		break;
	default:
		g_assert_not_reached ();
	}
}

#endif /* CCSS_DEBUG */

/*
 * Select by pseudo class.
 */
typedef struct {
	ccss_selector_t	 parent;
	char		*pseudo_class;
} ccss_pseudo_class_selector_t;

ccss_selector_t *
ccss_pseudo_class_selector_new (char const *pseudo_class)
{
	ccss_pseudo_class_selector_t *self;

	g_assert (pseudo_class);

	self = g_new0 (ccss_pseudo_class_selector_t, 1);
	self->parent.modality = CCSS_SELECTOR_MODALITY_PSEUDO_CLASS;
	self->parent.d = 1;
	self->pseudo_class = g_strdup (pseudo_class);

	return (ccss_selector_t *) self;
}

static ccss_selector_t *
pseudo_class_selector_dup (ccss_pseudo_class_selector_t const *original)
{
	ccss_pseudo_class_selector_t *self;

	self = g_new0 (ccss_pseudo_class_selector_t, 1);
	selector_sync ((ccss_selector_t const *) original, &self->parent);
	self->pseudo_class = g_strdup (original->pseudo_class);

	return (ccss_selector_t *) self;
}

static void
pseudo_class_selector_free (ccss_pseudo_class_selector_t *self)
{
	g_assert (self);

	g_free (self->pseudo_class);
	g_free (self);
}

#ifdef CCSS_DEBUG

static void
pseudo_class_selector_dump (ccss_pseudo_class_selector_t const *self)
{
	printf (":%s", self->pseudo_class);
}

#endif /* CCSS_DEBUG */

ccss_selector_t *
ccss_selector_copy (ccss_selector_t const *original)
{
	ccss_selector_t *self;

	g_assert (original);

	switch (original->modality) {
	case CCSS_SELECTOR_MODALITY_UNIVERSAL:
		self = universal_selector_dup ((ccss_universal_selector_t const *) original);
		break;
	case CCSS_SELECTOR_MODALITY_TYPE:
	case CCSS_SELECTOR_MODALITY_BASE_TYPE:
		self = type_selector_dup ((ccss_type_selector_t const *) original);
		break;
	case CCSS_SELECTOR_MODALITY_CLASS:
		self = class_selector_dup ((ccss_class_selector_t const *) original);
		break;
	case CCSS_SELECTOR_MODALITY_ID:
		self = id_selector_dup ((ccss_id_selector_t const *) original);
		break;
	case CCSS_SELECTOR_MODALITY_ATTRIBUTE:
		self = attribute_selector_dup ((ccss_attribute_selector_t const *) original);
		break;
	case CCSS_SELECTOR_MODALITY_PSEUDO_CLASS:
		self = pseudo_class_selector_dup ((ccss_pseudo_class_selector_t const *) original);
		break;
	default:
		g_warning ("Unknown selector modality %d", original->modality);
		self = NULL;
		return self;
	}

	if (original->refinement) {
		self->refinement = ccss_selector_copy (original->refinement);
	}

	if (original->container) {
		self->container = ccss_selector_copy (original->container);
	}

	if (original->antecessor) {
		self->antecessor = ccss_selector_copy (original->antecessor);
	}

	return self;
}

/*
 * Create a selector based on an existing one.
 */
ccss_selector_t *
ccss_selector_copy_as_base (ccss_selector_t const	*original,
			   int			 specificity_e)
{
	ccss_selector_t *self;

	g_assert (original && original->modality == CCSS_SELECTOR_MODALITY_TYPE);

	self = ccss_selector_copy (original);
	self->modality = CCSS_SELECTOR_MODALITY_BASE_TYPE;

	if (specificity_e > -1) {
		if (original->d > 0) {
			self->d--;
		} else {
			g_warning ("Specificity d == 0");
		}

		if (original->e == 0) {
#if DEBUG
			if (specificity_e > CCSS_SELECTOR_MAX_SPECIFICITY) {
				g_assert_not_reached ();
			}
#endif
			g_assert (specificity_e <= CCSS_SELECTOR_MAX_SPECIFICITY);
			self->e = (unsigned) specificity_e;
		} else {
			g_warning ("Specificity e != 0");
		}
	}

	return self;
}

/*
 * Free the whole selector chain.
 */
void
ccss_selector_free (ccss_selector_t *self)
{
	g_assert (self);

	if (self->refinement) {
		ccss_selector_free (self->refinement), self->refinement = NULL;
	}

	if (self->container) {
		ccss_selector_free (self->container), self->container = NULL;
	}

	if (self->antecessor) {
		ccss_selector_free (self->antecessor), self->antecessor = NULL;
	}

	switch (self->modality) {
	case CCSS_SELECTOR_MODALITY_UNIVERSAL:
		universal_selector_free ((ccss_universal_selector_t *) self);
		break;
	case CCSS_SELECTOR_MODALITY_TYPE:
	case CCSS_SELECTOR_MODALITY_BASE_TYPE:
		type_selector_free ((ccss_type_selector_t *) self);
		break;
	case CCSS_SELECTOR_MODALITY_CLASS:
		class_selector_free ((ccss_class_selector_t *) self);
		break;
	case CCSS_SELECTOR_MODALITY_ID:
		id_selector_free ((ccss_id_selector_t *) self);
		break;
	case CCSS_SELECTOR_MODALITY_ATTRIBUTE:
		attribute_selector_free ((ccss_attribute_selector_t *) self);
		break;
	case CCSS_SELECTOR_MODALITY_PSEUDO_CLASS:
		pseudo_class_selector_free ((ccss_pseudo_class_selector_t *) self);
		break;
	default:
		g_warning ("Unknown selector modality %d", self->modality);
	}
}

/*
 * Does it matter that the refinements order is reversed?
 */
void
ccss_selector_refine (ccss_selector_t *self,
		     ccss_selector_t *selector)
{
	g_assert (self && self->refinement == NULL && selector);

	self->refinement = selector;

	/* propagate specificity */
	self->a += selector->a;
	self->b += selector->b;
	self->c += selector->c;
	self->d += selector->d;
	/* `e' is not relevant for refinements.
	 * self->e += selector->e; */
}

/*
 * Add a selector to the chain.
 * This can change the selector instance, make sure to regard the return value.
 */
ccss_selector_t *
ccss_selector_append_child (ccss_selector_t *self,
			   ccss_selector_t *selector)
{
	/* propagate specificity */
	selector->a += self->a;
	selector->b += self->b;
	selector->c += self->c;
	selector->d += self->d;
	/* TODO: invert the meaing of `e' and only apply
	 * "MAX - e" when calculating specificity.
	 * Will this let us propagate correctly?
	 * selector->e += self->e; */

	selector->container = self;

	return selector;
}

/*
 * Add a selector to the chain.
 * This can change the selector instance, make sure to regard the return value.
 */
ccss_selector_t *
ccss_selector_append_descendant (ccss_selector_t *self,
				ccss_selector_t *selector)
{
	/* propagate specificity */
	selector->a += self->a;
	selector->b += self->b;
	selector->c += self->c;
	selector->d += self->d;
	/* TODO: invert the meaing of `e' and only apply
	 * "MAX - e" when calculating specificity.
	 * Will this let us propagate correctly?
	 * selector->e += self->e; */

	selector->antecessor = self;

	return selector;
}

bool
ccss_selector_is_type (ccss_selector_t const *self)
{
	switch (self->modality) {
	case CCSS_SELECTOR_MODALITY_UNIVERSAL:
	case CCSS_SELECTOR_MODALITY_TYPE:
		return true;
	case CCSS_SELECTOR_MODALITY_BASE_TYPE:
	case CCSS_SELECTOR_MODALITY_CLASS:
	case CCSS_SELECTOR_MODALITY_ID:
	case CCSS_SELECTOR_MODALITY_ATTRIBUTE:
	case CCSS_SELECTOR_MODALITY_PSEUDO_CLASS:
	default:
		return false;
	}
}

bool
ccss_selector_is_class (ccss_selector_t const *self)
{
	switch (self->modality) {
	case CCSS_SELECTOR_MODALITY_CLASS:
		return true;		
	case CCSS_SELECTOR_MODALITY_UNIVERSAL:
	case CCSS_SELECTOR_MODALITY_TYPE:
	case CCSS_SELECTOR_MODALITY_BASE_TYPE:
	case CCSS_SELECTOR_MODALITY_ID:
	case CCSS_SELECTOR_MODALITY_ATTRIBUTE:
	case CCSS_SELECTOR_MODALITY_PSEUDO_CLASS:
	default:
		return false;
	}
}

bool
ccss_selector_is_id (ccss_selector_t const *self)
{
	switch (self->modality) {
	case CCSS_SELECTOR_MODALITY_ID:
		return true;		
	case CCSS_SELECTOR_MODALITY_UNIVERSAL:
	case CCSS_SELECTOR_MODALITY_TYPE:
	case CCSS_SELECTOR_MODALITY_BASE_TYPE:
	case CCSS_SELECTOR_MODALITY_CLASS:
	case CCSS_SELECTOR_MODALITY_ATTRIBUTE:
	case CCSS_SELECTOR_MODALITY_PSEUDO_CLASS:
	default:
		return false;
	}
}

ccss_block_t const *
ccss_selector_get_block (ccss_selector_t const *self)
{
	g_assert (self);

	return self->block;
}

void
ccss_selector_set_block (ccss_selector_t		*self,
			ccss_block_t const	*block)
{
	g_assert (self);

	self->block = block;
}

/*
 * Depending on the modality of the selector this may return NULL.
 */
char const *
ccss_selector_get_key (ccss_selector_t const *self)
{
	g_return_val_if_fail (self, NULL);

	switch (self->modality) {
	case CCSS_SELECTOR_MODALITY_UNIVERSAL:
		return "*";
	case CCSS_SELECTOR_MODALITY_TYPE:
	case CCSS_SELECTOR_MODALITY_BASE_TYPE:
		return ((ccss_type_selector_t *) self)->type_name;
	case CCSS_SELECTOR_MODALITY_CLASS:
		return ((ccss_class_selector_t *) self)->class_name;
	case CCSS_SELECTOR_MODALITY_ID:
		return ((ccss_id_selector_t *) self)->id;
	case CCSS_SELECTOR_MODALITY_ATTRIBUTE:
	case CCSS_SELECTOR_MODALITY_PSEUDO_CLASS:
	default:
		return NULL;
	}
}

uint32_t
ccss_selector_get_specificity (ccss_selector_t const *self)
{
	g_assert (self->a <= CCSS_SELECTOR_MAX_SPECIFICITY &&
		  self->b <= CCSS_SELECTOR_MAX_SPECIFICITY &&
		  self->c <= CCSS_SELECTOR_MAX_SPECIFICITY &&
		  self->d <= CCSS_SELECTOR_MAX_SPECIFICITY &&
		  self->e <= CCSS_SELECTOR_MAX_SPECIFICITY);

	return self->e | (self->d << 6) | (self->c << 12) | (self->b << 18) | (self->a << 24);
}

void
ccss_selector_get_specificity_values (ccss_selector_t const	*self, 
				     unsigned int		*a,
				     unsigned int		*b,
				     unsigned int		*c,
				     unsigned int		*d,
				     unsigned int		*e)
{
	g_assert (self);

	if (a)
		*a = self->a;

	if (b)
		*b = self->b;

	if (c)
		*c = self->c;

	if (d)
		*d = self->d;

	if (e)
		*e = self->e;
}

static bool
match_antecessor_r (ccss_selector_t const	*self, 
		    ccss_node_t const		*node)
{
	ccss_node_class_t const	*node_class;
	ccss_node_t		*container;
	bool			 is_matching;

	node_class = node->node_class;

	container = node_class->get_container (node);
	if (container) {
		is_matching = ccss_selector_query_apply (self, container, NULL);
		if (!is_matching) {
			is_matching = match_antecessor_r (self, container);
		}
		node_class->release (container);
	} else {
		is_matching = false;
	}

	return is_matching;
}

/*
 * `style' may be NULL, in which case this just tests for a match.
 */
bool
ccss_selector_query_apply (ccss_selector_t const	*self, 
			  ccss_node_t const	*node,
			  ccss_style_t		*style)
{
	ccss_node_class_t const	*node_class;
	char const	*name;
	char		*value;
	bool		 is_matching;

	g_return_val_if_fail (self && node, false);

	node_class = node->node_class;

	is_matching = false;
	switch (self->modality) {
	case CCSS_SELECTOR_MODALITY_UNIVERSAL:
		is_matching = true;
		break;
	case CCSS_SELECTOR_MODALITY_TYPE:
		is_matching = ccss_node_is_a (node, ((ccss_type_selector_t *) self)->type_name);
		break;
	case CCSS_SELECTOR_MODALITY_BASE_TYPE:
		/* HACK warning: let's just say it matches, because the base type selectors have
		 * been set up internally -- that is in the fixup run after loading the stylesheet. */
		is_matching = true;
		break;
	case CCSS_SELECTOR_MODALITY_CLASS:
		name = node_class->get_class (node);
		is_matching = name ? 0 == strcmp (name, ((ccss_class_selector_t *) self)->class_name) : false;
		break;
	case CCSS_SELECTOR_MODALITY_ID:
		name = node_class->get_id (node);
		is_matching = name ? 0 == strcmp (name, ((ccss_id_selector_t *) self)->id) : false;
		break;
	case CCSS_SELECTOR_MODALITY_ATTRIBUTE:
		name = ((ccss_attribute_selector_t *) self)->name;
		value = node_class->get_attribute (node, name);
		switch (((ccss_attribute_selector_t *) self)->match) {
		case CCSS_ATTRIBUTE_SELECTOR_MATCH_EXISTS:
			is_matching = value ? true : false;
			break;
		case CCSS_ATTRIBUTE_SELECTOR_MATCH_EQUALS:
			is_matching = value ? 0 == strcmp (value, ((ccss_attribute_selector_t *) self)->value) : false;			
			break;
		default:
			g_assert_not_reached ();
			is_matching = false;
		}
		g_free (value), value = NULL;
		break;
	case CCSS_SELECTOR_MODALITY_PSEUDO_CLASS:
		name = node_class->get_pseudo_class (node);
		is_matching = name ? 0 == strcmp (name, ((ccss_pseudo_class_selector_t *) self)->pseudo_class) : false;
		break;
	default:
		g_assert_not_reached ();
	}

	if (!is_matching) {
		return false;
	}

	/* recursively match refinements */
	if (self->refinement) {
		is_matching = ccss_selector_query_apply (self->refinement, node, style);
		if (!is_matching) {
			return false;
		}
	}

	/* match container */
	if (self->container) {
		ccss_node_t *container;
		container = node_class->get_container (node);
		is_matching = false;
		if (container) {
			is_matching = ccss_selector_query_apply (self->container, container, style);
			node_class->release (container);
		}
		if (!is_matching) {
			return false;
		}
	}

	/* recursively match antecessor */
	if (self->antecessor) {
		is_matching = match_antecessor_r (self->antecessor, node);
		if (!is_matching) {
			return false;
		}
	}

	/* merge */
	if (self->block && style) {

		/* Update dynamic block variables. 
		 * FIXME: find some good place/way to to this. */
		/* if (CCSS_BACKGROUND_FIXED == self->block->background.bg_attachment.attachment) {
			// TODO: set viewport

		} */
		ccss_selector_apply (self, style);
	}

	return true;
}

bool
ccss_selector_apply (ccss_selector_t const	*self,
		    ccss_style_t			*style)
{
	GHashTableIter	iter;
	gpointer	key;
	gpointer	value;

	g_return_val_if_fail (self && self->block && style, false);

	g_hash_table_iter_init (&iter, self->block->properties);
	while (g_hash_table_iter_next (&iter, &key, &value)) {

		g_hash_table_insert (style->properties, key, value);
	}

	return true;
}

#ifdef CCSS_DEBUG

void 
ccss_selector_dump (ccss_selector_t const *self)
{
	g_assert (self);

	printf ("%p: ", (void *) self);

	if (self->container) {
		ccss_selector_t const *container;
		printf ("( ");
		container = self->container;
		while (container) {
			printf("%s < ", ccss_selector_get_key (container));
			container = container->container;
		}
		printf (")");
	}

	if (self->antecessor) {
		ccss_selector_t const *container;
		printf ("( ");
		container = self->antecessor;
		while (container) {
			printf("%s", ccss_selector_get_key (container));
			container = container->antecessor;
		}
		printf (")");
	}

	switch (self->modality) {
	case CCSS_SELECTOR_MODALITY_UNIVERSAL:
		universal_selector_dump ((ccss_universal_selector_t *) self);
		break;
	case CCSS_SELECTOR_MODALITY_TYPE:
	case CCSS_SELECTOR_MODALITY_BASE_TYPE:
		type_selector_dump ((ccss_type_selector_t *) self);
		break;
	case CCSS_SELECTOR_MODALITY_CLASS:
		class_selector_dump ((ccss_class_selector_t *) self);
		break;
	case CCSS_SELECTOR_MODALITY_ID:
		id_selector_dump ((ccss_id_selector_t *) self);
		break;
	case CCSS_SELECTOR_MODALITY_ATTRIBUTE:
		attribute_selector_dump ((ccss_attribute_selector_t *) self);
		break;
	case CCSS_SELECTOR_MODALITY_PSEUDO_CLASS:
		pseudo_class_selector_dump ((ccss_pseudo_class_selector_t *) self);
		break;
	default:
		g_warning ("Unknown selector modality %d", self->modality);
	}

	if (self->refinement) {
		ccss_selector_dump (self->refinement);
	}

	if (self->block) {
		printf (" {\n");
		ccss_block_dump (self->block);
		printf ("}");
	}

	printf (" # modality: %d, specificity: %d,%d,%d,%d,%d\n", 
		self->modality, self->a, self->b, self->c, self->d, self->e);
}

#endif /* CCSS_DEBUG */

