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
/* 
   The time class; return on outlet 0 the logical time of the
   given clock, when it receive the bang.
   Accept an optional argument, declaring the clock used; default
   to ms.

   Don't call this object time_t: it conflict with system types
   in many platforms.
*/

#include "fts.h"

typedef struct 
{
  fts_object_t ob;
  fts_symbol_t clock_sym;
} timeobj_t;

static void
time_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  timeobj_t *x = (timeobj_t *)o;

  fts_outlet_float(o, 0, fts_clock_get_time(x->clock_sym));
}

static void
time_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  timeobj_t *x = (timeobj_t *)o;

  x->clock_sym = fts_get_symbol_arg(ac, at, 1, 0);
}


static fts_status_t
time_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  /* initialize the class */

  fts_class_init(cl, sizeof(timeobj_t), 2, 1, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, time_init, 2, a, 1);

  /* user methods */

  fts_method_define(cl, 0, fts_s_bang, time_bang, 0, 0);

  /* Type the outlet */

  a[0] = fts_s_float;
  fts_outlet_type_define(cl, 0,	fts_s_int, 1, a);

  return fts_Success;
}


void
time_config(void)
{
  fts_metaclass_create(fts_new_symbol("time"),time_instantiate, fts_always_equiv);
}
