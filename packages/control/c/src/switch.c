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

/************************************************************
 *
 *  object
 *
 */

typedef struct 
{
  fts_object_t o;
  int on;
} switch_t;

static void
switch_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  switch_t *this = (switch_t *)o;
  
  this->on = fts_get_int_arg(ac, at, 1, 0);
}

static void
switch_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  switch_t *this = (switch_t *)o;
}

/************************************************************
 *
 *  user methods
 *
 */

static void
switch_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  switch_t *this = (switch_t *)o;

  if(this->on != 0)
    fts_outlet_send(o, 0, s, 1, at);
}

static void
switch_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  switch_t *this = (switch_t *)o;

  this->on = fts_get_number_int(at);
}

/************************************************************
 *
 *  class
 *
 */
static fts_status_t
switch_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  fts_class_init(cl, sizeof(switch_t), 2, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, switch_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, switch_delete);

  fts_method_define_varargs(cl, 0, fts_s_anything, switch_input);

  a[0] = fts_s_number;
  fts_method_define(cl, 1, fts_s_int, switch_set, 1, a);
  fts_method_define(cl, 1, fts_s_float, switch_set, 1, a);

  return fts_Success;
}

void
switch_config(void)
{
  fts_metaclass_install(fts_new_symbol("switch"), switch_instantiate, fts_always_equiv);
}
