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

typedef struct loadbang_t {
  fts_object_t _o;
} loadbang_t;

static void
loadbang_load_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_bang(o, 0);
}

static fts_status_t
loadbang_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(loadbang_t), 1, 1, 0);

  fts_method_define(cl, fts_SystemInlet, fts_new_symbol("load_init"), loadbang_load_init, 0, 0);

  fts_outlet_type_define(cl, 0, fts_s_bang, 0, 0);

  return fts_Success;
}


void
loadbang_config(void)
{
  fts_metaclass_create(fts_new_symbol("loadbang"),loadbang_instantiate, fts_always_equiv);
}
