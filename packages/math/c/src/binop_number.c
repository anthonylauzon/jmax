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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Miller Puckette.
 *
 */

#include "fts.h"
#include "binop.h"

/**************************************************************************************
 *
 *  object
 *
 */

typedef struct
{
  fts_object_t o;
  fts_atom_t right;
  fts_atom_t res;
} binop_number_t;

static void
binop_number_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  this->right = at[1];

  if(ac > 2)
    {
      this->res = at[2];

      if(float_vector_atom_is(at + 2))
	float_vector_refer(float_vector_atom_get(at + 2));
      else if(int_vector_atom_is(at + 2))
	int_vector_refer(int_vector_atom_get(at + 2));
    }
}

/**************************************************************************************
 *
 *  user methods
 *
 */

static void
binop_number_set_right(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  this->right = at[0];
}

/**************************************************************************************
 *
 *  integer (o) number
 *
 */

/* int (o) number arithmetics */

static void
binop_number_add_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    fts_outlet_int(o, 0, fts_get_int(at) + fts_get_int(&this->right));
  else
    fts_outlet_float(o, 0, (float)fts_get_int(at) + fts_get_float(&this->right));
}

static void
binop_number_sub_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    fts_outlet_int(o, 0, fts_get_int(at) - fts_get_int(&this->right));
  else
    fts_outlet_float(o, 0, (float)fts_get_int(at) - fts_get_float(&this->right));
}

static void
binop_number_mul_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    fts_outlet_int(o, 0, fts_get_int(at) * fts_get_int(&this->right));
  else
    fts_outlet_float(o, 0, (float)fts_get_int(at) * fts_get_float(&this->right));
}

static void
binop_number_div_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    fts_outlet_int(o, 0, fts_get_int(at) / fts_get_int(&this->right));
  else
    fts_outlet_float(o, 0, (float)fts_get_int(at) / fts_get_float(&this->right));
}

static void
binop_number_bus_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    fts_outlet_int(o, 0, fts_get_int(&this->right) - fts_get_int(at));
  else
    fts_outlet_float(o, 0, fts_get_float(&this->right) - (float)fts_get_int(at));
}

static void
binop_number_vid_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    fts_outlet_int(o, 0, fts_get_int(&this->right) / fts_get_int(at));
  else
    fts_outlet_float(o, 0, fts_get_float(&this->right) / (float)fts_get_int(at));
}

/* int (o) number comparison  */

static void
binop_number_ee_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    fts_outlet_int(o, 0, fts_get_int(at) == fts_get_int(&this->right));
  else
    fts_outlet_int(o, 0, (float)fts_get_int(at) == fts_get_float(&this->right));
}

static void
binop_number_ne_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    fts_outlet_int(o, 0, fts_get_int(at) != fts_get_int(&this->right));
  else
    fts_outlet_int(o, 0, (float)fts_get_int(at) != fts_get_float(&this->right));
}

static void
binop_number_gt_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    fts_outlet_int(o, 0, fts_get_int(at) > fts_get_int(&this->right));
  else
    fts_outlet_int(o, 0, (float)fts_get_int(at) > fts_get_float(&this->right));
}

static void
binop_number_ge_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    fts_outlet_int(o, 0, fts_get_int(at) >= fts_get_int(&this->right));
  else
    fts_outlet_int(o, 0, (float)fts_get_int(at) >= fts_get_float(&this->right));
}

static void
binop_number_lt_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    fts_outlet_int(o, 0, fts_get_int(at) < fts_get_int(&this->right));
  else
    fts_outlet_int(o, 0, (float)fts_get_int(at) < fts_get_float(&this->right));
}

static void
binop_number_le_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    fts_outlet_int(o, 0, fts_get_int(at) <= fts_get_int(&this->right));
  else
    fts_outlet_int(o, 0, (float)fts_get_int(at) <= fts_get_float(&this->right));
}

/* int (o) number min/max  */

static void
binop_number_min_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    {
      int left = fts_get_int(at);
      int right = fts_get_int(&this->right);

      if(right < left)
	fts_outlet_send(o, 0, fts_s_int, 1, &this->right);
      else
	fts_outlet_send(o, 0, fts_s_int, 1, at);  
    }
  else
    {
      float left = (float)fts_get_int(at);
      float right = fts_get_float(&this->right);
      
      if(right < left)
	fts_outlet_send(o, 0, fts_s_int, 1, &this->right);
      else
	fts_outlet_send(o, 0, fts_s_int, 1, at);  
    }
}

static void
binop_number_max_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    {
      int left = fts_get_int(at);
      int right = fts_get_int(&this->right);

      if(right > left)
	fts_outlet_int(o, 0, right);
      else
	fts_outlet_send(o, 0, fts_s_int, 1, at);  
    }
  else
    {
      float left = (float)fts_get_int(at);
      float right = fts_get_float(&this->right);
      
      if(right > left)
	fts_outlet_float(o, 0, right);
      else
	fts_outlet_send(o, 0, fts_s_int, 1, at);  
    }
}

/**************************************************************************************
 *
 *  float (o) number
 *
 */

/* float (o) number arithmetics */

static void
binop_number_add_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  fts_outlet_float(o, 0, fts_get_float(at) + fts_get_number_float(&this->right));
}

static void
binop_number_sub_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  fts_outlet_float(o, 0, fts_get_float(at) - fts_get_number_float(&this->right));
}

static void
binop_number_mul_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  fts_outlet_float(o, 0, fts_get_float(at) * fts_get_number_float(&this->right));
}

static void
binop_number_div_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  fts_outlet_float(o, 0, fts_get_float(at) / fts_get_number_float(&this->right));
}

static void
binop_number_bus_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  fts_outlet_float(o, 0, fts_get_number_float(&this->right) - fts_get_float(at));
}

static void
binop_number_vid_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  fts_outlet_float(o, 0, fts_get_number_float(&this->right) / fts_get_float(at));
}

/* float (o) number comparison  */

static void
binop_number_ee_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  fts_outlet_int(o, 0, fts_get_float(at) == fts_get_number_float(&this->right));
}

static void
binop_number_ne_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  fts_outlet_int(o, 0, fts_get_float(at) != fts_get_number_float(&this->right));
}

static void
binop_number_gt_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  fts_outlet_int(o, 0, fts_get_float(at) > fts_get_number_float(&this->right));
}

static void
binop_number_ge_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  fts_outlet_int(o, 0, fts_get_float(at) >= fts_get_number_float(&this->right));
}

static void
binop_number_lt_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  fts_outlet_int(o, 0, fts_get_float(at) < fts_get_number_float(&this->right));
}

static void
binop_number_le_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  fts_outlet_int(o, 0, fts_get_float(at) <= fts_get_number_float(&this->right));
}

/* float (o) number min/max  */

static void
binop_number_min_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  float left = fts_get_float(at);
    
  if(fts_get_number_float(&this->right) < left)
    fts_outlet_send(o, 0, fts_get_selector(&this->right), 1, &this->right);
  else
    fts_outlet_send(o, 0, fts_s_float, 1, at);  
}

static void
binop_number_max_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  float left = fts_get_float(at);
    
  if(fts_get_number_float(&this->right) > left)
    fts_outlet_send(o, 0, fts_get_selector(&this->right), 1, &this->right);
  else
    fts_outlet_send(o, 0, fts_s_float, 1, at);  
}

/**************************************************************************************
 *
 *  class
 *
 */

fts_status_t
binop_number_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t name = fts_get_symbol(at);

  fts_class_init(cl, sizeof(binop_number_t), 2, 1, 0);

  /* inlet system */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, binop_number_init);

  if(name == math_sym_add)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_add_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_add_left_float);
    }
  else if(name == math_sym_sub)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_sub_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_sub_left_float);
    }
  else if(name == math_sym_mul)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_mul_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_mul_left_float);
    }
  else if(name == math_sym_div)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_div_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_div_left_float);
    }
  else if(name == math_sym_bus)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_bus_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_bus_left_float);
    }
  else if(name == math_sym_vid)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_vid_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_vid_left_float);
    }
  else if(name == math_sym_ee)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_ee_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_ee_left_float);
    }
  else if(name == math_sym_ne)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_ne_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_ne_left_float);
    }
  else if(name == math_sym_gt)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_gt_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_gt_left_float);
    }
  else if(name == math_sym_ge)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_ge_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_ge_left_float);
    }
  else if(name == math_sym_lt)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_lt_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_lt_left_float);
    }
  else if(name == math_sym_le)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_le_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_le_left_float);
    }
  else if(name == math_sym_min)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_min_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_min_left_float);
    }
  else if(name == math_sym_max)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_max_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_max_left_float);
    }

  /* right inlet: set right operand */
  fts_method_define_varargs(cl, 1, fts_s_int, binop_number_set_right);
  fts_method_define_varargs(cl, 1, fts_s_float, binop_number_set_right);

  return fts_Success;
}
