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

typedef struct gate
{
  fts_object_t o;
  int opened;
} gate_t;

static void gate_open(fts_object_t *o, int winlet, fts_symbol_t s,
		      int ac, const fts_atom_t *at)
{
  ((gate_t *)o)->opened = fts_get_int_arg(ac, at, 0, 0);
}

static void gate_realize(fts_object_t *o, int winlet, fts_symbol_t s,
			 int ac, const fts_atom_t *at)
{
  if (((gate_t *)o)->opened)
    fts_outlet_send(o, 0, s, ac, at);
}

static void
gate_init(fts_object_t *o, int winlet, fts_symbol_t s,
	  int ac, const fts_atom_t *at)
{
  ((gate_t *)o)->opened = fts_get_long_arg(ac, at, 1, 0);
}

static fts_status_t
gate_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(gate_t), 2, 1, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, gate_init, 2, a, 1);


  a[0] = fts_s_number;
  fts_method_define(cl, 0, fts_s_anything, gate_open, 1, a);

  fts_method_define_varargs(cl, 1, fts_s_anything, gate_realize);

  return fts_Success;
}

void
gate_config(void)
{
  fts_class_install(fts_new_symbol("gate"),gate_instantiate);
}
