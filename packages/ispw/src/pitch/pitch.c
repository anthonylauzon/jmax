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

/* pitch.c */

#include <math.h>
#include "fts.h"

#define FTS_TWO_PI (6.283185307f)

#include "pt_obj.h"
#include "pt_tools.h"
#include "pt_meth.h"

#define CLASS_NAME "pitch~"
#define DSP_NAME "_pitch"

#define INLET_sig 0
#define N_INLETS 1

#define OUTLET_midi_pitch 0
#define OUTLET_attack 1
#define OUTLET_micro_pitch 2
#define OUTLET_power 3
#define N_OUTLETS 4

/* shared by all objects of class */
static fts_symbol_t dsp_symbol = 0;

#define DB_POW(x) (log(x) * 4.342944819f)
#define POWER_OUT_OFFSET 100

#define N_HISTORY 16 /* 0x00010 */
#define HISTORY_MASK (N_HISTORY-1) /* 0x0000F */

typedef struct{
  float    pitch;
  float    power;
} history_t;

typedef struct{
  history_t ory[N_HISTORY];
  int idx;
  int count;
  int n_pitch;
  int n_power;
} pitch_hist_t;

typedef struct{
  long vib_time;
  float vib_depth;
  float reattack_thresh;
  int reattack_time;
  float max_error;
  int print_me;
  int loud;
} pitch_ctl_t;

typedef struct{
  float pitch;    /* pitch of ongoing note; zero if none */
  long last_int_pitch;
  int attack;
  int peaked;
} pitch_stat_t;

typedef struct{
  int attack;
  float midi_pitch;    /* pitch to send to outlet on clock tick */
  float power;
  float micro_pitch;
  float time;
} pitch_out_t;

typedef struct{
  pt_common_obj_t pt;    /* the mandatory fts_object */
  fts_alarm_t clock;
  float *wind;
  pitch_ctl_t ctl;
  pitch_hist_t hist;
  pitch_stat_t stat;
  pitch_out_t out;
} pitch_t;

/*************************************************
 *
 *    analysis
 */
static void 
analysis(pitch_t *x)
{
  float candidate;
  float pitch_power, total_power;
  float new_pitch, new_power;
  int found_candidate = 0; /* reasonable doubt */
  int i;
  
  new_power = pt_common_millers_mean_power(x->pt.buf.main, (float *)x->pt.buf.for_fft, x->wind, x->pt.n_points);
  x->out.power = (new_power > 0.0f)? (DB_POW(new_power)): (-120);

  if(x->ctl.print_me)
    { 
      post("  power measure (in dB) %f\n", x->out.power); 
    }
  
  found_candidate = pt_common_find_pitch_candidate(&x->pt, &candidate, &pitch_power, &total_power, x->ctl.print_me);

  if(found_candidate)
    {
      new_pitch = pt_common_candidate_midi_pitch(&x->pt, candidate);

      if(x->ctl.print_me)
	{
	  post("  found candidate:\n");
	  post("    bin %f\n", candidate);
	  post("    note %f\n", new_pitch);
	}
    }
  else
    {
      if(x->ctl.print_me) 
	post("  sorry, no pitch!\n");

      new_pitch = 0.0f;
    }
  
  /* update history */
  x->hist.idx = (x->hist.idx + 1) & HISTORY_MASK;
  x->hist.ory[x->hist.idx].pitch = new_pitch;
  x->hist.ory[x->hist.idx].power = new_power;

  /* test for turnoff -- if "new_pitch" is more than x->ctl.vib_depth
   * of a halftone away from the current note, (or from an
   * octave up or down from it) the current note has stopped
   * playing.  This also takes care of the case where new_pitch == 0.
   */
  
  if(x->stat.pitch > 0)
    {
      float error = new_pitch - x->stat.pitch;
      
      /* eliminate octave jumps */
      if(error > 6) error -= 12;
      else if(error < -6) error += 12;
      
      if(error > x->ctl.vib_depth || error < -x->ctl.vib_depth)
	{
	  x->stat.pitch = 0;
	  x->hist.ory[(x->hist.idx - 1) & HISTORY_MASK].pitch = 0.0f;
	  /* bash previous pitch history. This prevents the stability criterion
	   * from being met until x->hist.n_pitch new pitches have been measured.
	   */
	}
    }
  
  /* test for reattack:
   * it is a condition on the amplitude envelope.
   * Reattack is also marked by setting current pitch to zero;
   * the note-on conditions must apply before a new note is output.
   */
  if(!x->stat.peaked)
    {
      if(x->hist.n_power && (new_power * (x->ctl.reattack_thresh + 1) < x->hist.ory[(x->hist.idx - x->hist.n_power) & HISTORY_MASK].power))
	{
	  x->stat.peaked = 1;
	}
    }
  else{
    if(x->hist.n_power &&
      pitch_power > x->pt.ctl.power_on &&
      (new_power > (x->ctl.reattack_thresh + 1) * x->hist.ory[(x->hist.idx - x->hist.n_power) & HISTORY_MASK].power)
    ){
      x->stat.pitch = 0.0f;

      if(x->ctl.loud)
	{
	  post("%s: reattack: power %f\n", CLASS_NAME, sqrt(pitch_power)/x->pt.n_points);
	}
      
      /* here's the good part:
       * there's a reattack and we haven't reported a note yet since the last one,
       * look again RETROACTIVELY, using laxer criteria
       */
      if(x->hist.count < 100)
	{ /* '100' seems like a value god gave to you, right? Miller? */
	  float best_pitch = 0.0f;
	  float best_power = 0.0f;
	  float recent_power = new_power;
	  int look_for_best = 0;
	  int hist_depth = (x->hist.count > N_HISTORY)? N_HISTORY: x->hist.count;
	  
	  for(i=1; i<hist_depth; i++)
	    {
	      float history_power = x->hist.ory[(x->hist.idx - i) & HISTORY_MASK].power;
	      
	      if(!look_for_best)
		{
		  if(history_power > recent_power) 
		    look_for_best = 1;

		  recent_power = history_power;
		}
	      else if(history_power > best_power)
		{
		  best_power = history_power;
		  best_pitch = x->hist.ory[(x->hist.idx - i) & HISTORY_MASK].pitch;
		}
	    }
	  
	  if(best_power > 0) 
	    x->stat.pitch = x->out.midi_pitch = best_pitch;
	}
      x->hist.count = 0; 
      x->stat.peaked = 0;
      x->out.attack = 1;
    }
  }
  
  x->hist.count++; /* count the ticks since the last reattack */
  
  /* test for turnon:
   * we must NOT have a current pitch;
   * we must have a candidate pitch;
   * we must have at least a certain amount of power and "quality".
   */
  if(x->stat.pitch == 0.0f &&
    found_candidate &&
    pitch_power > x->pt.ctl.power_on &&
    pitch_power > x->pt.ctl.quality_on * total_power
  ){
    int int_pitch;
    int deviated = 0;
    float error, pitch_average;
    
    /* in addition we must have a stable pitch in the sense that it
     * does not deviate more than x->ctl.vib_depth from the mean over the
     * last x->hist.n_pitch values.
     */
    int_pitch = (int)(new_pitch + 0.5f);
    error = new_pitch - int_pitch;
    if(x->hist.n_pitch){
      pitch_average = 0;
      for(i=0; i<x->hist.n_pitch; i++)
        pitch_average += x->hist.ory[(x->hist.idx-i) & HISTORY_MASK].pitch;
      pitch_average /= x->hist.n_pitch;
      
      for(i=0; i<x->hist.n_pitch; i++){
        float deviation = x->hist.ory[(x->hist.idx-i) & HISTORY_MASK].pitch - pitch_average;
        if(x->ctl.print_me){
	  post("  deviation %f\n", deviation);
	}
        if(deviation < -x->ctl.vib_depth || deviation > x->ctl.vib_depth){
          if(x->ctl.print_me) 
	    post("  pitch deviated! -> called off\n");
          deviated = 1;
        }
      }
    }else{
      pitch_average = new_pitch;
    }
    
    if(
      error < x->ctl.max_error &&
      error > -x->ctl.max_error &&
      !deviated
    ){
      int last_int_pitch = x->stat.last_int_pitch;
      if(pt_common_debounce_time_is_up(&x->pt, &x->out.time) ||
        (int_pitch != last_int_pitch &&
        int_pitch != last_int_pitch + 12 &&
        int_pitch != last_int_pitch - 12)
      ){
        if(x->ctl.loud){
          post("%s: ongoing note: %f\n", CLASS_NAME, pitch_average);
	  post("  power: %f\n", new_power);
	  post("  relative power (in %): %f\n", pitch_power / total_power);
          if(x->hist.n_pitch) 
	    post("  stable over %d points!\n", x->hist.n_pitch);
          else 
	    post("  no stability criterion!\n");
        }
        x->stat.last_int_pitch = int_pitch;
        x->stat.peaked = 0;
        x->hist.count = 1000; /* acts like last reattack has been a long long time ago */
        x->stat.pitch = x->out.midi_pitch = pitch_average;
      }
    }else if(x->ctl.print_me) post("  out of tune!\n");
  }
  x->out.micro_pitch = new_pitch;
  
  x->ctl.print_me = 0;
  fts_alarm_set_delay(&x->clock, 0.01f); /* output that stuff */
  fts_alarm_arm(&x->clock);
}

/*************************************************
 *
 *    user methods
 */
 
static void 
pitch_vibrato(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pitch_t *x = (pitch_t *)o;
  long n = fts_get_int_arg(ac, at, 0, 0);
  float f = fts_get_float_arg(ac, at, 1, 0);
  
  x->ctl.vib_time = (n >= 0)? n: 0;
  x->ctl.vib_depth = (f >= 0)? f: 0;
  x->hist.n_pitch = (x->ctl.vib_time * 0.001f * x->pt.srate)/(x->pt.n_points - x->pt.n_overlap);
  if(x->hist.n_pitch >= N_HISTORY) x->hist.n_pitch = N_HISTORY-1;
  else if(x->hist.n_pitch < 2) x->hist.n_pitch = 0;
}

static void 
pitch_max_error(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pitch_t *x = (pitch_t *)o;
  float f = fts_get_float_arg(ac, at, 0, 0);
  
  if(f < 0.0f) f = 0.0f;
  else if(f > 0.5f) f = 0.5f;
  x->ctl.max_error = f;
}

static void 
pitch_reattack(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pitch_t *x = (pitch_t *)o;
  float f = (float) fts_get_float_arg(ac, at, 0, 0);
  long n = fts_get_int_arg(ac, at, 1, 0);
  
  x->ctl.reattack_thresh = (f > 0)? f: 0;
  x->ctl.reattack_time = (n > 0)? n: 0;
  x->hist.n_power = (x->ctl.reattack_time * 0.001f * x->pt.srate)/(x->pt.n_points - x->pt.n_overlap);
  if(x->hist.n_power >= N_HISTORY) x->hist.n_power = N_HISTORY-1;
}

static void 
pitch_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pitch_t *x = (pitch_t *)o;
  long n = fts_get_int_arg(ac, at, 0, 1);
  
  x->ctl.print_me = (n > 0)? n: 0;
  
  post("%s:\n", CLASS_NAME);
  
  pt_common_print_obj(&x->pt);
  
  post("  vibrato: %d %f\n", (int)x->ctl.vib_time, x->ctl.vib_depth);
  post("  max-error: %f\n", x->ctl.max_error);
  post("  reattack threshold: %f\n", x->ctl.reattack_thresh);
  post("  reattack time: %ld\n", x->ctl.reattack_time);
  post("  loud: %d\n", (int)x->ctl.loud);
  
  pt_common_print_ctl(&x->pt);
}

static void 
pitch_loud(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{  
  ((pitch_t *)o)->ctl.loud = fts_get_int_arg(ac, at, 0, 0);
}

/*************************************************
 *
 *    system called methods
 */

static void 
pitch_tick(fts_alarm_t *alarm, void *p)
{
  fts_object_t *o = (fts_object_t *)p;
  pitch_t *x = (pitch_t *)p;

  fts_alarm_unarm(alarm);

  fts_outlet_float(o, OUTLET_micro_pitch, x->out.micro_pitch);
  fts_outlet_float(o, OUTLET_power, x->out.power + POWER_OUT_OFFSET);
  if(x->out.midi_pitch > 0.0f){
    fts_outlet_float(o, OUTLET_midi_pitch, x->out.midi_pitch);
    x->out.midi_pitch = 0.0f;
  }
  if(x->out.attack){
    fts_outlet_bang(o, OUTLET_attack);
    x->out.attack = 0;
  } 
}

static void 
pitch_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pitch_t *x = (pitch_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  fts_atom_t a[5];
  
  pt_common_dsp_fun_put(&x->pt, dsp);
  
  fts_set_long(a, x->ctl.vib_time);
  fts_set_float(a+1, x->ctl.vib_depth);
  pitch_vibrato(o, 0, 0, 2, a);
  
  fts_set_float(a, x->ctl.reattack_thresh);
  fts_set_long(a+1, x->ctl.reattack_time);
  pitch_reattack(o, 0, 0, 2, a);
  
  fts_set_ptr(a, (void *)o);
  fts_set_fun(a+1, (void (*)(void))analysis);
  fts_set_symbol(a+2, fts_dsp_get_input_name(dsp, 0));
  fts_set_long(a+3, fts_dsp_get_input_size(dsp, 0));
  dsp_add_funcall(dsp_symbol, 4, a);
}

static void 
pitch_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pitch_t *x = (pitch_t *)o;
  long pt_common_arg_0 = fts_get_int_arg(ac, at, 1, 0);
  long pt_common_arg_1 = fts_get_int_arg(ac, at, 2, 0);
  int i;
  
  fts_alarm_init(&(x->clock), 0, pitch_tick, x);  

  if(!pt_common_init(&x->pt, pt_common_arg_0, pt_common_arg_1)) return;

  for(i=0; i<N_HISTORY; i++) x->hist.ory[i].power = x->hist.ory[i].pitch = 0.0f;
  x->hist.idx = 0;
  x->hist.count = 1000;
  x->hist.n_pitch = x->hist.n_power = 0;
  
  x->ctl.vib_time = 0.75f;
  x->ctl.vib_depth = 0;
  x->ctl.reattack_thresh = 0;
  x->ctl.reattack_time = 0;
  x->ctl.max_error = 0.4f;
  x->ctl.print_me = 0;
  x->ctl.loud = 0;

  x->wind = (float *)fts_malloc(x->pt.n_points * sizeof(float));
  if(!x->wind)
    {
      post("%s: can not allocate window: init failed\n", CLASS_NAME);
      return;
    } 
  else
    {
      float inv_n_points = 1.0f / x->pt.n_points;
      for(i=0; i<x->pt.n_points; i++)
	x->wind[i] = inv_n_points * (1.0f - cos(i * inv_n_points * FTS_TWO_PI));
    }

  x->stat.pitch = 0.0f;
  x->stat.last_int_pitch = 0;
  x->stat.attack = 0;
  x->stat.peaked = 0;
  
  x->out.attack = 0;
  x->out.midi_pitch = 0.0f;
  x->out.power = 0.0f;
  x->out.micro_pitch = 0.0f;
  x->out.time = 0.0;
  
  dsp_list_insert(o);
}

static void 
pitch_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pitch_t *x = (pitch_t *)o;

  fts_alarm_unarm(&x->clock);  
  pt_common_delete(&x->pt);
  if(x->wind) fts_free(x->wind);
  dsp_list_remove(o);
}

static fts_status_t 
class_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[4];
  
  fts_class_init(cl, sizeof(pitch_t), N_INLETS, N_OUTLETS, 0);
  pt_common_instantiate(cl);

  /* the system methods */
  a[0] = fts_s_symbol;
  a[1] = fts_s_number;
  a[2] = fts_s_number;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, pitch_init, 3, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, pitch_delete, 0, 0);
  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, pitch_put, 1, a);
  
  /* class' own methods */
  a[0] = fts_s_number;
  a[1] = fts_s_number;
  fts_method_define(cl, 0, fts_new_symbol("vibrato"), pitch_vibrato, 2, a);
  a[0] = fts_s_number;
  fts_method_define(cl, 0, fts_new_symbol("max-error"), pitch_max_error, 1, a);
  a[0] = fts_s_number;
  a[1] = fts_s_number;
  fts_method_define(cl, 0, fts_new_symbol("reattack"), pitch_reattack, 2, a);
  
  a[0] = fts_s_number;
  fts_method_define(cl, 0, fts_new_symbol("loud"), pitch_loud, 1, a);
  a[0] = fts_s_number;
  fts_method_define(cl, 0, fts_new_symbol("print"), pitch_print, 1, a);

  /* classes signal inlets and outlets */
  dsp_sig_inlet(cl, INLET_sig);

  /* classes outlets */
  fts_outlet_type_define(cl, OUTLET_attack, fts_s_bang, 0, 0);
  a[0] = fts_s_float;
  fts_outlet_type_define(cl, OUTLET_midi_pitch, fts_s_float, 1, a);
  a[0] = fts_s_float;
  fts_outlet_type_define(cl, OUTLET_power, fts_s_float, 1, a);
  a[0] = fts_s_float;
  fts_outlet_type_define(cl, OUTLET_micro_pitch, fts_s_float, 1, a);

  dsp_symbol = fts_new_symbol(DSP_NAME);
  dsp_declare_function(dsp_symbol, pt_common_dsp_function);

  /* DSP properties  */

  /* fts_class_put_prop(cl, fts_s_dsp_is_sink, fts_true); */
  
  return(fts_Success);
}

void 
pitch_config(void)
{
  fts_class_install(fts_new_symbol(CLASS_NAME),class_instantiate);
}
