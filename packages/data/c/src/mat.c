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

#include "fts.h"
#include "matrix.h"

static fts_symbol_t sym_ascii = 0;

/******************************************************************
 *
 *  object mat
 *
 *  matrix of atoms
 *
 */

typedef struct 
{
  fts_object_t ob;
  matrix_t *mx; /* atom matrix */
  fts_atom_t buf;
} mat_t;

static void
mat_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;

  this->mx = matrix_create(ac - 1, at + 1);
  matrix_refer(this->mx);

  fts_set_void(&this->buf);
}

static void
mat_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;

  matrix_release(this->mx);
}

/********************************************************************
 *
 *   user methods
 *
 */

static void
mat_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;
  matrix_t *mx = (matrix_t *)this->mx;
  int m = matrix_get_m(mx);
  int n = matrix_get_n(mx);
  int size = m * n;

  if(ac >= 3 && fts_is_number(at) && fts_is_number(at + 1))
    {
      int i = fts_get_number_int(at);
      int j = fts_get_number_int(at + 1);
      int offset = i * n + j;

      ac -= 2;
      at += 2;

      if(ac + offset > size)
	ac = size - offset;

      matrix_set_from_atom_list(mx, offset, ac, at);
    }
}

static void
mat_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;
  matrix_t *mx = (matrix_t *)this->mx;
  int m = matrix_get_m(mx);
  int n = matrix_get_n(mx);
  
  if(ac > 1 && fts_is_number(at) && fts_is_number(at + 1))
    {
      int i = fts_get_number_int(at);
      int j = fts_get_number_int(at + 1);

      if(i >= 0 && i < m && j >= 0 && j < n)
	matrix_void_element(mx, i, j);
    }
  else if(ac == 1)
    {
      int i = fts_get_number_int(at);

      if(i >= 0 && i < m)
	{
	  int j;

	  /* void row */
	  for(j=0; j<n; j++)
	    matrix_void_element(mx, i, j);
	}
    }
  else
    matrix_void(mx);
}

static void
mat_fill(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;

  if(ac > 0)
    matrix_fill(this->mx, at[0]);
}

static void
mat_import(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_type = fts_get_symbol_arg(ac, at, 1, sym_ascii);
  matrix_t *mx = this->mx;
  int size = 0;

  if(!file_name)
    return;

  if(file_type == sym_ascii)
    {
      fts_symbol_t separator = fts_get_symbol_arg(ac, at, 2, 0);

      if(separator)
	size = matrix_import_ascii_separator(mx, file_name, separator, ac - 3, at + 3);
      else
	size = matrix_import_ascii_newline(mx, file_name);

    }

  if(size >= 0)
    {
      fts_atom_t a[2];
      
      fts_set_int(a + 0, matrix_get_m(mx));
      fts_set_int(a + 1, matrix_get_n(mx));
      fts_outlet_send(o, 1, fts_s_list, 2, a);
    }
}

static void
mat_export(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mat_t *this = (mat_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_type = fts_get_symbol_arg(ac, at, 1, sym_ascii);
  matrix_t *mx = this->mx;

  if(!file_name)
    return;

  if(file_type == sym_ascii)
    {
      fts_symbol_t separator = fts_get_symbol_arg(ac, at, 2, 0);

      if(separator)
	matrix_export_ascii_separator(mx, file_name, separator);
      else
	matrix_export_ascii_newline(mx, file_name);
    }
}

static void
mat_assist(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_symbol_t cmd = fts_get_symbol_arg(ac, at, 0, 0);

  if (cmd == fts_s_object)
    fts_object_blip(o, "matrix of values");
  else if (cmd == fts_s_inlet)
    {
      int n = fts_get_int_arg(ac, at, 1, 0);

      switch(n)
	{
	case 0:
	  fts_object_blip(o, "list <int> <int>: row and col index to recall/store value from/to matrix");
	  break;
	case 1:
	  fts_object_blip(o, "<anything>: set value to be stored");
	}
    }
  else if (cmd == fts_s_outlet)
    {
      int n = fts_get_int_arg(ac, at, 1, 0);

      switch(n)
	{
	case 0:
	  fts_object_blip(o, "<anything>: recalled value", n);
	  break;
	case 1:
	  fts_object_blip(o, "list <int> <int>: # of rows and columns read from file");
	}
    }
}

/********************************************************************
 *
 *   class
 *
 */

static void
mat_get_matrix(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  mat_t *this = (mat_t *)obj;

  matrix_atom_set(value, this->mx);
}

static fts_status_t
mat_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  if(matrix_get_constructor(ac - 1, at + 1))
    {
      fts_class_init(cl, sizeof(mat_t), 1, 1, 0);
  
      /* init/delete */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, mat_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, mat_delete);

      /* define variable */
      fts_class_add_daemon(cl, obj_property_get, fts_s_state, mat_get_matrix);

      /* .bmax load and save */
      /*fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, mat_set_from_atom_list);*/
      /*fts_method_define_varargs(cl, fts_SystemInlet, fts_s_save_bmax, mat_save_bmax);*/

      /* help */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("assist"), mat_assist); 
      
      /* user methods */
      fts_method_define_varargs(cl, 0, fts_new_symbol("clear"), mat_clear);
      fts_method_define_varargs(cl, 0, fts_new_symbol("fill"), mat_fill);
      
      fts_method_define_varargs(cl, 0, fts_new_symbol("set"), mat_set);
            
      fts_method_define_varargs(cl, 0, fts_new_symbol("import"), mat_import); 
      fts_method_define_varargs(cl, 0, fts_new_symbol("export"), mat_export); 
      
      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;

}

static int
mat_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  return (matrix_get_constructor(ac0 - 1, at0 + 1) == matrix_get_constructor(ac1 - 1, at1 + 1));
}

void
mat_config(void)
{
  sym_ascii = fts_new_symbol("ascii");

  fts_metaclass_install(fts_new_symbol("mat"), mat_instantiate, mat_equiv);
}
