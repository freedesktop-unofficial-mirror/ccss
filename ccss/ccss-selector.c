/* vim: set ts=8 sw=8 noexpandtab: */

/* The `C' CSS Library.
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
#include <glib.h>
#include "ccss-block-priv.h"
#include "ccss-node-priv.h"
#include "ccss-selector.h"
#include "ccss-style-priv.h"
#include "config.h"

#define CCSS_SELECTOR_MODALITY_IS_VALID(selector_)			       \
	((selector_)->modality > CCSS_SELECTOR_MODALITY_ERROR_UNDERFLOW &&     \
	 (selector_)->modality < CCSS_SELECTOR_MODALITY_ERROR_OVERFLOW)

typedef enum {
	CCSS_SELECTOR_MODALITY_ERROR_UNDERFLOW = 0,
	CCSS_SELECTOR_MODALITY_UNIVERSAL = 1,	/* Universal selector. */
	CCSS_SELECTOR_MODALITY_TYPE,		/* By element type. */
	CCSS_SELECTOR_MODALITY_BASE_TYPE,	/* By element type. */
	CCSS_SELECTOR_MODALITY_CLASS,		/* By element class. */
	CCSS_SELECTOR_MODALITY_ID,		/* By element ID. */
	CCSS_SELECTOR_MODALITY_ATTRIBUTE,	/* By element attribute. */
	CCSS_SELECTOR_MODALITY_PSEUDO_CLASS,	/* By pseudo class. */
	CCSS_SELECTOR_MODALITY_INSTANCE,	/* By node instance. */
	CCSS_SELECTOR_MODALITY_ERROR_OVERFLOW
} ccss_selector_modality_t;

/*
 * Abstract base selector.
 * See http://www.w3.org/TR/CSS21/cascade.html#specificity for an explantion 
 * of the meaning of the fields `a', `b', `c' and `d'. Field `e' (lowest priority)
 * is used for merging base styles. Field `a' is not used currently.
 */
struct ccss_selector_ {
	ccss_selector_modality_t	 modality;
	unsigned int			 importance : 2;
	unsigned int			 precedence : 5;
	unsigned int			 a : 5;
	unsigned int			 b : 5;
	unsigned int			 c : 5;
	unsigned int			 d : 5;
	unsigned int			 e : 5;
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
	to->importance = self->importance;
	to->precedence = self->precedence;
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
ccss_universal_selector_create (unsigned int		precedence,
			     ccss_selector_importance_t	importance)
{
	ccss_universal_selector_t *self;

	self = g_new0 (ccss_universal_selector_t, 1);
	self->modality = CCSS_SELECTOR_MODALITY_UNIVERSAL;
	self->importance = importance;
	self->precedence = precedence;

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
universal_selector_destroy (ccss_universal_selector_t *self)
{
	g_assert (self);

	g_free (self);
}

static void
universal_selector_serialize (ccss_universal_selector_t const	*self,
			      GString				*string_repr)
{
	g_string_append (string_repr, "*");
}

/*
 * Select by element type.
 */
typedef struct {
	ccss_selector_t	 parent;
	char		*type_name;
} ccss_type_selector_t;

ccss_selector_t *
ccss_type_selector_create (char const			*type_name,
			unsigned int			 precedence,
			ccss_selector_importance_t	 importance)
{
	ccss_type_selector_t *self;

	g_assert (type_name);

	self = g_new0 (ccss_type_selector_t, 1);
	self->parent.modality = CCSS_SELECTOR_MODALITY_TYPE;
	self->parent.importance = importance;
	self->parent.precedence = precedence;
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
type_selector_destroy (ccss_type_selector_t *self)
{
	g_assert (self);

	g_free (self->type_name);
	g_free (self);
}

static void
type_selector_serialize (ccss_type_selector_t const	*self,
			 GString			*string_repr)
{
	g_string_append (string_repr, self->type_name);
}

/*
 * Select by element's base type.
 * Derived from the type selector
 */
ccss_selector_t * 
ccss_base_type_selector_create (char const			*type_name,
			     unsigned int		 precedence,
			     ccss_selector_importance_t	 importance,
			     unsigned int		 specificity_e)
{
	ccss_selector_t *self;

	self = ccss_type_selector_create (type_name, precedence, importance);
	self->modality = CCSS_SELECTOR_MODALITY_BASE_TYPE;
	self->importance = importance;
	self->precedence = precedence;
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
ccss_class_selector_create (char const			*class_name,
			 unsigned int			 precedence,
			 ccss_selector_importance_t	 importance)
{
	ccss_class_selector_t *self;

	g_assert (class_name);

	self = g_new0 (ccss_class_selector_t, 1);
	self->parent.modality = CCSS_SELECTOR_MODALITY_CLASS;
	self->parent.importance = importance;
	self->parent.precedence = precedence;
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
class_selector_destroy (ccss_class_selector_t *self)
{
	g_assert (self);

	g_free (self->class_name);
	g_free (self);
}

static void
class_selector_serialize (ccss_class_selector_t const   *self,
			  GString			*string_repr)
{
	g_string_append_printf (string_repr, ".%s", self->class_name);
}

/*
 * Select by element ID.
 */
typedef struct {
	ccss_selector_t	 parent;
	char		*id;
} ccss_id_selector_t;

ccss_selector_t *
ccss_id_selector_create (char const			*id,
		      unsigned int			 precedence,
		      ccss_selector_importance_t	 importance)
{
	ccss_id_selector_t *self;

	g_assert (id);

	self = g_new0 (ccss_id_selector_t, 1);
	self->parent.modality = CCSS_SELECTOR_MODALITY_ID;
	self->parent.importance = importance;
	self->parent.precedence = precedence;
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
id_selector_destroy (ccss_id_selector_t *self)
{
	g_assert (self);

	g_free (self->id);
	g_free (self);
}

static void
id_selector_serialize (ccss_id_selector_t const *self,
		       GString			*string_repr)
{
	g_string_append_printf (string_repr, "#%s", self->id);
}

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
ccss_attribute_selector_create (char const				*name,
			     char const				*value,
			     ccss_attribute_selector_match_t	 match,
			     unsigned int			 precedence,
			     ccss_selector_importance_t		 importance)
{
	ccss_attribute_selector_t *self;

	g_assert (name && value);

	self = g_new0 (ccss_attribute_selector_t, 1);
	self->parent.modality = CCSS_SELECTOR_MODALITY_ATTRIBUTE;
	self->parent.importance = importance;
	self->parent.precedence = precedence;
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
attribute_selector_destroy (ccss_attribute_selector_t *self)
{
	g_assert (self);

	g_free (self->name);
	g_free (self->value);
	g_free (self);
}

static void
attribute_selector_serialize (ccss_attribute_selector_t const   *self,
			      GString				*string_repr)
{
	switch (self->match) {
	case CCSS_ATTRIBUTE_SELECTOR_MATCH_EXISTS:
		g_string_append_printf (string_repr, "[%s]", self->name);
		break;
	case CCSS_ATTRIBUTE_SELECTOR_MATCH_EQUALS:
		g_string_append_printf (string_repr, "[%s=%s]", self->name, self->value);
		break;
	default:
		g_assert_not_reached ();
	}
}

/*
 * Select by pseudo class.
 */
typedef struct {
	ccss_selector_t	 parent;
	char		*pseudo_class;
} ccss_pseudo_class_selector_t;

ccss_selector_t *
ccss_pseudo_class_selector_create (char const			*pseudo_class,
				unsigned int			 precedence,
				ccss_selector_importance_t	 importance)
{
	ccss_pseudo_class_selector_t *self;

	g_assert (pseudo_class);

	self = g_new0 (ccss_pseudo_class_selector_t, 1);
	self->parent.modality = CCSS_SELECTOR_MODALITY_PSEUDO_CLASS;
	self->parent.importance = importance;
	self->parent.precedence = precedence;
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
pseudo_class_selector_destroy (ccss_pseudo_class_selector_t *self)
{
	g_assert (self);

	g_free (self->pseudo_class);
	g_free (self);
}

static void
pseudo_class_selector_serialize (ccss_pseudo_class_selector_t const     *self,
				 GString				*string_repr)
{
	g_string_append_printf (string_repr, ":%s", self->pseudo_class);
}

/*
 * Select by unique instance.
 */
typedef struct {
	ccss_selector_t	 parent;
	ptrdiff_t	 instance;
} ccss_instance_selector_t;

ccss_selector_t *
ccss_instance_selector_create (ptrdiff_t			instance,
			    unsigned int		precedence,
			    ccss_selector_importance_t	importance)
{
	ccss_instance_selector_t *self;

	g_assert (instance);

	self = g_new0 (ccss_instance_selector_t, 1);
	self->parent.modality = CCSS_SELECTOR_MODALITY_INSTANCE;
	self->parent.importance = importance;
	self->parent.precedence = precedence;
	self->parent.a = 1;
	self->instance = instance;

	return (ccss_selector_t *) self;
}

static ccss_selector_t *
instance_selector_dup (ccss_instance_selector_t const *original)
{
	ccss_instance_selector_t *self;

	self = g_new0 (ccss_instance_selector_t, 1);
	selector_sync ((ccss_selector_t const *) original, &self->parent);
	self->instance = original->instance;

	return (ccss_selector_t *) self;
}

static void
instance_selector_destroy (ccss_instance_selector_t *self)
{
	g_assert (self);

	g_free (self);
}

static void
instance_selector_serialize (ccss_instance_selector_t const     *self,
			     GString				*string_repr)
{
	g_string_append_printf (string_repr, "/*:instance(%x)*/", self->instance);
}

ccss_selector_t *
ccss_selector_copy (ccss_selector_t const *original)
{
	ccss_selector_t *self;

	g_assert (original);

	self = NULL;
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
	case CCSS_SELECTOR_MODALITY_INSTANCE:
		self = instance_selector_dup ((ccss_instance_selector_t const *) original);
		break;
	default:
		g_assert_not_reached ();
		return FALSE;
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
ccss_selector_destroy (ccss_selector_t *self)
{
	g_return_if_fail (self);
	g_return_if_fail (CCSS_SELECTOR_MODALITY_IS_VALID (self));

	if (self->refinement) {
		ccss_selector_destroy (self->refinement), self->refinement = NULL;
	}

	if (self->container) {
		ccss_selector_destroy (self->container), self->container = NULL;
	}

	if (self->antecessor) {
		ccss_selector_destroy (self->antecessor), self->antecessor = NULL;
	}

	switch (self->modality) {
	case CCSS_SELECTOR_MODALITY_UNIVERSAL:
		universal_selector_destroy ((ccss_universal_selector_t *) self);
		break;
	case CCSS_SELECTOR_MODALITY_TYPE:
	case CCSS_SELECTOR_MODALITY_BASE_TYPE:
		type_selector_destroy ((ccss_type_selector_t *) self);
		break;
	case CCSS_SELECTOR_MODALITY_CLASS:
		class_selector_destroy ((ccss_class_selector_t *) self);
		break;
	case CCSS_SELECTOR_MODALITY_ID:
		id_selector_destroy ((ccss_id_selector_t *) self);
		break;
	case CCSS_SELECTOR_MODALITY_ATTRIBUTE:
		attribute_selector_destroy ((ccss_attribute_selector_t *) self);
		break;
	case CCSS_SELECTOR_MODALITY_PSEUDO_CLASS:
		pseudo_class_selector_destroy ((ccss_pseudo_class_selector_t *) self);
		break;
	case CCSS_SELECTOR_MODALITY_INSTANCE:
		instance_selector_destroy ((ccss_instance_selector_t *) self);
		break;
	default:
		g_assert_not_reached ();
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
	/* PONDERING: invert the meaing of `e' and only apply
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
	/* PONDERING: invert the meaing of `e' and only apply
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
	case CCSS_SELECTOR_MODALITY_INSTANCE:
		break;
	default:
		g_assert_not_reached ();
	}

	return false;
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
	case CCSS_SELECTOR_MODALITY_INSTANCE:
		break;
	default:
		g_assert_not_reached ();
	}

	return false;
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
	case CCSS_SELECTOR_MODALITY_INSTANCE:
		break;
	default:
		g_assert_not_reached ();
	}

	return false;
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

ccss_selector_importance_t
ccss_selector_get_importance (ccss_selector_t const *self)
{
	g_assert (self);

	return self->importance;
}

/*
ccss_stylesheet_precedence_t
ccss_selector_get_precedence (ccss_selector_t const *self)
{
	g_assert (self);

	return self->precedence;
}
*/

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
	case CCSS_SELECTOR_MODALITY_INSTANCE:
		break;
	default:
		g_assert_not_reached ();
	}

	return NULL;
}

uint32_t
ccss_selector_get_specificity (ccss_selector_t const *self)
{
	g_assert (self->importance <= CCSS_SELECTOR_MAX_IMPORTANCE &&
		  self->precedence <= CCSS_SELECTOR_MAX_PRECEDENCE &&
		  self->a <= CCSS_SELECTOR_MAX_SPECIFICITY &&
		  self->b <= CCSS_SELECTOR_MAX_SPECIFICITY &&
		  self->c <= CCSS_SELECTOR_MAX_SPECIFICITY &&
		  self->d <= CCSS_SELECTOR_MAX_SPECIFICITY &&
		  self->e <= CCSS_SELECTOR_MAX_SPECIFICITY);

	return self->e | (self->d << 5) | (self->c << 10) | (self->b << 15) | (self->a << 20) | (self->precedence << 25) | (self->importance << 30);
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
		    ccss_node_t			*node)
{
	ccss_node_t		*container;
	bool			 is_matching;

	container = ccss_node_get_container (node);
	if (container) {
		is_matching = ccss_selector_query (self, container);
		if (!is_matching) {
			is_matching = match_antecessor_r (self, container);
		}
		ccss_node_release (container);
	} else {
		is_matching = false;
	}

	return is_matching;
}

bool
ccss_selector_query (ccss_selector_t const	*self, 
		     ccss_node_t 		*node)
{
	char const	*name;
	char		*value;
	ptrdiff_t	 instance;
	bool		 is_matching;

	g_return_val_if_fail (self && node, false);

	is_matching = false;
	switch (self->modality) {
	case CCSS_SELECTOR_MODALITY_UNIVERSAL:
		is_matching = true;
		break;
	case CCSS_SELECTOR_MODALITY_TYPE:
		is_matching = ccss_node_is_a (node, 
				((ccss_type_selector_t *) self)->type_name);
		break;
	case CCSS_SELECTOR_MODALITY_BASE_TYPE:
		/* HACK warning: let's just say it matches, because the base
		 * type selectors have been set up internally -- that is in the
		 * fixup run after loading the stylesheet. */
		is_matching = true;
		break;
	case CCSS_SELECTOR_MODALITY_CLASS:
		name = ccss_node_get_class (node);
		is_matching = !g_strcmp0 (name, 
				((ccss_class_selector_t *) self)->class_name);
		break;
	case CCSS_SELECTOR_MODALITY_ID:
		name = ccss_node_get_id (node);
		is_matching = !g_strcmp0 (name, 
				((ccss_id_selector_t *) self)->id);
		break;
	case CCSS_SELECTOR_MODALITY_ATTRIBUTE:
		name = ((ccss_attribute_selector_t *) self)->name;
		value = ccss_node_get_attribute (node, name);
		switch (((ccss_attribute_selector_t *) self)->match) {
		case CCSS_ATTRIBUTE_SELECTOR_MATCH_EXISTS:
			is_matching = value ? true : false;
			break;
		case CCSS_ATTRIBUTE_SELECTOR_MATCH_EQUALS:
			is_matching = !g_strcmp0 (value, 
				((ccss_attribute_selector_t *) self)->value);
			break;
		}
		g_free (value), value = NULL;
		break;
	case CCSS_SELECTOR_MODALITY_PSEUDO_CLASS:
		for (const char **pseudo_classes = ccss_node_get_pseudo_classes (node);
		     pseudo_classes && *pseudo_classes;
		     pseudo_classes++) {
			is_matching = !g_strcmp0 (*pseudo_classes,
				((ccss_pseudo_class_selector_t *) self)->pseudo_class);
		}
		break;
	case CCSS_SELECTOR_MODALITY_INSTANCE:
		instance = ccss_node_get_instance (node);
		is_matching = (instance ==
				((ccss_instance_selector_t *) self)->instance);
		break;
	default:
		g_assert_not_reached ();
		return false;
	}

	if (!is_matching) {
		return false;
	}

	/* recursively match refinements */
	if (self->refinement) {
		is_matching = ccss_selector_query (self->refinement, node);
		if (!is_matching) {
			return false;
		}
	}

	/* match container */
	if (self->container) {
		ccss_node_t *container;
		container = ccss_node_get_container (node);
		is_matching = false;
		if (container) {
			is_matching = ccss_selector_query (self->container, container);
			ccss_node_release (container);
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

	return true;
}

bool
ccss_selector_apply (ccss_selector_t const	*self,
		     ccss_node_t const		*node,
		     ccss_style_t		*style)
{
	GHashTableIter		 iter;
	gpointer		 key;
	gpointer		 value;
	uint32_t		 x, y, width, height;
	bool			 ret;

	g_return_val_if_fail (self && self->block && style, false);

	/* Apply css properties to the style.
	 * FIXME: this simple merge strategy doesn't work for `border-image',
	 * as they should be overridden by a higher specificity `border'. */
	g_hash_table_iter_init (&iter, self->block->properties);
	while (g_hash_table_iter_next (&iter, &key, &value)) {

		g_hash_table_insert (style->properties, key, value);
#ifdef CCSS_DEBUG
		/* Track where the property comes from. */
		ccss_style_set_property_selector (style,
						  (ccss_property_base_t const *) value,
						  self);
#endif
	}

	/* Requiring a node instance here would make the type-based API impossible. */
	if (node) {
		/* Update viewport (for things like fixed background-images). */
		ret = ccss_node_get_viewport (node, &x, &y, &width, &height);
		if (ret) {
			style->viewport_x = x;
			style->viewport_y = y;
			style->viewport_width = width;
			style->viewport_height = height;
		}
	}

	return true;
}

void
ccss_selector_serialize_specificity (ccss_selector_t const      *self,
				     GString			*specificity)
{
	g_return_if_fail (self);
	g_return_if_fail (specificity);

	g_string_append_printf (specificity, "%d,%d,%d,%d,%d,%d,%d",
				self->importance,
				self->precedence,
				self->a,
				self->b,
				self->c,
				self->d,
				self->e);
}

void
ccss_selector_serialize_selector (ccss_selector_t const *self,
				  GString		*selector)
{
	g_return_if_fail (self);
	g_return_if_fail (selector);
	g_return_if_fail (CCSS_SELECTOR_MODALITY_IS_VALID (self));

	if (self->container) {
		ccss_selector_serialize_selector (self->container, selector);
		g_string_append (selector, " > ");
	}

	if (self->antecessor) {
		ccss_selector_serialize_selector (self->antecessor, selector);
		g_string_append (selector, " ");
	}

	switch (self->modality) {
	case CCSS_SELECTOR_MODALITY_UNIVERSAL:
		universal_selector_serialize ((ccss_universal_selector_t *) self,
					      selector);
		break;
	case CCSS_SELECTOR_MODALITY_TYPE:
	case CCSS_SELECTOR_MODALITY_BASE_TYPE:
		type_selector_serialize ((ccss_type_selector_t *) self,
					 selector);
		break;
	case CCSS_SELECTOR_MODALITY_CLASS:
		class_selector_serialize ((ccss_class_selector_t *) self,
					  selector);
		break;
	case CCSS_SELECTOR_MODALITY_ID:
		id_selector_serialize ((ccss_id_selector_t *) self,
				       selector);
		break;
	case CCSS_SELECTOR_MODALITY_ATTRIBUTE:
		attribute_selector_serialize ((ccss_attribute_selector_t *) self,
					      selector);
		break;
	case CCSS_SELECTOR_MODALITY_PSEUDO_CLASS:
		pseudo_class_selector_serialize ((ccss_pseudo_class_selector_t *) self,
						 selector);
		break;
	case CCSS_SELECTOR_MODALITY_INSTANCE:
		instance_selector_serialize ((ccss_instance_selector_t *) self,
					     selector);
		break;
	default:
		g_assert_not_reached ();
		return;
	}

	if (self->refinement) {
		ccss_selector_serialize_selector (self->refinement, selector);
	}
}

void
ccss_selector_dump (ccss_selector_t const *self)
{
	GString *selector;

	selector = g_string_new (NULL);
	ccss_selector_serialize_selector (self, selector);
	g_string_append (selector, " /* ");
	ccss_selector_serialize_specificity (self, selector);
	g_string_append (selector, " */\n");
	printf ("%s", selector->str);
	g_string_free (selector, TRUE), selector = NULL;

	if (self->block) {
		printf ("{\n");
		ccss_block_dump (self->block);
		printf ("}\n\n");
	}
}

