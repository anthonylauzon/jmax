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

typedef struct 
{
  fts_object_t o;
} listhead_t;

static void
listhead_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(ac > 0)
    if (fts_is_float(at))
      fts_outlet_send(o, 0, fts_s_float, 1, at);
    else if (fts_is_long(at))
      fts_outlet_send(o, 0, fts_s_int, 1, at);
    else if (fts_is_symbol(at))
      fts_outlet_send(o, 0, fts_s_symbol, 1, at);
}

static fts_status_t
listhead_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(listhead_t), 1, 1, 0); 

  /* define the methods */

  fts_method_define_varargs(cl, 0, fts_s_list, listhead_list);

  return fts_Success;
}

void
listhead_config(void)
{
  fts_class_install(fts_new_symbol("listhead"), listhead_instantiate);
}





