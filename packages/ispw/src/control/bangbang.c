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

/*------------------------- bangbang class -------------------------------------*/

typedef struct
{
  fts_object_t o;
} bangbang_t;

static void
bangbang_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int i;

  for (i = o->cl->noutlets - 1; i >= 0; i--)
    fts_outlet_send(o, i, fts_s_bang, 0, 0);
}


static fts_status_t
bangbang_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int i;
  int noutlets;
  fts_symbol_t a[1];

  if ((ac >= 2)  && fts_is_long(&at[1]))
    noutlets = fts_get_long(&at[1]);
  else
    noutlets = 2;

  fts_class_init(cl, sizeof(bangbang_t), 1, noutlets, 0);

  fts_method_define_varargs(cl, 0, fts_s_anything, bangbang_bang);

  for (i = 0; i < noutlets; i++)
    fts_outlet_type_define(cl, i, fts_s_bang, 0, 0);

  return fts_Success;
}

void
bangbang_config(void)
{
  fts_metaclass_install(fts_new_symbol("bangbang"),bangbang_instantiate, fts_arg_equiv);
}
