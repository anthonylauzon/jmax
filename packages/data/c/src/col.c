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
 */

#include <fts/fts.h>
#include "col.h"

fts_metaclass_t *col_type = 0;
fts_symbol_t col_symbol = 0;

/********************************************************
 *
 *  utility functions
 *
 */

void
col_void(col_t *this)
{
  mat_t *mat = this->mat;
  int size = mat_get_m(mat);
  int j = this->j;
  int i;

  for(i=0; i<size; i++)
    {
      fts_atom_t *elem = mat_get_element(mat, i, j);

      fts_atom_void(elem);
    }
}

void
col_set_const(col_t *this, fts_atom_t value)
{
  mat_t *mat = this->mat;
  int size = mat_get_m(mat);
  int j = this->j;
  int i;

  for(i=0; i<size; i++)
    {
      fts_atom_t *elem = mat_get_element(mat, i, j);

      fts_atom_assign(elem, &value);
    }
}

void
col_set_from_atoms(col_t *this, int onset, int ac, const fts_atom_t *at)
{
  mat_t *mat = this->mat;
  int size = mat_get_m(mat);
  int j = this->j;
  int i;
  
  if(onset + ac > size)
    ac = size - onset;

  for(i=0; i<ac; i++)
    {
      fts_atom_t *elem = mat_get_element(mat, onset + i, j);

      fts_atom_assign(elem, at + i);
    }
}

/********************************************************************
 *
 *   user methods
 *
 */

static void
col_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_object(o, 0, o);
}

static void
col_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  col_t *this = (col_t *)o;
  
  if(ac > 1 && fts_is_number(at))
    {
      int m = mat_get_m(this->mat);
      int n = mat_get_n(this->mat);
      int i = fts_get_number_int(at);
      int j = this->j;

      if(i >= 0 && i < m && j >= 0 && j < n)
	mat_void_element(this->mat, i, j);
    }
  else
    col_void(this);
}

static void
col_fill(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  col_t *this = (col_t *)o;

  if(ac > 0)
    col_set_const(this, at[0]);
}

static void
col_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  col_t *this = (col_t *)o;

  if(ac > 2 && fts_is_number(at) && fts_is_number(at + 1))
    {
      int m = mat_get_m(this->mat);
      int n = mat_get_n(this->mat);
      int onset = fts_get_number_int(at);
      int j = this->j;

      if(onset >= 0 && onset < m && j >= 0 && j < n)
	col_set_from_atoms(this, onset, ac - 1, at + 1);
    }
}

/********************************************************************
 *
 *  system functions
 *
 */

static void
col_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  col_t *this = (col_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
  mat_t *mat = this->mat;
  int size = col_get_size(this);
  int i;
  
  fts_spost(stream, "{");

  for(i=0; i<size; i++)
    fts_spost_atoms(stream, 1, col_get_element(this, i));

  fts_spost(stream, "}\n");
}

static void
col_getobj(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object(value, obj);
}

/********************************************************************
 *
 *   class
 *
 */

static void
col_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  col_t *this = (col_t *)o;

  if(mat_atom_is(at + 0))
    {
      mat_t *mat = mat_atom_get(at + 0);

      this->mat = mat;
      fts_object_refer((fts_object_t *)mat);

      if(ac > 1 && fts_is_number(at + 1))
	{
	  int col_index = fts_get_number_int(at + 1);

	  if(col_index >= 0)
	    this->j = col_index;
	  else
	    fts_object_set_error(o, "Index must be positive");
	}
      else
	this->j = 0;
    }
  else
    fts_object_set_error(o, "First argument of mat required");
}

static void
col_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  col_t *this = (col_t *)o;
  
  fts_object_release((fts_object_t *)this->mat);
}

static fts_status_t
col_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(col_t), 1, 1, 0);
  
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, col_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, col_delete);
  
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_print, col_print); 
  
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, col_getobj);
  
  /* user methods */
  fts_method_define_varargs(cl, 0, fts_s_bang, col_output);
  
  fts_method_define_varargs(cl, 0, fts_s_clear, col_clear);
  fts_method_define_varargs(cl, 0, fts_s_fill, col_fill);      
  fts_method_define_varargs(cl, 0, fts_s_set, col_set);
  
  /* type outlet */
  fts_outlet_type_define(cl, 0, col_symbol, 1, &col_symbol);
  
  return fts_ok;
}

void
col_config(void)
{
  col_symbol = fts_new_symbol("col");

  col_type = fts_class_install(col_symbol, col_instantiate);
}
