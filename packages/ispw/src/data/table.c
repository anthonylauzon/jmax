/*
 * jMax
 * Copyright (C) 1999 by IRCAM
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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
      int stored_size;

      data = fts_get_data(&atom);
      fts_data_refer(data);

      stored_size = fts_integer_vector_get_size((fts_integer_vector_t *)data);

      if(size != stored_size)
	{
	  post("table %s: size mismatch (fit to larger)\n", fts_symbol_name(name));
	  
	  if(size > stored_size)
	    fts_integer_vector_set_size((fts_integer_vector_t *)data, size);
	}

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
  fts_data_t *vector; /* integer vector */
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
	fts_integer_vector_delete((fts_integer_vector_t *)this->vector);
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
table_get_integer_vector(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
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

/* set and get single elements */
static void
table_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;
  fts_integer_vector_t *vec = (fts_integer_vector_t *)this->vector;
  int index = fts_get_int_arg(ac, at, 0, 0);
  int size = fts_integer_vector_get_size((fts_integer_vector_t *)this->vector);

  if (index < 0)
    index = 0;
  else if (index >= size)
    index = size - 1;

  if(fts_is_number(&this->value))
     fts_integer_vector_set_element(vec, index, fts_get_number_int(&this->value));
  else
    fts_outlet_int(o, 0, fts_integer_vector_get_element(vec, index));

  fts_set_void(&this->value);
}


static void
table_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if ((ac >= 2) && (fts_is_number(&at[1])))
    table_store_value(o, winlet, s, 1, at + 1);

  if ((ac >= 1) && (fts_is_number(&at[0])))
    table_index(o, winlet, s, 1, at + 0);
}

/* set by atom list */
static void
table_set_from_atom_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_integer_vector_t *vec = (fts_integer_vector_t *)((table_t *)o)->vector;
  int offset;

  offset = fts_get_int_arg(ac, at, 0, 0);

  if (ac > 1)
    fts_integer_vector_set_from_atom_list(vec, offset, ac - 1, at + 1);
}

static void
table_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_integer_vector_t *vec = (fts_integer_vector_t *)((table_t *)o)->vector;

  fts_integer_vector_set_const(vec, 0);
}

static void
table_const(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_integer_vector_t *vec = (fts_integer_vector_t *)((table_t *)o)->vector;
  int constant = fts_get_int(&at[0]);

  fts_integer_vector_set_const(vec, constant);
}

static void
table_inv(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
table_get_random(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_integer_vector_t *vec = (fts_integer_vector_t *)((table_t *)o)->vector;

  fts_outlet_int(o, 0, quantile(vec, rand() & 0x7fff));
}

static void
table_quantile(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_integer_vector_t *vec = (fts_integer_vector_t *)((table_t *)o)->vector;
  int n = fts_get_int(at);

  fts_outlet_int(o, 0, quantile(vec, n));
}

static void
table_sum(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_integer_vector_t *vec = (fts_integer_vector_t *)((table_t *)o)->vector;

  fts_outlet_int(o, 0, fts_integer_vector_get_sum(vec));
}

static void
table_resize(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_integer_vector_t *vec = (fts_integer_vector_t *)((table_t *)o)->vector;
  int size = fts_get_int(&at[0]);

  fts_integer_vector_set_size(vec, size);
}

static void
table_save_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_integer_vector_t *vec = (fts_integer_vector_t *)((table_t *)o)->vector;
  fts_bmax_file_t *f = (fts_bmax_file_t *) fts_get_ptr(at);
  
  fts_integer_vector_save_bmax(vec, f);
}

/********************************************************************
 *
 *   class
 *
 */

static fts_status_t
table_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  fts_class_init(cl, sizeof(table_t), 2, 1, 0);
  
  /* system methods */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, table_init);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, table_delete, 0, 0);

  /* the method set is also installed on the system inlet, and is used for .bmax loading */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, table_set_from_atom_list);
  
  /* get data for editor */
  fts_class_add_daemon(cl, obj_property_get, fts_s_data, table_get_integer_vector);

  if(ac > 1 && !fts_is_data(at + 1))
    {
      /* save to bmax file if not instantiated with reference */
      a[0] = fts_s_ptr;
      fts_method_define(cl, fts_SystemInlet, fts_s_save_bmax, table_save_bmax, 1, a);

      /* define variable if not instantiated with reference and without name */
      if(!fts_is_symbol(at + 1))
	fts_class_add_daemon(cl, obj_property_get, fts_s_state, table_get_integer_vector);
    }

  if(ac > 1 && fts_is_data(at + 1) && !fts_data_is(fts_get_data(at + 1), fts_s_integer_vector))
    return &fts_CannotInstantiate;

  /* user methods */
  fts_method_define_number(cl, 0, table_index);
  fts_method_define_number(cl, 1, table_store_value);

  fts_method_define_varargs(cl, 0, fts_s_list, table_list);
  fts_method_define_varargs(cl, 0, fts_s_set, table_set_from_atom_list);

  fts_method_define(cl, 0, fts_new_symbol("const"), table_const, 1, a);
  fts_method_define(cl, 0, fts_s_clear, table_clear, 0, 0);
  
  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_new_symbol("inv"), table_inv, 1, a);
  fts_method_define(cl, 0, fts_new_symbol("quantile"), table_quantile, 1,  a);
  fts_method_define(cl, 0, fts_s_bang, table_get_random, 0, 0);

  fts_method_define(cl, 0, fts_new_symbol("sum"), table_sum, 0, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_new_symbol("size"), table_resize, 1, a);

  fts_outlet_type_define(cl, 0, fts_s_int, 1, a);

  return fts_Success;
}

void
table_config(void)
{
  fts_hash_table_init(&table_integer_vector_table);

  fts_metaclass_install(fts_new_symbol("table"), table_instantiate, fts_arg_type_equiv);
}
