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



/************************************************
 *
 *    object
 *
 */

typedef struct
{
  fts_object_t _o;

  ftl_data_t ftl_data_min;
  ftl_data_t ftl_data_max;
} sigclip_t;

enum
{
  OBJ_ARG_class = 0,
  OBJ_ARG_min = 1,
  OBJ_ARG_max = 2,
  N_OBJ_ARGS 
};

enum
{
  INLET_sig = 0,
  INLET_min = 1,
  INLET_max = 2,
  N_INLETS
};

enum
{
  OUTLET_sig = 0,
  N_OUTLETS
};


static void
sigclip_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigclip_t *this = (sigclip_t *)o;
  float min, max;

  this->ftl_data_min = ftl_data_new(float);
  this->ftl_data_max = ftl_data_new(float);

  min = fts_get_float_arg(ac, at, OBJ_ARG_min, 0.0f);
  max = fts_get_float_arg(ac, at, OBJ_ARG_max, 0.0f);

  ftl_data_copy(float, this->ftl_data_min, &min);
  ftl_data_copy(float, this->ftl_data_max, &max);

  dsp_list_insert(o);
}


static void
sigclip_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigclip_t *this = (sigclip_t *)o;

  ftl_data_free(this->ftl_data_min);
  ftl_data_free(this->ftl_data_max);

  dsp_list_remove(o);
}

/************************************************
 *
 *    dsp
 *
 */

enum
{
  DSP_ARG_in = 0,
  DSP_ARG_min = 1,
  DSP_ARG_max = 2,
  DSP_ARG_out = 3,
  DSP_ARG_n_tick = 4,
  N_DSP_ARGS = 5
};

static void
sigclip_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigclip_t *this = (sigclip_t *)o;
  fts_atom_t argv[N_DSP_ARGS];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);

  fts_set_symbol(argv + DSP_ARG_in,   fts_dsp_get_input_name(dsp, 0));
  fts_set_ftl_data(argv + DSP_ARG_min, this->ftl_data_min);
  fts_set_ftl_data(argv + DSP_ARG_max, this->ftl_data_max);
  fts_set_symbol(argv + DSP_ARG_out, fts_dsp_get_output_name(dsp, 0));
  fts_set_long  (argv + DSP_ARG_n_tick, fts_dsp_get_input_size(dsp, 0));

  dsp_add_funcall(ftl_sym.clip.f.vec, N_DSP_ARGS, argv);
}

/************************************************
 *
 *    user methods
 *
 */
 
static void
sigclip_min(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigclip_t *this = (sigclip_t *)o;
  float min;

  min = fts_get_float_arg(ac, at, 0, 0.0f);
  ftl_data_copy(float, this->ftl_data_min, &min);
}


static void
sigclip_max(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigclip_t *this = (sigclip_t *)o;
  float max;

  max = fts_get_float_arg(ac, at, 0, 0.0f);
  ftl_data_copy(float, this->ftl_data_max, &max);
}

/************************************************
 *
 *    class
 *
 */

static fts_status_t
class_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  fts_class_init(cl, sizeof(sigclip_t), N_INLETS, N_OUTLETS, 0);

  a[OBJ_ARG_class] = fts_s_symbol;
  a[OBJ_ARG_min] = fts_s_number;
  a[OBJ_ARG_max] = fts_s_number;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, sigclip_init, N_OBJ_ARGS, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sigclip_delete, 0, a);

  a[0] = fts_s_ptr;  
  fts_method_define(cl, fts_SystemInlet, fts_s_put, sigclip_put, 1, a);
  
  a[0] = fts_s_float;
  fts_method_define(cl, INLET_min, fts_s_float, sigclip_min, 1, a);
  
  a[0] = fts_s_float;
  fts_method_define(cl, INLET_max, fts_s_float, sigclip_max, 1, a);
  
  dsp_sig_inlet(cl, INLET_sig);
  dsp_sig_outlet(cl, OUTLET_sig);
  
  return fts_Success;
}

void
vecclip_config(void)
{
  fts_class_install(fts_new_symbol("clip~"),class_instantiate);
}
