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

#include <math.h>
#include <fts/fts.h>
#include <utils/c/include/utils.h>
#include "delay.h"

#define random_get(min, max) ((min) + (((max) - (min)) * rand() / RAND_MAX))
#define cent_to_linear(x) (exp(0.00057762265047 * (x)))

static fts_symbol_t harmtap_dsp_symbol = 0;

static float * harmtap_window = 0;

/**************************************************
 *
 *  window
 *
 */

#define WINDOW_BITS 12
#define WINDOW_SIZE (1 << WINDOW_BITS)

/**************************************************
 *
 *  parammeters
 *
 */

typedef struct _harmtap_params_
{
  double trans; /* transposition factor */
  double window; /* window size in samples */
  double fade; /* fade size in samples */

  double delay_set; /* delay set*/
  double delay; /* current delay */

  double primary_delay; /* current delay of primary frame */
  double secondary_delay; /* current delay of secondary frame */
  double incr; /* new increment */

  fts_intphase_t phi; /* window phase */
  fts_intphase_t phi_incr; /* current window phase increment */
  fts_intphase_t wind_incr; /* standard  window phase increment when fading */
  fts_intphase_t fade_incr; /* fast window phase increment when fading */
  int sync;

  delayline_t *delayline; /* the delay line structure */
  double sr;
} harmtap_params_t;

static void
harmtap_params_init(harmtap_params_t *params)
{
  params->trans = 1.0;
  params->window = 50.0; /* 50 msec default window */
  params->fade = 10.0; /* 10 msec default window */
  
  params->delay_set = 0.0;
  params->delay = 0.0;

  params->primary_delay = 0.0;
  params->secondary_delay = 0.0;
  params->incr = 0.0;

  params->phi = 0;
  params->phi_incr = 0;
  params->wind_incr = 0;
  params->fade_incr = 0;

  params->delayline = NULL;
  params->sr = 1.0;
}

static void
harmtap_reset_window(harmtap_params_t *params)
{
  double size = (double)delayline_get_delay_size(params->delayline);

  if(params->window < 4.0)
    params->window = 4.0;
  else if(params->window > size)
    params->window = size;
}

static void
harmtap_reset_trans(harmtap_params_t *params)
{
  double incr = 1.0 - params->trans;

  params->incr = incr;
  params->phi_incr = params->wind_incr = (fts_intphase_t)((double)FTS_INTPHASE_RANGE * incr / params->window + 0.5);
}

static void
harmtap_reset_fade(harmtap_params_t *params)
{
  if(params->fade < 4.0)
    params->fade = 4.0;

  params->fade_incr = (double)FTS_INTPHASE_RANGE / params->fade;
}

static void
harmtap_reset_delay(harmtap_params_t *params)
{
  double del_size = (double)delayline_get_delay_size(params->delayline);
  double del_tick = (double)delayline_get_n_tick(params->delayline);
  double delay;

  if(params->delay_set < 0)
    delay = 0.0;
  else if(params->delay_set > del_size - params->window)
    delay = del_size - params->window;
  else
    delay = params->delay_set;
  
  params->delay = delay + del_tick;
}

static void
harmtap_reset_frames(harmtap_params_t *params)
{
  params->primary_delay = params->delay;
  params->secondary_delay = params->delay;

  params->phi = (fts_intphase_t)FTS_INTPHASE_RANGE_HALF;

  params->sync = 0;
}

static void
harmtap_params_reset(harmtap_params_t *params, double sr, int n_tick)
{
  sr *= 0.001; /* convert to samples per msec */

  if(params->sr == 1.0)
    {
      params->delay_set *= sr;
      params->window *= sr;
      params->fade *= sr;
    }
  
  harmtap_reset_window(params);
  harmtap_reset_trans(params);
  harmtap_reset_fade(params);
  harmtap_reset_delay(params);
  harmtap_reset_frames(params);

  params->sr = sr;
}

/**************************************************
 *
 *    object
 *
 */

typedef struct _harmtap_
{
  fts_object_t obj;
  fts_symbol_t name;
  fts_object_t *next; /* pointer to the other delreader for the same delay line */ 
  ftl_data_t params;
} harmtap_t;

/**************************************************
 *
 *    dsp
 *
 */

static void
harmtap_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  harmtap_t *this = (harmtap_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  double sr = fts_dsp_get_output_srate(dsp, 0);
  harmtap_params_t *params = (harmtap_params_t *)ftl_data_get_ptr(this->params);
  fts_atom_t argv[3];
  
  harmtap_params_reset(params, sr, n_tick);
  
  fts_set_symbol(argv + 0, fts_dsp_get_output_name(dsp, 0));
  fts_set_ftl_data(argv + 1, this->params);
  fts_set_int(argv + 2, n_tick);
  dsp_add_funcall(harmtap_dsp_symbol, 3, argv);
}

static void
ftl_harmtap(fts_word_t *argv)
{
  float * restrict out = (float *)fts_word_get_pointer(argv + 0);
  harmtap_params_t *params = (harmtap_params_t *) fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  delayline_t *dl = params->delayline;
  float * restrict del_ptr = delayline_get_buffer(dl);
  int del_phase = delayline_get_phase(dl);
  fts_intphase_t phi = params->phi;
  fts_intphase_t pho;
  fts_intphase_t phi_incr = params->phi_incr;
  double primary_delay = params->primary_delay;
  double secondary_delay = params->secondary_delay;
  double incr = params->incr;
  int primary_int_delay, secondary_int_delay;
  int primary_index, secondary_index;
  fts_intphase_t primary_frac, secondary_frac;
  int i;

  for(i=0; i<n_tick; i++)
    {
      float fp, fs;
      float wp, ws;
      
      phi = fts_intphase_wrap(phi + phi_incr);
      pho = fts_intphase_wrap(phi + (fts_intphase_t)FTS_INTPHASE_RANGE_HALF);
      
      primary_delay += incr;
      secondary_delay += incr;
      
      if(phi_incr > 0)
	{
	  if(phi < phi_incr)
	    {
	      primary_delay = params->delay;
	      params->sync = 1;
	    }
	  else if(pho < phi_incr)
	    {
	      secondary_delay = params->delay;
	      
	      if(phi_incr == params->fade_incr && params->sync)
		params->phi_incr = phi_incr = params->wind_incr;
	    }
	}
      else if(phi_incr < 0)
	{
	  if(phi > FTS_INTPHASE_MAX + phi_incr)
	    {
	      primary_delay = params->delay + params->window;
	      params->sync = 1;
	    }
	  else if(pho > FTS_INTPHASE_MAX + phi_incr)
	    {
	      secondary_delay = params->delay + params->window;
	      
	      if(phi_incr == -params->fade_incr && params->sync)
		params->phi_incr = phi_incr = params->wind_incr;
	    }
	}
      
      primary_int_delay = (int)primary_delay;
      secondary_int_delay = (int)secondary_delay;
      
      primary_index = del_phase + i - primary_int_delay;
      secondary_index = del_phase + i - secondary_int_delay;
      
      primary_frac = fts_cubic_intphase_scale(primary_int_delay - primary_delay);
      secondary_frac = fts_cubic_intphase_scale(secondary_int_delay - secondary_delay);
      
      if(primary_index < 1)
	primary_index += delayline_get_ring_size(dl);
      if(secondary_index < 1)
	secondary_index += delayline_get_ring_size(dl);
      
      wp = harmtap_window[fts_intphase_get_index(phi, WINDOW_BITS)];
      ws = harmtap_window[fts_intphase_get_index(pho, WINDOW_BITS)];
      
      fts_cubic_intphase_interpolate(del_ptr + primary_index, primary_frac, &fp);
      fts_cubic_intphase_interpolate(del_ptr + secondary_index, secondary_frac, &fs);
      
      out[i] = fp * wp + fs * ws;
    }
  
  params->phi = phi;  
  params->primary_delay = primary_delay;
  params->secondary_delay = secondary_delay;
}

/************************************************
 *
 *  user methods
 *
 */

static void 
harmtap_set_delayline(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  harmtap_t *this = (harmtap_t *)o;
  harmtap_params_t *params = ftl_data_get_ptr(this->params);

  params->delayline = (delayline_t *)fts_get_object(at);  
}

static void 
harmtap_set_delay(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  harmtap_t *this = (harmtap_t *)o;
  harmtap_params_t *params = ftl_data_get_ptr(this->params);

  params->delay_set = fts_get_number_float(at) * params->sr;

  if(params->sr != 1.0)
    {
      harmtap_reset_delay(params);

      if(params->phi_incr > 0 && params->phi_incr < params->fade_incr)
	params->phi_incr = params->fade_incr;
      else if(params->phi_incr <= 0 && params->phi_incr > -params->fade_incr)
	params->phi_incr = -params->fade_incr;

      params->sync = 0;
    }
}

static void 
harmtap_set_pitch(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  harmtap_t *this = (harmtap_t *)o;
  harmtap_params_t *params = ftl_data_get_ptr(this->params);
  double pitch = fts_get_number_float(at);
  double trans = cent_to_linear(pitch);

  params->trans = trans;

  if(params->sr != 1.0)
    harmtap_reset_trans(params);
}

static void 
harmtap_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(ac > 0)
    {
      /* first set delay line */
      if(fts_is_a(at, delayline_metaclass))
	harmtap_set_delayline(o, 0, 0, 1, at);  
      
      switch(ac)
	{
	case 3:
	  if(fts_is_number(at + 2))
	    harmtap_set_delay(o, 0, 0, 1, at + 2);
	case 2:
	  if(fts_is_number(at + 1))
	    harmtap_set_pitch(o, 0, 0, 1, at + 1);
	case 1:
	case 0:
	  break;
	}
    }
}

static void
harmtap_set_window(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  harmtap_t *this = (harmtap_t *)o;
  harmtap_params_t *params = ftl_data_get_ptr(this->params);
  
  params->window = fts_get_number_float(at) * params->sr;

  if(params->sr != 1.0)
    {
      harmtap_reset_window(params);
      harmtap_reset_trans(params);
      harmtap_reset_delay(params);
      harmtap_reset_frames(params);
    }
}

static void
harmtap_set_fade(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  harmtap_t *this = (harmtap_t *)o;
  harmtap_params_t *params = ftl_data_get_ptr(this->params);
  
  params->fade = fts_get_number_float(at) * params->sr;

  if(params->sr != 1.0)
    harmtap_reset_fade(params);
}

/**************************************************
 *
 *    class
 *
 */

static void
harmtap_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  harmtap_t *this = (harmtap_t *)o;
  harmtap_params_t *params;

  fts_dsp_add_object(o);

  this->params = ftl_data_new(harmtap_params_t);
  params = ftl_data_get_ptr(this->params);
      
  harmtap_params_init(params);
  harmtap_set(o, 0, 0, ac, at);

  if(params->delayline == NULL)
    fts_object_set_error(o, "First argument of delay~ required");
}


static void
harmtap_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  harmtap_t *this = (harmtap_t *)o;

  ftl_data_free(this->params);
  fts_dsp_remove_object(o);
}

static void
harmtap_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(harmtap_t), harmtap_init, harmtap_delete);

  fts_class_method_varargs(cl, fts_s_put, harmtap_put);
  
  fts_class_method_varargs(cl, fts_s_set, harmtap_set);
  fts_class_method_varargs(cl, fts_new_symbol("window"), harmtap_set_window);
  fts_class_method_varargs(cl, fts_new_symbol("fade"), harmtap_set_fade);
  fts_class_method_varargs(cl, fts_new_symbol("delayline"), harmtap_set_delayline);
  
  fts_class_inlet_int(cl, 0, harmtap_set_pitch);
  fts_class_inlet_float(cl, 0, harmtap_set_pitch);
  
  fts_class_inlet_int(cl, 1, harmtap_set_delay);
  fts_class_inlet_float(cl, 1, harmtap_set_delay);
  
  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);        
  
  harmtap_dsp_symbol = fts_new_symbol("ftl_harmtap");
  dsp_declare_function(harmtap_dsp_symbol, ftl_harmtap);
  
  if(!harmtap_window)
    harmtap_window = fts_fftab_get_sine_first_half(WINDOW_SIZE);
  }

void
signal_harmtap_config(void)
{
  fts_class_install(fts_new_symbol("harmtap~"), harmtap_instantiate);
}
