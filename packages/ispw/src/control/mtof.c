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
/*************** MIDI to frequency convertor *******************/


#include "fts.h"
#include <math.h>

typedef struct 
{
  fts_object_t ob;
} mtof_t;


static float
mtof(float f)
{
  return (440.0f * exp(.057762265f * (f - 69.f)));
}

static void
mtof_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_float(o, 0, mtof(fts_get_float(at)));
}

static void
mtof_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_float(o, 0, mtof((float) fts_get_long(at)));
}

static fts_status_t
mtof_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  /* initialize the class */

  fts_class_init(cl, sizeof(mtof_t), 1, 1, 0); 

  /* methods */

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, mtof_int, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, mtof_float, 1, a);

  /* Type the outlet */

  a[0] = fts_s_float;
  fts_outlet_type_define(cl, 0,	fts_s_float, 1, a);

  return fts_Success;
}

void
mtof_config(void)
{
  fts_class_install(fts_new_symbol("mtof"), mtof_instantiate);
}


