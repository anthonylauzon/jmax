/*
 *                      Copyright (c) 1995 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *      $Revision: 1.1 $ IRCAM $Date: 1998/09/19 14:36:08 $
 *
 * Eric Viara for Ircam <February 1995>
 *
 */

#include "fts.h"

#define is_ibinop(ac, at)    (((ac) == 1) || (((ac) == 2) && fts_is_long(&(at)[1])))

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



static void
ibinop_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->operator = (long) fts_get_number(at);
}


static void
ibinop_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = (long) fts_get_int_arg(ac, at, 0, 0);
}


static void
ibinop_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (ac >= 2)
    if (fts_is_long(&at[1]) || fts_is_float(&at[1]))
      ibinop_number_1(o, winlet, s, 1, at + 1);

  if (ac >= 1)
    if (fts_is_long(&at[0]))
      fts_message_send(o, 0, fts_s_int, 1, at);
    else if (fts_is_float(&at[0]))
      fts_message_send(o, 0, fts_s_float, 1, at);
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
  if ((ac == 1) || ((ac == 2) && fts_is_long(&at[1])))
    {
      fts_symbol_t a[2];

      fts_class_init(cl, sizeof(ibinop_t), 2, 1, 0);

      /* system inlet */

      a[0] = fts_s_symbol;
      a[1] = fts_s_int;
      fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, ibinop_init, 2, a, 1);

      /* inlet #0 */

      a[0] = fts_s_number;
      fts_method_define(cl, 0, fts_new_symbol("set"), ibinop_set, 1, a);

      a[0] = fts_s_int;
      fts_method_define(cl, 0, fts_s_int, number_meth, 1, a);

      a[0] = fts_s_float;
      fts_method_define(cl, 0, fts_s_float, number_meth, 1, a);

      fts_method_define(cl, 0, fts_s_bang, bang_meth, 0, 0);

      fts_method_define_varargs(cl, 0, fts_s_list, ibinop_list);

      /* inlet #1 */

      a[0] = fts_s_float;
      fts_method_define(cl, 1, fts_s_float, ibinop_number_1, 1, a);

      a[0] = fts_s_int;
      fts_method_define(cl, 1, fts_s_int, ibinop_number_1, 1, a);

      /* outlet #0 */

      a[0] = fts_s_int;
      fts_outlet_type_define(cl, 0, fts_s_int, 1, a);
      
      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}


static void
fbinop_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fbinop_t *this = (fbinop_t *)o;

  this->operator = (float) fts_get_number(at);
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
iadd_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = (long) fts_get_number(at);
  fts_outlet_int(o, 0, this->value + this->operator);
}


static void
iadd_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, this->value + this->operator);
}


static fts_status_t
iadd_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return ibinop_instantiate(cl, ac, at, iadd_bang, iadd_number);
}


static void
imul_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number(at);
  fts_outlet_int(o, 0, this->value * this->operator);
}


static void
imul_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac,	const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, this->value * this->operator);
}


static fts_status_t
imul_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return ibinop_instantiate(cl, ac, at, imul_bang, imul_number);
}


static void
idiv_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number(at);

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


static fts_status_t
idiv_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return ibinop_instantiate(cl, ac, at, idiv_bang, idiv_number);
}


static void
isub_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number(at);
  fts_outlet_int(o, 0, this->value - this->operator);
}

static void
isub_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac,
	const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, this->value - this->operator);
}


static fts_status_t
isub_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return ibinop_instantiate(cl, ac, at, isub_bang, isub_number);
}


static void
ibus_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number(at);
  fts_outlet_int(o, 0, this->operator - this->value);
}

static void
ibus_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac,
	const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, this->operator - this->value);
}


static fts_status_t
ibus_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return ibinop_instantiate(cl, ac, at, ibus_bang, ibus_number);
}


static void
ivid_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number(at);
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


static fts_status_t
ivid_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return ibinop_instantiate(cl, ac, at, ivid_bang, ivid_number);
}


static void
ige_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac,
       const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number(at);
  fts_outlet_int(o, 0, this->value >= this->operator);
}



static void
ige_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac,
       const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, this->value >= this->operator);
}


static fts_status_t
ige_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return ibinop_instantiate(cl, ac, at, ige_bang, ige_number);
}


static void
ile_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number(at);
  fts_outlet_int(o, 0, this->value <= this->operator);
}


static void
ile_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, this->value <= this->operator);
}


static fts_status_t
ile_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return ibinop_instantiate(cl, ac, at, ile_bang, ile_number);
}


static void
igt_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number(at);
  fts_outlet_int(o, 0, this->value > this->operator);
}


static void
igt_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, this->value > this->operator);
}

static fts_status_t
igt_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return ibinop_instantiate(cl, ac, at, igt_bang, igt_number);
}


static void
ilt_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number(at);
  fts_outlet_int(o, 0, this->value < this->operator);
}


static void
ilt_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, this->value < this->operator);
}


static fts_status_t
ilt_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return ibinop_instantiate(cl, ac, at, ilt_bang, ilt_number);
}


static void
iee_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number(at);
  fts_outlet_int(o, 0, this->value == this->operator);
}

static void
iee_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, this->value == this->operator);
}

static fts_status_t
iee_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return ibinop_instantiate(cl, ac, at, iee_bang, iee_number);
}


static void
ine_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number(at);
  fts_outlet_int(o, 0, this->value != this->operator);
}

static void
ine_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, this->value != this->operator);
}

static fts_status_t
ine_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return ibinop_instantiate(cl, ac, at, ine_bang, ine_number);
}


static void
imin_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number(at);
  fts_outlet_int(o, 0, (this->value < this->operator)? this->value: this->operator);
}

static void
imin_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, (this->value < this->operator)? this->value: this->operator);
}

static fts_status_t
imin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return ibinop_instantiate(cl, ac, at, imin_bang, imin_number);
}

static void
imax_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number(at);
  fts_outlet_int(o, 0, (this->value > this->operator)? this->value: this->operator);
}

static void
imax_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  fts_outlet_int(o, 0, (this->value > this->operator)? this->value: this->operator);
}

static fts_status_t
imax_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return ibinop_instantiate(cl, ac, at, imax_bang, imax_number);
}

/* Binops that are only integers */

static void
ba_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number(at);
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


static void
bo_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number(at);
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


static void
la_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number(at);
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


static void
lo_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number(at);
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

static void
pc_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number(at);

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

  this->value = fts_get_number(at);
  fts_outlet_int(o, 0, this->value << this->operator);
}


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


static void
rs_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ibinop_t *this = (ibinop_t *)o;

  this->value = fts_get_number(at);
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
ibinop_config(void)
{
  /* integer components of generic multiclasses */

  fts_metaclass_create(fts_new_symbol("+"), iadd_instantiate, fts_arg_type_equiv);
  fts_metaclass_create(fts_new_symbol("*"), imul_instantiate, fts_arg_type_equiv);
  fts_metaclass_create(fts_new_symbol("/"), idiv_instantiate, fts_arg_type_equiv);
  fts_metaclass_create(fts_new_symbol("-"), isub_instantiate, fts_arg_type_equiv);
  fts_metaclass_create(fts_new_symbol("inv+"), ibus_instantiate, fts_arg_type_equiv);
  fts_metaclass_create(fts_new_symbol("inv*"), ivid_instantiate, fts_arg_type_equiv);

  fts_metaclass_create(fts_new_symbol(">="), ige_instantiate,  fts_arg_type_equiv);
  fts_metaclass_create(fts_new_symbol("<="), ile_instantiate,  fts_arg_type_equiv);
  fts_metaclass_create(fts_new_symbol(">"), igt_instantiate,  fts_arg_type_equiv);
  fts_metaclass_create(fts_new_symbol("<"), ilt_instantiate,  fts_arg_type_equiv);
  fts_metaclass_create(fts_new_symbol("=="), iee_instantiate,  fts_arg_type_equiv);
  fts_metaclass_create(fts_new_symbol("!="), ine_instantiate,  fts_arg_type_equiv);
  
  fts_metaclass_create(fts_new_symbol("minimum"), imin_instantiate,  fts_arg_type_equiv);
  fts_metaclass_create(fts_new_symbol("maximum"), imax_instantiate,  fts_arg_type_equiv);

  /* integer only */

  fts_metaclass_create(fts_new_symbol("&"), ba_instantiate,  fts_arg_type_equiv);
  fts_metaclass_create(fts_new_symbol("|"), bo_instantiate,  fts_arg_type_equiv);
  fts_metaclass_create(fts_new_symbol("&&"), la_instantiate,  fts_arg_type_equiv);
  fts_metaclass_create(fts_new_symbol("||"), lo_instantiate,  fts_arg_type_equiv);
  fts_metaclass_create(fts_new_symbol("%"), pc_instantiate,  fts_arg_type_equiv);
  fts_metaclass_create(fts_new_symbol("<<"), ls_instantiate,  fts_arg_type_equiv);
  fts_metaclass_create(fts_new_symbol(">>"), rs_instantiate,  fts_arg_type_equiv);
}





