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


#include "fts.h"

typedef struct random
{
  fts_object_t o;
  long r_n;
} random_t;

static void
random_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  extern int rand (void);
  fts_atom_t a;

  fts_set_long(&a, (((random_t *)o)->r_n * (rand() & 0x7fffL)) >> 15);
  fts_outlet_send(o, 0, fts_s_int, 1, &a);
}

static void
random_in1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  long n = fts_get_int_arg(ac, at, 0, 0);

  if (n <= 1)
    n = 1;

  ((random_t *)o)->r_n = n;
}

static void
random_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  long n = fts_get_long_arg(ac, at, 1, 0);

  if (n < 1)
    n = 1;
  ((random_t *)o)->r_n = n;
}

static fts_status_t
random_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(random_t), 2, 1, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, random_init, 2, a, 1);

  fts_method_define(cl, 0, fts_s_bang, random_bang, 0, 0);

  a[0] = fts_s_number;
  fts_method_define_optargs(cl, 1, fts_s_int, random_in1, 1, a, 0);

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0, fts_s_int, 1, a);
  return fts_Success;
}

void
random_config(void)
{
  fts_metaclass_create(fts_new_symbol("random"),random_instantiate, fts_always_equiv);
}
