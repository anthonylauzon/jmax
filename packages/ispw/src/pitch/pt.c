/*
 * Copyright (c) 1993 by IRCAM 
 *	All rights reserved.
 *
 *	For any information regarding this and other IRCAM software, please
 *	send email to:
 *    manager@ircam.fr
 *
 */

#include "fts.h"


#include "pt_obj.h"
#include "pt_tools.h"
#include "pt_meth.h"

#define CLASS_NAME "pt~"
#define ALIAS_NAME "pitchtrack~"
#define DSP_NAME "_pt"

#define INLET_sig 0
#define N_INLETS 1

#define OUTLET_midi 0
#define OUTLET_freq 1
#define N_OUTLETS 2

/* shared by all objects of class */
static fts_symbol_t dsp_symbol = 0;

typedef struct
{
  long gliss_time;
  float reattack_slope_thresh;
  long reattack_time;
  int print_me;
} pt_ctl_t;

typedef struct
{
  float power;
  float pitch;
  int pitch_last_out;
  float reattack_slope;
  float reattack_decay;
  float gliss_frame;
  int peaked;
} pt_stat_t;

typedef struct
{
  float freq;
  int pitch;			/* pitch to send to outlet on clock tick */
  float time;
} pt_out_t;

typedef struct
{
  pt_common_obj_t pt;			/* the pt object (is also an fts object) */
  fts_alarm_t clock;
  pt_ctl_t ctl;
  pt_stat_t stat;
  pt_out_t out;
} pt_t;


/*************************************************
 *
 *		analysis
 */
static void analysis(fts_object_t *o)
{
  pt_t *x = (pt_t *)o;
  float candidate;
  float pitch_power, total_power;
  float new_pitch, new_freq;
  int found_candidate = 0; /* reasonable doubt */
	
  found_candidate = pt_common_find_pitch_candidate(&x->pt, &candidate, &pitch_power, &total_power, x->ctl.print_me);
  if(found_candidate)
    {
      new_pitch = pt_common_candidate_midi_pitch(&x->pt, candidate);
      new_freq = pt_common_candidate_frequency(&x->pt, candidate);
      if(x->ctl.print_me) post("  found candidate: bin %f, note %f (%fHz)\n", candidate, new_pitch, new_freq);
    }
  else
    {
      new_pitch = new_freq = 0.0f;
      if(x->ctl.print_me) post("  sorry, no pitch!\n");
    }

  x->out.freq = new_freq;

  /* test for turnoff:
   * if "new_pitch" is more than 3/4 of a halftone away from the current note,
   * (or from an octave up or down from it) the current note has stopped playing.
   * This also takes care of the case where new_pitch == 0.
   */
  if(x->out.pitch > 0)
    {
      float error = new_pitch - x->out.pitch;
      float last_power = x->stat.power;
	
      if (error > 6) error -= 12;
      else if (error < -6) error += 12;
      if(error > 0.75f || error < -0.75f) x->out.pitch = 0;
      
      /* now test for reattack. */
      if(last_power > total_power)
	{
	  x->stat.peaked = 1;
	  x->stat.reattack_slope = 0;
	}
      else
	{
	  float growth = x->stat.reattack_slope * x->stat.reattack_decay +
	    ((last_power > 0) ? ((total_power - last_power) / last_power) : (0));
		
	  if (growth >= x->ctl.reattack_slope_thresh) x->out.pitch = 0;
	  x->stat.reattack_slope = growth;
	}
    }
  x->stat.power = total_power;
  
  /* test for turnon:
   * we must NOT have a current pitch;
   * we must have a candidate pitch;
   * we must have at least a certain amount of power and "quality".
   * and, if there's a "glisstime" specified,
   * the current pitch has to agree roughly with that of the last frame
   */
  if(!x->out.pitch &&
     found_candidate &&
     pitch_power > x->pt.ctl.power_on &&
     pitch_power > x->pt.ctl.quality_on * total_power && 
     (x->ctl.gliss_time == 0 || (new_pitch >= x->stat.pitch - x->stat.gliss_frame && new_pitch <= x->stat.pitch + x->stat.gliss_frame)))
    {
      int int_pitch = new_pitch + 0.5f;
      float error = new_pitch - int_pitch;
      if(x->ctl.print_me) post("  error %f\n", error);
		
      if(error < 0.4f && error > -0.4f)
	{
	  int pitch_last_out = x->stat.pitch_last_out;

	  if(pt_common_debounce_time_is_up(&x->pt, &x->out.time) || 
	     (int_pitch != pitch_last_out &&
	      int_pitch != pitch_last_out + 12 &&
	      int_pitch != pitch_last_out - 12)
	     )
	    {
	      x->stat.pitch_last_out = int_pitch;
	      x->stat.peaked = 0;
	      x->stat.reattack_slope = 0;
	      fts_alarm_set_delay(&x->clock, 0.01f); /* output that stuff */
	      fts_alarm_arm(&x->clock);
	    }
	  x->out.pitch = int_pitch;
	}
      else if(x->ctl.print_me)
	post("  out of tune!\n");
    }
  x->ctl.print_me = 0;
  x->stat.pitch = new_pitch;
}

/*************************************************
 *
 *  user methods
 */

static void pt_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pt_t *x = (pt_t *)o;
  fts_outlet_float(o, OUTLET_freq, x->out.freq);
}

static void pt_gliss_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pt_t *x = (pt_t *)o;
  long n = fts_get_number_arg(ac, at, 0, 0);
	
  x->ctl.gliss_time = (n > 0)? n: 0;
  if(x->ctl.gliss_time)
    x->stat.gliss_frame = (x->pt.n_points - x->pt.n_overlap)/(x->ctl.gliss_time * 0.001f * x->pt.srate);
  else
    x->stat.gliss_frame = 0;
}

static void pt_reattack(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pt_t *x = (pt_t *)o;
  float f = fts_get_number_arg(ac, at, 0, 0);
  long n = fts_get_number_arg(ac, at, 1, 0);
	
  if(f > 0 && n > 0)
    {
      x->ctl.reattack_slope_thresh = f;
      x->ctl.reattack_time = (n<1 ? 1: n);
      x->stat.reattack_decay = exp(-(x->pt.n_points - x->pt.n_overlap)/(.001f * x->ctl.reattack_time * x->pt.srate));
    }
  else
    {
      x->ctl.reattack_slope_thresh = 0;
      x->ctl.reattack_time = 0;
      x->stat.reattack_decay = 0;
    }
}

static void pt_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pt_t *x = (pt_t *)o;
  long n = fts_get_number_arg(ac, at, 0, 1);
	
  x->ctl.print_me = (n > 0)? n: 0;
	
  post("%s:\n", CLASS_NAME);
  post("  gliss-time %d\n", (int)x->ctl.gliss_time);
  post("  reattack %f %d\n", x->ctl.reattack_slope_thresh, (int)x->ctl.reattack_time);
	
  pt_common_print_ctl(&x->pt);
}

/*************************************************
 *
 *  system called methods
 */

static void pt_tick(fts_alarm_t *alarm, void *p)
{
  fts_object_t *o = (fts_object_t *)p;
  pt_t *x = (pt_t *)p;

  fts_alarm_unarm(alarm);

  fts_outlet_int(o, OUTLET_midi, x->out.pitch);
}

static void dsp_fun_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pt_t *x = (pt_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  fts_atom_t a[4];
	
  pt_common_dsp_fun_put(&x->pt, dsp);

  fts_set_long(a, x->ctl.gliss_time);
  pt_gliss_time(o, 0, 0, 1, a);

  fts_set_float(a, x->ctl.reattack_slope_thresh);
  fts_set_long(a+1, x->ctl.reattack_time);
  pt_reattack(o, 0, 0, 2, a);
	
  fts_set_ptr(a, (void *)o);
  fts_set_fun(a+1, analysis);
  fts_set_symbol(a+2, fts_dsp_get_input_name(dsp, 0));
  fts_set_long(a+3, fts_dsp_get_input_size(dsp, 0));
  dsp_add_funcall(dsp_symbol, 4, a);
}

static void pt_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pt_t *x = (pt_t *)o;
  long pt_common_arg_0 = fts_get_number_arg(ac, at, 1, 0);
  long pt_common_arg_1 = fts_get_number_arg(ac, at, 2, 0);
	
  fts_alarm_init(&(x->clock), 0, pt_tick, x);	

  if(!pt_common_init(&x->pt, pt_common_arg_0, pt_common_arg_1)) return;

  x->ctl.gliss_time = 0;
  x->ctl.reattack_slope_thresh = 0.5f;
  x->ctl.reattack_time = 200;
  x->ctl.print_me = 0;
		
  x->stat.power = 0.0f;
  x->stat.pitch = 0.0f;
  x->stat.pitch_last_out = 0;
  x->stat.reattack_slope = 0.0f;
  x->stat.reattack_decay = 0.0f;
  x->stat.gliss_frame = 0.0f;
  x->stat.peaked = 0;

  x->out.freq = 0.0f;
  x->out.pitch = 0;
  x->out.time = 0.0;
	
  dsp_list_insert(o);
}

static void pt_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pt_t *x = (pt_t *)o;

  fts_alarm_unarm(&x->clock);	
  pt_common_delete(&x->pt);	
  dsp_list_remove(o);
}

static fts_status_t pt_class_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[4];
	
  fts_class_init(cl, sizeof(pt_t), N_INLETS, N_OUTLETS, 0);
  pt_common_instantiate(cl);
	
  /* the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_number;
  a[2] = fts_s_number;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, pt_init, 3, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, pt_delete, 0, 0);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, dsp_fun_put, 1, a);
	
  /* class' own methods */
  fts_method_define(cl, 0, fts_s_bang, pt_bang, 0, 0);
  a[0] = fts_s_number;
  fts_method_define(cl, 0, fts_new_symbol("gliss-time"), pt_gliss_time, 1, a);
  a[0] = fts_s_number;
  a[1] = fts_s_number;
  fts_method_define(cl, 0, fts_new_symbol("reattack"), pt_reattack, 2, a);
	
  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_new_symbol("print"), pt_print, 1, a);
	
  /* classes signal inlets and outlets */
  dsp_sig_inlet(cl, INLET_sig);

  /* classes outlets */
  a[0] = fts_s_int;
  fts_outlet_type_define(cl, OUTLET_midi, fts_s_int, 1, a);
  a[0] = fts_s_float;
  fts_outlet_type_define(cl, OUTLET_freq, fts_s_float, 1, a);

  dsp_symbol = fts_new_symbol(DSP_NAME);
  dsp_declare_function(dsp_symbol, pt_common_dsp_function);

  /* DSP properties  */

  /*  fts_class_put_prop(cl, fts_s_dsp_is_sink, fts_true); */
	
  return(fts_Success);
}

void
pt_config(void)
{
  fts_metaclass_create(fts_new_symbol(CLASS_NAME), pt_class_instantiate, fts_always_equiv);
  fts_metaclass_alias(fts_new_symbol(ALIAS_NAME), fts_new_symbol(CLASS_NAME));
}
