/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


#ifndef _DATA_H
#define _DATA_H

/** package with data classes (matrices, vectors, dictionaries, etc.).
 *  @file     data.h
 *  @defgroup data		data package
 *
 *  @{
 */

#include <fts/fts.h>
#include <fts/packages/utils/utils.h>

#ifdef WIN32
#if defined(DATA_EXPORTS)
#define DATA_API __declspec(dllexport)
#else
#define DATA_API __declspec(dllimport)
#endif
#else
#define DATA_API extern
#endif

#define data_float_equals(f, g) (fabs((f) - (g)) < 1.0e-7)

DATA_API fts_method_status_t data_object_output(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret);  

/******************************************************************************
 *
 *  @defgroup expression		expression class
 */

typedef struct
{
  fts_object_t o;
  fts_symbol_t symbol;
  fts_array_t descr;
  fts_expression_t *expression;
  fts_status_t status;
} expr_t;

DATA_API fts_class_t *expr_class;

DATA_API fts_method_status_t expr_evaluate(expr_t *self, fts_hashtable_t *locals, int ac, const fts_atom_t *at, fts_atom_t *ret);
DATA_API fts_method_status_t expr_evaluate_in_scope(expr_t *self, fts_patcher_t *scope, int ac, const fts_atom_t *at, fts_atom_t *ret);




/******************************************************************* 
 *
 *  @defgroup enumeration	enumeration type support
 *
 */

typedef struct
{
  fts_hashtable_t hash;
  fts_symbol_t *array;
  int alloc;
  int size;
} enumeration_t;

#define enumeration_get_size(e) ((e)->size)

enumeration_t *enumeration_new(fts_symbol_t name);
enumeration_t *enumeration_get_by_name(fts_symbol_t name);
fts_symbol_t enumeration_add_name(enumeration_t *e, const fts_symbol_t name);
int enumeration_get_index(enumeration_t *e, const fts_symbol_t name);
fts_symbol_t enumeration_get_name(enumeration_t *e, int index);




/******************************************************************* 
 *
 *  @defgroup propobj	base class with properties
 *
 */

typedef struct
{
  int index;
  fts_symbol_t name;
  fts_symbol_t type;
} propobj_property_t;

#define propobj_property_get_index(p) ((p)->index)
#define propobj_property_get_name(p) ((p)->name)
#define propobj_property_get_type(p) ((p)->type)

typedef struct
{
  fts_hashtable_t hash;
  propobj_property_t *array;
  int alloc;
  int n_properties;
} propobj_class_description_t;

typedef struct
{
  fts_object_t o;
  fts_array_t properties;
} propobj_t;

#define propobj_get_type(p) ((p)->type)

void propobj_get_property(propobj_t *self, propobj_property_t *prop, fts_atom_t *p);
void propobj_set_property(propobj_t *self, propobj_property_t *prop, const fts_atom_t *value);
void propobj_get_property_by_index(propobj_t *self, int index, fts_atom_t *p);
void propobj_set_property_by_index(propobj_t *self, int index, const fts_atom_t *value);
void propobj_get_property_by_name(propobj_t *self, fts_symbol_t name, fts_atom_t *p);
void propobj_set_property_by_name(propobj_t *self, fts_symbol_t name, const fts_atom_t *value);
void propobj_set_int_property_by_name(propobj_t *self, fts_symbol_t name, const fts_atom_t *value);
void propobj_set_float_property_by_name(propobj_t *self, fts_symbol_t name, const fts_atom_t *value);

void propobj_init(fts_object_t *o);
void propobj_delete(fts_object_t *o);

/* propobj class */
void propobj_class_init(fts_class_t *cl);
propobj_class_description_t *propobj_class_get_descritption(fts_class_t *cl);
propobj_class_description_t *propobj_get_descritption(propobj_t *self);

propobj_property_t *propobj_class_add_int_property(fts_class_t *cl, fts_symbol_t name, fts_method_t set_method);
propobj_property_t *propobj_class_add_float_property(fts_class_t *cl, fts_symbol_t name, fts_method_t set_method);
propobj_property_t *propobj_class_add_symbol_property(fts_class_t *cl, fts_symbol_t name, fts_method_t set_method);
propobj_property_t *propobj_class_get_property_by_name(fts_class_t *cl, fts_symbol_t name);
propobj_property_t *propobj_class_get_property_by_index(fts_class_t *cl, int index);

/* utilities */
void propobj_class_append_properties(fts_class_t *cl, fts_array_t *array);
void propobj_append_properties(propobj_t *self, fts_array_t *array);
void propobj_post_properties(propobj_t *self, fts_bytestream_t *stream);
fts_method_status_t propobj_dump_properties(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret);
fts_method_status_t propobj_remove_property(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret);

void propobj_copy(propobj_t *org, propobj_t *copy);
void propobj_copy_function(const fts_object_t *from, fts_object_t *to);
int propobj_equals(const propobj_t *a, const propobj_t *b);




/******************************************************************* 
 *
 *  data classes
 *
 */

#include <fts/packages/data/mat.h>
#include <fts/packages/data/ivec.h>
#include <fts/packages/data/fmat.h>
#include <fts/packages/data/fvec.h>
#include <fts/packages/data/dict.h>
#include <fts/packages/data/bpf.h>
#include <fts/packages/data/tree.h>

DATA_API void data_config(void);


/** @} end of package data */

#endif /* _DATA_H */
