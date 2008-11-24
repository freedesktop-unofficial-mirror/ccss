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

#ifndef CCSS_PROPERTY_H
#define CCSS_PROPERTY_H

#include <stdbool.h>
#include <libcroco/libcroco.h>
#include <ccss/ccss-macros.h>

CCSS_BEGIN_DECLS

/* #include <ccss/ccss-block.h> avoid circular dependency. */
struct ccss_block_;

/**
 * ccss_property_state_t:
 * @CCSS_PROPERTY_STATE_UNSET:		error state, invalid property.
 * @CCSS_PROPERTY_STATE_NONE:		property set to `none', switched off.
 * @CCSS_PROPERTY_STATE_INHERIT:	inherit property from container.
 * @CCSS_PROPERTY_STATE_SET:		property is valid and set.
 *
 * This enum must be embedded as first field in every property implementation.
 **/
typedef enum {
	CCSS_PROPERTY_STATE_UNSET = 0,
	CCSS_PROPERTY_STATE_NONE,
	CCSS_PROPERTY_STATE_INHERIT,
	CCSS_PROPERTY_STATE_SET
} ccss_property_state_t;

/**
 * ccss_property_type_t:
 * @CCSS_PROPERTY_TYPE_DOUBLE:	property represented by a floating point number.
 * @CCSS_PROPERTY_TYPE_STRING:	property represented by a string.
 *
 * Type descriptions for generic properties.
 **/
typedef enum {
	CCSS_PROPERTY_TYPE_DOUBLE,
	CCSS_PROPERTY_TYPE_STRING
} ccss_property_type_t;

typedef struct ccss_property_base_ ccss_property_base_t;

/**
 * ccss_property_new_f:
 * @values:	libcroco CSS values to parse for the property, see #CRTerm.
 *
 * Hook function for instantiating a property.
 *
 * Returns: pointer to the allocated property instance or %NULL if parsing fails.
 **/
typedef ccss_property_base_t * (*ccss_property_new_f) (CRTerm const *values);

/**
 * ccss_property_free_f:
 * @self:	pointer to property instance.
 *
 * Hook function for deallocating a property instance.
 **/
typedef void (*ccss_property_free_f) (ccss_property_base_t *self);

/** 
 * ccss_property_convert_f:
 * @self:	pointer to property instance.
 * @target:	conversion target type, see #ccss_property_type_t.
 * @value:	pointer to memory location where to place the converted value.
 *
 * Hook function for converting a property instance.
 *
 * Returns: %TRUE if the conversion was successful.
 **/
typedef bool (*ccss_property_convert_f) (ccss_property_base_t const	*self,
					 ccss_property_type_t		 target,
					 void				*value);

/**
 * ccss_property_factory_f:
 * @block:	the #ccss_block_t the properties will be associated to.
 * @values:	libcroco CSS values to parse for the property, see #CRTerm.
 *
 * Hook function to handle the creation of multiple properties from a single CSS property, e.g. `border'.
 *
 * Returns: %TRUE when sucessful.
 **/
typedef bool (*ccss_property_factory_f) (struct ccss_block_	*block,
					 CRTerm const		*values);
/**
 * ccss_property_class_t:
 * @name:	property name.
 * @alloc:	allocation hook, see #ccss_property_new_f.
 * @dealloc:	deallocation hook, see #ccss_property_free_f.
 * @convert:	conversion hook, see #ccss_property_convert_f.
 * @factory:	factory hook, see #ccss_property_factory_f.
 *
 * Entry in the table of property implementations passed when initialising ccss, see #ccss_init.
 **/
typedef struct {
	char const		*name;
	ccss_property_new_f	 property_new;
	ccss_property_free_f	 property_free;
	ccss_property_convert_f	 property_convert;
	ccss_property_factory_f	 property_factory;
} ccss_property_class_t;

/**
 * ccss_property_base_t:
 * @property_class:	class descriptor, see #ccss_property_class_t.
 * @state:		property state, see #ccss_property_state_t.
 *
 * This structure has to be embedded at the beginning of every custom property.
 **/
struct ccss_property_base_ {
	ccss_property_class_t const	*property_class;
	ccss_property_state_t		 state;
};

void
ccss_property_init (ccss_property_base_t	*self,
		    ccss_property_class_t const	*property_class);

ccss_property_state_t
ccss_property_parse_state (CRTerm const **value);

CCSS_END_DECLS

#endif /* CCSS_PROPERTY_H */

