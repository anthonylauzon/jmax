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
#include <fts/fts.h>
#include "vec.h"
#include "ivec.h"
#include "fvec.h"
#include "mat.h"

#define MAXINT 2147483647
#define MININT -2147483647

#define MAXFLOAT 2147483647
#define MINFLOAT -2147483647

/******************************************************
 *
 *  user methods
 *
 */

static void
getrange_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *vec = ivec_atom_get(at);
  int min = MAXINT;
  int max = MININT;
  int size = ivec_get_size(vec);
  int *ptr = ivec_get_ptr(vec);
  int i;

  for(i=0; i<size; i++)
    {
      int value = ptr[i];

      if(value > max)
	max = value;

      if(value < min)
	min = value;
    }

  fts_outlet_int(o, 1, max);
  fts_outlet_int(o, 0, min);
}

static void
getrange_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *vec = fvec_atom_get(at);
  float min = MAXFLOAT;
  float max = MINFLOAT;
  int size = fvec_get_size(vec);
  float *ptr = fvec_get_ptr(vec);
  int i;

  for(i=0; i<size; i++)
    {
      float value = ptr[i];

      if(value > max)
	max = value;

      if(value < min)
	min = value;
    }

  fts_outlet_float(o, 1, max);
  fts_outlet_float(o, 0, min);
}

/******************************************************
 *
 *  class
 *
 */

static fts_status_t
getrange_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  if(ac == 1)
    {
      /* initialize the class */
      fts_class_init(cl, sizeof(fts_object_t), 1, 1, 0); 
      
      fts_method_define_varargs(cl, 0, ivec_symbol, getrange_ivec);
      fts_method_define_varargs(cl, 0, fvec_symbol, getrange_fvec);
      
      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}

void
getrange_config(void)
{
  fts_class_install(fts_new_symbol("getrange"), getrange_instantiate);
}
