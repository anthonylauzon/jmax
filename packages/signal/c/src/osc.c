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
 * Authors: Norbert Schnell
 *
 */

#include <fts/fts.h>
#include <utils/c/include/utils.h>
#include "osc.h"

struct osc_ftl_symbols osc_ftl_symbols_ptr = {0, 0, 0};
struct osc_ftl_symbols osc_ftl_symbols_fvec = {0, 0, 0};
struct osc_ftl_symbols phi_ftl_symbols = {0, 0, 0};

/***************************************************************************************
 *
 *  osc~
 *
 */

static void 
osc_set_freq(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_t *this = (osc_t *)o;
  float freq = fts_get_number_float(at);
  
  osc_data_set_incr(this->data, freq / this->sr);

  this->freq = freq;
}

static void
osc_set_freq_prop(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  osc_t *this = (osc_t *)o;
  float freq = fts_get_number_float(value);
  
  osc_data_set_incr(this->data, freq / this->sr);

  this->freq = freq;
}

static void 
osc_set_phase(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_t *this = (osc_t *)o;
  float phase = fts_get_number_float(at);
  
  osc_data_set_phase(this->data, phase);

  this->phase = phase;
}

static void
osc_set_phase_prop(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  osc_t *this = (osc_t *)o;
  float phase = fts_get_number_float(value);
  
  osc_data_set_phase(this->data, phase);

  this->phase = phase;
}

static void 
osc_set_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_t *this = (osc_t *)o;
  fvec_t *fvec = (fvec_t *)fts_get_object(at);

  /* check float vector size */
  if(fvec_get_size(fvec) < OSC_TABLE_SIZE + 1)
    fvec_set_size(fvec, OSC_TABLE_SIZE + 1);

  osc_data_set_fvec(this->data, fvec);
}

static void 
osc_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_t *this = (osc_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  float sr = fts_dsp_get_output_srate(dsp, 0);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  struct osc_ftl_symbols *sym;

  if(this->fvec)
    sym = &osc_ftl_symbols_fvec;
  else
    sym = &osc_ftl_symbols_ptr;
  
  this->sr = sr;

  osc_data_set_phase(this->data, this->phase);

  if(fts_dsp_is_input_null(dsp, 0))
    {
      /* no input connected */
      fts_atom_t a[3];

      osc_data_set_incr(this->data, this->freq / sr);
      
      fts_set_ftl_data(a + 0, this->data);
      fts_set_symbol(a + 1, fts_dsp_get_output_name(dsp, 0));
      fts_set_int(a + 2, n_tick);      
      fts_dsp_add_function(sym->control_input, 3, a);
    }
  else
    {
      osc_data_set_factor(this->data, sr);
      
      if (fts_dsp_get_input_name(dsp, 0) != fts_dsp_get_output_name(dsp, 0))
	{
	  fts_atom_t a[4];
	  
	  fts_set_ftl_data(a + 0, this->data);
	  fts_set_symbol(a + 1, fts_dsp_get_input_name(dsp, 0));
	  fts_set_symbol(a + 2, fts_dsp_get_output_name(dsp, 0));
	  fts_set_int(a + 3, n_tick);
	  fts_dsp_add_function(sym->signal_input, 4, a);
	}
      else /* inplace */
	{
	  fts_atom_t a[3];
	  
	  fts_set_ftl_data(a + 0, this->data);
	  fts_set_symbol(a + 1, fts_dsp_get_output_name(dsp, 0));
	  fts_set_int(a + 2, n_tick);
	  fts_dsp_add_function(sym->signal_input_inplace, 3, a);
	}
    }
}

static void
osc_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  osc_t *this = (osc_t *)o;

  fts_dsp_object_init((fts_dsp_object_t *)o);

  /* init osc */
  this->freq = 0.0;
  this->phase = 0.0;
  this->sr = fts_dsp_get_sample_rate();

  this->data = osc_data_new();

  if(ac == 0 || (ac == 1 && fts_is_number(at)))
    {
      /* cosine version */
      if(ac > 0)
	osc_set_freq(o, 0, 0, 1, at);

      osc_data_set_ptr(this->data, fts_fftab_get_cosine(OSC_TABLE_SIZE));
      fts_object_set_inlets_number(o, 1);

      this->fvec = 0;
    }
  else if ((ac == 1 && fts_is_a(at, fvec_type)) || (ac == 2 && fts_is_number(at) && fts_is_a(at + 1, fvec_type)))
    {
      /* fvec version */
      if(ac > 1)
	{
	  osc_set_freq(o, 0, 0, 1, at);
	  osc_set_fvec(o, 0, 0, 1, at + 1);
	}
      else
	osc_set_fvec(o, 0, 0, 1, at);

      this->fvec = 1;
    }
  else
    fts_object_set_error(o, "bad arguments");
}

static void
osc_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_t *this = (osc_t *)o;

  /* release fvec */
  if(this->fvec)
    osc_data_set_fvec(this->data, 0);

  ftl_data_free(this->data);
  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

static void
osc_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(osc_t), osc_init, osc_delete);

  fts_class_message_varargs(cl, fts_s_put, osc_put);
  
  fts_class_message_varargs(cl, fts_new_symbol("phase"), osc_set_phase);
  fts_class_message_varargs(cl, fts_new_symbol("freq"), osc_set_freq);

  fts_class_inlet_int(cl, 0, osc_set_freq);
  fts_class_inlet_float(cl, 0, osc_set_freq);

  fts_class_inlet(cl, 1, fvec_type, osc_set_fvec);

  fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("phase"), osc_set_phase_prop);
  fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("freq"), osc_set_freq_prop);      

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
    }

/***************************************************************************************
 *
 *  phi~
 *
 */

static void 
phi_set_freq(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_t *this = (osc_t *)o;
  float freq = fts_get_number_float(at);
  
  phi_data_set_incr(this->data, freq / this->sr);

  this->freq = freq;
}

static void
phi_set_freq_prop(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  osc_t *this = (osc_t *)o;
  float freq = fts_get_number_float(value);
  
  phi_data_set_incr(this->data, freq / this->sr);

  this->freq = freq;
}

static void 
phi_set_phase(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_t *this = (osc_t *)o;
  float phase = fts_get_number_float(at);
  
  phi_data_set_phase(this->data, phase);

  this->phase = phase;
}

static void
phi_set_phase_prop(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  osc_t *this = (osc_t *)o;
  float phase = fts_get_number_float(value);
  
  phi_data_set_phase(this->data, phase);

  this->phase = phase;
}

static void 
phi_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_t *this = (osc_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  float sr = fts_dsp_get_output_srate(dsp, 0);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  
  this->sr = sr;

  phi_data_set_phase(this->data, this->phase);

  if(fts_dsp_is_input_null(dsp, 0))
    {
      /* no input connected */
      fts_atom_t a[3];
      
      phi_data_set_incr(this->data, this->freq / sr);
      
      fts_set_ftl_data(a + 0, this->data);
      fts_set_symbol(a + 1, fts_dsp_get_output_name(dsp, 0));
      fts_set_int(a + 2, n_tick);
      
      fts_dsp_add_function(phi_ftl_symbols.control_input, 3, a);
    }
  else
    {
      phi_data_set_factor(this->data, sr);
      
      if (fts_dsp_get_input_name(dsp, 0) != fts_dsp_get_output_name(dsp, 0))
	{
	  fts_atom_t a[4];
	  
	  fts_set_ftl_data(a + 0, this->data);
	  fts_set_symbol(a + 1, fts_dsp_get_input_name(dsp, 0));
	  fts_set_symbol(a + 2, fts_dsp_get_output_name(dsp, 0));
	  fts_set_int(a + 3, n_tick);
	  fts_dsp_add_function(phi_ftl_symbols.signal_input, 4, a);
	}
      else /* inplace */
	{
	  fts_atom_t a[3];
	  
	  fts_set_ftl_data(a + 0, this->data);
	  fts_set_symbol(a + 1, fts_dsp_get_output_name(dsp, 0));
	  fts_set_int(a + 2, n_tick);
	  fts_dsp_add_function(phi_ftl_symbols.signal_input_inplace, 3, a);
	}
    }
}

static void
phi_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  osc_t *this = (osc_t *)o;

  fts_dsp_object_init((fts_dsp_object_t *)o);

  /* init osc */
  this->freq = 0.0;
  this->phase = 0.0;
  this->sr = fts_dsp_get_sample_rate();

  this->data = phi_data_new();

  if(ac == 1)
    phi_set_freq(o, 0, 0, 1, at);
}

static void
phi_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_t *this = (osc_t *)o;

  ftl_data_free(this->data);
  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

static void
phi_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(osc_t), phi_init, phi_delete);

  fts_class_message_varargs(cl, fts_s_put, phi_put);
  
  fts_class_message_varargs(cl, fts_new_symbol("phase"), phi_set_phase);
  fts_class_message_varargs(cl, fts_new_symbol("freq"), phi_set_freq);
  
  fts_class_inlet_int(cl, 0, phi_set_freq);
  fts_class_inlet_float(cl, 0, phi_set_freq);
  
  fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("freq"), phi_set_freq_prop);      
  fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("phase"), phi_set_phase_prop);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
    }

void
signal_osc_config(void)
{
  /* ftl functions using a pointer to float as wave table */
  osc_ftl_symbols_ptr.control_input = fts_new_symbol("osc_ptr_control_input");
  osc_ftl_symbols_ptr.signal_input = fts_new_symbol("osc_ptr_signal_input");
  osc_ftl_symbols_ptr.signal_input_inplace = fts_new_symbol("osc_ptr_signal_input_inplace");

  /* ftl functions using a float vector as wave table */
  osc_ftl_symbols_fvec.control_input = fts_new_symbol("osc_fvec_control_input");
  osc_ftl_symbols_fvec.signal_input = fts_new_symbol("osc_fvec_signal_input");
  osc_ftl_symbols_fvec.signal_input_inplace = fts_new_symbol("osc_fvec_signal_input_inplace");

  /* ftl functions using a float vector as wave table */
  phi_ftl_symbols.control_input = fts_new_symbol("phi_control_input");
  phi_ftl_symbols.signal_input = fts_new_symbol("phi_signal_input");
  phi_ftl_symbols.signal_input_inplace = fts_new_symbol("phi_signal_input_inplace");

  /* declare the oscillator related FTL functions (platform dependent) */
  osc_declare_functions();

  fts_class_install(fts_new_symbol("osc~"), osc_instantiate);
  fts_class_install(fts_new_symbol("phi~"), phi_instantiate);
}
