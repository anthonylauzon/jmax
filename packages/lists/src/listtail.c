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

typedef struct 
{
  fts_object_t o;
} listtail_t;

static void
listtail_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(ac > 1)
    fts_outlet_send(o, 0, fts_s_list, ac-1, at+1);
}

static fts_status_t
listtail_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(listtail_t), 1, 1, 0); 

  /* define the methods */

  fts_method_define_varargs(cl, 0, fts_s_list, listtail_list);

  /* Type the outlet */

  fts_outlet_type_define_varargs(cl, 0,	fts_s_list);

  return fts_Success;
}

void
listtail_config(void)
{
  fts_metaclass_create(fts_new_symbol("listtail"), listtail_instantiate, fts_always_equiv);
}





