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
 * Authors: Francois Dechelle, Norbert Schnell
 *
 */

#include <fts/fts.h>
#include "fvec.h"

#define OSC_TABLE_BITS 9
#define OSC_TABLE_SIZE 512

/***********************************************************
 *
 * wave table oscillator with control input
 *
 */

static fts_symbol_t sym_osc_ctl = 0;

typedef struct _osc_ctl_data_
{ 
  fvec_t *fvec;
  fts_intphase_t phase;
  fts_intphase_t phase_incr;
} osc_ctl_data_t;

typedef struct 
{
  fts_object_t o;
  ftl_data_t data;
  double sr;
  double freq;
} osc_ctl_t;

void
osc_ctl_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_ctl_t *this = (osc_ctl_t *)o;
  osc_ctl_data_t *data = (osc_ctl_data_t *)ftl_data_get_ptr(this->data);
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  float sr = fts_dsp_get_output_srate(dsp, 0);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  fts_atom_t a[3];
  
  data->phase = 0;
  data->phase_incr = (fts_intphase_t)(this->freq * FTS_INTPHASE_RANGE / sr);
  this->sr = sr;

  fts_set_ftl_data(a + 0, this->data);
  fts_set_symbol(a + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(a + 2, n_tick);
  
  fts_dsp_add_function(sym_osc_ctl, 3, a);
}

static void
osc_ctl_ftl(fts_word_t *argv)
{
  osc_ctl_data_t *data = (osc_ctl_data_t *)fts_word_get_pointer(argv + 0);
  float *out = (float *) fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  fvec_t *fvec = data->fvec;
  fts_intphase_t phi = data->phase;
  fts_intphase_t phase_incr = data->phase_incr;
  float *buf = fvec_get_ptr(data->fvec);
  int i;

  for(i=0; i<n_tick; i++)
    {
      int index = fts_intphase_get_index(phi, OSC_TABLE_BITS);
      float frac = fts_intphase_get_frac(phi, OSC_TABLE_BITS);
      
      out[i] = buf[index] + frac * (buf[index + 1] - buf[index]);
      
      phi = fts_intphase_wrap(phi + phase_incr);
    }

  data->phase = phi;  
}

static void 
osc_ctl_set_freq(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_ctl_t *this = (osc_ctl_t *)o;
  osc_ctl_data_t *data = (osc_ctl_data_t *)ftl_data_get_ptr(this->data);
  float freq = fts_get_number_float(at);
  
  data->phase_incr = (fts_intphase_t)(freq * FTS_INTPHASE_RANGE / this->sr);
  this->freq = freq;
}
 
static void 
osc_ctl_set_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_ctl_t *this = (osc_ctl_t *)o;
  osc_ctl_data_t *data = (osc_ctl_data_t *)ftl_data_get_ptr(this->data);
  fvec_t *fvec = fvec_atom_get(at);

  if(fvec_get_size(fvec) >= OSC_TABLE_SIZE + 1)
    {
      fts_object_release((fts_object_t *)data->fvec);
      data->fvec = fvec;
      fts_object_refer((fts_object_t *)fvec);
    }
}

static void
osc_ctl_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  osc_ctl_t *this = (osc_ctl_t *)o;
  fvec_t *fvec = fvec_atom_get(at + 1);
  osc_ctl_data_t *data;

  fts_dsp_add_object(o);

  this->data = ftl_data_new(osc_ctl_data_t);
  data = (osc_ctl_data_t *)ftl_data_get_ptr(this->data);

  /* check float vector size */
  if(fvec_get_size(fvec) < OSC_TABLE_SIZE + 1)
    fvec_set_size(fvec, OSC_TABLE_SIZE + 1);

  /* refer to fvec */
  data->fvec = fvec;
  fts_object_refer((fts_object_t *)fvec);

  /* init phase */
  data->phase = 0;
  data->phase_incr = 0;

  this->sr = FTS_INTPHASE_RANGE;

  osc_ctl_set_freq(o, 0, 0, 1, at);
}

static void
osc_ctl_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  osc_ctl_t *this = (osc_ctl_t *)o;
  osc_ctl_data_t *data = (osc_ctl_data_t *)ftl_data_get_ptr(this->data);

  fts_object_release((fts_object_t *)data->fvec);

  ftl_data_free(this->data);
  fts_dsp_remove_object(o);
}

static fts_status_t
osc_ctl_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(osc_ctl_t), 2, 1, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, osc_ctl_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, osc_ctl_delete);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("put"), osc_ctl_put);
  
  fts_method_define_varargs(cl, 0, fts_s_int, osc_ctl_set_freq);
  fts_method_define_varargs(cl, 0, fts_s_float, osc_ctl_set_freq);

  fts_method_define_varargs(cl, 1,fvec_symbol , osc_ctl_set_fvec);

  fts_dsp_declare_outlet(cl, 0);

  sym_osc_ctl = fts_new_symbol("osc_ctl");
  fts_dsp_declare_function(sym_osc_ctl, osc_ctl_ftl);
    
  return fts_Success;
}

/***********************************************************
 *
 * wave table oscillator with signal input
 *
 */

static fts_symbol_t sym_osc_sig = 0;

typedef struct _osc_sig_data_
{ 
  fvec_t *fvec;
  fts_intphase_t phase;
  double phase_factor;
} osc_sig_data_t;

typedef struct 
{
  fts_object_t o;
  ftl_data_t data;
  double sr;
} osc_sig_t;

void
osc_sig_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_sig_t *this = (osc_sig_t *)o;
  osc_sig_data_t *data = (osc_sig_data_t *)ftl_data_get_ptr(this->data);
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  float sr = fts_dsp_get_output_srate(dsp, 0);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  fts_atom_t a[4];

  data->phase = 0;
  data->phase_factor = FTS_INTPHASE_RANGE / sr;
  this->sr = sr;

  fts_set_ftl_data(a + 0, this->data);
  fts_set_symbol(a + 1, fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(a + 2, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(a + 3, n_tick);
  
  fts_dsp_add_function(sym_osc_sig, 4, a);
}

static void
osc_sig_ftl(fts_word_t *argv)
{
  osc_sig_data_t *data = (osc_sig_data_t *)fts_word_get_pointer(argv + 0);
  float *in = (float *) fts_word_get_pointer(argv + 1);
  float *out = (float *) fts_word_get_pointer(argv + 2);
  int n_tick = fts_word_get_int(argv + 3);
  fvec_t *fvec = data->fvec;
  fts_intphase_t phi = data->phase;
  fts_intphase_t phase_factor = data->phase_factor;
  float *buf = fvec_get_ptr(data->fvec);
  int i;

  for(i=0; i<n_tick; i++)
    {
      int index = fts_intphase_get_index(phi, OSC_TABLE_BITS);
      float frac = fts_intphase_get_frac(phi, OSC_TABLE_BITS);
      fts_intphase_t phase_incr = (fts_intphase_t)(phase_factor * in[i]);
      
      out[i] = buf[index] + frac * (buf[index + 1] - buf[index]);
      
      phi = fts_intphase_wrap(phi + phase_incr);
    }

  data->phase = phi;  
}

static void 
osc_sig_set_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_sig_t *this = (osc_sig_t *)o;
  osc_sig_data_t *data = (osc_sig_data_t *)ftl_data_get_ptr(this->data);
  fvec_t *fvec = fvec_atom_get(at);

  if(fvec_get_size(fvec) >= OSC_TABLE_SIZE + 1)
    {
      fts_object_release((fts_object_t *)data->fvec);
      data->fvec = fvec;
      fts_object_refer((fts_object_t *)fvec);
    }
}

static void
osc_sig_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  osc_sig_t *this = (osc_sig_t *)o;
  fvec_t *fvec = fvec_atom_get(at + 1);  
  osc_sig_data_t *data;

  fts_dsp_add_object(o);

  this->data = ftl_data_new(osc_sig_data_t);
  data = (osc_sig_data_t *)ftl_data_get_ptr(this->data);

  /* init float vector */
  if(fvec_get_size(fvec) < OSC_TABLE_SIZE + 1)
    fvec_set_size(fvec, OSC_TABLE_SIZE + 1);

  data->fvec = fvec;
  fts_object_refer((fts_object_t *)fvec);

  /* init phase */
  data->phase = 0;
  data->phase_factor = 0.0;

  this->sr = FTS_INTPHASE_RANGE;
}

static void
osc_sig_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  osc_sig_t *this = (osc_sig_t *)o;
  osc_sig_data_t *data = (osc_sig_data_t *)ftl_data_get_ptr(this->data);

  fts_object_release((fts_object_t *)data->fvec);

  ftl_data_free(this->data);
  fts_dsp_remove_object(o);
}

static fts_status_t
osc_sig_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(osc_sig_t), 2, 1, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, osc_sig_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, osc_sig_delete);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("put"), osc_sig_put);

  fts_method_define_varargs(cl, 1, fvec_symbol, osc_sig_set_fvec);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);

  sym_osc_sig = fts_new_symbol("osc_sig");
  fts_dsp_declare_function(sym_osc_sig, osc_sig_ftl);
    
  return fts_Success;
}

static fts_status_t
osc_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac > 0 && !fvec_atom_is(at))
    return &fts_CannotInstantiate;

  if(ac == 0 || (ac == 1 && fvec_atom_is(at)))
    return osc_sig_instantiate(cl, ac, at);
  else if ((ac == 1 && fts_is_number(at)) || (ac == 2 && fts_is_number(at) && fvec_atom_is(at + 1)))
    return osc_ctl_instantiate(cl, ac, at);
  else
    return &fts_CannotInstantiate;
  }

void
signal_osc_config(void)
{
  fts_metaclass_install(fts_new_symbol("osc~"), osc_instantiate, fts_arg_type_equiv);
}
