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
 */

#include <fts/fts.h>
#include "col.h"

fts_class_t *col_type = 0;
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
 *   class
 *
 */

static void
col_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  col_t *this = (col_t *)o;

  if(ac > 0 && fts_is_a(at, mat_type))
    {
      mat_t *mat = (mat_t *)fts_get_object(at + 0);

      this->mat = mat;
      fts_object_refer((fts_object_t *)mat);

      if(ac > 1 && fts_is_number(at + 1))
	{
	  int col_index = fts_get_number_int(at + 1);

	  if(col_index >= 0)
	    this->j = col_index;
	  else
	    fts_object_error(o, "index must be positive");
	}
      else
	this->j = 0;
    }
  else
    fts_object_error(o, "first argument of mat required");
}

static void
col_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  col_t *this = (col_t *)o;
  
  if(this->mat != NULL)
    fts_object_release((fts_object_t *)this->mat);
}

static void
col_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(col_t), col_init, col_delete);
  
  fts_class_message_varargs(cl, fts_s_fill, col_fill);      
  fts_class_message_varargs(cl, fts_s_set, col_set);

  fts_class_inlet_bang(cl, 0, data_object_output);

  fts_class_inlet_thru(cl, 0);
  fts_class_outlet_thru(cl, 0);  
}

void
col_config(void)
{
  col_symbol = fts_new_symbol("col");

  col_type = fts_class_install(col_symbol, col_instantiate);
}
