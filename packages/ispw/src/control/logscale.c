#include "fts.h"
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
} logscale_t;

/********************
 *
 *  computes:
 *    out = out_low + out_range * {log[(in-in_low)/in_range]/log(base)-1} / (base - 1)
 *  as:
 *    out = out_scale * log(in_scale * in + in_shift) + out_shift
 *  with:
 *    in_range = in_high - in_low
 *    in_scale = (base - 1) / in_range
 *    in_shift = -in_low * in_scale - 1.
 *    out_range = out_high - out_low
 *    out_scale = out_range / log(base)
 *    out_shift = out_low - out_scale
 *
 */

static void
logscale_compute_params(logscale_t *x)
{
  float in_range, out_range;

  if(x->params[base] <= 0.)
    {
      post("error: logscale: base must be > 0.\n");
      return;
    }

  in_range = x->params[in_high] - x->params[in_low];
  out_range = x->params[out_high] - x->params[out_low];
  if(in_range == 0. || out_range == 0.)
    {
      post("error: logscale: high and low value can not be the same\n");
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
      x->in_scale = (x->params[base] - 1.) / in_range;
      x->in_shift = -x->params[in_low] * x->in_scale + 1;
      x->out_scale = out_range / log(x->params[base]);
      x->out_shift = x->params[out_low];
      x->linear = 0;
    }
}

static void
logscale_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  logscale_t *x = (logscale_t *)o;

  at++; ac--; /* skip class name */
  x->params[in_low]   = fts_get_float_arg(ac, at, in_low, 0.0f);
  x->params[in_high]  = fts_get_float_arg(ac, at, in_high, 1.0f);
  x->params[out_low]  = fts_get_float_arg(ac, at, out_low, 0.0f);
  x->params[out_high] = fts_get_float_arg(ac, at, out_high, 127.0f);
  x->params[base] = fts_get_float_arg(ac, at, base, 1.0f);
  logscale_compute_params(x);
}

static void
logscale_param(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  logscale_t *x = (logscale_t *)o;
  x->params[winlet-1] = fts_get_float_arg(ac, at, 0, 0.0f);
  logscale_compute_params(x);
}

static void
logscale_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  logscale_t *x = (logscale_t *)o;
  int i;
  
  for(i=0; i<ac; i++)
    x->params[i] = fts_get_float_arg(ac, at, i, 0.0f);
  logscale_compute_params(x);
}

static void
logscale_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  logscale_t *x = (logscale_t *)o;
  float f = fts_get_float_arg(ac, at, 0, 0.0f);

  if(x->linear)
    f = f * x->in_scale + x->in_shift;
  else
    f = x->out_scale * log(f * x->in_scale + x->in_shift) + x->out_shift;
  
  fts_outlet_float(o, 0, f);
}

static fts_status_t
logscale_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[6];
  int i;

  fts_class_init(cl, sizeof(logscale_t), 1 + n_params, 1, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_number;
  a[2] = fts_s_number;
  a[3] = fts_s_number;
  a[4] = fts_s_number;
  a[5] = fts_s_number;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, logscale_init, 6, a, 1);

  fts_method_define_varargs(cl, 0, fts_new_symbol("set"), logscale_set);

  a[0] = fts_s_number;
  fts_method_define(cl, 0, fts_s_int, logscale_number, 1, a);
  fts_method_define(cl, 0, fts_s_float, logscale_number, 1, a);

  a[0] = fts_s_number;
  for (i=1; i<=n_params; i++)
    {
      fts_method_define(cl, i, fts_s_int, logscale_param, 1, a);
      fts_method_define(cl, i, fts_s_float, logscale_param, 1, a);
    }

  a[0] = fts_s_float;
  fts_outlet_type_define(cl, 0, fts_s_float, 1, a);
  
  return fts_Success;
}

void
logscale_config(void)
{
  fts_metaclass_create(fts_new_symbol("logscale"), logscale_instantiate, fts_always_equiv);
}
