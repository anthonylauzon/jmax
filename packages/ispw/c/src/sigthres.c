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

/* sigthresh.c */

#include <fts/fts.h>


typedef struct
{
  int status; /* true if waiting for the low threshold */
  float hi_thresh;
  float lo_thresh;
  long hi_dead_samples;
  long lo_dead_samples;
  long wait; /* ticks to wait before becoming active */
  fts_timer_t *timer;
} sigthres_state_t;

typedef struct
{
  fts_object_t _o;
  sigthres_state_t ctl;
  float hi_dead_msec;
  float lo_dead_msec;
  float samples_per_msec;
} sigthres_t;

static fts_symbol_t sigthres_symbol = 0;

/************************************************
 *
 *    methods
 *
 */

static void
sigthres_alarm(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigthres_t *this = (sigthres_t *)o;

  if (this->ctl.status)
    fts_outlet_bang((fts_object_t *)o, 0);
  else
    fts_outlet_bang((fts_object_t *)o, 1);
}

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

  this->ctl.status = (int)fts_get_int_arg(ac, at, 0, 0);
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
  long n_tick = fts_word_get_int(&argv[DSP_ARG_n_tick]);
  
  if (ctl->wait)
    ctl->wait--;
  else if (ctl->status)
    {
      if (in0[n_tick-1] < ctl->lo_thresh)
	{
	  ctl->status = 0;
	  ctl->wait = ctl->lo_dead_samples;
	  fts_timer_set_delay(ctl->timer, 0.0, 0);
	}
    }
  else if (in0[n_tick-1] >= ctl->hi_thresh)
    {
      ctl->status = 1;
      ctl->wait = ctl->hi_dead_samples;
      fts_timer_set_delay(ctl->timer, 0.0, 0);
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
  fts_set_int(&argv[DSP_ARG_n_tick], fts_dsp_get_input_size(dsp, 0));
  dsp_add_funcall(sigthres_symbol, N_DSP_ARGS, argv);
}

/************************************************
 *
 *    class
 *
 */

static void
sigthres_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigthres_t *this = (sigthres_t *)o;
  
  this->ctl.hi_thresh = fts_get_float_arg(ac, at, 0, 0.0f);
  this->hi_dead_msec  = fts_get_float_arg(ac, at, 1, 0.0f);
  this->ctl.lo_thresh = fts_get_float_arg(ac, at, 2, 0.0f);
  this->lo_dead_msec  = fts_get_float_arg(ac, at, 3, 0.0f);
  
  this->ctl.status = 0;
  this->ctl.wait = 0;
  this->samples_per_msec = 0.0f; /* correct set in put routine */
  
  this->ctl.timer = fts_timer_new(o, 0);
  dsp_list_insert(o);
}

static void
sigthres_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigthres_t *this = (sigthres_t *)o;

  fts_timer_delete(this->ctl.timer);
  dsp_list_remove(o);
}

static fts_status_t
class_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t obj_arg[4];

  fts_class_init(cl, sizeof(sigthres_t), 5, 2, 0);

  /* system methods */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, sigthres_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, sigthres_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, sigthres_put_function);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_timer_alarm, sigthres_alarm);
  
  /* user methods */  
  fts_method_define_varargs(cl, 0, fts_s_int, sigthres_status_int);
  fts_method_define_varargs(cl, 0, fts_s_float, sigthres_status_float);

  fts_method_define_varargs(cl, 0, fts_new_symbol("set"), sigthres_set);
  
  fts_method_define_varargs(cl, 1, fts_s_float, sigthres_hi_thresh);
  fts_method_define_varargs(cl, 1, fts_s_int, sigthres_hi_thresh);

  fts_method_define_varargs(cl, 2, fts_s_float, sigthres_hi_time);
  fts_method_define_varargs(cl, 2, fts_s_int, sigthres_hi_time);

  fts_method_define_varargs(cl, 3, fts_s_float, sigthres_lo_thresh);
  fts_method_define_varargs(cl, 3, fts_s_int, sigthres_lo_thresh);

  fts_method_define_varargs(cl, 4, fts_s_float, sigthres_lo_time);
  fts_method_define_varargs(cl, 4, fts_s_int, sigthres_lo_time);

  dsp_sig_inlet(cl, 0);

  fts_outlet_type_define_varargs(cl, 0, fts_s_bang);
  fts_outlet_type_define_varargs(cl, 1, fts_s_bang);
  
  dsp_declare_function(sigthres_symbol, sigthres_dsp);
    
  return fts_Success;
}

void
sigthreshold_config(void)
{
  sigthres_symbol = fts_new_symbol("threshold~");
  fts_class_install(sigthres_symbol, class_instantiate);
}
