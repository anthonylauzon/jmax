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


#include <fts/fts.h>

#define is_ibinop(ac, at)    (((ac) == 1) || (((ac) == 2) && fts_is_long(&(at)[1])))
#define is_fbinop(ac, at)    (((ac) == 2) && fts_is_float(&(at)[1]))
#define is_sbinop(ac, at)    (((ac) == 2) && fts_is_symbol(&(at)[1]))

typedef struct
{
  fts_object_t o;
  long operator;
  long value;
} ibinop_t;


typedef struct
{
  fts_object_t o;
  float value;
  float operator;
} fbinop_t;

typedef struct
{
  fts_object_t o;
  fts_symbol_t operator;
  fts_symbol_t value;
} sbinop_t;

/**************************************************************************************
 *
 *  generic methods for integer binops
 *
 *    set_right
 *    set_left
 *    list
 *    init
 *    instantiate
 *
 */

static void
ibinop_set_right(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->operator = (long) fts_get_number_int(at);
}


static void
ibinop_set_left(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = (long) fts_get_int_arg(ac, at, 0, 0);
}


static void
ibinop_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (ac >= 2)
    if (fts_is_long(&at[1]) || fts_is_float(&at[1]))
      ibinop_set_right(o, winlet, s, 1, at + 1);

  if (ac >= 1)
    {
      if (fts_is_long(&at[0]))
	fts_message_send(o, 0, fts_s_int, 1, at);
      else if (fts_is_float(&at[0]))
	fts_message_send(o, 0, fts_s_float, 1, at);
    }
}


static void
ibinop_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = 0;
  this->operator = fts_get_long_arg(ac, at, 1, 0);
}


static fts_status_t
ibinop_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at,
		   fts_method_t bang_meth,
		   fts_method_t number_meth)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(ibinop_t), 2, 1, 0);

  /* system inlet */

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, ibinop_init, 2, a, 1);

  /* inlet #0 */

  a[0] = fts_s_number;
  fts_method_define(cl, 0, fts_new_symbol("set"), ibinop_set_left, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, number_meth, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, number_meth, 1, a);

  fts_method_define(cl, 0, fts_s_bang, bang_meth, 0, 0);

  fts_method_define_varargs(cl, 0, fts_s_list, ibinop_list);

  /* inlet #1 */

  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, ibinop_set_right, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, ibinop_set_right, 1, a);

  /* outlet #0 */

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0, fts_s_int, 1, a);
      
  return fts_Success;
}


/**************************************************************************************
 *
 *  generic methods for integer binops
 *
 *    set_right
 *    set_left
 *    list
 *    init
 *    instantiate
 *
 */

static void
fbinop_set_right(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  this->operator = (float) fts_get_number_float(at);
}


static void
fbinop_set_left(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  this->value = (float) fts_get_float_arg(ac, at, 0, 0.0f);
}


static void
fbinop_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (ac >= 2)
    if (fts_is_long(&at[1]) || fts_is_float(&at[1]))
      fbinop_set_right(o, winlet, s, 1, at + 1);

  if (ac >= 1)
    {
      if (fts_is_long(&at[0]))
	fts_message_send(o, 0, fts_s_int, 1, at);
      else if (fts_is_float(&at[0]))
	fts_message_send(o, 0, fts_s_float, 1, at);
    }
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
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(fbinop_t), 2, 1, 0);

  /* inlet system */

  a[0] = fts_s_symbol;
  a[1] = fts_s_float;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, fbinop_init, 2, a);

  /* inlet #0 */
  a[0] = fts_s_number;
  fts_method_define(cl, 0, fts_new_symbol("set"), fbinop_set_left, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, number_meth, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, number_meth, 1, a);
      
  fts_method_define_varargs(cl, 0, fts_s_list, fbinop_list);
  
  fts_method_define(cl, 0, fts_s_bang, bang_meth, 0, 0);

  /* inlet #1 */
  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, fbinop_set_right, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, fbinop_set_right, 1, a);

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


/**************************************************************************************
 *
 *  generic methods for symbol binops
 *
 *    set_right
 *    set_left
 *    list
 *    init
 *    instantiate
 *
 */

static void
sbinop_set_right(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sbinop_t *this = (sbinop_t *)o;

  this->operator = fts_get_symbol(at);
}


static void
sbinop_set_left(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sbinop_t *this = (sbinop_t *)o;

  this->value = fts_get_symbol_arg(ac, at, 0, 0);
}


static void
sbinop_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (ac >= 2 && fts_is_symbol(&at[1]))
    sbinop_set_right(o, winlet, s, 1, at + 1);

  if (ac >= 1 && fts_is_symbol(&at[0]))
    fts_message_send(o, 0, fts_s_int, 1, at);
}


static void
sbinop_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  sbinop_t *this = (sbinop_t *)o;

  this->value = 0;
  this->operator = fts_get_symbol_arg(ac, at, 1, 0);
}


static fts_status_t
sbinop_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at,
		   fts_method_t bang_meth,
		   fts_method_t symbol_meth)
{
  if((ac == 2) && fts_is_symbol(&at[1]))
    {
      fts_symbol_t a[2];

      fts_class_init(cl, sizeof(sbinop_t), 2, 1, 0);

      /* system inlet */

      a[0] = fts_s_symbol;
      a[1] = fts_s_symbol;
      fts_method_define(cl, fts_SystemInlet, fts_s_init, sbinop_init, 2, a);

      /* inlet #0 */

      a[0] = fts_s_symbol;
      fts_method_define(cl, 0, fts_new_symbol("set"), sbinop_set_left, 1, a);

      a[0] = fts_s_symbol;
      fts_method_define(cl, 0, fts_s_symbol, symbol_meth, 1, a);

      fts_method_define(cl, 0, fts_s_bang, bang_meth, 0, 0);

      fts_method_define_varargs(cl, 0, fts_s_list, sbinop_list);

      /* inlet #1 */

      a[0] = fts_s_symbol;
      fts_method_define(cl, 1, fts_s_symbol, sbinop_set_right, 1, a);

      /* outlet #0 */

      a[0] = fts_s_int;
      fts_outlet_type_define(cl, 0, fts_s_int, 1, a);
      
      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}

/**************************************************************************************
 *
 *  addition methods
 *
 */

static void
iadd_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = (long) fts_get_number_int(at);
  fts_outlet_int(o, 0, this->value + this->operator);
}


static void
iadd_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, this->value + this->operator);
}

static void
fadd_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  this->value = (float) fts_get_number_float(at);
  fts_outlet_float(o, 0, this->value + this->operator);
}


static void
fadd_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  fts_outlet_float(o, 0, this->value + this->operator);
}


static fts_status_t
add_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if (is_ibinop(ac, at))
    return ibinop_instantiate(cl, ac, at, iadd_bang, iadd_number);
  else if (is_fbinop(ac, at))
    return fbinop_instantiate(cl, ac, at, fadd_bang, fadd_number, fts_s_float);
  else
    return &fts_CannotInstantiate;
}


/**************************************************************************************
 *
 *  substaction methods
 *
 */

static void
isub_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number_int(at);
  fts_outlet_int(o, 0, this->value - this->operator);
}

static void
isub_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac,
	const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, this->value - this->operator);
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
sub_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if (is_ibinop(ac, at))
  return ibinop_instantiate(cl, ac, at, isub_bang, isub_number);
  else if (is_fbinop(ac, at))
    return fbinop_instantiate(cl, ac, at, fsub_bang, fsub_number, fts_s_float);
  else
    return &fts_CannotInstantiate;
}


/**************************************************************************************
 *
 *  multiplication methods
 *
 */

static void
imul_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number_int(at);
  fts_outlet_int(o, 0, this->value * this->operator);
}


static void
imul_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac,	const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, this->value * this->operator);
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
mul_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if (is_ibinop(ac, at))
    return ibinop_instantiate(cl, ac, at, imul_bang, imul_number);
  else if (is_fbinop(ac, at))
    return fbinop_instantiate(cl, ac, at, fmul_bang, fmul_number, fts_s_float);
  else
    return &fts_CannotInstantiate;
}


/**************************************************************************************
 *
 *  division methods
 *
 */

static void
idiv_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number_int(at);

  if (this->operator == 0)
    {
      fts_outlet_int(o, 0, 0);
    }
  else
    fts_outlet_int(o, 0, this->value / this->operator);
}


static void
idiv_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac,	const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  if (this->operator == 0)
    {
      fts_outlet_int(o, 0, 0);
    }
  else
    fts_outlet_int(o, 0, this->value / this->operator);
}


static void
fdiv_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  this->value = (float) fts_get_number_float(at);

  /*  if (this->operator == 0.)
    {
      fts_outlet_float(o, 0, 0.);
    }
  else */
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
div_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if (is_ibinop(ac, at))
    return ibinop_instantiate(cl, ac, at, idiv_bang, idiv_number);
  else if (is_fbinop(ac, at))
    return fbinop_instantiate(cl, ac, at, fdiv_bang, fdiv_number, fts_s_float);
  else
    return &fts_CannotInstantiate;
}


/**************************************************************************************
 *
 *  reversed substaction methods
 *
 */

static void
ibus_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number_int(at);
  fts_outlet_int(o, 0, this->operator - this->value);
}

static void
ibus_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac,
	const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, this->operator - this->value);
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
bus_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if (is_ibinop(ac, at))
    return ibinop_instantiate(cl, ac, at, ibus_bang, ibus_number);
  else if (is_fbinop(ac, at))
    return fbinop_instantiate(cl, ac, at, fbus_bang, fbus_number, fts_s_float);
  else
    return &fts_CannotInstantiate;
}


/**************************************************************************************
 *
 *  reversed division  methods
 *
 */

static void
ivid_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number_int(at);
  if (this->value == 0)
    {
      fts_outlet_int(o, 0, 0);
    }
  else
    fts_outlet_int(o, 0, this->operator / this->value);
}

static void
ivid_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac,
	const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  if (this->value == 0)
    {
      fts_outlet_int(o, 0, 0);
    }
  else
    fts_outlet_int(o, 0, this->operator / this->value);
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
vid_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if (is_ibinop(ac, at))
    return ibinop_instantiate(cl, ac, at, ivid_bang, ivid_number);
  else if (is_fbinop(ac, at))
    return fbinop_instantiate(cl, ac, at, fvid_bang, fvid_number, fts_s_float);
  else
    return &fts_CannotInstantiate;
}


/**************************************************************************************
 *
 *  greater or equal (>=) methods
 *
 */

static void
ige_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac,
       const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number_int(at);
  fts_outlet_int(o, 0, this->value >= this->operator);
}



static void
ige_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac,
       const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, this->value >= this->operator);
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
ge_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if (is_ibinop(ac, at))
    return ibinop_instantiate(cl, ac, at, ige_bang, ige_number);
  else if (is_fbinop(ac, at))
    return fbinop_instantiate(cl, ac, at, fge_bang, fge_number, fts_s_int);
  else
    return &fts_CannotInstantiate;
}


/**************************************************************************************
 *
 *  less or equal (<=) methods
 *
 */

static void
ile_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number_int(at);
  fts_outlet_int(o, 0, this->value <= this->operator);
}


static void
ile_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, this->value <= this->operator);
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
le_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if (is_ibinop(ac, at))
    return ibinop_instantiate(cl, ac, at, ile_bang, ile_number);
  else if (is_fbinop(ac, at))
    return fbinop_instantiate(cl, ac, at, fle_bang, fle_number, fts_s_int);
  else
    return &fts_CannotInstantiate;
}


/**************************************************************************************
 *
 *  greater (>) methods
 *
 */

static void
igt_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number_int(at);
  fts_outlet_int(o, 0, this->value > this->operator);
}


static void
igt_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, this->value > this->operator);
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
gt_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if (is_ibinop(ac, at))
    return ibinop_instantiate(cl, ac, at, igt_bang, igt_number);
  else if (is_fbinop(ac, at))
    return fbinop_instantiate(cl, ac, at, fgt_bang, fgt_number, fts_s_int);
  else
    return &fts_CannotInstantiate;
}


/**************************************************************************************
 *
 *  less (<) methods
 *
 */

static void
ilt_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number_int(at);
  fts_outlet_int(o, 0, this->value < this->operator);
}


static void
ilt_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, this->value < this->operator);
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
lt_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if (is_ibinop(ac, at))
    return ibinop_instantiate(cl, ac, at, ilt_bang, ilt_number);
  else if (is_fbinop(ac, at))
    return fbinop_instantiate(cl, ac, at, flt_bang, flt_number, fts_s_int);
  else
    return &fts_CannotInstantiate;
}


/**************************************************************************************
 *
 *  equal (==) methods
 *
 */

static void
iee_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number_int(at);
  fts_outlet_int(o, 0, this->value == this->operator);
}

static void
iee_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, this->value == this->operator);
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


static void
see_symbol(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sbinop_t *this = (sbinop_t *)o;

  this->value = fts_get_symbol(at);
  fts_outlet_int(o, 0, this->value == this->operator);
}

static void
see_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sbinop_t *this = (sbinop_t *)o;

  fts_outlet_int(o, 0, this->value == this->operator);
}

static fts_status_t
ee_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if (is_ibinop(ac, at))
    return ibinop_instantiate(cl, ac, at, iee_bang, iee_number);
  else if (is_fbinop(ac, at))
    return fbinop_instantiate(cl, ac, at, fee_bang, fee_number, fts_s_int);
  else if (is_sbinop(ac, at))
    return sbinop_instantiate(cl, ac, at, see_bang, see_symbol);
  else
    return &fts_CannotInstantiate;
}


/**************************************************************************************
 *
 *  not equal (!=) methods
 *
 */

static void
ine_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number_int(at);
  fts_outlet_int(o, 0, this->value != this->operator);
}

static void
ine_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, this->value != this->operator);
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

static void
sne_symbol(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  sbinop_t *this = (sbinop_t *)o;

  this->value = fts_get_symbol(at);
  fts_outlet_int(o, 0, this->value != this->operator);
}


static void
sne_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sbinop_t *this = (sbinop_t *)o;

  fts_outlet_int(o, 0, this->value != this->operator);
}

static fts_status_t
ne_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if (is_ibinop(ac, at))
    return ibinop_instantiate(cl, ac, at, ine_bang, ine_number);
  else if (is_fbinop(ac, at))
    return fbinop_instantiate(cl, ac, at, fne_bang, fne_number, fts_s_int);
  else if (is_sbinop(ac, at))
    return sbinop_instantiate(cl, ac, at, sne_bang, sne_symbol);
  else
    return &fts_CannotInstantiate;
}


/**************************************************************************************
 *
 *  minimum methods
 *
 */

static void
imin_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number_int(at);
  fts_outlet_int(o, 0, (this->value < this->operator)? this->value: this->operator);
}

static void
imin_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, (this->value < this->operator)? this->value: this->operator);
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
min_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if (is_ibinop(ac, at))
    return ibinop_instantiate(cl, ac, at, imin_bang, imin_number);
  else if (is_fbinop(ac, at))
    return fbinop_instantiate(cl, ac, at, fmin_bang, fmin_number, fts_s_float);
  else
    return &fts_CannotInstantiate;
}

/**************************************************************************************
 *
 *  maximum methods
 *
 */

static void
imax_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number_int(at);
  fts_outlet_int(o, 0, (this->value > this->operator)? this->value: this->operator);
}

static void
imax_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, (this->value > this->operator)? this->value: this->operator);
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
max_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if (is_ibinop(ac, at))
    return ibinop_instantiate(cl, ac, at, imax_bang, imax_number);
  else if (is_fbinop(ac, at))
    return fbinop_instantiate(cl, ac, at, fmax_bang, fmax_number, fts_s_float);
  else
    return &fts_CannotInstantiate;
}

/* Binops that are only integers */

/**************************************************************************************
 *
 *  bitwise AND (&) methods
 *
 */

static void
ba_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number_int(at);
  fts_outlet_int(o, 0, this->value & this->operator);
}


static void
ba_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac,
       const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, this->value & this->operator);
}


static fts_status_t
ba_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return ibinop_instantiate(cl, ac, at, ba_bang, ba_number);
}


/**************************************************************************************
 *
 *  bitwise OR (|) methods
 *
 */

static void
bo_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number_int(at);
  fts_outlet_int(o, 0, this->value | this->operator);
}


static void
bo_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, this->value | this->operator);
}


static fts_status_t
bo_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return ibinop_instantiate(cl, ac, at, bo_bang, bo_number);
}


/**************************************************************************************
 *
 *  logig AND (&&) methods
 *
 */

static void
la_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number_int(at);
  fts_outlet_int(o, 0, this->value && this->operator);
}


static void
la_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, this->value && this->operator);
}

static fts_status_t
la_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return ibinop_instantiate(cl, ac, at, la_bang, la_number);
}


/**************************************************************************************
 *
 *  logic OR (||) methods
 *
 */

static void
lo_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number_int(at);
  fts_outlet_int(o, 0, this->value || this->operator);
}


static void
lo_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, this->value || this->operator);
}


static fts_status_t
lo_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return ibinop_instantiate(cl, ac, at, lo_bang, lo_number);
}

/**************************************************************************************
 *
 *  modulo (%) methods
 *
 */

static void
pc_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number_int(at);

  if (this->operator == 0)
    {
      fts_outlet_int(o, 0, 0);
    }
  else
    fts_outlet_int(o, 0, this->value % this->operator);
}


static void
pc_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac,
       const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  if (this->operator == 0)
    {
      fts_outlet_int(o, 0, 0);
    }
  else
    fts_outlet_int(o, 0, this->value % this->operator);
}


static fts_status_t
pc_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return ibinop_instantiate(cl, ac, at, pc_bang, pc_number);
}


static void
ls_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number_int(at);
  fts_outlet_int(o, 0, this->value << this->operator);
}


/**************************************************************************************
 *
 *  bit shift left (<<) methods
 *
 */

static void
ls_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, this->value << this->operator);
}


static fts_status_t
ls_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return ibinop_instantiate(cl, ac, at, ls_bang, ls_number);
}


/**************************************************************************************
 *
 *  bit shift right (>>) methods
 *
 */

static void
rs_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number_int(at);
  fts_outlet_int(o, 0, this->value >> this->operator);
}


static void
rs_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, this->value >> this->operator);
}


static fts_status_t
rs_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return ibinop_instantiate(cl, ac, at, rs_bang, rs_number);
}

void
binop_config(void)
{
  /* integer components of generic multiclasses */

  fts_metaclass_install(fts_new_symbol("+"), add_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("*"), mul_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("/"), div_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("-"), sub_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("inv+"), bus_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("inv*"), vid_instantiate, fts_arg_type_equiv);

  fts_metaclass_install(fts_new_symbol(">="), ge_instantiate,  fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("<="), le_instantiate,  fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol(">"),  gt_instantiate,  fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("<"),  lt_instantiate,  fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("=="), ee_instantiate,  fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("!="), ne_instantiate,  fts_arg_type_equiv);
  
  fts_metaclass_install(fts_new_symbol("minimum"), min_instantiate,  fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("maximum"), max_instantiate,  fts_arg_type_equiv);

  /* integer only */
  fts_metaclass_install(fts_new_symbol("&"),  ba_instantiate,  fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("|"),  bo_instantiate,  fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("&&"), la_instantiate,  fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("||"), lo_instantiate,  fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("%"),  pc_instantiate,  fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("<<"), ls_instantiate,  fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol(">>"), rs_instantiate,  fts_arg_type_equiv);
}
