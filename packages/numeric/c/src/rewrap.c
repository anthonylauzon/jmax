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

typedef struct {
  fts_object_t o;
  float min;
  float max;
} rewrap_t;

static void
rewrap_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  rewrap_t *this = (rewrap_t *)o;
  float a = fts_get_float_arg(ac, at, 1, 0.0f);
  float b = fts_get_float_arg(ac, at, 2, 0.0f);

  if(a <= b)
    {
      this->min = a;
      this->max = b;
    }
  else
    {
      this->min = b;
      this->max = a;
    }
}

static void
rewrap_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  rewrap_t *this = (rewrap_t *)o;
}

/************************************************************
 *
 *  user methods
 *
 */

static void
rewrap_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  rewrap_t *this = (rewrap_t *)o;
  float f = fts_get_number_float(at);
  float min = this->min;
  float max = this->max;
  float range = 2.0f * (max - min);
  int i;

  if(range == 0.0f)
    {
      f = min;
      fts_outlet_int(o, 1, 0);
    }
  else if(f < min)
    {
      float diff = min - f;
      float n = ceil(diff / range);

      f += n * range;

      if(f >= max)
	{
	  f = 2 * max - f;
	  n -= 0.5;
	}

      fts_outlet_int(o, 1, (int)(-2.0f * n));
    }
  else if (f >= max)
    {
      float diff = f - max;
      float n = floor(diff / range) + 1.0f;

      f -= n * range;

      if(f < min)
	{
	  f = 2 * min - f;
	  n -= 0.5;
	}

      fts_outlet_int(o, 1, (int)(2.0f * n));
    }
  else
    fts_outlet_int(o, 1, 0.0f);
    

  fts_outlet_float(o, 0, f);
}

static void
rewrap_a(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  rewrap_t *this = (rewrap_t *)o;
  float a = fts_get_float_arg(ac, at, 0, 0.0f);
  float max = this->max;

  if(a <= max)
    this->min = a;
  else
    {
      this->min = max;
      this->max = a;
    }
}

static void
rewrap_b(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  rewrap_t *this = (rewrap_t *)o;
  float b = fts_get_float_arg(ac, at, 0, 0.0f);
  float min = this->min;

  if(b >= min)
    this->max = b;
  else
    {
      this->max = min;
      this->min = b;
    }
}

/************************************************************
 *
 *  class
 *
 */
static fts_status_t
rewrap_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  fts_class_init(cl, sizeof(rewrap_t), 3, 2, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, rewrap_init);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, rewrap_delete, 0, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, rewrap_number, 1, a);
  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, rewrap_number, 1, a);

  a[0] = fts_s_number;
  fts_method_define(cl, 1, fts_s_int, rewrap_a, 1, a);
  fts_method_define(cl, 1, fts_s_float, rewrap_a, 1, a);

  a[0] = fts_s_number;
  fts_method_define(cl, 2, fts_s_int, rewrap_b, 1, a);
  fts_method_define(cl, 2, fts_s_float, rewrap_b, 1, a);

  return fts_Success;
}

void
rewrap_config(void)
{
  fts_metaclass_install(fts_new_symbol("rewrap"), rewrap_instantiate, fts_narg_equiv);
}
