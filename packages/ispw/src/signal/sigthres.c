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
/* sigthresh.c */

#include "fts.h"


#define CLASS_NAME "threshold~"
#define DSP_NAME "_threshold"

typedef struct
{
  int status; /* true if waiting for the low threshold */
  float hi_thresh;
  float lo_thresh;
  long hi_dead_samples;
  long lo_dead_samples;
  long wait; /* ticks to wait before becoming active */
  fts_alarm_t alarm;
} sigthres_state_t;

typedef struct
{
  fts_object_t _o;
  sigthres_state_t ctl;
  float hi_dead_msec;
  float lo_dead_msec;
  float samples_per_msec;
} sigthres_t;

enum
{
  OBJ_ARG_class = 0,
  OBJ_ARG_hi_thresh = 1,
  OBJ_ARG_hi_dead_msec = 2,
  OBJ_ARG_lo_thresh = 3,
  OBJ_ARG_lo_dead_msec = 4,
  N_OBJ_ARGS = 5
};
  
enum
{
  INLET_sig = 0,
  INLET_status = 0,
  INLET_hi_thresh = 1,
  INLET_hi_time = 2,
  INLET_lo_thresh = 3,
  INLET_lo_time = 4,
  N_INLETS
};

enum
{
  OUTLET_hi_bang = 0,
  OUTLET_lo_bang = 1,
  N_OUTLETS
};

static fts_symbol_t sigthres_symbol = 0;
static void alarm_tick(fts_alarm_t *alarm, void *o);

/************************************************
 *
 *    object
 *
 */

static void
sigthres_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigthres_t *this = (sigthres_t *)o;
  
  this->ctl.hi_thresh = fts_get_float_arg(ac, at, OBJ_ARG_hi_thresh, 0.0f);
  this->hi_dead_msec  = fts_get_float_arg(ac, at, OBJ_ARG_hi_dead_msec, 0.0f);
  this->ctl.lo_thresh = fts_get_float_arg(ac, at, OBJ_ARG_lo_thresh, 0.0f);
  this->lo_dead_msec  = fts_get_float_arg(ac, at, OBJ_ARG_lo_dead_msec, 0.0f);
  
  this->ctl.status = 0;
  this->ctl.wait = 0;
  this->samples_per_msec = 0.0f; /* correct set in put routine */
  
  fts_alarm_init(&(this->ctl.alarm), 0, alarm_tick, this);
  dsp_list_insert(o);
}

static void
sigthres_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigthres_t *this = (sigthres_t *)o;

  fts_alarm_unarm(&(this->ctl.alarm));
  dsp_list_remove(o);
}

/************************************************
 *
 *    alarms
 *
 */

static void
alarm_tick(fts_alarm_t *alarm, void *o)
{
  sigthres_t *this = (sigthres_t *)o;

  fts_alarm_unarm(alarm);

  if (this->ctl.status)
    fts_outlet_bang((fts_object_t *)o, OUTLET_hi_bang);
  else
    fts_outlet_bang((fts_object_t *)o, OUTLET_lo_bang);
}

/************************************************
 *
 *    methods
 *
 */

static void
set_times(sigthres_t *this)
{
  long hi_dead_samples = (long)(this->samples_per_msec * this->hi_dead_msec) - 1;
  long lo_dead_samples = (long)(this->samples_per_msec * this->lo_dead_msec) - 1;
  
  this->ctl.hi_dead_samples = (hi_dead_samples > 0)? hi_dead_samples: 0;
  this->ctl.lo_dead_samples = (lo_dead_samples > 0)? lo_dead_samples: 0;
}

static void
sigthres_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigthres_t *this = (sigthres_t *)o;

  /* fall thru switch !! */

  switch (ac)
    {
    case 4:
      this->lo_dead_msec = fts_get_float_arg(ac, at, 3, 0.0f);
    case 3:
      this->ctl.lo_thresh = fts_get_float_arg(ac, at, 2, 0.0f);
    case 2:
      this->hi_dead_msec = fts_get_float_arg(ac, at, 1, 0.0f);
    case 1:
      this->ctl.hi_thresh = fts_get_float_arg(ac, at, 0, 0.0f);
    }

  if (this->ctl.lo_thresh > this->ctl.hi_thresh)
    this->ctl.lo_thresh = this->ctl.hi_thresh;

  set_times(this);
}

static void
sigthres_status_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigthres_t *this = (sigthres_t *)o;

  this->ctl.status = (int)fts_get_long_arg(ac, at, 0, 0);
  this->ctl.wait = 0;
}

static void
sigthres_status_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigthres_t *this = (sigthres_t *)o;

  this->ctl.status = fts_get_int_arg(ac, at, 0, 0);
  this->ctl.wait = 0;
}

static void
sigthres_hi_thresh(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigthres_t *this = (sigthres_t *)o;

  this->ctl.hi_thresh = fts_get_float_arg(ac, at, 0, 0.0f);

  if (this->ctl.lo_thresh > this->ctl.hi_thresh)
    this->ctl.lo_thresh = this->ctl.hi_thresh;
}

static void
sigthres_hi_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigthres_t *this = (sigthres_t *)o;
  long hi_dead_msec = fts_get_int_arg(ac, at, 0, 0);
  long hi_dead_samples = (long)(this->samples_per_msec * hi_dead_msec) - 1;
  
  this->ctl.hi_dead_samples = (hi_dead_samples > 0)? hi_dead_samples: 0;
  this->hi_dead_msec = hi_dead_msec;
}
  
static void
sigthres_lo_thresh(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigthres_t *this = (sigthres_t *)o;

  this->ctl.lo_thresh = fts_get_float_arg(ac, at, 0, 0.0f);

  if (this->ctl.lo_thresh > this->ctl.hi_thresh)
    this->ctl.lo_thresh = this->ctl.hi_thresh;
}
  
static void
sigthres_lo_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigthres_t *this = (sigthres_t *)o;
  long lo_dead_msec = fts_get_int_arg(ac, at, 0, 0);
  long lo_dead_samples = (long)(this->samples_per_msec * lo_dead_msec) - 1;
  
  this->ctl.lo_dead_samples = (lo_dead_samples > 0)? lo_dead_samples: 0;
  this->lo_dead_msec = lo_dead_msec;
}
  
/************************************************
 *
 *    dsp
 *
 */

enum
{
  DSP_ARG_ctl = 0,
  DSP_ARG_in0 = 1,
  DSP_ARG_n_tick = 2,
  N_DSP_ARGS = 3
};


static void
sigthres_dsp(fts_word_t *argv)
{
  sigthres_state_t *ctl = (sigthres_state_t *)fts_word_get_ptr(&argv[DSP_ARG_ctl]);
  float *in0 = (float *)fts_word_get_ptr(&argv[DSP_ARG_in0]);
  long n_tick = fts_word_get_long(&argv[DSP_ARG_n_tick]);
  
  if (ctl->wait)
    ctl->wait--;
  else if (ctl->status)
    {
      if (in0[n_tick-1] < ctl->lo_thresh)
	{
	  ctl->status = 0;
	  ctl->wait = ctl->lo_dead_samples;
	  fts_alarm_set_delay(&ctl->alarm, 0.01f);
	  fts_alarm_arm(&ctl->alarm);
	}
    }
  else if (in0[n_tick-1] >= ctl->hi_thresh)
    {
      ctl->status = 1;
      ctl->wait = ctl->hi_dead_samples;
      fts_alarm_set_delay(&ctl->alarm, 0.01f);
      fts_alarm_arm(&ctl->alarm);
    }
}

static void
sigthres_put_function(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigthres_t *this = (sigthres_t *)o;
  fts_atom_t argv[N_DSP_ARGS];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);

  this->samples_per_msec = fts_dsp_get_input_srate(dsp, 0) / (1000.0f * fts_dsp_get_input_size(dsp, 0));
  set_times(this);

  fts_set_ptr(&argv[DSP_ARG_ctl], &(this->ctl));
  fts_set_symbol(&argv[DSP_ARG_in0], fts_dsp_get_input_name(dsp, 0));
  fts_set_long(&argv[DSP_ARG_n_tick], fts_dsp_get_input_size(dsp, 0));
  dsp_add_funcall(sigthres_symbol, N_DSP_ARGS, argv);
}

/************************************************
 *
 *    class
 *
 */

static fts_status_t
class_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t obj_arg[N_OBJ_ARGS];
  fts_symbol_t a[5];

  fts_class_init(cl, sizeof(sigthres_t), N_INLETS, N_OUTLETS, 0);

  /* system methods */
  
  obj_arg[OBJ_ARG_class] = fts_s_symbol;
  obj_arg[OBJ_ARG_hi_thresh] = fts_s_number;
  obj_arg[OBJ_ARG_hi_dead_msec] = fts_s_number;
  obj_arg[OBJ_ARG_lo_thresh] = fts_s_number;
  obj_arg[OBJ_ARG_lo_dead_msec] = fts_s_number;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, sigthres_init, N_OBJ_ARGS, obj_arg, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sigthres_delete, 0, 0);

  a[0] = fts_s_ptr;  
  fts_method_define(cl, fts_SystemInlet, fts_s_put, sigthres_put_function, 1, a);
  
  sigthres_symbol = fts_new_symbol(DSP_NAME);
  dsp_declare_function(sigthres_symbol, sigthres_dsp);
    
  /* in/outlets */
  
  dsp_sig_inlet(cl, INLET_sig);
  fts_outlet_type_define(cl, OUTLET_hi_bang, fts_s_bang, 0, 0);
  fts_outlet_type_define(cl, OUTLET_lo_bang, fts_s_bang, 0, 0);
  
  /* user methods */
  
  a[0] = fts_s_int;
  fts_method_define(cl, INLET_status, fts_s_int, sigthres_status_int, 1, a);
  a[0] = fts_s_number;
  fts_method_define(cl, INLET_status, fts_s_float, sigthres_status_float, 1, a);

  a[0] = fts_s_number;
  a[1] = fts_s_number;
  a[2] = fts_s_number;
  a[3] = fts_s_number;

  fts_method_define_optargs(cl, INLET_status, fts_new_symbol("set"), sigthres_set, 4, a, 0);
  
  a[0] = fts_s_float;
  fts_method_define(cl, INLET_hi_thresh, fts_s_float, sigthres_hi_thresh, 1, a);
  a[0] = fts_s_int;
  fts_method_define(cl, INLET_hi_thresh, fts_s_int, sigthres_hi_thresh, 1, a);
  a[0] = fts_s_float;
  fts_method_define(cl, INLET_hi_time, fts_s_float, sigthres_hi_time, 1, a);
  a[0] = fts_s_int;
  fts_method_define(cl, INLET_hi_time, fts_s_int, sigthres_hi_time, 1, a);
  a[0] = fts_s_float;
  fts_method_define(cl, INLET_lo_thresh, fts_s_float, sigthres_lo_thresh, 1, a);
  a[0] = fts_s_int;
  fts_method_define(cl, INLET_lo_thresh, fts_s_int, sigthres_lo_thresh, 1, a);
  a[0] = fts_s_float;
  fts_method_define(cl, INLET_lo_time, fts_s_float, sigthres_lo_time, 1, a);
  a[0] = fts_s_int;
  fts_method_define(cl, INLET_lo_time, fts_s_int, sigthres_lo_time, 1, a);

  return fts_Success;
}

void
sigthreshold_config(void)
{
  fts_class_install(fts_new_symbol(CLASS_NAME),class_instantiate);
}





