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
 *
 */

/** @file data.c data object doc
 */

#include <fts/fts.h>
#include <fts/packages/data/data.h>
#include <string.h>

#include <ftsconfig.h>

#if HAVE_ALLOCA_H
#include <alloca.h>
#endif


/** @defgroup dataobj data object user messages
 *
 *  Messages that can be sent to or invoked on data objects.
 */

fts_method_status_t
data_object_output(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_outlet_object(o, 0, o);
  
  return fts_ok;
}




/***********************************************************************
 *
 * expression class
 *
 */

static fts_symbol_t sym_expr = NULL;
fts_class_t *expr_class = NULL;

static fts_method_status_t
expr_set(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  expr_t *self = (expr_t *) o;
  fts_symbol_t sym = fts_get_symbol(at);
  const char *str = fts_symbol_name(sym);
  int len = strlen(str);
  char *buff = (char *)alloca(sizeof(char) * len + 2);
  fts_tokenizer_t tokenizer;
  fts_status_t status;
  fts_atom_t a;
  
  self->symbol = fts_get_symbol(at);

  strcpy(buff, str);
  buff[len + 1] = '\0'; /* tokenizer needs double null termination */
  
  fts_tokenizer_init_buffer(&tokenizer, buff, len + 2);
  
  while(fts_tokenizer_next(&tokenizer, &a) != 0)
    fts_array_append(&self->descr, 1, &a);
  
  fts_tokenizer_destroy(&tokenizer);
  
  status = fts_expression_set(self->expression, fts_array_get_size(&self->descr), fts_array_get_atoms(&self->descr));
  
  if(status != fts_ok)
  {
    fts_object_error(o, fts_status_get_description(status));
    fts_expression_delete(self->expression);
    self->expression = NULL;
  }
  
  self->status = fts_ok;  
  
  return fts_ok;
}

static fts_status_t
expr_method_callback(int ac, const fts_atom_t *at, void *o)
{
  expr_t *self = (expr_t *)o;
  
  fts_array_set(&self->descr, ac, at);
  
  return fts_ok;
}

static fts_method_status_t
_expr_evaluate(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  expr_t *self = (expr_t *)o;
  fts_hashtable_t *locals = NULL;
  
  if(fts_is_a(at, dict_class))
  {
    dict_t *dict = (dict_t *)fts_get_object(at);
    locals = dict_get_hashtable(dict);
    
    ac--;
    at++;
  }
  
  if(self->expression != NULL)
  {
    fts_status_t status = fts_expression_evaluate(self->expression, locals, fts_get_global_definitions(), ac, at, expr_method_callback, self);
    int ret_ac = fts_array_get_size(&self->descr);
    fts_atom_t *ret_at = fts_array_get_atoms(&self->descr);
    
    if(status != fts_ok)
      fts_object_error(o, fts_status_get_description(status));
    
    self->status = status;
    
    if(ret_ac > 1)
    {
      fts_object_t *tup = fts_object_create(fts_tuple_class, ret_ac, ret_at);
      fts_set_object(ret, tup);
    }
    else if(ret_ac > 0)
      *ret = *ret_at;
    
  }
  
  return fts_ok;
}

static fts_status_t
expr_function_callback(int ac, const fts_atom_t *at, void *o)
{
  fts_atom_t *a = (fts_atom_t *)o;
  
  if(ac > 1)
  {
    fts_object_t *tup = fts_object_create(fts_tuple_class, ac, at);
    fts_set_object(a, tup);
  }
  else if(ac > 0)
    *a = *at;
  
  return fts_ok;
}

fts_method_status_t
expr_evaluate_in_scope(expr_t *self, fts_patcher_t *scope, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if(scope == NULL)
    scope = fts_get_root_patcher();
  
  if(self->expression != NULL)
  {
    fts_status_t status = fts_expression_reduce(self->expression, scope, ac, at, expr_function_callback, (void *)ret);
    
    if(status != fts_ok)
      fts_set_void(ret);
    
    self->status = status;
  }
  
  return fts_ok;
}

fts_method_status_t
expr_evaluate(expr_t *self, fts_hashtable_t *locals, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if(self->expression != NULL)
  {
    fts_status_t status = fts_expression_evaluate(self->expression, locals, fts_get_global_definitions(), ac, at, expr_function_callback, (void *)ret);
    
    if(status != fts_ok)
      fts_set_void(ret);
    
    self->status = status;
  }
  
  return fts_ok;
}

static void
expr_description_function(fts_object_t *o, fts_array_t *array)
{
  expr_t *self = (expr_t *)o;
  
  fts_array_append_symbol(array, sym_expr);
  fts_array_append_symbol(array, self->symbol);
}

static fts_method_status_t
expr_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  expr_t *self = (expr_t *)o;
  
  self->symbol = NULL;
  self->expression = NULL;
  fts_array_init(&self->descr, 0, 0);  
  
  if(ac > 0 && fts_is_symbol(at))
  {
    fts_expression_new(0, 0, &self->expression);
    expr_set(o, NULL, ac, at, fts_nix);
  }
  else
    fts_object_error(o, "expression argument required");
  
  return fts_ok;
}

static fts_method_status_t
expr_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  expr_t *self = (expr_t *)o;
  
  fts_array_destroy(&self->descr);
  
  if(self->expression != NULL)
    fts_expression_delete(self->expression);
  
  return fts_ok;
}

static void
expr_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(expr_t), expr_init, expr_delete);
  
  fts_class_set_description_function(cl, expr_description_function);
  fts_class_message_varargs(cl, fts_s_name, fts_object_name);
  
  fts_class_message_varargs(cl, fts_new_symbol("eval"), _expr_evaluate);
  
  fts_class_doc(cl, sym_expr, "<sym: expression description>", "expression");
  fts_class_doc(cl, fts_new_symbol("eval"), "[<dict: local names>] [<any: arguments> ...]", "evaluate expression");
}




/***********************************************************************
 *
 *
 *  enumeration type support
 *
 */

fts_hashtable_t global_enumeration_table; 

#define ENUMERATION_ALLOC_BLOCK 32

enumeration_t *
enumeration_new(fts_symbol_t name)
{
  fts_atom_t k, v;
  
  fts_set_symbol(&k, name);
  if(!fts_hashtable_get(&global_enumeration_table, &k, &v))
  {
    enumeration_t *e = (enumeration_t *)fts_malloc(sizeof(enumeration_t));
    
    fts_hashtable_init(&e->hash, FTS_HASHTABLE_MEDIUM);
    e->array = (fts_symbol_t *)fts_malloc(ENUMERATION_ALLOC_BLOCK * sizeof(fts_symbol_t));
    e->alloc = ENUMERATION_ALLOC_BLOCK;
    e->size = 0;
    
    fts_set_pointer(&v, e);
    fts_hashtable_put(&global_enumeration_table, &k, &v);
    
    return e;
  }
  
  return NULL;
}

enumeration_t *
enumeration_get_by_name(fts_symbol_t name)
{
  fts_atom_t k, v;
  
  fts_set_symbol(&k, name);
  if(fts_hashtable_get(&global_enumeration_table, &k, &v))
    return (enumeration_t *)fts_get_pointer(&v);
  
  return NULL;
}

fts_symbol_t 
enumeration_add_name(enumeration_t *e, const fts_symbol_t name)
{
  fts_atom_t k, a;
  
  fts_set_symbol(&k, name);
  if(!fts_hashtable_get(&e->hash, &k, &a))
  {
    int index = e->size;
    
    fts_set_int(&a, index);
    fts_hashtable_put(&e->hash, &k, &a);
    
    if(index >= e->alloc)
    {
      e->alloc += ENUMERATION_ALLOC_BLOCK;
      fts_realloc(e->array, e->alloc * sizeof(fts_symbol_t));
    }
    
    e->array[index] = name;
    e->size++;
    
    return name;
  }
  
  return NULL;
}

int
enumeration_get_index(enumeration_t *e, const fts_symbol_t name)
{
  fts_atom_t k, a;
  
  fts_set_symbol(&k, name);
  if(fts_hashtable_get(&e->hash, &k, &a))
    return fts_get_int(&a);
  
  return -1;
}

fts_symbol_t 
enumeration_get_name(enumeration_t *e, int index)
{
  if(index >= 0 && index < e->size)
    return e->array[index];
  
  return NULL;
}

/***********************************************************************
*
*  propobj, base class with dynamic properties
*
*/
void
propobj_get_property(propobj_t *self, propobj_property_t *prop, fts_atom_t *p)
{
  if(prop->index < fts_array_get_size(&self->properties))
    *p = *fts_array_get_element(&self->properties, prop->index);
  else
    fts_set_void(p);
}

void
propobj_set_property(propobj_t *self, propobj_property_t *prop, const fts_atom_t *value)
{  
  if(prop->index > 0)
    fts_array_set_element(&self->properties, prop->index, value);
}

void
propobj_get_property_by_index(propobj_t *self, int index, fts_atom_t *p)
{
  if(index >= 0 && index < fts_array_get_size(&self->properties))
    *p = *fts_array_get_element(&self->properties, index);
  else
    fts_set_void(p);
}

void
propobj_set_property_by_index(propobj_t *self, int index, const fts_atom_t *value)
{
  fts_array_set_element(&self->properties, index, value);
}

void
propobj_get_property_by_name(propobj_t *self, fts_symbol_t name, fts_atom_t *p)
{
  propobj_property_t *prop = propobj_class_get_property_by_name(fts_object_get_class((fts_object_t *)self), name);
  
  if(prop != NULL)
    propobj_get_property_by_index(self, prop->index, p);
  else
    fts_set_void(p);
}

void
propobj_set_property_by_name(propobj_t *self, fts_symbol_t name, const fts_atom_t *value)
{
  propobj_property_t *prop = propobj_class_get_property_by_name(fts_object_get_class((fts_object_t *)self), name);
  
  if(prop != NULL)
    propobj_set_property_by_index(self, prop->index, value);
}

void
propobj_set_int_property_by_name(propobj_t *self, fts_symbol_t name, const fts_atom_t *value)
{
  propobj_property_t *prop = propobj_class_get_property_by_name(fts_object_get_class((fts_object_t *)self), name);  
  fts_atom_t a;
  
  fts_set_int(&a, fts_get_number_int(value));
  
  if(prop != NULL)
    propobj_set_property_by_index(self, prop->index, value);
}

void
propobj_set_float_property_by_name(propobj_t *self, fts_symbol_t name, const fts_atom_t *value)
{
  propobj_property_t *prop = propobj_class_get_property_by_name(fts_object_get_class((fts_object_t *)self), name);
  fts_atom_t a;
  
  fts_set_float(&a, fts_get_number_float(value));
  
  if(prop != NULL)
    propobj_set_property_by_index(self, prop->index, value);
}

fts_method_status_t
propobj_remove_property(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  propobj_t *self = (propobj_t *)o;
  fts_symbol_t name = fts_get_symbol(at);
  
  propobj_set_property_by_name(self, name, fts_null);
  
  return fts_ok;
}

void
propobj_append_properties(propobj_t *self, fts_array_t *array)
{
  propobj_class_description_t *descr = propobj_get_descritption(self);
  int size = fts_array_get_size(&self->properties);
  fts_atom_t *atoms = fts_array_get_atoms(&self->properties);
  int i;
  
  for(i=0; i<size; i++)
  {
    if(!fts_is_void(atoms + i))
    {
      fts_array_append_symbol(array, descr->array[i].name);
      fts_array_append(array, 1, atoms + i);
    }
  }
}

void
propobj_post_properties(propobj_t *self, fts_bytestream_t *stream)
{
  propobj_class_description_t *descr = propobj_get_descritption(self);
  int size = fts_array_get_size(&self->properties);
  fts_atom_t *atoms = fts_array_get_atoms(&self->properties);
  int i;
  
  for(i=0; i<size; i++)
  {
    if(!fts_is_void(atoms + i))
    {
      fts_spost(stream, ", %s: ", fts_symbol_name(descr->array[i].name));
      fts_spost_atoms(stream, 1, atoms + i);
    }
  }
}

fts_method_status_t
propobj_dump_properties(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  propobj_t *self = (propobj_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_class_t *cl = fts_object_get_class(o);
  propobj_class_description_t *descr = (propobj_class_description_t *)fts_object_get_context((fts_object_t *)cl);
  int size = fts_array_get_size(&self->properties);
  fts_atom_t *atoms = fts_array_get_atoms(&self->properties);
  int i;
  
  for(i=0; i<size; i++)
  {
    if(!fts_is_void(atoms + i))
      fts_dumper_send(dumper, descr->array[i].name, 1, atoms + i);
  }
  
  return fts_ok;
}

void 
propobj_copy(propobj_t *org, propobj_t *copy)
{
  int size = fts_array_get_size(&org->properties);
  fts_atom_t *atoms = fts_array_get_atoms(&org->properties);
  
  fts_array_set(&copy->properties, size, atoms);
}

void
propobj_copy_function(const fts_object_t *from, fts_object_t *to)
{
  propobj_copy((propobj_t *)from, (propobj_t *)to);
}

int
propobj_equals(const propobj_t *o, const propobj_t *p)
{
  int o_n_prop = fts_array_get_size(&o->properties);
  int p_n_prop = fts_array_get_size(&p->properties);
  int i;
  
  /* send a message for each of the optional properties */
  for(i=0; i<o_n_prop; i++)
  {
    fts_atom_t *o_prop = fts_array_get_element(&o->properties, i);
    
    if(i < p_n_prop)
    {
      if(!fts_atom_equals(o_prop, fts_array_get_element(&p->properties, i)))
        return 0;
    }
    else if(!fts_is_void(o_prop))
      return 0;
  }
  
  for(; i<p_n_prop; i++)
    if(!fts_is_void(fts_array_get_element(&p->properties, i)))
      return 0;
  
  return 1;
}

/******************************************************************************
*
*  propobj class
*
*/
#define PROPOBJ_ALLOC_BLOCK 32

void
propobj_init(fts_object_t *o)
{
  propobj_t *self = (propobj_t *)o;
  
  fts_array_init(&self->properties, 0, 0);
}

void
propobj_delete(fts_object_t *o)
{
  propobj_t *self = (propobj_t *)o;
  
  fts_array_destroy(&self->properties);
}

void
propobj_class_init(fts_class_t *cl)
{
  propobj_class_description_t *descr = fts_malloc(sizeof(propobj_class_description_t));
  
  descr->array = (propobj_property_t *)fts_malloc(PROPOBJ_ALLOC_BLOCK * sizeof(propobj_property_t));
  descr->alloc = PROPOBJ_ALLOC_BLOCK;
  fts_hashtable_init(&descr->hash, FTS_HASHTABLE_SMALL);
  descr->n_properties = 0;
  
  fts_object_set_context((fts_object_t *)cl, (fts_context_t *)descr);  
}

propobj_class_description_t *
propobj_class_get_descritption(fts_class_t *cl)
{
  fts_class_instantiate(cl);
  
  return (propobj_class_description_t *)fts_object_get_context((fts_object_t *)cl);
}

propobj_class_description_t *
propobj_get_descritption(propobj_t *self)
{
  fts_class_t *cl = fts_object_get_class((fts_object_t *)self);
  return (propobj_class_description_t *)fts_object_get_context((fts_object_t *)cl);
}

static fts_method_status_t
_default_set_method(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  propobj_t *self = (propobj_t *)o;
  
  propobj_set_property_by_name(self, s, at);
  
  return fts_ok;
}

static fts_method_status_t
_default_set_int_method(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  propobj_t *self = (propobj_t *)o;

  propobj_set_int_property_by_name(self, s, at);
  
  return fts_ok;
}

static fts_method_status_t
_default_set_float_method(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  propobj_t *self = (propobj_t *)o;

  propobj_set_float_property_by_name(self, s, at);
  
  return fts_ok;
}

static fts_method_status_t
_default_get_method(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  propobj_t *self = (propobj_t *)o;
  
  propobj_get_property_by_name(self, s, ret);
  
  return fts_ok;
}

static propobj_property_t * 
propobj_class_insert_property(fts_class_t *cl, fts_symbol_t name, fts_symbol_t type)
{
  propobj_class_description_t *descr = propobj_class_get_descritption(cl);
  int index = descr->n_properties;
  fts_atom_t k, a;
  
  fts_set_symbol(&k, name);
  fts_set_pointer(&a, descr->array + index);
  fts_hashtable_put(&descr->hash, &k, &a);
  
  if(index >= descr->alloc)
  {
    descr->alloc += PROPOBJ_ALLOC_BLOCK;
    fts_realloc(descr->array, descr->alloc * sizeof(propobj_property_t));      
  }
  
  descr->array[index].index = index;
  descr->array[index].name = name;
  descr->array[index].type = type;  
  descr->n_properties++;
  
  return descr->array + index;
}

propobj_property_t * 
propobj_class_add_int_property(fts_class_t *cl, fts_symbol_t name, fts_method_t set_method)
{
  propobj_property_t *prop = propobj_class_insert_property(cl, name, fts_s_int);
  
  if(set_method != NULL)
    fts_class_message_number(cl, name, set_method);
  else
    fts_class_message_number(cl, name, _default_set_int_method);

  fts_class_message_void(cl, name, _default_get_method);
  
  return prop;
}

propobj_property_t * 
propobj_class_add_float_property(fts_class_t *cl, fts_symbol_t name, fts_method_t set_method)
{
  propobj_property_t *prop = propobj_class_insert_property(cl, name, fts_s_float);
  
  if(set_method != NULL)
    fts_class_message_number(cl, name, set_method);
  else
    fts_class_message_number(cl, name, _default_set_float_method);
  
  fts_class_message_void(cl, name, _default_get_method);
  
  return prop;
}

propobj_property_t * 
propobj_class_add_symbol_property(fts_class_t *cl, fts_symbol_t name, fts_method_t set_method)
{
  propobj_property_t *prop = propobj_class_insert_property(cl, name, fts_s_symbol);
  
  if(set_method != NULL)
    fts_class_message_symbol(cl, name, set_method);
  else
    fts_class_message_symbol(cl, name, _default_set_method);
  
  fts_class_message_void(cl, name, _default_get_method);
  
  return prop;
}

propobj_property_t *
propobj_class_get_property_by_name(fts_class_t *cl, fts_symbol_t name)
{
  propobj_class_description_t *descr = propobj_class_get_descritption(cl);
  fts_atom_t k, v;
  
  fts_set_symbol(&k, name);
  if(fts_hashtable_get(&descr->hash, &k, &v))
    return (propobj_property_t *)fts_get_pointer(&v);
  else
    return NULL;
}

propobj_property_t *
propobj_class_get_property_by_index(fts_class_t *cl, int index)
{
  propobj_class_description_t *descr = propobj_class_get_descritption(cl);
  
  if(index >= 0 && index < descr->n_properties)
    return descr->array + index;
  else
    return NULL;
}

void
propobj_class_append_properties(fts_class_t *cl, fts_array_t *array)
{
  propobj_class_description_t *descr = propobj_class_get_descritption(cl);
  int i;
  
  for(i=0; i<descr->n_properties; i++)
  {
    fts_array_append_symbol(array, descr->array[i].name);
    fts_array_append_symbol(array, descr->array[i].type);
  }
}

/***********************************************************************
 *
 *  config
 *
 */
FTS_PACKAGE_INIT(data)
{
  sym_expr = fts_new_symbol("expr");
  expr_class = fts_class_install(sym_expr, expr_instantiate);
  
  FTS_MODULE_INIT_CALL(ivec);
  FTS_MODULE_INIT_CALL(mat);
  FTS_MODULE_INIT_CALL(fmat);
  FTS_MODULE_INIT_CALL(fvec);
  FTS_MODULE_INIT_CALL(bpf);
  FTS_MODULE_INIT_CALL(dict);
  FTS_MODULE_INIT_CALL(tree);
  FTS_MODULE_INIT_CALL(tabeditor);
  
  FTS_MODULE_INIT_CALL(getrange);  
  FTS_MODULE_INIT_CALL(dumpfile);
  
  fts_hashtable_init(&global_enumeration_table, FTS_HASHTABLE_MEDIUM);
}
