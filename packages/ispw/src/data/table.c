/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
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
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include <stdlib.h>

#include "fts.h"
#include "table.h"

/*******************************************************************
 *
 *   named integer vector
 *
 */

static fts_symbol_t sym_niv = 0;

typedef struct 
{
  fts_object_t ob;
  fts_symbol_t name;
  int_vector_t *vec;
} niv_t;

static void
niv_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  niv_t *this = (niv_t *)o;

  this->name = fts_get_symbol(at + 1);
  this->vec = int_vector_atom_get(at + 2);

  fts_register_named_object(o, this->name);
  int_vector_set_creator(this->vec, o);
}

static void
niv_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  niv_t *this = (niv_t *)o;

  fts_unregister_named_object(o, this->name);
  int_vector_set_creator(this->vec, 0);
}

/* set by atom list */
static void
niv_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int_vector_t *vec = ((niv_t *)o)->vec;
  int offset;

  offset = fts_get_int_arg(ac, at, 0, 0);

  if (ac > 1)
    int_vector_set_from_atom_list(vec, offset, ac - 1, at + 1);
}

static fts_status_t
niv_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(niv_t), 0, 0, 0);
      
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, niv_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, niv_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, niv_set);

  return fts_Success;
}

/* find a tables int_vector by name */
int_vector_t *
table_int_vector_get_by_name(fts_symbol_t name)
{
  fts_object_t *obj = fts_get_object_by_name(name);

  if(obj && (fts_object_get_class_name(obj) == sym_niv))
    return ((niv_t *)obj)->vec;
  else
    return 0;
}

/********************************************************************
 *
 *   table object
 *
 */

typedef struct 
{
  fts_object_t ob;
  fts_atom_t value;
  int_vector_t *vector; /* integer vector */
  fts_symbol_t name;
} table_t;

static void
table_init_define(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;
  int size = fts_get_int_arg(ac, at, 1, FTS_TABLE_DEFAULT_SIZE);
  
  this->vector = int_vector_new(size);
  int_vector_refer(this->vector);
  int_vector_set_creator(this->vector, o);
}

static void
table_init_refer(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;
  int size;
  
  this->vector = int_vector_atom_get(at + 1);
  int_vector_refer(this->vector);
}

static void
table_init_symbol(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;
  fts_symbol_t name = fts_get_symbol(at + 1);
  int size = fts_get_int_arg(ac, at, 2, FTS_TABLE_DEFAULT_SIZE);
  int_vector_t *vec = table_int_vector_get_by_name(name);

  if(vec)
    {
      /* refer to existing vector */
      int_vector_refer(vec);

      if(size > int_vector_get_size(vec))
	int_vector_set_size(vec, size);
    }
  else
    {
      fts_atom_t a[3];
      fts_object_t *obj;

      /* new vector */
      vec = int_vector_new(size);
      int_vector_refer(vec);

      /* new niv */
      fts_set_symbol(a, sym_niv);
      fts_set_symbol(a + 1, name);
      int_vector_atom_set(a + 2, vec);
      fts_object_new(0, 3, a, &obj);
    }

  this->vector = vec;
  this->name = name;
}

static void
table_delete_define(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;

  int_vector_release(this->vector);
}

static void
table_delete_refer(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;

  int_vector_release(this->vector);
}

static void
table_delete_symbol(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;
  int_vector_t *vec = this->vector;
  fts_symbol_t name = this->name;

  if(vec && vec->refdata.cnt == 1)
    {       
      fts_object_t *obj = fts_get_object_by_name(name);
      
      if(obj && (fts_object_get_class_name(obj) == sym_niv))
	fts_object_delete(obj);
    }

  int_vector_release(vec);
}

/*********************************************************************
 *
 *  client methods
 *
 */

static void
table_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  table_t *this = (table_t *)obj;

  int_vector_atom_set(value, this->vector);  
}

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

static int quantile(int_vector_t *vector, int n);

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
  int_vector_t *vec = this->vector;
  int index = fts_get_int_arg(ac, at, 0, 0);
  int size = int_vector_get_size(this->vector);

  if (index < 0)
    index = 0;
  else if (index >= size)
    index = size - 1;

  if(fts_is_number(&this->value))
     int_vector_set_element(vec, index, fts_get_number_int(&this->value));
  else
    fts_outlet_int(o, 0, int_vector_get_element(vec, index));

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
  int_vector_t *vec = ((table_t *)o)->vector;
  int offset;

  offset = fts_get_int_arg(ac, at, 0, 0);

  if (ac > 1)
    int_vector_set_from_atom_list(vec, offset, ac - 1, at + 1);
}

static void
table_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int_vector_t *vec = ((table_t *)o)->vector;

  int_vector_set_const(vec, 0);
}

static void
table_const(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int_vector_t *vec = ((table_t *)o)->vector;
  int constant = fts_get_int(&at[0]);

  int_vector_set_const(vec, constant);
}

static void
table_inv(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int_vector_t *vec = ((table_t *)o)->vector;
  int value = fts_get_int(at);
  int size = int_vector_get_size(vec);
  int i;

  for(i=0; i<size; i++)
    if (int_vector_get_element(vec, i) > value)
      break;

  if(i == 0 || i == size)
    fts_outlet_int(o, 0, 0);
  else
    fts_outlet_int(o, 0, i - 1);
}

static void
table_get_random(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int_vector_t *vec = ((table_t *)o)->vector;

  fts_outlet_int(o, 0, quantile(vec, rand() & 0x7fff));
}

static void
table_quantile(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int_vector_t *vec = ((table_t *)o)->vector;
  int n = fts_get_int(at);

  fts_outlet_int(o, 0, quantile(vec, n));
}

static void
table_sum(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int_vector_t *vec = ((table_t *)o)->vector;

  fts_outlet_int(o, 0, int_vector_get_sum(vec));
}

static void
table_resize(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int_vector_t *vec = ((table_t *)o)->vector;
  int size = fts_get_int(&at[0]);

  int_vector_set_size(vec, size);
}

static void
table_save_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int_vector_t *vec = ((table_t *)o)->vector;
  fts_bmax_file_t *f = (fts_bmax_file_t *)fts_get_ptr(at);
  
  int_vector_save_bmax(vec, f);
}

static int get_int_property( fts_object_t *object, fts_symbol_t property_name, int def)
{
  fts_atom_t a;

  fts_object_get_prop( object, property_name, &a);

  if ( !fts_is_void( &a))
    return fts_get_int( &a);

  return def;
}

#define MAX_ATOMS_PER_LINE 16

static void table_save_dotpat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *) o;
  FILE *file;
  fts_atom_t a;
  int size, x_left, y_top, x_right, y_bottom, flags, range;
  int x, y, w, font_index;
  int i, count;
  int_vector_t *vec;

  file = (FILE *)fts_get_ptr( at);

  size = int_vector_get_size( this->vector);

  x_left = get_int_property( o, fts_s_wx, 100);
  y_top = get_int_property( o, fts_s_wy, 100);
  x_right = x_left + get_int_property( o, fts_s_ww, 250);
  y_bottom = y_top + get_int_property( o, fts_s_wh, 250);

  flags = 16;
  range = 128;

  fprintf( file, "#N vtable %d %d %d %d %d %d %d", size, x_left, y_top, x_right, y_bottom, flags, range);

  if ( this->name)
    fprintf( file, " %s", fts_symbol_name( this->name)); 

  fprintf( file, ";\n");

  vec = this->vector;
  count = 0;

  for( i = 0; i < size; i++)
    {
      if ( count == 0)
	fprintf( file, "#T set %4d", i);
	   
      fprintf( file, " %4d", int_vector_get_element(vec, i));

      count++;
      if ( count >= MAX_ATOMS_PER_LINE)
	{
	  fprintf( file, ";\n");
	  count = 0;
	}
    }

  if ( count != 0)
    fprintf( file, ";\n");

  fts_object_get_prop( o, fts_s_x, &a);
  x = fts_get_int( &a);
  fts_object_get_prop( o, fts_s_y, &a);
  y = fts_get_int( &a);
  fts_object_get_prop( o, fts_s_width, &a);
  w = fts_get_int( &a);
  font_index = 1;

  fprintf( file, "#P newobj %d %d %d %d table", x, y, w, font_index);

  if ( this->name)
    fprintf( file, " %s", fts_symbol_name( this->name)); 

  fprintf( file, ";\n");
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
  
  if(ac == 1 || (ac == 2 && fts_is_int(at + 1)))
    {
      /* [var :] table [int] */
      fts_class_init(cl, sizeof(table_t), 2, 1, 0);
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, table_init_define);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, table_delete_define);

      /* save/load bmax file if not instantiated with reference */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_save_bmax, table_save_bmax);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, table_set_from_atom_list);

      a[0] = fts_s_ptr;
      fts_method_define( cl, fts_SystemInlet, fts_s_save_dotpat, table_save_dotpat, 1, a); 

      /* define variable */
      fts_class_add_daemon(cl, obj_property_get, fts_s_state, table_get_state);
    }
  else if(ac == 2 && int_vector_atom_is(at + 1))
    {
      /* table <int vector> */
      fts_class_init(cl, sizeof(table_t), 2, 1, 0);
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, table_init_refer);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, table_delete_refer);
    }
  else if((ac == 2 && fts_is_symbol(at + 1)) || (ac == 3 && fts_is_symbol(at + 1) && fts_is_number(at + 2)))
    {
      /* table <symbol> [int] */
      fts_class_init(cl, sizeof(table_t), 2, 1, 0);
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, table_init_symbol);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, table_delete_symbol);

      /* save/load bmax file if not instantiated with reference */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_save_bmax, table_save_bmax);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, table_set_from_atom_list);

      a[0] = fts_s_ptr;
      fts_method_define( cl, fts_SystemInlet, fts_s_save_dotpat, table_save_dotpat, 1, a); 
    }
  else
    return &fts_CannotInstantiate;

  /* get data for editor */
  fts_class_add_daemon(cl, obj_property_get, fts_s_data, table_get_data);

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
  sym_niv = fts_new_symbol("niv");

  fts_class_install(sym_niv, niv_instantiate);
  fts_metaclass_install(fts_new_symbol("table"), table_instantiate, fts_arg_type_equiv);
}

/********************************************************************
 *
 *   utils
 *
 */

static int
quantile(int_vector_t *vector, int n)
{
  int index, i;
  int v = 0;
  int size = int_vector_get_size(vector);

  v = 0;
  for(i=0; i<size; i++)
    v += int_vector_get_element(vector, i);

  if(v == 0)
    return 0;
  else
    {
      index = ((n * v) >> 15) + 1;
      
      for (i=0; i<size; i++)
	{
	  index -= int_vector_get_element(vector, i);
	  if (index <= 0)
	    break;
	}
  
      if (i >= size)
	return (size - 1);
      else
	return i;
    }
}
