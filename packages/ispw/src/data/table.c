/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include <stdlib.h>

#include "fts.h"
#include "table.h"

/********************************************************************
 *
 *   named tables house keeping
 *
 */

/* Naming and reference counting of fts_integer_vector_t is done here, locally
   and privately for tables; tables store integer_vector in the table only
   if named, otherwise allocate directly the vecint.
   */  
static fts_hash_table_t table_integer_vector_table; /* the name binding table */

static fts_data_t *
table_integer_vector_get(fts_symbol_t name, int size)
{
  fts_atom_t atom;
  fts_data_t *data;

  if(fts_hash_table_lookup(&table_integer_vector_table, name, &atom))
    {
      data = fts_get_data(&atom);
      fts_data_refer(data);

      return data;
    }
  else
    {
      data = (fts_data_t *)fts_integer_vector_new(size);

      fts_data_refer(data);
      fts_data_set_name(data, name);

      fts_set_data(&atom, data);
      fts_hash_table_insert(&table_integer_vector_table, name, &atom);

      return data;
    }
}


static void
table_integer_vector_release(fts_symbol_t name)
{
  fts_atom_t atom;

  if (fts_hash_table_lookup(&table_integer_vector_table, name, &atom))
    {
      fts_data_t *data = fts_get_data(&atom);
      
      if(fts_data_derefer(data) == 0)
	{
	  fts_integer_vector_delete((fts_integer_vector_t *)data);
	  fts_hash_table_remove(&table_integer_vector_table, name);
	}
    }
  else
    return;
}

/* find a tables fts_integer_vector by name */
fts_integer_vector_t *
table_integer_vector_get_by_name(fts_symbol_t name)
{
  fts_atom_t atom;

  if (fts_hash_table_lookup(&table_integer_vector_table, name, &atom))
    {
      fts_integer_vector_t *vec = (fts_integer_vector_t *) fts_get_ptr(&atom);
      return vec;
    }
  else
    return 0;
}


/********************************************************************
 *
 *   utils
 *
 */

static int
quantile(fts_integer_vector_t *vector, int n)
{
  int index, i;
  int v = 0;
  int size = fts_integer_vector_get_size(vector);

  v = 0;
  for(i=0; i<size; i++)
    v += fts_integer_vector_get_element(vector, i);

  if(v == 0)
    return 0;
  else
    {
      index = ((n * v) >> 15) + 1;
      
      for (i=0; i<size; i++)
	{
	  index -= fts_integer_vector_get_element(vector, i);
	  if (index <= 0)
	    break;
	}
  
      if (i >= size)
	return (size - 1);
      else
	return i;
    }
}

/********************************************************************
 *
 *   object
 *
 */

typedef struct 
{
  fts_object_t ob;
  fts_symbol_t name;
  fts_atom_t value;
  fts_data_t *vector; /* integer or float vector */
} table_t;

static void
table_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;
  fts_symbol_t name = 0;
  int size;
  
  if(ac == 2 && fts_is_data(at + 1))
    {
      this->vector = fts_get_data(at + 1);
      fts_data_refer(this->vector);
    }
  else if(ac > 1 && fts_is_symbol(at + 1))
    {
      name = fts_get_symbol(at + 1);
      size = fts_get_int_arg(ac, at, 2, FTS_TABLE_DEFAULT_SIZE);
      
      this->vector = table_integer_vector_get(name, size);
      fts_register_named_object(o, name);
    }
  else
    {
      size = fts_get_int_arg(ac, at, 1, FTS_TABLE_DEFAULT_SIZE);
      
      this->vector = (fts_data_t *)fts_integer_vector_new(size);
      fts_data_refer(this->vector);
    }

  this->name = name;    
}

static void
table_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;

  if(this->name)
    {
      table_integer_vector_release(this->name);
      fts_unregister_named_object(o, this->name);
    }
  else
    {
      if(fts_data_derefer(this->vector) == 0)
	{
	  if(fts_data_is(this->vector, fts_s_integer_vector))
	    fts_integer_vector_delete((fts_integer_vector_t *)this->vector);
	  else
	    fts_float_vector_delete((fts_float_vector_t *)this->vector);
	}
    }
}

/*********************************************************************
 *
 *  client methods
 *
 */

/* Daemon for getting the property "data".
   Note that we return a pointer to the data; 
   if the request come from the client, it will be the
   kernel to handle the export of the data, not the table
   object.
 */
static void
table_get_data(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  table_t *this = (table_t *)obj;

  fts_set_data(value, (fts_data_t *)this->vector);
}

/********************************************************************
 *
 *   user methods
 *
 */

static void
table_store_value(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;

  this->value = at[0];
}

/********************************************************************
 *
 *   integer table user methods
 *
 */

/* set and get single elements */
static void
table_int_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;
  fts_integer_vector_t *vec = (fts_integer_vector_t *)this->vector;
  int index = fts_get_int_arg(ac, at, 0, 0);
  int size = fts_integer_vector_get_size((fts_integer_vector_t *)this->vector);

  if (index < 0)
    index = 0;
  else if (index >= size)
    index = size - 1;

  if(fts_is_int(&this->value))
     fts_integer_vector_set_element(vec, index, fts_get_int(&this->value));
  if(fts_is_float(&this->value))
     fts_integer_vector_set_element(vec, index, (int)fts_get_float(&this->value));
  else
    fts_outlet_int(o, 0, fts_integer_vector_get_element(vec, index));

  fts_set_void(&this->value);
}


static void
table_int_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if ((ac >= 2) && (fts_is_number(&at[1])))
    table_store_value(o, winlet, s, 1, at + 1);

  if ((ac >= 1) && (fts_is_number(&at[0])))
    table_int_index(o, winlet, s, 1, at + 0);
}

/* set by atom list */
static void
table_int_set_from_atom_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_integer_vector_t *vec = (fts_integer_vector_t *)((table_t *)o)->vector;
  int offset;

  offset = fts_get_int_arg(ac, at, 0, 0);

  if (ac > 1)
    fts_integer_vector_set_from_atom_list(vec, offset, ac - 1, at + 1);
}

/* set by float vector */
static void
table_int_set_from_float_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_integer_vector_t *ivec = (fts_integer_vector_t *)((table_t *)o)->vector;
  fts_float_vector_t *fvec = (fts_float_vector_t *)fts_get_data(at);
  int iv_size = fts_integer_vector_get_size(ivec);
  int fv_size = fts_float_vector_get_size(fvec);
  int i, n;

  if(iv_size > fv_size)
    n = fv_size;
  else
    n = iv_size;

  for(i=0; i<n; i++)
    {
      int value = (int)(fts_float_vector_get_element(fvec, i) + 0.5);
      fts_integer_vector_set_element(ivec, i, value);
    }
}

static void
table_int_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_integer_vector_t *vec = (fts_integer_vector_t *)((table_t *)o)->vector;

  fts_integer_vector_set_const(vec, 0);
}

static void
table_int_const(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_integer_vector_t *vec = (fts_integer_vector_t *)((table_t *)o)->vector;
  int constant = fts_get_int(&at[0]);

  fts_integer_vector_set_const(vec, constant);
}

static void
table_int_inv(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_integer_vector_t *vec = (fts_integer_vector_t *)((table_t *)o)->vector;
  int value = fts_get_int(at);
  int size = fts_integer_vector_get_size(vec);
  int i;

  for(i=0; i<size; i++)
    if (fts_integer_vector_get_element(vec, i) > value)
      break;

  if(i == 0 || i == size)
    fts_outlet_int(o, 0, 0);
  else
    fts_outlet_int(o, 0, i - 1);
}

static void
table_int_get_random(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_integer_vector_t *vec = (fts_integer_vector_t *)((table_t *)o)->vector;

  fts_outlet_int(o, 0, quantile(vec, rand() & 0x7fff));
}

static void
table_int_quantile(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_integer_vector_t *vec = (fts_integer_vector_t *)((table_t *)o)->vector;
  int n = fts_get_int(at);

  fts_outlet_int(o, 0, quantile(vec, n));
}

static void
table_int_sum(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_integer_vector_t *vec = (fts_integer_vector_t *)((table_t *)o)->vector;

  fts_outlet_int(o, 0, fts_integer_vector_get_sum(vec));
}

static void
table_int_resize(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_integer_vector_t *vec = (fts_integer_vector_t *)((table_t *)o)->vector;
  int size = fts_get_int(&at[0]);

  fts_integer_vector_set_size(vec, size);
}

static void
table_int_save_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_integer_vector_t *vec = (fts_integer_vector_t *)((table_t *)o)->vector;
  fts_bmax_file_t *f = (fts_bmax_file_t *) fts_get_ptr(at);
  
  fts_integer_vector_save_bmax(vec, f);
}

/********************************************************************
 *
 *   float table user methods
 *
 */

static void
table_float_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;
  fts_float_vector_t *vec = (fts_float_vector_t *)this->vector;
  int index = fts_get_int_arg(ac, at, 0, 0);
  int size = fts_float_vector_get_size(vec);

  if (index < 0)
    index = 0;
  else if (index >= size)
    index = size - 1;

  if(fts_is_float(&this->value))
    fts_float_vector_set_element(vec, index, fts_get_float(&this->value));
  if(fts_is_int(&this->value))
    fts_float_vector_set_element(vec, index, (float)fts_get_int(&this->value));
  else
    fts_outlet_float(o, 0, fts_float_vector_get_element(vec, index));

  fts_set_void(&this->value);
}


static void
table_float_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if ((ac >= 2) && (fts_is_number(&at[1])))
    table_store_value(o, winlet, s, 1, at + 1);

  if ((ac >= 1) && (fts_is_number(&at[0])))
    table_float_index(o, winlet, s, 1, at + 0);
}

static void
table_float_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_float_vector_t *vec = (fts_float_vector_t *)((table_t *)o)->vector;
  int offset;

  offset = fts_get_int_arg(ac, at, 0, 0);

  if (ac > 1)
    fts_float_vector_set_from_atom_list(vec, offset, ac - 1, at + 1);
}

static void
table_float_inv(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_float_vector_t *vec = (fts_float_vector_t *)((table_t *)o)->vector;

  float value = fts_get_float(at);
  int size = fts_float_vector_get_size(vec);
  int i;

  for(i=0; i<size; i++)
    if (fts_float_vector_get_element(vec, i) >= value)
      break;

  if(i < 1 || i >= size)
    fts_outlet_float(o, 0, 0.0f);
  else
    {
      float f2 = fts_float_vector_get_element(vec, i);
      float f1 = fts_float_vector_get_element(vec, i - 1);
      
      /* output linear interploation between surounding points */
      fts_outlet_float(o, 0, (float)i + (value - f1) / (f2 - f1));
    }
}

static void
table_float_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_float_vector_t *vec = (fts_float_vector_t *)((table_t *)o)->vector;

  fts_float_vector_set_const(vec, 0.0f);
}

static void
table_float_const(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_float_vector_t *vec = (fts_float_vector_t *)((table_t *)o)->vector;
  int constant = fts_get_float(&at[0]);

  fts_float_vector_set_const(vec, constant);
}

static void
table_float_sum(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_float_vector_t *vec = (fts_float_vector_t *)((table_t *)o)->vector;

  fts_outlet_float(o, 0, fts_float_vector_get_sum(vec));
}

static void
table_float_resize(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_float_vector_t *vec = (fts_float_vector_t *)((table_t *)o)->vector;
  int size = fts_get_int(&at[0]);

  fts_float_vector_set_size(vec, size);
}

static void
table_float_save_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_float_vector_t *vec = (fts_float_vector_t *)((table_t *)o)->vector;
  fts_bmax_file_t *f = (fts_bmax_file_t *) fts_get_ptr(at);
  
  fts_float_vector_save_bmax(vec, f);
}

/********************************************************************
 *
 *   class
 *
 */

static int
table_is_integer(int ac, const fts_atom_t *at)
{
  if(ac > 1)
    {
      if(fts_is_data(at + 1))
	{
	  fts_data_t *data = fts_get_data(at + 1); 
	  if(fts_data_get_class_name(data) == fts_s_integer_vector)
	    return 1;
	  else
	    return 0;
	}
      else
	return 1;
    }
  else
    return 1;
}
    
static int
table_is_const(int ac, const fts_atom_t *at)
{
  if(ac > 1)
    {
      if(fts_is_data(at + 1))
	{
	  fts_data_t *data = fts_get_data(at + 1); 
	  if(fts_data_is_const(data))
	    return 1;
	  else
	    return 0;
	}
      else
	return 0;
    }
  else
    return 0;
}
    
static void
table_int_define_methods(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  if(!table_is_const(ac, at))
    {
      /* the method set is also installed on the system inlet, and is used for .bmax loading */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, table_int_set_from_atom_list);
      
      /* save to bmax file */
      a[0] = fts_s_ptr;
      fts_method_define(cl, fts_SystemInlet, fts_s_save_bmax, table_int_save_bmax, 1, a);

      fts_method_define_varargs(cl, 0, fts_s_list, table_int_list);
      fts_method_define_varargs(cl, 0, fts_s_set, table_int_set_from_atom_list);
      a[0] = fts_s_data;
      fts_method_define_varargs(cl, 0, fts_s_float_vector, table_int_set_from_float_vector);
      fts_method_define(cl, 0, fts_new_symbol("const"), table_int_const, 1, a);
      fts_method_define(cl, 0, fts_s_clear, table_int_clear, 0, 0);
    }
  
  fts_method_define_number(cl, 0, table_int_index);
  
  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_new_symbol("inv"), table_int_inv, 1, a);
  fts_method_define(cl, 0, fts_new_symbol("quantile"), table_int_quantile, 1,  a);
  fts_method_define(cl, 0, fts_s_bang, table_int_get_random, 0, 0);

  fts_method_define(cl, 0, fts_new_symbol("sum"), table_int_sum, 0, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_new_symbol("size"), table_int_resize, 1, a);
}

static void
table_float_define_methods(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  if(!table_is_const(ac, at))
    {
      /* the method set is also installed on the system inlet, and is used for .bmax loading */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, table_float_set);
      
      /* save to bmax file */
      a[0] = fts_s_ptr;
      fts_method_define(cl, fts_SystemInlet, fts_s_save_bmax, table_float_save_bmax, 1, a);

      fts_method_define_varargs(cl, 0, fts_s_list, table_float_list);
      fts_method_define_varargs(cl, 0, fts_s_set, table_float_set);
      fts_method_define(cl, 0, fts_new_symbol("const"), table_float_const, 1, a);
      fts_method_define(cl, 0, fts_s_clear, table_float_clear, 0, 0);
    }

  fts_method_define_number(cl, 0, table_float_index);
  fts_method_define(cl, 0, fts_new_symbol("sum"), table_float_sum, 0, 0);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_new_symbol("inv"), table_float_inv, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_new_symbol("size"), table_float_resize, 1, a);
}

static fts_status_t
table_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  if(table_is_const(ac, at))
    {
      fts_class_init(cl, sizeof(table_t), 1, 1, 0);  
    }
  else
    {
      fts_class_init(cl, sizeof(table_t), 2, 1, 0);

      fts_method_define_number(cl, 1, table_store_value);
      
      /* variable definition */
      fts_class_add_daemon(cl, obj_property_get, fts_s_data, table_get_data);
    }  

  /* system methods */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, table_init);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, table_delete, 0, 0);

  if(table_is_integer(ac, at))
    {
      table_int_define_methods(cl, ac, at);
      fts_outlet_type_define(cl, 0, fts_s_int, 1, a);
    }
  else
    {
      table_int_define_methods(cl, ac, at);
      fts_outlet_type_define(cl, 0, fts_s_float, 1, a);
    }
    
  return fts_Success;
}

static int
table_class_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  return (
	  table_is_integer(ac0, at0) == table_is_integer(ac1, at1) &&
	  table_is_const(ac0, at0) == table_is_const(ac1, at1)
	  );
}

void
table_config(void)
{
  fts_hash_table_init(&table_integer_vector_table);
  fts_metaclass_create(fts_new_symbol("table"), table_instantiate, table_class_equiv);
}
