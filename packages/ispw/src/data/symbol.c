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


/* --------------------- metaclass symbol: symbol.c ---------------------- */

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

/* ---------------------- the symbol object type ---------------------------

   a fts type object must be a structure which includes a 'fts_object_t' field
   at its first position; this is done to simulate the inheritance
*/

typedef struct {
  fts_object_t o;		/* fts_object_t inheritance */
  fts_symbol_t s;		/* the accumulator of a symbol object */
} symbol_obj_t;

/* ---------------------- forward declarations ---------------------------- */

static void symbol_obj_bang(fts_object_t *, int, fts_symbol_t , int, const fts_atom_t *);
static void symbol_obj_symbol(fts_object_t *, int, fts_symbol_t , int,  const fts_atom_t *);
static void symbol_obj_symbol_1(fts_object_t *, int, fts_symbol_t , int, const fts_atom_t *);
static void symbol_obj_list(fts_object_t *, int, fts_symbol_t , int, const fts_atom_t *);
static void symbol_obj_init(fts_object_t *, int, fts_symbol_t , int, const fts_atom_t *);
static fts_status_t symbol_obj_instantiate(fts_class_t *, int, const fts_atom_t *);
static void symbol_obj_get_state(fts_daemon_action_t action, fts_object_t *obj,
			    fts_symbol_t property, fts_atom_t *value);

/* ---------------- the metaclass configuration function ------------------ */

/* takes no arguments, returns a status */

void
symbol_obj_config(void)
{
  fts_class_install(fts_new_symbol("symbol"),symbol_obj_instantiate);
}

/* ---------------- the metaclass instantiation function ------------------ */

/* fts_class_t *cl: the pointer to the class being instantiated
   int ac, const fts_atom_t *at: the instantiation arguments
   they are dummy as this function do not take them into account
*/

static fts_status_t
symbol_obj_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  /* initialize the class */

  fts_class_init(cl, sizeof(symbol_obj_t), 2, 1, 0);

  /* define message template entries */

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, symbol_obj_init, 2, a, 1);

  fts_method_define(cl, 0, fts_s_bang, symbol_obj_bang, 0, 0);

  a[0] = fts_s_symbol;
  fts_method_define(cl, 0, fts_s_int, symbol_obj_symbol, 1, a);

  fts_method_define_varargs(cl, 0, fts_s_list, symbol_obj_list);

  a[0] = fts_s_symbol;
  fts_method_define(cl, 1, fts_s_int, symbol_obj_symbol_1, 1, a);

  /* defined outlet selector */

  a[0] = fts_s_symbol;
  fts_outlet_type_define(cl, 0, fts_s_symbol, 1, a);

  return fts_Success;
}

/* ------------------------- the $init method --------------------------- */

static void
symbol_obj_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  symbol_obj_t *this = (symbol_obj_t *) o;

  this->s = fts_get_symbol_arg(ac, at, 1, 0);
}

/* ------------------------- the bang method ----------------------------- */

/* the bang method simply send the accumulator value to the outlet of
   the object */

static void
symbol_obj_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  symbol_obj_t *this = (symbol_obj_t *) o;

  fts_outlet_symbol(o, 0, this->s);
}

/* ------------------------- the number method ----------------------------- */

/* the symbol method put the received value into the object accumulator and
   sent what it receives to its outlet */

static void
symbol_obj_symbol(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  symbol_obj_t *this = (symbol_obj_t *) o;

  this->s = fts_get_symbol_arg(ac, at, 0, 0);
  fts_outlet_symbol(o, 0, this->s);
}

/* the symbol 1 method simply set its accumulator */

static void
symbol_obj_symbol_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  symbol_obj_t *this = (symbol_obj_t *) o;

  this->s = fts_get_symbol_arg(ac, at, 0, 0);
}


/* in case of list, only the first value is significative */

static void
symbol_obj_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (fts_is_symbol(at))
    symbol_obj_symbol(o, winlet, fts_s_symbol, 1, at);
}

