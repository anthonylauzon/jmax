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




/* the class */
enum{
  INLET_sig = 0,
  N_INLETS
};

enum{
  OUTLET_sig = 0,
  N_OUTLETS
};

#define DEFINE_PUT_FUN(name)\
  static void dsp_put_ ## name\
    (fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)\
    {dsp_put_all(o, (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0), ftl_sym.name.vec);}

  
/************************************************
 *
 *    object
 *
 */

typedef struct{
  fts_object_t head;
} obj_t;

static void obj_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_list_insert(o);
}

static void obj_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_list_remove(o);
}

/************************************************
 *
 *    dsp
 *
 */

enum{
  DSP_ARG_in,
  DSP_ARG_out,
  DSP_ARG_n_tick,
  N_DSP_ARGS
};

static void dsp_put_all(fts_object_t *o, fts_dsp_descr_t *dsp, fts_symbol_t dsp_symbol)
{
  obj_t *obj = (obj_t *)o;
  fts_atom_t argv[N_DSP_ARGS];

  fts_set_symbol(argv + DSP_ARG_in,   fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv + DSP_ARG_out, fts_dsp_get_output_name(dsp, 0));
  fts_set_long  (argv + DSP_ARG_n_tick, fts_dsp_get_input_size(dsp, 0));
  dsp_add_funcall(dsp_symbol, N_DSP_ARGS, argv);
}

/* a single put fun for each class */

  DEFINE_PUT_FUN(sqrt)
  DEFINE_PUT_FUN(rsqr)
  
/************************************************
 *
 *    class
 *
 */

static fts_status_t
class_instantiate_realize(fts_class_t *cl, int ac, const fts_atom_t *at, fts_method_t mth)
{
  fts_symbol_t a[4];

  fts_class_init(cl, sizeof(obj_t), N_INLETS, N_OUTLETS, 0);
  
  a[0] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, obj_init, 1, a);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, obj_delete, 0, 0);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, mth, 1, a);

  dsp_sig_inlet(cl, INLET_sig);
  dsp_sig_outlet(cl, OUTLET_sig);
  
  return fts_Success;
}

static fts_status_t
sqrt_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return class_instantiate_realize(cl, ac, at, dsp_put_sqrt);
}

static fts_status_t
rsqrt_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return class_instantiate_realize(cl, ac, at, dsp_put_rsqr);
}

void
vecsqrt_config(void)
{
  fts_metaclass_create(fts_new_symbol("sqrt~"), sqrt_instantiate, fts_always_equiv);
  fts_metaclass_create(fts_new_symbol("rsqrt~"), rsqrt_instantiate, fts_always_equiv);
}
