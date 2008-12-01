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

#ifndef CCSS_SELECTOR_H
#define CCSS_SELECTOR_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <ccss/ccss-block.h>
#include <ccss/ccss-macros.h>
#include <ccss/ccss-node.h>
#include <ccss/ccss-style.h>

CCSS_BEGIN_DECLS

#define CCSS_SELECTOR_MAX_IMPORTANCE	(2)
#define CCSS_SELECTOR_MAX_PRECEDENCE	((1 << 5) - 1)
#define CCSS_SELECTOR_MAX_SPECIFICITY	((1 << 5) - 1)

typedef struct ccss_selector_ ccss_selector_t;

typedef enum {
	CCSS_SELECTOR_IMPORTANCE_NONE	= 0,
	CCSS_SELECTOR_IMPORTANCE_AUTHOR,
	CCSS_SELECTOR_IMPORTANCE_USER
} ccss_selector_importance_t;

typedef enum {
	CCSS_ATTRIBUTE_SELECTOR_MATCH_EXISTS, 
	CCSS_ATTRIBUTE_SELECTOR_MATCH_EQUALS
	/* more match types go here */
} ccss_attribute_selector_match_t;

ccss_selector_t *
ccss_universal_selector_create	(unsigned int			 precedence,
				 ccss_selector_importance_t	 importance);
ccss_selector_t *
ccss_type_selector_create	(char const			*type_name,
				 unsigned int			 precedence,
				 ccss_selector_importance_t	 importance);
ccss_selector_t *
ccss_base_type_selector_create	(char const			*type_name,
				 unsigned int			 precedence,
				 ccss_selector_importance_t	 importance,
				 unsigned int			 specificity_e);
ccss_selector_t *
ccss_class_selector_create	(char const			*class_name,
				 unsigned int			 precedence,
				 ccss_selector_importance_t	 importance);
ccss_selector_t *
ccss_id_selector_create		(char const			*id,
				 unsigned int			 precedence,
				 ccss_selector_importance_t	 importance);
ccss_selector_t *
ccss_attribute_selector_create	(char const			*name,
				 char const			*value,
				 ccss_attribute_selector_match_t match,
				 unsigned int			 precedence,
				 ccss_selector_importance_t	 importance);
ccss_selector_t *
ccss_pseudo_class_selector_create (char const			*pseudo_class,
				 unsigned int			 precedence,
				 ccss_selector_importance_t	 importance);
ccss_selector_t *
ccss_instance_selector_create	(ptrdiff_t			 instance,
				 unsigned int			 precedence,
				 ccss_selector_importance_t	 importance);

ccss_selector_t * ccss_selector_copy		(ccss_selector_t const *original);
ccss_selector_t * ccss_selector_copy_as_base	(ccss_selector_t const *original,
						 int specificity_e);

void ccss_selector_destroy	(ccss_selector_t *self);

void		 ccss_selector_refine		(ccss_selector_t *self, ccss_selector_t *selector);
ccss_selector_t * ccss_selector_append_child	(ccss_selector_t *self, ccss_selector_t *selector);
ccss_selector_t * ccss_selector_append_descendant	(ccss_selector_t *self, ccss_selector_t *selector);

bool ccss_selector_is_type	(ccss_selector_t const *self);
bool ccss_selector_is_class	(ccss_selector_t const *self);
bool ccss_selector_is_id		(ccss_selector_t const *self);

ccss_block_t const *	ccss_selector_get_block	(ccss_selector_t const *self);
void			ccss_selector_set_block	(ccss_selector_t *self, ccss_block_t const *block);

char const *			ccss_selector_get_key		(ccss_selector_t const *self);
ccss_selector_importance_t	ccss_selector_get_importance	(ccss_selector_t const *self);
/*ccss_stylesheet_precedence_t	ccss_selector_get_precedence	(ccss_selector_t const *self);*/
uint32_t			ccss_selector_get_specificity	(ccss_selector_t const *self);
void				ccss_selector_get_specificity_values	(ccss_selector_t const *self, 
									 unsigned int *a,
									 unsigned int *b,
									 unsigned int *c,
									 unsigned int *d,
									 unsigned int *e);

bool
ccss_selector_query (ccss_selector_t const	*self,
		     ccss_node_t const		*node);

bool
ccss_selector_apply (ccss_selector_t const	*self,
		     ccss_node_t const		*node,
		     ccss_style_t		*style);

void ccss_selector_dump (ccss_selector_t const *self);

CCSS_END_DECLS

#endif /* CCSS_SELECTOR_H */

