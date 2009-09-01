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

/**
 * SECTION:property-impl
 * @short_description: internal functionality associated to ccss_property_t.
 * @see_also: #ccss_property_t
 * @stability: Unstable
 * @include: ccss/ccss-property-impl.h
 *
 * Declarations for the implementation of custom properties.
 * This declarations are not to be considered part of the stable
 * ccss interface. Use with care.
 **/

#ifndef CCSS_PROPERTY_IMPL_H
#define CCSS_PROPERTY_IMPL_H

/* Allow standalone inclusion. */
#define CCSS_STANDALONE_HEADER

#include <libcroco/libcroco.h>
#include <ccss/ccss-property.h>

CCSS_BEGIN_DECLS

/* Avoid circular dependencies. */
struct ccss_block_;
struct ccss_grammar_;
struct ccss_style_;

/**
 * ccss_property_create_f:
 * @grammar:	the grammar associated with this property.
 * @values:	libcroco CSS values to parse for the property, see #CRTerm.
 * @user_data:	user data passed to property- or function-handler.
 *
 * Hook function for instantiating a property.
 *
 * Returns: pointer to the allocated property instance or %NULL if parsing fails.
 **/
typedef ccss_property_t * (*ccss_property_create_f) (struct ccss_grammar_ const	*grammar,
						     CRTerm const		*values,
						     void			*user_data);

/**
 * ccss_property_destroy_f:
 * @self:	pointer to property instance.
 *
 * Hook function for deallocating a property instance.
 **/
typedef void (*ccss_property_destroy_f) (ccss_property_t *self);

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
typedef bool (*ccss_property_convert_f) (ccss_property_t const	*self,
					 ccss_property_type_t	 target,
					 void			*value);

/**
 * ccss_property_factory_f:
 * @grammar:	the grammar associated with this property.
 * @block:	the #ccss_block_t the properties will be associated to.
 * @name:	name of the property.
 * @values:	libcroco CSS values to parse for the property, see #CRTerm.
 * @user_data:	user data passed to property- or function-handler.
 *
 * Hook function to handle the creation of multiple properties from a single CSS property, e.g. `border'.
 *
 * Returns: %TRUE when sucessful.
 **/
typedef bool (*ccss_property_factory_f) (struct ccss_grammar_ const	*grammar,
					 struct ccss_block_		*block,
					 char const			*name,
					 CRTerm const			*values,
					 void				*user_data);

/**
 * ccss_property_inherit_f:
 * @container_style:	style to inherit from.
 * @style:		style to inherit to.
 *
 * Hook function to inherit multi-value properties like `border'.
 *
 * Returns: %TRUE if property inheritance could be resolved.
 **/
typedef bool (*ccss_property_inherit_f) (struct ccss_style_ const	*container_style,
					 struct ccss_style_		*style);

/**
 * ccss_property_serialize_f:
 * @self:	pointer to property instance.
 *
 * Hook function to reformat a property to CSS syntax.
 *
 * Returns: %TRUE if property inheritance could be resolved.
 **/
typedef char * (*ccss_property_serialize_f) (ccss_property_t const *self);

/**
 * ccss_property_class_t:
 * @name:	property name.
 * @create:	allocation hook, see #ccss_property_create_f.
 * @destroy:	deallocation hook, see #ccss_property_destroy_f.
 * @convert:	conversion hook, see #ccss_property_convert_f.
 * @factory:	factory hook, see #ccss_property_factory_f.
 * @inherit:	inherit hook, see #ccss_property_inherit_f.
 * @inherit:	inherit hook, see #ccss_property_inherit_f.
 * @serialize:	serialize hook, see #ccss_property_serialize_f.
 *
 * Property interpretation vtable entry.
 **/
struct ccss_property_class_ {
	char const			*name;
	ccss_property_create_f		 create;
	ccss_property_destroy_f		 destroy;
	ccss_property_convert_f		 convert;
	ccss_property_factory_f		 factory;
	ccss_property_inherit_f		 inherit;
	ccss_property_serialize_f	 serialize;
	/*< private >*/
	void (*_padding_0) (void);
	void (*_padding_1) (void);
	void (*_padding_2) (void);
	void (*_padding_3) (void);
	void (*_padding_4) (void);
};

/**
 * ccss_property_generic_t:
 * @base:	base property.
 * @name:	name of the property, e.g. %color.
 * @values:	linked list of values.
 *
 * Implementation of a generic, single-value property.
 **/
typedef struct {
	/*< private >*/
	CCSS_DEPRECATED (ccss_property_t	 base);

	CCSS_DEPRECATED (char			*name);
	CCSS_DEPRECATED (CRTerm			*values);
} ccss_property_generic_t;

ccss_property_state_t
ccss_property_parse_state       (CRTerm const			**value);

void
ccss_property_init		(ccss_property_t		 *self,
				 ccss_property_class_t const	 *property_class);

void
ccss_property_destroy		(ccss_property_t		 *self);

bool
ccss_style_interpret_property   (struct ccss_style_ const	 *self,
				 char const			 *property_name,
				 ccss_property_create_f		  property_ctor,
				 void				 *user_data,
				 ccss_property_t		**property);

CCSS_END_DECLS

#endif /* CCSS_PROPERTY_IMPL_H */

