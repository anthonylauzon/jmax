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

#include <fts/fts.h>

typedef struct 
{
  fts_object_t o;
  fts_array_t list;
  fts_array_t right_list;
  fts_atom_t right_atom;
  fts_symbol_t right_type;
} listarith_t;

/*********************************************
 *
 *  atom arithmetics
 *
 */

static void
atom_add(const fts_atom_t *left, const fts_atom_t *right, fts_atom_t *result)
{
  if(fts_is_int(left))
    {
      long l = fts_get_int(left);
      
      if(fts_is_int(right))
	fts_set_int(result, l + fts_get_int(right));
      else if(fts_is_float(right))
	fts_set_float(result, l + fts_get_float(right));
      else
	fts_set_int(result, 0);
    }
  else if(fts_is_float(left))
    {
      float l = fts_get_float(left);
      
      if(fts_is_int(right))
	fts_set_float(result, l + fts_get_int(right));
      else if(fts_is_float(right))
	fts_set_float(result, l + fts_get_float(right));
      else
	fts_set_int(result, 0);
    }
  else
    fts_set_int(result, 0);
}

static void
atom_sub(const fts_atom_t *left, const fts_atom_t *right, fts_atom_t *result)
{
  if(fts_is_int(left))
    {
      long l = fts_get_int(left);
      
      if(fts_is_int(right))
	fts_set_int(result, l - fts_get_int(right));
      else if(fts_is_float(right))
	fts_set_float(result, l - fts_get_float(right));
      else
	fts_set_int(result, 0);
    }
  else if(fts_is_float(left))
    {
      float l = fts_get_float(left);
      
      if(fts_is_int(right))
	fts_set_float(result, l - fts_get_int(right));
      else if(fts_is_float(right))
	fts_set_float(result, l - fts_get_float(right));
      else
	fts_set_int(result, 0);
    }
  else
    fts_set_int(result, 0);
}

static void
atom_mul(const fts_atom_t *left, const fts_atom_t *right, fts_atom_t *result)
{
  if(fts_is_int(left))
    {
      long l = fts_get_int(left);
      
      if(fts_is_int(right))
	fts_set_int(result, l * fts_get_int(right));
      else if(fts_is_float(right))
	fts_set_float(result, l * fts_get_float(right));
      else
	fts_set_int(result, 0);
    }
  else if(fts_is_float(left))
    {
      float l = fts_get_float(left);
      
      if(fts_is_int(right))
	fts_set_float(result, l * fts_get_int(right));
      else if(fts_is_float(right))
	fts_set_float(result, l * fts_get_float(right));
      else
	fts_set_int(result, 0);
    }
  else
    fts_set_int(result, 0);
}

static void
atom_div(const fts_atom_t *left, const fts_atom_t *right, fts_atom_t *result)
{
  if(fts_is_int(left))
    {
      long l = fts_get_int(left);
      
      if(fts_is_int(right))
	{
	  int r = fts_get_int(right);

	  if(r != 0)
	    fts_set_int(result, l / r);
	  else
	    fts_set_int(result, 0);
	}
      else if(fts_is_float(right))
	{
	  float r = fts_get_float(right);

	  if(right != 0)
	    fts_set_float(result, l / r);
	  else
	    fts_set_float(result, 0);
	}
      else
	fts_set_int(result, 0);
    }
  else if(fts_is_float(left))
    {
      float l = fts_get_float(left);
      
      if(fts_is_int(right))
	{
	  float r = (float)fts_get_int(right);

	  if(r != 0)
	    fts_set_float(result, l / r);
	  else
	    fts_set_float(result, 0.0);
	}
      else if(fts_is_float(right))
	{
	  float r = fts_get_float(right);	  

	  if(r != 0.0)
	    fts_set_float(result, l / r);
	  else
	    fts_set_float(result, 0.0);

	}
      else
	fts_set_int(result, 0);
    }
  else
    fts_set_int(result, 0);
}

static void
atom_gt(const fts_atom_t *left, const fts_atom_t *right, fts_atom_t *result)
{
  if(fts_is_int(left))
    {
      long l = fts_get_int(left);
      
      if(fts_is_int(right))
	fts_set_int(result, l > fts_get_int(right));
      else if(fts_is_float(right))
	fts_set_int(result, l > fts_get_float(right));
      else
	fts_set_int(result, 0);
    }
  else if(fts_is_float(left))
    {
      float l = fts_get_float(left);
      
      if(fts_is_int(right))
	fts_set_int(result, l > fts_get_int(right));
      else if(fts_is_float(right))
	fts_set_int(result, l > fts_get_float(right));
      else
	fts_set_int(result, 0);
    }
  else
    fts_set_int(result, 0);
}

static void
atom_ge(const fts_atom_t *left, const fts_atom_t *right, fts_atom_t *result)
{
  if(fts_is_int(left))
    {
      long l = fts_get_int(left);
      
      if(fts_is_int(right))
	fts_set_int(result, l >= fts_get_int(right));
      else if(fts_is_float(right))
	fts_set_int(result, l >= fts_get_float(right));
      else
	fts_set_int(result, 0);
    }
  else if(fts_is_float(left))
    {
      float l = fts_get_float(left);
      
      if(fts_is_int(right))
	fts_set_int(result, l >= fts_get_int(right));
      else if(fts_is_float(right))
	fts_set_int(result, l >= fts_get_float(right));
      else
	fts_set_int(result, 0);
    }
  else
    fts_set_int(result, 0);
}

static void
atom_lt(const fts_atom_t *left, const fts_atom_t *right, fts_atom_t *result)
{
  if(fts_is_int(left))
    {
      long l = fts_get_int(left);
      
      if(fts_is_int(right))
	fts_set_int(result, l < fts_get_int(right));
      else if(fts_is_float(right))
	fts_set_int(result, l < fts_get_float(right));
      else
	fts_set_int(result, 0);
    }
  else if(fts_is_float(left))
    {
      float l = fts_get_float(left);
      
      if(fts_is_int(right))
	fts_set_int(result, l < fts_get_int(right));
      else if(fts_is_float(right))
	fts_set_int(result, l < fts_get_float(right));
      else
	fts_set_int(result, 0);
    }
  else
    fts_set_int(result, 0);
}

static void
atom_le(const fts_atom_t *left, const fts_atom_t *right, fts_atom_t *result)
{
  if(fts_is_int(left))
    {
      long l = fts_get_int(left);
      
      if(fts_is_int(right))
	fts_set_int(result, l <= fts_get_int(right));
      else if(fts_is_float(right))
	fts_set_int(result, l <= fts_get_float(right));
      else
	fts_set_int(result, 0);
    }
  else if(fts_is_float(left))
    {
      float l = fts_get_float(left);
      
      if(fts_is_int(right))
	fts_set_int(result, l <= fts_get_int(right));
      else if(fts_is_float(right))
	fts_set_int(result, l <= fts_get_float(right));
      else
	fts_set_int(result, 0);
    }
  else
    fts_set_int(result, 0);
}

static void
atom_ne(const fts_atom_t *left, const fts_atom_t *right, fts_atom_t *result)
{
  if(fts_is_int(left))
    {
      long l = fts_get_int(left);
      
      if(fts_is_int(right))
	fts_set_int(result, l != fts_get_int(right));
      else if(fts_is_float(right))
	fts_set_int(result, l != fts_get_float(right));
      else
	fts_set_int(result, 1);
    }
  else if(fts_is_float(left))
    {
      float l = fts_get_float(left);
      
      if(fts_is_int(right))
	fts_set_int(result, l != fts_get_int(right));
      else if(fts_is_float(right))
	fts_set_int(result, l != fts_get_float(right));
      else
	fts_set_int(result, 1);
    }
  else
    fts_set_int(result, (!fts_atom_same_type(left, right)) || (fts_get_int(left) != fts_get_int(right)));
}

static void
atom_ee(const fts_atom_t *left, const fts_atom_t *right, fts_atom_t *result)
{
  if(fts_is_int(left))
    {
      long l = fts_get_int(left);
      
      if(fts_is_int(right))
	fts_set_int(result, l == fts_get_int(right));
      else if(fts_is_float(right))
	fts_set_int(result, l == fts_get_float(right));
      else
	fts_set_int(result, 0);
    }
  else if(fts_is_float(left))
    {
      float l = fts_get_float(left);
      
      if(fts_is_int(right))
	fts_set_int(result, l == fts_get_int(right));
      else if(fts_is_float(right))
	fts_set_int(result, l == fts_get_float(right));
      else
	fts_set_int(result, 0);
    }
  else
    fts_set_int(result, (fts_atom_same_type(left, right)) && (fts_get_int(left) == fts_get_int(right)));
}

/*********************************************
 *
 *  methods
 *
 */

static void
listarith_set_right_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;

  fts_array_set(&this->right_list, ac, at);
  this->right_type = fts_s_list;
}

static void
listarith_set_right_atom(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;
  
  this->right_atom = at[0];
  this->right_type = fts_get_selector(at);
} 


static void
listarith_add(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;
  int i;

  if(this->right_type == fts_s_list)
    {
      int size = fts_array_get_size(&this->right_list);
      const fts_atom_t *ptr = fts_array_get_atoms(&this->right_list);

      if(size < ac)
	ac = size;
      
      fts_array_set_size(&this->list, ac);
      
      for(i=0; i<ac; i++)
	atom_add(at + i, ptr + i, fts_array_get_atoms(&this->list) + i);
    }
  else
    {
      fts_array_set_size(&this->list, ac);
      
      for(i=0; i<ac; i++)
	atom_add(at + i, &this->right_atom, fts_array_get_atoms(&this->list) + i);
    }

  fts_outlet_atoms(o, 0, ac, fts_array_get_atoms(&this->list));
}

static void
listarith_sub(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;
  int i;

  if(this->right_type == fts_s_list)
    {
      int size = fts_array_get_size(&this->right_list);
      const fts_atom_t *ptr = fts_array_get_atoms(&this->right_list);

      if(size < ac)
	ac = size;
      
      fts_array_set_size(&this->list, ac);
      
      for(i=0; i<ac; i++)
	atom_sub(at + i, ptr + i, fts_array_get_atoms(&this->list) + i);
    }
  else
    {
      fts_array_set_size(&this->list, ac);
      
      for(i=0; i<ac; i++)
	atom_sub(at + i, &this->right_atom, fts_array_get_atoms(&this->list) + i);
    }
  
  fts_outlet_atoms(o, 0, ac, fts_array_get_atoms(&this->list));
}

static void
listarith_mul(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;
  int i;

  if(this->right_type == fts_s_list)
    {
      int size = fts_array_get_size(&this->right_list);
      const fts_atom_t *ptr = fts_array_get_atoms(&this->right_list);

      if(size < ac)
	ac = size;
      
      fts_array_set_size(&this->list, ac);
      
      for(i=0; i<ac; i++)
	atom_mul(at + i, ptr + i, fts_array_get_atoms(&this->list) + i);
    }
  else
    {
      fts_array_set_size(&this->list, ac);
      
      for(i=0; i<ac; i++)
	atom_mul(at + i, &this->right_atom, fts_array_get_atoms(&this->list) + i);
    }

  fts_outlet_atoms(o, 0, ac, fts_array_get_atoms(&this->list));
}

static void
listarith_div(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;
  int i;

  if(this->right_type == fts_s_list)
    {
      int size = fts_array_get_size(&this->right_list);
      const fts_atom_t *ptr = fts_array_get_atoms(&this->right_list);

      if(size < ac)
	ac = size;
      
      fts_array_set_size(&this->list, ac);
      
      for(i=0; i<ac; i++)
	atom_div(at + i, ptr + i, fts_array_get_atoms(&this->list) + i);
    }
  else
    {
      fts_array_set_size(&this->list, ac);
      
      for(i=0; i<ac; i++)
	atom_div(at + i, &this->right_atom, fts_array_get_atoms(&this->list) + i);
    }
  
  fts_outlet_atoms(o, 0, ac, fts_array_get_atoms(&this->list));
}

static void
listarith_gt(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;
  int i;

  if(this->right_type == fts_s_list)
    {
      int size = fts_array_get_size(&this->right_list);
      const fts_atom_t *ptr = fts_array_get_atoms(&this->right_list);

      if(size < ac)
	ac = size;
      
      fts_array_set_size(&this->list, ac);
      
      for(i=0; i<ac; i++)
	atom_gt(at + i, ptr + i, fts_array_get_atoms(&this->list) + i);
    }
  else
    {
      fts_array_set_size(&this->list, ac);
      
      for(i=0; i<ac; i++)
	atom_gt(at + i, &this->right_atom, fts_array_get_atoms(&this->list) + i);
    }
  
  fts_outlet_atoms(o, 0, ac, fts_array_get_atoms(&this->list));
}

static void
listarith_ge(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;
  int i;

  if(this->right_type == fts_s_list)
    {
      int size = fts_array_get_size(&this->right_list);
      const fts_atom_t *ptr = fts_array_get_atoms(&this->right_list);

      if(size < ac)
	ac = size;
      
      fts_array_set_size(&this->list, ac);
      
      for(i=0; i<ac; i++)
	atom_ge(at + i, ptr + i, fts_array_get_atoms(&this->list) + i);
    }
  else
    {
      fts_array_set_size(&this->list, ac);
      
      for(i=0; i<ac; i++)
	atom_ge(at + i, &this->right_atom, fts_array_get_atoms(&this->list) + i);
    }
  
  fts_outlet_atoms(o, 0, ac, fts_array_get_atoms(&this->list));
}

static void
listarith_lt(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;
  int i;

  if(this->right_type == fts_s_list)
    {
      int size = fts_array_get_size(&this->right_list);
      const fts_atom_t *ptr = fts_array_get_atoms(&this->right_list);

      if(size < ac)
	ac = size;
      
      fts_array_set_size(&this->list, ac);
      
      for(i=0; i<ac; i++)
	atom_lt(at + i, ptr + i, fts_array_get_atoms(&this->list) + i);
    }
  else
    {
      fts_array_set_size(&this->list, ac);
      
      for(i=0; i<ac; i++)
	atom_lt(at + i, &this->right_atom, fts_array_get_atoms(&this->list) + i);
    }
  
  fts_outlet_atoms(o, 0, ac, fts_array_get_atoms(&this->list));
}

static void
listarith_le(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;
  int i;

  if(this->right_type == fts_s_list)
    {
      int size = fts_array_get_size(&this->right_list);
      const fts_atom_t *ptr = fts_array_get_atoms(&this->right_list);

      if(size < ac)
	ac = size;
      
      fts_array_set_size(&this->list, ac);
      
      for(i=0; i<ac; i++)
	atom_le(at + i, ptr + i, fts_array_get_atoms(&this->list) + i);
    }
  else
    {
      fts_array_set_size(&this->list, ac);
      
      for(i=0; i<ac; i++)
	atom_le(at + i, &this->right_atom, fts_array_get_atoms(&this->list) + i);
    }
  
  fts_outlet_atoms(o, 0, ac, fts_array_get_atoms(&this->list));
}

static void
listarith_ne(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;
  int i;

  if(this->right_type == fts_s_list)
    {
      int size = fts_array_get_size(&this->right_list);
      const fts_atom_t *ptr = fts_array_get_atoms(&this->right_list);

      if(size < ac)
	ac = size;
      
      fts_array_set_size(&this->list, ac);
      
      for(i=0; i<ac; i++)
	atom_ne(at + i, ptr + i, fts_array_get_atoms(&this->list) + i);
    }
  else
    {
      fts_array_set_size(&this->list, ac);
      
      for(i=0; i<ac; i++)
	atom_ne(at + i, &this->right_atom, fts_array_get_atoms(&this->list) + i);
    }
  
  fts_outlet_atoms(o, 0, ac, fts_array_get_atoms(&this->list));
}

static void
listarith_ee(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;
  int i;

  if(this->right_type == fts_s_list)
    {
      int size = fts_array_get_size(&this->right_list);
      const fts_atom_t *ptr = fts_array_get_atoms(&this->right_list);

      if(size < ac)
	ac = size;
      
      fts_array_set_size(&this->list, ac);
      
      for(i=0; i<ac; i++)
	atom_ee(at + i, ptr + i, fts_array_get_atoms(&this->list) + i);
    }
  else
    {
      fts_array_set_size(&this->list, ac);
      
      for(i=0; i<ac; i++)
	atom_ee(at + i, &this->right_atom, fts_array_get_atoms(&this->list) + i);
    }
  
  fts_outlet_atoms(o, 0, ac, fts_array_get_atoms(&this->list));
}

/*********************************************
 *
 *  class
 *
 */

static void
listarith_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;

  ac--;
  at++;

  fts_array_init(&this->list, 0, 0);
  fts_set_void(&this->right_atom);

  if(ac == 1)
    {
      if(!fts_is_object(at))
	listarith_set_right_atom(o, 0, 0, 1, at);

      fts_array_init(&this->right_list, 0, 0);
    }
  else
    fts_array_init(&this->right_list, ac, at);
}

static void
listarith_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;

  fts_array_destroy(&this->list);
  fts_array_destroy(&this->right_list);
}

static fts_status_t
listarith_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at, fts_method_t meth)
{
  fts_class_init(cl, sizeof(listarith_t), 2, 1, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, listarith_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, listarith_delete);

  fts_method_define_varargs(cl, 1, fts_s_list, listarith_set_right_list);

  fts_method_define_float(cl, 1, listarith_set_right_atom);
  fts_method_define_int(cl, 1, listarith_set_right_atom);
  fts_method_define_symbol(cl, 1, listarith_set_right_atom);

  fts_method_define_varargs(cl, 0, fts_s_list, meth);

  fts_outlet_type_define_varargs(cl, 0,	fts_s_list);

  return fts_Success;
}

static fts_status_t
listarith_add_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return listarith_instantiate(cl, ac, at, listarith_add);
}

static fts_status_t
listarith_sub_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return listarith_instantiate(cl, ac, at, listarith_sub);
}

static fts_status_t
listarith_mul_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return listarith_instantiate(cl, ac, at, listarith_mul);
}

static fts_status_t
listarith_div_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return listarith_instantiate(cl, ac, at, listarith_div);
}

static fts_status_t
listarith_lt_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return listarith_instantiate(cl, ac, at, listarith_lt);
}

static fts_status_t
listarith_le_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return listarith_instantiate(cl, ac, at, listarith_le);
}

static fts_status_t
listarith_gt_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return listarith_instantiate(cl, ac, at, listarith_gt);
}

static fts_status_t
listarith_ge_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return listarith_instantiate(cl, ac, at, listarith_ge);
}

static fts_status_t
listarith_ne_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return listarith_instantiate(cl, ac, at, listarith_ne);
}

static fts_status_t
listarith_ee_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return listarith_instantiate(cl, ac, at, listarith_ee);
}

void
listarith_config(void)
{
  fts_class_install(fts_new_symbol("list+"), listarith_add_instantiate);
  fts_class_install(fts_new_symbol("list-"), listarith_sub_instantiate);
  fts_class_install(fts_new_symbol("list*"), listarith_mul_instantiate);
  fts_class_install(fts_new_symbol("list/"), listarith_div_instantiate);

  fts_class_install(fts_new_symbol("list<"), listarith_lt_instantiate);
  fts_class_install(fts_new_symbol("list<="), listarith_le_instantiate);
  fts_class_install(fts_new_symbol("list>"), listarith_gt_instantiate);
  fts_class_install(fts_new_symbol("list>="), listarith_ge_instantiate);
  fts_class_install(fts_new_symbol("list!="), listarith_ne_instantiate);
  fts_class_install(fts_new_symbol("list=="), listarith_ee_instantiate);

  /* aliases */
  fts_alias_install(fts_new_symbol("l+"), fts_new_symbol("list+"));
  fts_alias_install(fts_new_symbol("l-"), fts_new_symbol("list-"));
  fts_alias_install(fts_new_symbol("l*"), fts_new_symbol("list*"));
  fts_alias_install(fts_new_symbol("l/"), fts_new_symbol("list/"));

  fts_alias_install(fts_new_symbol("l<"), fts_new_symbol("list<"));
  fts_alias_install(fts_new_symbol("l<="), fts_new_symbol("list<="));
  fts_alias_install(fts_new_symbol("l>"), fts_new_symbol("list>"));
  fts_alias_install(fts_new_symbol("l>="), fts_new_symbol("list>="));
  fts_alias_install(fts_new_symbol("l!="), fts_new_symbol("list!="));
  fts_alias_install(fts_new_symbol("l=="), fts_new_symbol("list=="));
}
