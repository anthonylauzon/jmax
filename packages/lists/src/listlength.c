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

#define MAX_length 128


typedef struct 
{
  fts_object_t ob;	 
} listlength_t;

static void
listlength_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_int(o, 0, ac);
}

static fts_status_t
listlength_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(listlength_t), 1, 1, 0); 

  /* list args */

  fts_method_define_varargs(cl, 0, fts_s_list, listlength_list);

  /* Type the outlet */

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0,	fts_s_int, 1, a);

  return fts_Success;
}

void
listlength_config(void)
{
  fts_metaclass_create(fts_new_symbol("listlength"), listlength_instantiate, fts_always_equiv);
}





