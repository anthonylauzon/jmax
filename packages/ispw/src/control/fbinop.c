/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */

#include "fts.h"

typedef struct {
  fts_object_t o;
  float value;
  float operator;
} fbinop_t;


static void
fbinop_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  this->operator = (float) fts_get_number_float(at);
}


static void
fbinop_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  this->value = (float) fts_get_float_arg(ac, at, 0, 0.0f);
}


static void
fbinop_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (ac >= 2)
    if (fts_is_long(&at[1]) || fts_is_float(&at[1]))
      fbinop_number_1(o, winlet, s, 1, at + 1);

  if (ac >= 1)
    if (fts_is_long(&at[0]))
      fts_message_send(o, 0, fts_s_int, 1, at);
    else if (fts_is_float(&at[0]))
      fts_message_send(o, 0, fts_s_float, 1, at);
}


static void
fbinop_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  this->value = 0.0f;
  this->operator = fts_get_float_arg(ac, at, 1, 0.0f);
}


static fts_status_t
fbinop_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at,
		  fts_method_t bang_meth,
		  fts_method_t number_meth,
		  fts_symbol_t outlet_type)
{
  if ((ac == 2) && fts_is_float(&at[1]))
    {
      fts_symbol_t a[2];

      fts_class_init(cl, sizeof(fbinop_t), 2, 1, 0);

      /* inlet system */

      a[0] = fts_s_symbol;
      a[1] = fts_s_float;
      fts_method_define(cl, fts_SystemInlet, fts_s_init, fbinop_init, 2, a);

      /* inlet #0 */
      a[0] = fts_s_number;
      fts_method_define(cl, 0, fts_new_symbol("set"), fbinop_set, 1, a);

      a[0] = fts_s_int;
      fts_method_define(cl, 0, fts_s_int, number_meth, 1, a);

      a[0] = fts_s_float;
      fts_method_define(cl, 0, fts_s_float, number_meth, 1, a);
      
      fts_method_define_varargs(cl, 0, fts_s_list, fbinop_list);

      fts_method_define(cl, 0, fts_s_bang, bang_meth, 0, 0);

      /* inlet #1 */
      a[0] = fts_s_float;
      fts_method_define(cl, 1, fts_s_float, fbinop_number_1, 1, a);

      a[0] = fts_s_int;
      fts_method_define(cl, 1, fts_s_int, fbinop_number_1, 1, a);

      /* outlet #0 */

      if (outlet_type == fts_s_int)
	{
	  a[0] = fts_s_int;
	  fts_outlet_type_define(cl, 0, fts_s_int, 1, a);
	}
      else
	{
	  a[0] = fts_s_float;
	  fts_outlet_type_define(cl, 0, fts_s_float, 1, a);
	}

      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}


static void
fadd_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  this->value = (float) fts_get_number_float(at);
  fts_outlet_float(o, 0, this->value + this->operator);
}


static void
fadd_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac,
	  const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  fts_outlet_float(o, 0, this->value + this->operator);
}


static fts_status_t
fadd_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return fbinop_instantiate(cl, ac, at, fadd_bang, fadd_number, fts_s_float);
}


static void
fmul_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  this->value = (float) fts_get_number_float(at);
  fts_outlet_float(o, 0, this->value * this->operator);
}


static void
fmul_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  fts_outlet_float(o, 0, this->value * this->operator);
}


static fts_status_t
fmul_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return fbinop_instantiate(cl, ac, at, fmul_bang, fmul_number, fts_s_float);
}


static void
fdiv_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  this->value = (float) fts_get_number_float(at);

  if (this->operator == 0.)
    {
      fts_outlet_float(o, 0, 0.);
    }
  else
    fts_outlet_float(o, 0, this->value / this->operator);
}


static void
fdiv_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  if (this->operator == 0.)
    {
      fts_outlet_float(o, 0, 0.);
    }
  else
    fts_outlet_float(o, 0, this->value / this->operator);
}


static fts_status_t
fdiv_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return fbinop_instantiate(cl, ac, at, fdiv_bang, fdiv_number, fts_s_float);
}


static void
fsub_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  this->value = (float) fts_get_number_float(at);
  fts_outlet_float(o, 0, this->value - this->operator);
}


static void
fsub_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  fts_outlet_float(o, 0, this->value - this->operator);
}


static fts_status_t
fsub_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return fbinop_instantiate(cl, ac, at, fsub_bang, fsub_number, fts_s_float);

}


static void
fbus_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  this->value = (float) fts_get_number_float(at);
  fts_outlet_float(o, 0, this->operator - this->value);
}


static void
fbus_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  fts_outlet_float(o, 0, this->operator - this->value);
}


static fts_status_t
fbus_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return fbinop_instantiate(cl, ac, at, fbus_bang, fbus_number, fts_s_float);

}


static void
fvid_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  this->value = (float) fts_get_number_float(at);
  if (this->value == 0.)
    {
      fts_outlet_float(o, 0, 0.);
    }
  else
    fts_outlet_float(o, 0, this->operator / this->value);
}


static void
fvid_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  if (this->value == 0.)
    {
      fts_outlet_float(o, 0, 0.);
    }
  else
    fts_outlet_float(o, 0, this->operator / this->value);
}


static fts_status_t
fvid_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return fbinop_instantiate(cl, ac, at, fvid_bang, fvid_number, fts_s_float);

}


static void
fge_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  this->value = (float) fts_get_number_float(at);
  fts_outlet_int(o, 0, this->value >= this->operator);
}


static void
fge_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  fts_outlet_int(o, 0, this->value >= this->operator);
}


static fts_status_t
fge_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return fbinop_instantiate(cl, ac, at, fge_bang, fge_number, fts_s_int);
}


static void
fle_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  this->value = (float) fts_get_number_float(at);
  fts_outlet_int(o, 0, this->value <= this->operator);
}

static void
fle_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  fts_outlet_int(o, 0, this->value <= this->operator);
}


static fts_status_t
fle_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return fbinop_instantiate(cl, ac, at, fle_bang, fle_number, fts_s_int);
}

static void
fgt_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  this->value = (float) fts_get_number_float(at);
  fts_outlet_int(o, 0, this->value >this->operator);
}

static void
fgt_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  fts_outlet_int(o, 0, this->value >this->operator);
}


static fts_status_t
fgt_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return fbinop_instantiate(cl, ac, at, fgt_bang, fgt_number, fts_s_int);
}


static void
flt_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  this->value = (float) fts_get_number_float(at);
  fts_outlet_int(o, 0, this->value < this->operator);
}


static void
flt_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  fts_outlet_int(o, 0, this->value < this->operator);
}


static fts_status_t
flt_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return fbinop_instantiate(cl, ac, at, flt_bang, flt_number, fts_s_int);
}


static void
fee_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  this->value = (float) fts_get_number_float(at);
  fts_outlet_int(o, 0, this->value == this->operator);
}

static void
fee_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  fts_outlet_int(o, 0, this->value == this->operator);
}


static fts_status_t
fee_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return fbinop_instantiate(cl, ac, at, fee_bang, fee_number, fts_s_int);
}


static void
fne_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  this->value = (float) fts_get_number_float(at);
  fts_outlet_int(o, 0, this->value != this->operator);
}

static void
fne_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  fts_outlet_int(o, 0, this->value != this->operator);
}


static fts_status_t
fne_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return fbinop_instantiate(cl, ac, at, fne_bang, fne_number, fts_s_int);
}

static void
fmin_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  this->value = fts_get_number_float(at);
  fts_outlet_int(o, 0, (this->value < this->operator)? this->value: this->operator);
}

static void
fmin_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  fts_outlet_int(o, 0, (this->value < this->operator)? this->value: this->operator);
}

static fts_status_t
fmin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return fbinop_instantiate(cl, ac, at, fmin_bang, fmin_number, fts_s_float);
}

static void
fmax_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  this->value = fts_get_number_float(at);
  fts_outlet_int(o, 0, (this->value > this->operator)? this->value: this->operator);
}

static void
fmax_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  fts_outlet_int(o, 0, (this->value > this->operator)? this->value: this->operator);
}

static fts_status_t
fmax_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return fbinop_instantiate(cl, ac, at, fmax_bang, fmax_number, fts_s_float);
}

void
fbinop_config(void)
{
 /* float components of generic multiclasses */

  fts_metaclass_install(fts_new_symbol("+"), fadd_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("*"), fmul_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("/"), fdiv_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("-"), fsub_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("inv+"), fbus_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("inv*"), fvid_instantiate, fts_arg_type_equiv);

  fts_metaclass_install(fts_new_symbol(">="), fge_instantiate,  fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("<="), fle_instantiate,  fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol(">"), fgt_instantiate,  fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("<"), flt_instantiate,  fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("=="), fee_instantiate,  fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("!="), fne_instantiate,  fts_arg_type_equiv);

  fts_metaclass_install(fts_new_symbol("minimum"), fmin_instantiate,  fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("maximum"), fmax_instantiate,  fts_arg_type_equiv);
}





