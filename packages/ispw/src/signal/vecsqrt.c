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

static void 
dsp_put_sqrt(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_put_all(o, (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0), sym_sqrt);
}

static void 
dsp_put_rsqrt(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_put_all(o, (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0), sym_rsqrt);
}

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
  fts_class_install(fts_new_symbol("sqrt~"), sqrt_instantiate);
  fts_class_install(fts_new_symbol("rsqrt~"), rsqrt_instantiate);
}
