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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Miller Puckette.
 *
 */


/* sigsamplerate.c */

#include "fts.h"


typedef struct
{
  fts_object_t _o;
  float c_rate;
  int c_vecsize;
  int c_isvecsize;
} sigparam_t;

static void
sigparam_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigparam_t *x = (sigparam_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);

  x->c_rate = fts_dsp_get_input_srate(dsp, 0);
  x->c_vecsize = fts_dsp_get_input_size(dsp, 0);
}

static void
sigparam_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigparam_t *x = (sigparam_t *)o;

  if (x->c_vecsize <= 0)
    post("samplerate~/vecsize~: not available until DSP started\n");
  else if (x->c_isvecsize)
    fts_outlet_int(o, 0, x->c_vecsize);
  else
    fts_outlet_float(o, 0, x->c_rate);
}

static void
sigsamplerate_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigparam_t *x = (sigparam_t *)o;

  x->c_vecsize   = -1;
  x->c_isvecsize = 0;

  dsp_list_insert(o);
}

static void
sigvectorsize_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigparam_t *x = (sigparam_t *)o;

  x->c_vecsize   = -1;
  x->c_isvecsize = 1;

  dsp_list_insert(o);
}

static void
sigparam_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_list_remove(o);
}

static fts_status_t
sigsamplerate_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(sigparam_t), 1, 1, 0);

  a[0] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, sigsamplerate_init, 1, a);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sigparam_delete, 0, 0);

  a[0] = fts_s_ptr;  
  fts_method_define(cl, fts_SystemInlet, fts_s_put, sigparam_put, 1, a);
  
  fts_method_define(cl, 0, fts_s_bang, sigparam_bang, 0, a);

  dsp_sig_inlet(cl, 0);
  
  a[0] = fts_s_float;
  fts_outlet_type_define(cl, 0, fts_s_float, 1, a);

  /* DSP properties  */

  /* fts_class_put_prop(cl, fts_s_dsp_is_sink, fts_true); */

  return fts_Success;
}

static fts_status_t
sigvectorsize_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(sigparam_t), 1, 1, 0);

  a[0] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, sigvectorsize_init, 1, a);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sigparam_delete, 0, a);

  a[0] = fts_s_ptr;  
  fts_method_define(cl, fts_SystemInlet, fts_s_put, sigparam_put, 1, a);
  
  fts_method_define(cl, 0, fts_s_bang, sigparam_bang, 0, a);

  dsp_sig_inlet(cl, 0);
  
  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0, fts_s_int, 1, a);

  /* DSP properties  */

  /*   fts_class_put_prop(cl, fts_s_dsp_is_sink, fts_true); */

  return fts_Success;
}


void
sigparam_config(void)
{
  fts_class_install(fts_new_symbol("samplerate~"),sigsamplerate_instantiate);
  fts_class_install(fts_new_symbol("vectorsize~"),sigvectorsize_instantiate);
}
