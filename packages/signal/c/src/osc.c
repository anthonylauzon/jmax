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

#include "fts.h"
#include "osc.h"

struct osc_ftl_symbols osc_ftl_symbols_ptr = {0, 0, 0};
struct osc_ftl_symbols osc_ftl_symbols_fvec = {0, 0, 0};

/***************************************************************************************
 *
 *  user methods
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
  float_vector_t *fvec = float_vector_atom_get(at);

  if(float_vector_get_alloc_size(fvec) >= OSC_TABLE_SIZE + 1)
    osc_data_set_fvec(this->data, fvec);
}

/***************************************************************************************
 *
 *  put
 *
 */

static void
osc_put(osc_t *this, fts_dsp_descr_t *dsp, struct osc_ftl_symbols *sym)
{
  float sr = fts_dsp_get_output_srate(dsp, 0);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  fts_atom_t a[3];
  
  this->sr = sr;

  osc_data_set_phase(this->data, this->phase);

  if(!fts_dsp_is_sig_inlet((fts_object_t *)this, 0) || fts_dsp_is_input_null(dsp, 0))
    {
      /* no input connected */

      osc_data_set_incr(this->data, this->freq / sr);
      
      fts_set_ftl_data(a + 0, this->data);
      fts_set_symbol(a + 1, fts_dsp_get_output_name(dsp, 0));
      fts_set_int(a + 2, n_tick);
      
      dsp_add_funcall(sym->control_input, 3, a);
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
	  dsp_add_funcall(sym->signal_input, 4, a);
	}
      else /* inplace */
	{
	  fts_atom_t a[3];
	  
	  fts_set_ftl_data(a + 0, this->data);
	  fts_set_symbol(a + 1, fts_dsp_get_output_name(dsp, 0));
	  fts_set_int(a + 2, n_tick);
	  dsp_add_funcall(sym->signal_input_inplace, 3, a);
	}
    }
}

static void 
osc_put_cosine(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_t *this = (osc_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr(at);

  osc_put(this, dsp, &osc_ftl_symbols_ptr);
}

static void 
osc_put_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_t *this = (osc_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr(at);

  osc_put(this, dsp, &osc_ftl_symbols_fvec);
}

/***************************************************************************************
 *
 *  init
 *
 */

static void
osc_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  osc_t *this = (osc_t *)o;

  dsp_list_insert((fts_object_t *)this);

  /* init osc */
  this->freq = 0.0;
  this->phase = 0.0;
  this->sr = FTS_INTPHASE_RANGE;

  this->data = osc_data_new();
}

static void
osc_init_cosine(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  osc_t *this = (osc_t *)o;

  osc_init(o, 0, 0, 0, 0);

  if(ac == 2)
    osc_set_freq(o, 0, 0, 1, at + 1);

  osc_data_set_ptr(this->data, fts_fftab_get_cosine(OSC_TABLE_SIZE));
}

static void
osc_init_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  osc_t *this = (osc_t *)o;
  float_vector_t *fvec;

  osc_init(o, 0, 0, 0, 0);

  if(ac == 3)
    {
      osc_set_freq(o, 0, 0, 1, at + 1);
      fvec = float_vector_atom_get(at + 2);
    }
  else
    fvec = float_vector_atom_get(at + 1);

  /* check float vector size */
  if(float_vector_get_alloc_size(fvec) < OSC_TABLE_SIZE + 1)
    float_vector_set_size(fvec, OSC_TABLE_SIZE + 1);

  osc_data_set_fvec(this->data, fvec);
}

/***************************************************************************************
 *
 *  delete
 *
 */

static void
osc_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_t *this = (osc_t *)o;

  ftl_data_free(this->data);

  dsp_list_remove(o);
}

static void
osc_delete_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_t *this = (osc_t *)o;

  /* release fvec */
  osc_data_set_fvec(this->data, 0);

  ftl_data_free(this->data);

  dsp_list_remove(o);
}

/***************************************************************************************
 *
 *  instantiate
 *
 */

static fts_status_t
osc_instantiate_cosine(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(osc_t), 1, 1, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, osc_init_cosine);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, osc_delete);
      
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("put"), osc_put_cosine);
  
  fts_method_define_varargs(cl, 0, fts_new_symbol("phase"), osc_set_phase);
  fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("phase"), osc_set_phase_prop);
  
  if(ac == 2)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, osc_set_freq);
      fts_method_define_varargs(cl, 0, fts_s_float, osc_set_freq);
      fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("freq"), osc_set_freq_prop);      
    }
  else
    dsp_sig_inlet(cl, 0);

  dsp_sig_outlet(cl, 0);
    
  return fts_Success;
}

static fts_status_t
osc_instantiate_fvec(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(osc_t), 2, 1, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, osc_init_fvec);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, osc_delete_fvec);
      
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("put"), osc_put_fvec);
  
  fts_method_define_varargs(cl, 0, fts_new_symbol("phase"), osc_set_phase);
  fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("phase"), osc_set_phase_prop);
  
  fts_method_define_varargs(cl, 1, float_vector_symbol, osc_set_fvec);

  if(ac == 3)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, osc_set_freq);
      fts_method_define_varargs(cl, 0, fts_s_float, osc_set_freq);
      fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("freq"), osc_set_freq_prop);
    }
  else
    dsp_sig_inlet(cl, 0);

  dsp_sig_outlet(cl, 0);
    
  return fts_Success;
}

static fts_status_t
osc_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac == 1 || (ac == 2 && fts_is_number(at + 1)))
    return osc_instantiate_cosine(cl, ac, at);
  else if ((ac == 2 && float_vector_atom_is(at + 1)) || (ac == 3 && fts_is_number(at + 1) && float_vector_atom_is(at + 2)))
    return osc_instantiate_fvec(cl, ac, at);
  else
    return &fts_CannotInstantiate;
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

  /* declare the oscillator related FTL functions (platform dependent) */
  osc_declare_functions();

  fts_metaclass_install(fts_new_symbol("osc~"), osc_instantiate, fts_arg_type_equiv);
}
