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

#include <fts/fts.h>
#include "ivec.h"
#include "naming.h"

#define FTS_TABLE_DEFAULT_SIZE 128

typedef struct 
{
  fts_object_t ob;
  fts_atom_t value;
  ivec_t *vec; /* integer vector */
  fts_symbol_t name;
} table_t;



static void
table_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 1, 0);
  fts_atom_t a[1];
  fts_object_t *obj;
  int size;

  if(name)
    {
      obj = ispw_get_object_by_name(name);
      size = fts_get_int_arg(ac, at, 2, FTS_TABLE_DEFAULT_SIZE);

      if(obj && (fts_object_get_class_name(obj) == ivec_symbol))
	{
	  /* refer to existing vector */
	  fts_object_refer(obj);
	  
	  if(size > ivec_get_size((ivec_t *)obj))
	    ivec_set_size((ivec_t *)obj, size);

	  this->vec = (ivec_t *)obj;
	  this->name = name;
  
	  return;
	}
    }
  else
    size = fts_get_int_arg(ac, at, 1, FTS_TABLE_DEFAULT_SIZE);          
  
  /* new table */
  fts_set_int(a, size);
  obj = fts_object_create(ivec_class, 1, a);
  
  fts_object_refer(obj);

  if(name)
    ispw_register_named_object(obj, name);

  this->vec = (ivec_t *)obj;
  this->name = name;
}

static void
table_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  table_t *this = (table_t *)o;

  if(this->name && fts_object_has_only_one_reference((fts_object_t *)this->vec))
    ispw_unregister_named_object((fts_object_t *)this->vec, this->name);

  fts_object_release((fts_object_t *)this->vec);
}

/********************************************************************
 *
 *   user methods
 *
 */

static int quantile(ivec_t *vec, int n);

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
  ivec_t *vec = this->vec;
  int index = fts_get_int_arg(ac, at, 0, 0);
  int size = ivec_get_size(this->vec);

  if (index < 0)
    index = 0;
  else if (index >= size)
    index = size - 1;

  if(fts_is_number(&this->value))
     ivec_set_element(vec, index, fts_get_number_int(&this->value));
  else
    fts_outlet_int(o, 0, ivec_get_element(vec, index));

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
  ivec_t *vec = ((table_t *)o)->vec;
  int offset;

  offset = fts_get_int_arg(ac, at, 0, 0);

  if (ac > 1)
    ivec_set_from_atom_list(vec, offset, ac - 1, at + 1);
}

static void
table_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *vec = ((table_t *)o)->vec;

  ivec_set_const(vec, 0);
}

static void
table_const(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *vec = ((table_t *)o)->vec;

  if(fts_is_number(at))
    {
      int constant = fts_get_int(at);
      
      ivec_set_const(vec, constant);
    }
}

static void
table_inv(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *vec = ((table_t *)o)->vec;
  int value = fts_get_int(at);
  int size = ivec_get_size(vec);
  int i;

  for(i=0; i<size; i++)
    if (ivec_get_element(vec, i) > value)
      break;

  if(i == 0 || i == size)
    fts_outlet_int(o, 0, 0);
  else
    fts_outlet_int(o, 0, i - 1);
}

static void
table_get_random(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *vec = ((table_t *)o)->vec;

  fts_outlet_int(o, 0, quantile(vec, rand() & 0x7fff));
}

static void
table_quantile(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *vec = ((table_t *)o)->vec;
  int n = fts_get_int(at);

  fts_outlet_int(o, 0, quantile(vec, n));
}

static void
table_sum(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *vec = ((table_t *)o)->vec;

  fts_outlet_int(o, 0, ivec_get_sum(vec));
}

static void
table_resize(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *vec = ((table_t *)o)->vec;
  int size = fts_get_int(&at[0]);

  ivec_set_size(vec, size);
}

/*********************************************************************
 *
 *  system methods
 *
 */

static void
table_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  table_t *this = (table_t *)obj;

  ivec_atom_set(value, this->vec);  
}

static void
table_save_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *vec = ((table_t *)o)->vec;
  fts_bmax_file_t *f = (fts_bmax_file_t *)fts_get_ptr(at);
  
  ivec_save_bmax(vec, f);
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
  ivec_t *vec;

  file = (FILE *)fts_get_ptr( at);

  size = ivec_get_size( this->vec);

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

  vec = this->vec;
  count = 0;

  for( i = 0; i < size; i++)
    {
      if ( count == 0)
	fprintf( file, "#T set %4d", i);
	   
      fprintf( file, " %4d", ivec_get_element(vec, i));

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
  
  if(ac == 1 || (ac == 2 && (fts_is_int(at + 1) || fts_is_symbol(at + 1))) || (ac == 3 && fts_is_symbol(at + 1) && fts_is_number(at + 2)))
    {
      /* [var :] table [int] */
      fts_class_init(cl, sizeof(table_t), 2, 1, 0);
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, table_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, table_delete);

      /* save/load bmax file if not instantiated with reference */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_save_bmax, table_save_bmax);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, table_set_from_atom_list);

      fts_method_define_varargs( cl, fts_SystemInlet, fts_s_save_dotpat, table_save_dotpat); 

      /* user methods */
      fts_method_define_number(cl, 0, table_index);
      fts_method_define_number(cl, 1, table_store_value);
      
      fts_method_define_varargs(cl, 0, fts_s_list, table_list);
      fts_method_define_varargs(cl, 0, fts_s_set, table_set_from_atom_list);
      
      fts_method_define_varargs(cl, 0, fts_new_symbol("const"), table_const);
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
  else
    return &fts_CannotInstantiate;
}

void
table_config(void)
{
  fts_metaclass_install(fts_new_symbol("table"), table_instantiate, fts_arg_type_equiv);
}

/********************************************************************
 *
 *   utils
 *
 */

static int
quantile(ivec_t *vec, int n)
{
  int index, i;
  int v = 0;
  int size = ivec_get_size(vec);

  v = 0;
  for(i=0; i<size; i++)
    v += ivec_get_element(vec, i);

  if(v == 0)
    return 0;
  else
    {
      index = ((n * v) >> 15) + 1;
      
      for (i=0; i<size; i++)
	{
	  index -= ivec_get_element(vec, i);
	  if (index <= 0)
	    break;
	}
  
      if (i >= size)
	return (size - 1);
      else
	return i;
    }
}
