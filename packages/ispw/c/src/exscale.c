/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include <fts/fts.h>
#include <math.h>

enum {
  in_low = 0,
  in_high,
  out_low,
  out_high,
  base,
  n_params
};

typedef struct{
  fts_object_t _o;
  int linear;
  float params[n_params];
  float in_shift;
  float in_scale;
  float out_shift;
  float out_scale;
} exscale_t;

/********************
 *
 *  computes:
 *    out = out_low + out_range * {exp[log(base)*(in-in_low)/in_range]-1} / (base - 1)
 *  as:
 *    out = out_scale * exp(in_scale * in + in_shift) + out_shift
 *  with:
 *    in_range = in_high - in_low
 *    in_scale = log(base) / in_range
 *    in_shift = -in_low * in_scale 
 *    out_range = out_high - out_low
 *    out_scale = out_range / (base - 1)
 *    out_shift = out_low - out_scale
 *
 */

static void
exscale_compute_params(exscale_t *x)
{
  float in_range, out_range;

  if(x->params[base] <= 0.)
    {
      post("error: exscale: base must be > 0.\n");
      return;
    }

  in_range = x->params[in_high] - x->params[in_low];
  out_range = x->params[out_high] - x->params[out_low];
  if(in_range == 0. || out_range == 0.)
    {
      post("error: exscale: high and low value can't be the same\n");
      return;
    }

  if(x->params[base] == 1.)
    {
      x->in_scale = out_range / in_range;
      x->in_shift = -x->params[in_low] * x->in_scale + x->params[out_low];
      x->linear = 1;
    }
  else
    {      
      x->in_scale = log(x->params[base]) / in_range;
      x->in_shift = -x->params[in_low] * x->in_scale;
      x->out_scale = out_range / (x->params[base] - 1.);
      x->out_shift = x->params[out_low] - x->out_scale;
      x->linear = 0;
    }
}

static void
exscale_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  exscale_t *x = (exscale_t *)o;

  at++; ac--; /* skip class name */
  x->params[in_low]   = fts_get_float_arg(ac, at, in_low, 0.f);
  x->params[in_high]  = fts_get_float_arg(ac, at, in_high, 127.f);
  x->params[out_low]  = fts_get_float_arg(ac, at, out_low, 0.f);
  x->params[out_high] = fts_get_float_arg(ac, at, out_high, 1.f);
  x->params[base]     = fts_get_float_arg(ac, at, base, 1.f);
  exscale_compute_params(x);
}

static void
exscale_param(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  exscale_t *x = (exscale_t *)o;
  x->params[winlet-1] = fts_get_float_arg(ac, at, 0, 0.f);
  exscale_compute_params(x);
}

static void
exscale_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  exscale_t *x = (exscale_t *)o;
  int i;
  
  for(i=0; i<ac; i++)
    x->params[i] = fts_get_float_arg(ac, at, i, 0.f);
  exscale_compute_params(x);
}

static void
exscale_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  exscale_t *x = (exscale_t *)o;
  float f = fts_get_float_arg(ac, at, 0, 0.0f);

  if(x->linear)
    f = f * x->in_scale + x->in_shift;
  else
    f = x->out_scale * exp(f * x->in_scale + x->in_shift) + x->out_shift;
  
  fts_outlet_float(o, 0, f);
}

static fts_status_t
exscale_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[6];
  int i;

  fts_class_init(cl, sizeof(exscale_t), 1 + n_params, 1, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_number;
  a[2] = fts_s_number;
  a[3] = fts_s_number;
  a[4] = fts_s_number;
  a[5] = fts_s_number;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, exscale_init, 6, a, 1);

  fts_method_define_varargs(cl, 0, fts_new_symbol("set"), exscale_set);

  a[0] = fts_s_number;
  fts_method_define(cl, 0, fts_s_int, exscale_number, 1, a);
  fts_method_define(cl, 0, fts_s_float, exscale_number, 1, a);

  a[0] = fts_s_number;
  for (i=1; i<=n_params; i++)
    {
      fts_method_define(cl, i, fts_s_int, exscale_param, 1, a);
      fts_method_define(cl, i, fts_s_float, exscale_param, 1, a);
    }

  a[0] = fts_s_float;
  fts_outlet_type_define(cl, 0, fts_s_float, 1, a);
  
  return fts_Success;
}

void
exscale_config(void)
{
  fts_class_install(fts_new_symbol("exscale"), exscale_instantiate);
}
