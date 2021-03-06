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

#include <stdlib.h>

#include <fts/fts.h>
#include <fts/packages/data/data.h>
#include "naming.h"

#define FTS_TABLE_DEFAULT_SIZE 128

typedef struct 
{
  fts_object_t ob;
  fts_atom_t value;
  ivec_t *vec; /* integer vector */
  fts_symbol_t name;
} table_t;

/********************************************************************
 *
 *   user methods
 *
 */

static int quantile(ivec_t *vec, int n);

static void
table_store_value(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  table_t *this = (table_t *)o;

  this->value = at[0];
}

/* set and get single elements */
static void
table_index(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
table_varargs(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if ((ac >= 2) && (fts_is_number(&at[1])))
    table_store_value(o, s, 1, at + 1, fts_nix);

  if ((ac >= 1) && (fts_is_number(&at[0])))
    table_index(o, s, 1, at + 0, fts_nix);
}

/* set by atom list */
static void
table_set_with_onset_from_atoms(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *vec = ((table_t *)o)->vec;
  int offset;

  offset = fts_get_int_arg(ac, at, 0, 0);

  if (ac > 1)
    ivec_set_with_onset_from_atoms(vec, offset, ac - 1, at + 1);
}

static void
table_clear(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *vec = ((table_t *)o)->vec;

  ivec_set_const(vec, 0);
}

static void
table_const(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *vec = ((table_t *)o)->vec;

  if(fts_is_number(at))
    {
      int constant = fts_get_int(at);
      
      ivec_set_const(vec, constant);
    }
}

static void
table_inv(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
table_get_random(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *vec = ((table_t *)o)->vec;

  fts_outlet_int(o, 0, quantile(vec, rand() & 0x7fff));
}

static void
table_quantile(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *vec = ((table_t *)o)->vec;
  int n = fts_get_int(at);

  fts_outlet_int(o, 0, quantile(vec, n));
}

static void
table_sum(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *vec = ((table_t *)o)->vec;

  fts_outlet_int(o, 0, ivec_get_sum(vec));
}

static void
table_size(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  ivec_t *vec = ((table_t *)o)->vec;

  fts_send_message((fts_object_t *)vec, fts_s_size, ac, at, fts_nix);
}

/*********************************************************************
 *
 *  system methods
 *
 */

static void
table_dump_state(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  table_t *this = (table_t *)o;
  
  fts_send_message_varargs((fts_object_t *)this->vec, fts_s_dump_state, ac, at);
}

static int 
get_int_property( fts_object_t *object, fts_symbol_t property_name, int def)
{
  fts_atom_t a;

  fts_object_get_prop( object, property_name, &a);

  if ( !fts_is_void( &a))
    return fts_get_int( &a);

  return def;
}

#define MAX_ATOMS_PER_LINE 16

static void table_save_dotpat(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  table_t *this = (table_t *) o;
  FILE *file;
  fts_atom_t a;
  int size, x_left, y_top, x_right, y_bottom, flags, range;
  int x, y, w, font_index;
  int i, count;
  ivec_t *vec;

  file = (FILE *)fts_get_pointer( at);

  size = ivec_get_size( this->vec);

  x_left = get_int_property( o, fts_s_wx, 100);
  y_top = get_int_property( o, fts_s_wy, 100);
  x_right = x_left + get_int_property( o, fts_s_ww, 250);
  y_bottom = y_top + get_int_property( o, fts_s_wh, 250);

  flags = 16;
  range = 128;

  fprintf( file, "#N vtable %d %d %d %d %d %d %d", size, x_left, y_top, x_right, y_bottom, flags, range);

  if ( this->name)
    fprintf( file, " %s", this->name); 

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
    fprintf( file, " %s", this->name); 

  fprintf( file, ";\n");
}

/********************************************************************
 *
 *   class
 *
 */

static void
table_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  table_t *this = (table_t *)o;
  fts_atom_t a;
  int size = FTS_TABLE_DEFAULT_SIZE;
  
  this->name = NULL;
  this->vec = NULL;
  
  if(ac > 0 && fts_is_symbol(at))
  {
    fts_symbol_t name = fts_get_symbol(at);
    fts_object_t *obj = ispw_get_object_by_name(name);
    
    if(ac > 1 && fts_is_number(at + 1)) 
      size = fts_get_number_int(at + 1);
    
    this->name = name;
    
    if(obj)
    {
      if(fts_object_get_class_name(obj) == ivec_symbol)
	    {
	      /* refer to existing vector */
	      fts_object_refer(obj);
	      
	      if(size > ivec_get_size((ivec_t *)obj))
          ivec_set_size(this->vec, size);
	      
	      this->vec = (ivec_t *)obj;
	    }
      else
        fts_object_error(o, "object %s is not a table", name);
      
      return;
    }
  }
  else if(ac > 0 && fts_is_number(at))
    size = fts_get_number_int(at);
  
  /* create and register new vector */
  this->vec = (ivec_t *)fts_object_create(ivec_type, 0, 0);
  fts_object_refer(this->vec);
  
  ivec_set_size(this->vec, size);
  
  fts_object_set_persistence((fts_object_t *)this->vec, 1);
  
  if(this->name != NULL)
    ispw_register_named_object((fts_object_t *)this->vec, this->name);
}

static void
table_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  table_t *this = (table_t *)o;

  if(this->name && fts_object_has_only_one_reference((fts_object_t *)this->vec))
    ispw_unregister_named_object((fts_object_t *)this->vec, this->name);

  fts_object_release((fts_object_t *)this->vec);
}

static void
table_open_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  table_t *this = (table_t *)o;
  fts_atom_t a;

  fts_set_object(&a, (fts_object_t *)this->vec);
  fts_send_message_varargs( (fts_object_t *)fts_object_get_patcher(o), fts_s_member_upload, 1, &a);  

  fts_send_message_varargs((fts_object_t *)this->vec, fts_s_openEditor, 0, 0);
}

static void
table_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(table_t), table_init, table_delete);
  
  fts_class_message_varargs(cl, fts_s_openEditor, table_open_editor);
  
  fts_class_message_varargs(cl, fts_s_dump_state, table_dump_state);  
  fts_class_message_varargs(cl, fts_s_save_dotpat, table_save_dotpat); 
  
  fts_class_message_varargs(cl, fts_s_set, table_set_with_onset_from_atoms);
  
  fts_class_message_varargs(cl, fts_new_symbol("const"), table_const);
  fts_class_message_varargs(cl, fts_s_clear, table_clear);
  
  fts_class_message_varargs(cl, fts_new_symbol("inv"), table_inv);
  fts_class_message_varargs(cl, fts_new_symbol("quantile"), table_quantile);
  fts_class_inlet_bang(cl, 0, table_get_random);
  
  fts_class_message_varargs(cl, fts_new_symbol("sum"), table_sum);  
  fts_class_message_varargs(cl, fts_s_size, table_size);
  
  fts_class_inlet_number(cl, 0, table_index);
  fts_class_inlet_varargs(cl, 0, table_varargs);

  fts_class_inlet_number(cl, 1, table_store_value);
  
  fts_class_outlet_varargs(cl, 0);
}

void
table_config(void)
{
  fts_class_install(fts_new_symbol("table"), table_instantiate);
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
