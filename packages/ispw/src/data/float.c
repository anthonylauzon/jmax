/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */

/* --------------------- metaclass float: float.c ---------------------- */

/* -------------------------- include files --------------------------------

   "fts.h"      defines all basic types and function prototypes

                 AND utility macros such as fts_get_long_arg()
   	         and useful symbol constants such as fts_s_int, fts_s_float ...

                 AND the macro versions of the most used message passing
		 functions: fts_outlet_send(), fts_outlet_int() and so on...
		 the macros will be defined if and only if the flag
		 OPTIMIZE is set at compilation time
*/

#include "fts.h"

/* ---------------------- the float object type ---------------------------

   a fts type object must be a structure which includes a 'fts_object_t' field
   at its first position; this is done to simulate the inheritance
*/

typedef struct {
  fts_object_t o;		/* fts_object_t inheritance */
  float f;			/* the accumulator of a float object */
} float_t;

/* ---------------------- forward declarations ---------------------------- */

static void float_bang(fts_object_t *, int, fts_symbol_t , int, const fts_atom_t *);
static void float_number(fts_object_t *, int, fts_symbol_t , int,  const fts_atom_t *);
static void float_number_1(fts_object_t *, int, fts_symbol_t , int, const fts_atom_t *);
static void float_list(fts_object_t *, int, fts_symbol_t , int, const fts_atom_t *);
static void float_init(fts_object_t *, int, fts_symbol_t , int, const fts_atom_t *);
static fts_status_t float_instantiate(fts_class_t *, int, const fts_atom_t *);

/* ---------------- the metaclass configuration function ------------------ */

/* takes no arguments, returns a status */

void
float_config(void)
{
  fts_class_install(fts_new_symbol("float"),float_instantiate);

  fts_class_alias(fts_new_symbol("f"), fts_new_symbol("float"));

}

/* ---------------- the metaclass instantiation function ------------------ */

/* fts_class_t *cl: the pointer to the class being instantiated
   int ac, const fts_atom_t *at: the instantiation arguments
   they are dummy as this function do not take them into account
*/

static fts_status_t
float_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  /* initialize the class */
  fts_class_init(cl, sizeof(float_t), 2, 1, 0);


  /* define message template entries */

  a[0] = fts_s_symbol;
  a[1] = fts_s_number;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, float_init, 2, a, 1);

  fts_method_define(cl, 0, fts_s_bang, float_bang, 0, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, float_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, float_number, 1, a);

  fts_method_define_varargs(cl, 0, fts_s_list, float_list);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, float_number_1, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, float_number_1, 1, a);

  /* defined outlet selector */

  a[0] = fts_s_float;
  fts_outlet_type_define(cl, 0, fts_s_float, 1, a);

  return fts_Success;
}

/* ------------------------- the $init method --------------------------- */

static void
float_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  float_t *this = (float_t *) o;

  this->f = fts_get_float_arg(ac, at, 1, 0.0f);
}

/* ------------------------- the bang method ----------------------------- */

/* the bang method simply send the accumulator value to the outlet of
   the object */

static void
float_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  float_t *this = (float_t *) o;

  fts_outlet_float(o, 0, this->f);
}

/* ------------------------- the number method ----------------------------- */

/* the int method put the received value into the object accumulator and
   sent what it receives to its outlet */

static void
float_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  float_t *this = (float_t *) o;

  this->f = fts_get_float_arg(ac, at, 0, 0.0f);
  fts_outlet_float(o, 0, this->f);
}

/* the number 1 method simply set its accumulator */

static void
float_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  float_t *this = (float_t *) o;

  this->f = fts_get_float_arg(ac, at, 0, 0.0f);
}


/* in case of list, only the first value is significative */

static void
float_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (fts_is_float(at))
    float_number(o, winlet, fts_s_float, 1, at);
}

