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
    fts_object_error(o, "bad value for right operand");
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
	fts_object_error(o, "bad value for left operand");
    case 1:
      if(fts_is_number(at))
	this->left = at[0];
      else
	fts_object_error(o, "bad value for left operand");
    case 0:
      break;
    }
}

/* called from binop method */
static void
binop_number_left(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  switch(ac)
    {
    default:
    case 2:
      binop_number_set_right(o, 0, 0, 1, at + 1);
    case 1:
      this->left = at[0];
    case 0:
      break;
    }
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
    fts_object_error(o, "bad argument");

  if(ac > 0)
    this->right = at[0];
}

static void
binop_number_instantiate(fts_class_t *cl, fts_method_t method)
{
  fts_class_init(cl, sizeof(binop_t), binop_number_init, 0);

  fts_class_message_varargs(cl, fts_s_set, binop_number_set);

  fts_class_inlet_bang(cl, 0, method);
  fts_class_inlet_number(cl, 0, method);
  fts_class_inlet_varargs(cl, 0, method);

  fts_class_inlet_number(cl, 1, binop_number_set_right);

  fts_class_outlet_number(cl, 0);
}

static void
binop_equal_set_right(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  if((this->type <= binop_float && fts_is_number(at)) || 
     (this->type == binop_symbol && fts_is_symbol(at)))
    this->right = at[0];
  else
    fts_object_error(o, "bad value for right operand");
}

static void
binop_equal_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  if(this->type <= binop_float)
    {
      /* number */
      switch(ac)
	{
	default:
	case 2:
	  if(fts_is_number(at + 1))
	    this->right = at[1];
	  else
	    fts_object_error(o, "bad value for left operand");
	case 1:
	  if(fts_is_number(at))
	    this->left = at[0];
	  else
	    fts_object_error(o, "bad value for left operand");
	case 0:
	  break;
	}
    }
  else
    {
      /* symbol */
      switch(ac)
	{
	default:
	case 2:
	  if(fts_is_symbol(at + 1))
	    this->right = at[1];
	  else
	    fts_object_error(o, "bad value for left operand");
	case 1:
	  if(fts_is_symbol(at))
	    this->left = at[0];
	  else
	    fts_object_error(o, "bad value for left operand");
	case 0:
	  break;
	}
    }
}

static void
binop_equal_left(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  switch(ac)
    {
    default:
    case 2:
      binop_equal_set_right(o, 0, 0, 1, at + 1);
    case 1:
      this->left = at[0];
    case 0:
      break;
    }
}

static void
binop_equal_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  if(ac == 0 || fts_is_int(at))
    this->type = binop_int;
  else if(ac > 0 && fts_is_float(at))
    this->type = binop_float;
  else if(ac > 0 && fts_is_symbol(at))
    this->type = binop_symbol;
  else
    fts_object_error(o, "bad argument");

  if(ac > 0)
    this->right = at[0];
}

static void
binop_equal_instantiate(fts_class_t *cl, fts_method_t method)
{
  fts_class_init(cl, sizeof(binop_t), binop_equal_init, 0);

  fts_class_message_varargs(cl, fts_s_set, binop_equal_set);

  fts_class_inlet_bang(cl, 0, method);
  fts_class_inlet_int(cl, 0, method);
  fts_class_inlet_float(cl, 0, method);
  fts_class_inlet_symbol(cl, 0, method);
  fts_class_inlet_varargs(cl, 0, method);

  fts_class_inlet_int(cl, 1, binop_equal_set_right);
  fts_class_inlet_float(cl, 1, binop_equal_set_right);
  fts_class_inlet_symbol(cl, 1, binop_equal_set_right);

  fts_class_outlet_int(cl, 0);
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

  binop_number_left(o, 0, 0, ac, at);
  
  if(this->type == binop_int)
    fts_outlet_int(o, 0, fts_get_number_int(&this->left) + fts_get_number_int(&this->right));
  else
    fts_outlet_float(o, 0, fts_get_number_float(&this->left) + fts_get_number_float(&this->right));
}

static void
sub_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  binop_number_left(o, 0, 0, ac, at);

  if(this->type == binop_int)
    fts_outlet_int(o, 0, fts_get_number_int(&this->left) - fts_get_number_int(&this->right));
  else
    fts_outlet_float(o, 0, fts_get_number_float(&this->left) - fts_get_number_float(&this->right));
}

static void
mul_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  binop_number_left(o, 0, 0, ac, at);

  if(this->type == binop_int)
    fts_outlet_int(o, 0, fts_get_number_int(&this->left) * fts_get_number_int(&this->right));
  else
    fts_outlet_float(o, 0, fts_get_number_float(&this->left) * fts_get_number_float(&this->right));
}

static void
div_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  binop_number_left(o, 0, 0, ac, at);

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

  binop_number_left(o, 0, 0, ac, at);

  if(this->type == binop_int)
    fts_outlet_int(o, 0, fts_get_number_int(&this->right) - fts_get_number_int(&this->left));
  else
    fts_outlet_float(o, 0, fts_get_number_float(&this->right) - fts_get_number_float(&this->left));
}

static void
vid_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  binop_number_left(o, 0, 0, ac, at);

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
ee_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  binop_equal_left(o, 0, 0, ac, at);

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

  binop_equal_left(o, 0, 0, ac, at);

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
ge_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  binop_number_left(o, 0, 0, ac, at);

  if(this->type == binop_int)
    fts_outlet_int(o, 0, fts_get_number_int(&this->left) >= fts_get_number_int(&this->right));
  else
    fts_outlet_int(o, 0, fts_get_number_float(&this->left) >= fts_get_number_float(&this->right));
}

static void
le_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  binop_number_left(o, 0, 0, ac, at);

  if(this->type == binop_int)
    fts_outlet_int(o, 0, fts_get_number_int(&this->left) <= fts_get_number_int(&this->right));
  else
    fts_outlet_int(o, 0, fts_get_number_float(&this->left) <= fts_get_number_float(&this->right));
}

static void
gt_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  binop_number_left(o, 0, 0, ac, at);

  if(this->type == binop_int)
    fts_outlet_int(o, 0, fts_get_number_int(&this->left) > fts_get_number_int(&this->right));
  else
    fts_outlet_int(o, 0, fts_get_number_float(&this->left) > fts_get_number_float(&this->right));
}

static void
lt_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  binop_number_left(o, 0, 0, ac, at);

  if(this->type == binop_int)
    fts_outlet_int(o, 0, fts_get_number_int(&this->left) < fts_get_number_int(&this->right));
  else
    fts_outlet_int(o, 0, fts_get_number_float(&this->left) < fts_get_number_float(&this->right));
}

static void
band_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  binop_number_left(o, 0, 0, ac, at);

  fts_outlet_int(o, 0, fts_get_number_int(&this->left) & fts_get_number_int(&this->right));
}

static void
bor_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  binop_number_left(o, 0, 0, ac, at);

  fts_outlet_int(o, 0, fts_get_number_int(&this->left) | fts_get_number_int(&this->right));
}

static void
land_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  binop_number_left(o, 0, 0, ac, at);

  fts_outlet_int(o, 0, fts_get_number_int(&this->left) && fts_get_number_int(&this->right));
}

static void
lor_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  binop_number_left(o, 0, 0, ac, at);

  fts_outlet_int(o, 0, fts_get_number_int(&this->left) || fts_get_number_int(&this->right));
}

static void
lshift_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  binop_number_left(o, 0, 0, ac, at);

  fts_outlet_int(o, 0, fts_get_number_int(&this->left) << fts_get_number_int(&this->right));
}

static void
rshift_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  binop_number_left(o, 0, 0, ac, at);

  fts_outlet_int(o, 0, fts_get_number_int(&this->left) >> fts_get_number_int(&this->right));
}

static void
mod_method(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;
  int right;

  binop_number_left(o, 0, 0, ac, at);

  right = fts_get_number_int(&this->right);

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

static void
add_instantiate(fts_class_t *cl)
{
  binop_number_instantiate(cl, add_method);
}

static void
sub_instantiate(fts_class_t *cl)
{
  binop_number_instantiate(cl, sub_method);
}

static void
mul_instantiate(fts_class_t *cl)
{
  binop_number_instantiate(cl, mul_method);
}

static void
div_instantiate(fts_class_t *cl)
{
  binop_number_instantiate(cl, div_method);
}

static void
bus_instantiate(fts_class_t *cl)
{
  binop_number_instantiate(cl, bus_method);
}

static void
vid_instantiate(fts_class_t *cl)
{
  binop_number_instantiate(cl, vid_method);
}

static void
ge_instantiate(fts_class_t *cl)
{
  binop_number_instantiate(cl, ge_method);
}

static void
le_instantiate(fts_class_t *cl)
{
  binop_number_instantiate(cl, le_method);
}

static void
gt_instantiate(fts_class_t *cl)
{
  binop_number_instantiate(cl, gt_method);
}

static void
lt_instantiate(fts_class_t *cl)
{
  binop_number_instantiate(cl, lt_method);
}

static void
ee_instantiate(fts_class_t *cl)
{
  binop_equal_instantiate(cl, ee_method);
}

static void
ne_instantiate(fts_class_t *cl)
{
  binop_equal_instantiate(cl, ne_method);
}

static void
band_instantiate(fts_class_t *cl)
{
  binop_number_instantiate(cl, band_method);
}

static void
bor_instantiate(fts_class_t *cl)
{
  binop_number_instantiate(cl, bor_method);
}

static void
land_instantiate(fts_class_t *cl)
{
  binop_number_instantiate(cl, land_method);
}

static void
lor_instantiate(fts_class_t *cl)
{
  binop_number_instantiate(cl, lor_method);
}

static void
lshift_instantiate(fts_class_t *cl)
{
  binop_number_instantiate(cl, lshift_method);
}

static void
rshift_instantiate(fts_class_t *cl)
{
  binop_number_instantiate(cl, rshift_method);
}

static void
mod_instantiate(fts_class_t *cl)
{
  binop_number_instantiate(cl, mod_method);
}

void
binop_config(void)
{
  fts_class_t *cl_bus, *cl_vid;

  fts_class_install(fts_new_symbol("+"), add_instantiate);
  fts_class_install(fts_new_symbol("*"), mul_instantiate);
  fts_class_install(fts_new_symbol("/"), div_instantiate);
  fts_class_install(fts_new_symbol("-"), sub_instantiate);
  cl_bus = fts_class_install(fts_new_symbol("-+"), bus_instantiate);
  cl_vid = fts_class_install(fts_new_symbol("/*"), vid_instantiate);

  /* compatibility */
  fts_class_alias(cl_bus, fts_new_symbol("inv+"));
  fts_class_alias(cl_vid, fts_new_symbol("inv*"));
   
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
