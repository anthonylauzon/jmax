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
#include "sampbuf.h"
#include "sampfilt.h"


/******************************************************************
 *
 *    object
 *
 */
 
typedef struct{
  fts_object_t _o;
  fts_symbol_t tab_name;
  fts_symbol_t unit;
  float conv;
} obj_t;

static void
obj_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  obj_t *obj = (obj_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 1, 0);
  fts_symbol_t unit = fts_unit_get_samples_arg(ac, at, 2, 0);
  float sr;

  obj->tab_name = name;

  if (! unit)
    unit = fts_s_msec; /* default */

  sr = fts_param_get_float(fts_s_sampling_rate, 44100.);
  obj->conv = fts_unit_convert_to_base(unit, 1.0f, &sr);
}

/******************************************************************
 *
 *    user methods
 *
 */
 
static void
meth_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  obj_t *obj = (obj_t *)o;
  float sr;

  sr = fts_param_get_float(fts_s_sampling_rate, 44100.);
  obj->conv = fts_unit_convert_to_base(obj->unit, 1.0f, &sr);
}

static void
meth_float(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  obj_t *obj = (obj_t *)o;
  float f = fts_get_float(at);
  sampbuf_t *buf = sampbuf_get(obj->tab_name);


  if (buf)
    {
      float onset = obj->conv * f;
      long index = onset;
      long frac = SAMP_NPOINTS * (onset - index);

      if (onset < 0 || index >= buf->size)
	fts_outlet_float(o, 0, 0.0f);
      else
	{
	  if (frac)
	    {
	      sampfilt_t *t = sampfilt_tab + frac;
	      if (index)
		{
		  float *samp = buf->samples + index - 1;
		  fts_outlet_float(o, 0,
				   t->f1 * samp[0] + t->f2 * samp[1] + t->f3 * samp[2] + t->f4 * samp[3]);
		}
	      else
		{
		  float *samp = buf->samples + index - 1;
		  fts_outlet_float(o, 0,
				   t->f1 * samp[1] + t->f2 * samp[1] + t->f3 * samp[2] + t->f4 * samp[3]);
		}
	  }
	  else
	    fts_outlet_float(o, 0, buf->samples[index]);
	}
    }
  else
    post("samppeek: can't find table~: %s\n", fts_symbol_name(obj->tab_name));
}

static void
meth_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t a;
  fts_set_float(&a, (float)fts_get_long(at));
  meth_float(o, winlet, s, 1, &a);
}

static void
meth_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  obj_t *obj = (obj_t *)o;
  fts_symbol_t tab_name = fts_get_symbol_arg(ac, at, 0, 0);

  if (sampbuf_get(tab_name))
    obj->tab_name = tab_name;
  else
    post("samppeek: can't find table~: %s\n", fts_symbol_name(tab_name));
}

static void
meth_set_by_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  char buf[64];
  fts_atom_t a;

  gensampname(buf, "sample", fts_get_long_arg(ac, at, 0, 0));
  fts_set_symbol(&a, fts_new_symbol_copy(buf));
  meth_set(o, winlet, s, 1, &a);
}

/******************************************************************
 *
 *    class
 *
 */
 
static fts_status_t
class_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  fts_class_init(cl, sizeof(obj_t), 2, 1, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  a[2] = fts_s_symbol;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, obj_init, 3, a, 2);

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define_optargs(cl, 0,  fts_new_symbol("init"), meth_init, 2, a, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, meth_int, 1, a);
  
  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, meth_float, 1, a);
  
  a[0] = fts_s_symbol;
  fts_method_define(cl, 0, fts_s_set, meth_set, 1, a);
  
  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, meth_set_by_int, 1, a);
  
  a[0] = fts_s_float;
  fts_outlet_type_define(cl, 0, fts_s_float, 1, a);

  return fts_Success;
}

void
samppeek_config(void)
{
  fts_class_install(fts_new_symbol("samppeek"), class_instantiate);
  fts_class_alias(fts_new_symbol("samppeek~"), fts_new_symbol("samppeek"));
}

