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
 * Authors: Norbert Schnell, Miller Puckette.
 *
 */


#include <fts/fts.h>

typedef struct
{
  fts_object_t o;
  fts_atom_t right;
  fts_atom_t left;
  enum binop_type {binop_int, binop_float, binop_symbol} type;
} binop_t;

/**************************************************************************************
 *
 *  generic methods for number binops
 *
 */

static void
binop_number_set_right(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  if(fts_is_number(at))
    this->right = at[0];
  else
    fts_object_signal_runtime_error(o, "Bad value for right operand");
}

static void
binop_number_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  switch(ac)
    {
    default:
    case 2:
      if(fts_is_number(at + 1))
	this->right = at[1];
      else
	fts_object_signal_runtime_error(o, "Bad value for left operand");
    case 1:
      if(fts_is_number(at))
	this->left = at[0];
      else
	fts_object_signal_runtime_error(o, "Bad value for left operand");
    case 0:
    }
}

static void
binop_number_left(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_set(o, 0, 0, ac, at);
  fts_send_message(o, fts_s_bang, 0, 0);
}

static void
binop_number_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  if(ac == 0 || fts_is_int(at))
    this->type = binop_int;
  else if(ac > 0 && fts_is_float(at))
    this->type = binop_float;
  else
    fts_object_set_error(o, "Bad argument");

  if(ac > 0)
    this->right = at[0];
}

static fts_status_t
binop_number_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at, fts_method_t method)
{
  fts_class_init(cl, sizeof(binop_t), 2, 1, 0);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, binop_number_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_bang, method);

  fts_method_define_varargs(cl, 0, fts_s_bang, method);
  fts_method_define_varargs(cl, 0, fts_s_set, binop_number_set);

  fts_method_define_varargs(cl, 0, fts_s_int, binop_number_left);
  fts_method_define_varargs(cl, 0, fts_s_float, binop_number_left);
  fts_method_define_varargs(cl, 0, fts_s_list, binop_number_left);

  fts_method_define_varargs(cl, 1, fts_s_int, binop_number_set_right);
  fts_method_define_varargs(cl, 1, fts_s_float, binop_number_set_right);

  return fts_ok;
}

static void
binop_set_right(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  if((this->type <= binop_float && fts_is_number(at)) || 
     (this->type == binop_symbol && fts_is_symbol(at)))
    this->right = at[0];
  else
    fts_object_signal_runtime_error(o, "Bad value for right operand");
}

static void
binop_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  if(this->type <= binop_float)
    {
      switch(ac)
	{
	default:
	case 2:
	  if(fts_is_number(at + 1))
	    this->right = at[1];
	  else
	    fts_object_signal_runtime_error(o, "Bad value for left operand");
	case 1:
	  if(fts_is_number(at))
	    this->left = at[0];
	  else
	    fts_object_signal_runtime_error(o, "Bad value for left operand");
	case 0:
	}
    }
  else
    {
      switch(ac)
	{
	default:
	case 2:
	  if(fts_is_symbol(at + 1))
	    this->right = at[1];
	  else
	    fts_object_signal_runtime_error(o, "Bad value for left operand");
	case 1:
	  if(fts_is_symbol(at))
	    this->left = at[0];
	  else
	    fts_object_signal_runtime_error(o, "Bad value for left operand");
	case 0:
	}
    }
}

static void
binop_left(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_set(o, winlet, s, 1, at);
  fts_send_message(o, fts_s_bang, 0, 0);
}

static void
binop_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  if(ac == 0 || fts_is_int(at))
    this->type = binop_int;
  else if(ac > 0 && fts_is_float(at))
    this->type = binop_float;
  else if(ac > 0 && fts_is_symbol(at))
    this->type = binop_symbol;
  else
    fts_object_set_error(o, "Bad argument");

  if(ac > 0)
    this->right = at[0];
}

static fts_status_t
binop_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at, fts_method_t method)
{
  fts_class_init(cl, sizeof(binop_t), 2, 1, 0);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, binop_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_bang, method);

  fts_method_define_varargs(cl, 0, fts_s_bang, method);
  fts_method_define_varargs(cl, 0, fts_s_set, binop_set);

  fts_method_define_varargs(cl, 0, fts_s_int, binop_left);
  fts_method_define_varargs(cl, 0, fts_s_float, binop_left);
  fts_method_define_varargs(cl, 0, fts_s_symbol, binop_left);
  fts_method_define_varargs(cl, 0, fts_s_list, binop_left);

  fts_method_define_varargs(cl, 1, fts_s_int, binop_set_right);
  fts_method_define_varargs(cl, 1, fts_s_float, binop_set_right);
  fts_method_define_varargs(cl, 1, fts_s_symbol, binop_set_right);

  return fts_ok;
}


/**************************************************************************************
 *
 *  methods
 *
 */

static void
add_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  if(this->type == binop_int)
    fts_outlet_int(o, 0, fts_get_number_int(&this->left) + fts_get_number_int(&this->right));
  else
    fts_outlet_float(o, 0, fts_get_number_float(&this->left) + fts_get_number_float(&this->right));
}

static void
sub_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  if(this->type == binop_int)
    fts_outlet_int(o, 0, fts_get_number_int(&this->left) - fts_get_number_int(&this->right));
  else
    fts_outlet_float(o, 0, fts_get_number_float(&this->left) - fts_get_number_float(&this->right));
}

static void
mul_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  if(this->type == binop_int)
    fts_outlet_int(o, 0, fts_get_number_int(&this->left) * fts_get_number_int(&this->right));
  else
    fts_outlet_float(o, 0, fts_get_number_float(&this->left) * fts_get_number_float(&this->right));
}

static void
div_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  if(this->type == binop_int)
    {
      int right = fts_get_number_int(&this->right);

      if(right != 0)
	fts_outlet_int(o, 0, fts_get_number_int(&this->left) / right);
      else
	fts_outlet_int(o, 0, 0);
    }	
  else
    {
      double right = fts_get_number_float(&this->right);

      if(right != 0.0)
	fts_outlet_float(o, 0, fts_get_number_float(&this->left) / right);
      else
	fts_outlet_float(o, 0, 0.0);
    }
}

static void
bus_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  if(this->type == binop_int)
    fts_outlet_int(o, 0, fts_get_number_int(&this->right) - fts_get_number_int(&this->left));
  else
    fts_outlet_float(o, 0, fts_get_number_float(&this->right) - fts_get_number_float(&this->left));
}

static void
vid_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  if(this->type == binop_int)
    {
      int left = fts_get_number_int(&this->left);

      if(left != 0)
	fts_outlet_int(o, 0, fts_get_number_int(&this->right) / left);
      else
	fts_outlet_int(o, 0, 0);
    }	
  else
    {
      double left = fts_get_number_float(&this->left);

      if(left != 0.0)
	fts_outlet_float(o, 0, fts_get_number_float(&this->right) / left);
      else
	fts_outlet_float(o, 0, 0.0);
    }
}

static void
ge_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  if(this->type == binop_int)
    fts_outlet_int(o, 0, fts_get_number_int(&this->left) >= fts_get_number_int(&this->right));
  else
    fts_outlet_int(o, 0, fts_get_number_float(&this->left) >= fts_get_number_float(&this->right));
}

static void
le_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  if(this->type == binop_int)
    fts_outlet_int(o, 0, fts_get_number_int(&this->left) <= fts_get_number_int(&this->right));
  else
    fts_outlet_int(o, 0, fts_get_number_float(&this->left) <= fts_get_number_float(&this->right));
}

static void
gt_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  if(this->type == binop_int)
    fts_outlet_int(o, 0, fts_get_number_int(&this->left) > fts_get_number_int(&this->right));
  else
    fts_outlet_int(o, 0, fts_get_number_float(&this->left) > fts_get_number_float(&this->right));
}

static void
lt_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  if(this->type == binop_int)
    fts_outlet_int(o, 0, fts_get_number_int(&this->left) < fts_get_number_int(&this->right));
  else
    fts_outlet_int(o, 0, fts_get_number_float(&this->left) < fts_get_number_float(&this->right));
}

static void
ee_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  switch(this->type)
    {
    case binop_int:
      fts_outlet_int(o, 0, fts_get_number_int(&this->left) == fts_get_number_int(&this->right));
      break;
    case binop_float:
      fts_outlet_int(o, 0, fts_get_number_float(&this->left) == fts_get_number_float(&this->right));
      break;
    case binop_symbol:
      fts_outlet_int(o, 0, fts_get_symbol(&this->left) == fts_get_symbol(&this->right));
      break;
    }
}

static void
ne_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  switch(this->type)
    {
    case binop_int:
      fts_outlet_int(o, 0, fts_get_number_int(&this->left) != fts_get_number_int(&this->right));
      break;
    case binop_float:
      fts_outlet_int(o, 0, fts_get_number_float(&this->left) != fts_get_number_float(&this->right));
      break;
    case binop_symbol:
      fts_outlet_int(o, 0, fts_get_symbol(&this->left) != fts_get_symbol(&this->right));
      break;
    }
}

static void
band_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  fts_outlet_int(o, 0, fts_get_number_int(&this->left) & fts_get_number_int(&this->right));
}

static void
bor_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  fts_outlet_int(o, 0, fts_get_number_int(&this->left) | fts_get_number_int(&this->right));
}

static void
land_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  fts_outlet_int(o, 0, fts_get_number_int(&this->left) && fts_get_number_int(&this->right));
}

static void
lor_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  fts_outlet_int(o, 0, fts_get_number_int(&this->left) || fts_get_number_int(&this->right));
}

static void
lshift_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  fts_outlet_int(o, 0, fts_get_number_int(&this->left) << fts_get_number_int(&this->right));
}

static void
rshift_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  fts_outlet_int(o, 0, fts_get_number_int(&this->left) >> fts_get_number_int(&this->right));
}

static void
mod_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;
  int right = fts_get_number_int(&this->right);

  if(right != 0)
    fts_outlet_int(o, 0, fts_get_number_int(&this->left) % right);
  else
    fts_outlet_int(o, 0, 0); 
}

/**************************************************************************************
 *
 * instantiate functions
 *
 */

static fts_status_t
add_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return binop_number_instantiate(cl, ac, at, add_method);
}

static fts_status_t
sub_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return binop_number_instantiate(cl, ac, at, sub_method);
}

static fts_status_t
mul_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return binop_number_instantiate(cl, ac, at, mul_method);
}

static fts_status_t
div_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return binop_number_instantiate(cl, ac, at, div_method);
}

static fts_status_t
bus_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return binop_number_instantiate(cl, ac, at, bus_method);
}

static fts_status_t
vid_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return binop_number_instantiate(cl, ac, at, vid_method);
}

static fts_status_t
ge_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return binop_number_instantiate(cl, ac, at, ee_method);
}

static fts_status_t
le_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return binop_number_instantiate(cl, ac, at, le_method);
}

static fts_status_t
gt_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return binop_number_instantiate(cl, ac, at, gt_method);
}

static fts_status_t
lt_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return binop_number_instantiate(cl, ac, at, lt_method);
}

static fts_status_t
ee_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return binop_instantiate(cl, ac, at, ee_method);
}

static fts_status_t
ne_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return binop_instantiate(cl, ac, at, ne_method);
}

static fts_status_t
band_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return binop_number_instantiate(cl, ac, at, band_method);
}

static fts_status_t
bor_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return binop_number_instantiate(cl, ac, at, bor_method);
}

static fts_status_t
land_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return binop_number_instantiate(cl, ac, at, land_method);
}

static fts_status_t
lor_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return binop_number_instantiate(cl, ac, at, lor_method);
}

static fts_status_t
lshift_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return binop_number_instantiate(cl, ac, at, lshift_method);
}

static fts_status_t
rshift_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return binop_number_instantiate(cl, ac, at, rshift_method);
}

static fts_status_t
mod_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return binop_number_instantiate(cl, ac, at, mod_method);
}

void
binop_config(void)
{
  fts_class_install(fts_new_symbol("+"), add_instantiate);
  fts_class_install(fts_new_symbol("*"), mul_instantiate);
  fts_class_install(fts_new_symbol("/"), div_instantiate);
  fts_class_install(fts_new_symbol("-"), sub_instantiate);
  fts_class_install(fts_new_symbol("-+"), bus_instantiate);
  fts_class_install(fts_new_symbol("/*"), vid_instantiate);

  /* compatibility */
  fts_class_install(fts_new_symbol("inv+"), bus_instantiate);
  fts_class_install(fts_new_symbol("inv*"), vid_instantiate);

  fts_class_install(fts_new_symbol(">="), ge_instantiate);
  fts_class_install(fts_new_symbol("<="), le_instantiate);
  fts_class_install(fts_new_symbol(">"),  gt_instantiate);
  fts_class_install(fts_new_symbol("<"),  lt_instantiate);
  fts_class_install(fts_new_symbol("=="), ee_instantiate);
  fts_class_install(fts_new_symbol("!="), ne_instantiate);
  
  fts_class_install(fts_new_symbol("&"),  band_instantiate);
  fts_class_install(fts_new_symbol("|"),  bor_instantiate);
  fts_class_install(fts_new_symbol("&&"), land_instantiate);
  fts_class_install(fts_new_symbol("||"), lor_instantiate);
  fts_class_install(fts_new_symbol("<<"), lshift_instantiate);
  fts_class_install(fts_new_symbol(">>"), rshift_instantiate);
  fts_class_install(fts_new_symbol("%"),  mod_instantiate);
}
