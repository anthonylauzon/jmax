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

#define LO 0
#define HI 1

typedef struct abs_t {
  fts_object_t _o;
} abs_t;

#define abs(x) ((x) > 0 ? (x) : -(x))

static void
abs_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_int(o, 0, abs(fts_get_long(at)));
}

static void
abs_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_float(o, 0, abs(fts_get_float(at)));
}

static fts_status_t
abs_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(abs_t), 1, 1, 0);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, abs_float, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, abs_int, 1, a);

  return fts_Success;
}

void
abs_config(void)
{
  fts_class_install(fts_new_symbol("abs"), abs_instantiate);
}
