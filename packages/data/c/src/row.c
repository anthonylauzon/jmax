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
#include "row.h"

fts_symbol_t row_symbol = 0;
fts_metaclass_t *row_type = 0;

/********************************************************
 *
 *  utility functions
 *
 */

void
row_void(row_t *this)
{
  mat_t *mat = this->mat;
  int size = mat_get_n(mat);
  int i = this->i;
  int j;

  for(j=0; j<size; j++)
    {
      fts_atom_t *elem = mat_get_element(mat, i, j);

      fts_set_void(elem);
    }
}

void
row_set_const(row_t *this, fts_atom_t value)
{
  mat_t *mat = this->mat;
  int size = mat_get_n(mat);
  int i = this->i;
  int j;

  for(j=0; j<size; j++)
    {
      fts_atom_t *elem = mat_get_element(mat, i, j);

      fts_atom_assign(elem, &value);
    }
}

void
row_set_from_atoms(row_t *this, int onset, int ac, const fts_atom_t *at)
{
  mat_t *mat = this->mat;
  int size = mat_get_n(mat);
  int i = this->i;
  int j;
  
  if(onset + ac > size)
    ac = size - onset;

  for(j=0; j<ac; j++)
    {
      fts_atom_t *elem = mat_get_element(mat, i, onset + j);

      fts_atom_assign(elem, at + j);
    }
}

/********************************************************************
 *
 *   user methods
 *
 */

static void
row_fill(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  row_t *this = (row_t *)o;

  if(ac > 0)
    row_set_const(this, at[0]);
}

static void
row_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  row_t *this = (row_t *)o;

  if(ac > 2 && fts_is_number(at) && fts_is_number(at + 1))
    {
      int m = mat_get_m(this->mat);
      int n = mat_get_n(this->mat);
      int i = this->i;
      int onset = fts_get_number_int(at);

      if(i >= 0 && i < m && onset >= 0 && onset < n)
	row_set_from_atoms(this, onset, ac - 1, at + 1);
    }
}

/********************************************************************
 *
 *  system functions
 *
 */

static void
row_getobj(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object(value, obj);
}

/********************************************************************
 *
 *   class
 *
 */

static void
row_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  row_t *this = (row_t *)o;

  if(fts_is_a(at, mat_type))
    {
      mat_t *mat = (mat_t *)fts_get_object(at);

      this->mat = mat;
      fts_object_refer((fts_object_t *)mat);

      if(ac > 1 && fts_is_number(at + 1))
	{
	  int row_index = fts_get_number_int(at + 1);

	  if(row_index >= 0)
	    this->i = row_index;
	  else
	    fts_object_set_error(o, "Index must be positive");
	}
      else
	this->i = 0;
    }
  else
    fts_object_set_error(o, "First argument of mat required");
}

static void
row_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  row_t *this = (row_t *)o;
  
  fts_object_release((fts_object_t *)this->mat);
}

static void
row_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(row_t), row_init, row_delete);
  
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, row_getobj);
  
  fts_class_message_varargs(cl, fts_s_fill, row_fill);      
  fts_class_message_varargs(cl, fts_s_set, row_set);
  }

void
row_config(void)
{
  row_symbol = fts_new_symbol("row");

  row_type = fts_class_install(row_symbol, row_instantiate);
}
