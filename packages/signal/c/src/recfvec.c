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
 * Authors: Riccardo Borghesi, Francois Dechelle, Norbert Schnell
 *
 */

#include <fts/fts.h>
#include "fvec.h"

#define MAX_INT 2147483647

/************************************************************
 *
 *  rec
 *
 */

typedef struct _rec_fvec_
{
  fts_object_t o;

  fvec_t *fvec;

  int begin; /* begin pasition */
  int end; /* end pasition */
  int index; /* current index */

  double sr; /* sample rate (in kHz) */
  double sp; /* sample period (in msec) */

  enum rec_mode {mode_stop, mode_pause, mode_rec} mode;
  
  fts_alarm_t alarm;
} rec_fvec_t;

static fts_symbol_t sym_rec = 0;

/************************************************************
 *
 *  methods
 *
 */

static void 
rec_fvec_bang_at_end(fts_alarm_t *alarm, void *o)
{
  fts_outlet_bang((fts_object_t *)o, 0);
}

static void
rec_fvec_set_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  rec_fvec_t *this = (rec_fvec_t *)o;
  fvec_t *fvec = fvec_atom_get(at);
  int size = fvec_get_size(fvec);

  if(this->fvec)
    fts_object_release((fts_object_t *)this->fvec);

  this->fvec = fvec;

  fts_object_refer((fts_object_t *)fvec);
}

static void 
rec_fvec_set_begin(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  rec_fvec_t *this = (rec_fvec_t *)o;
  int value = fts_get_number_float(at) * this->sr;
  
  if(value < 0.0)
    value = 0.0;

  this->begin = value;
}

static void 
rec_fvec_set_end(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  rec_fvec_t *this = (rec_fvec_t *)o;
  int value = fts_get_number_float(at) * this->sr;
  
  if(value < 0.0)
    value = 0.0;

  this->end = value;
}

static void 
rec_fvec_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  rec_fvec_t *this = (rec_fvec_t *)o;

  switch (ac)
    {
    default:
    case 3:
      rec_fvec_set_end(o, 0, 0, 1, at + 2);
    case 2:
      rec_fvec_set_begin(o, 0, 0, 1, at + 1);
    case 1:
      rec_fvec_set_fvec(o, 0, 0, 1, at);
      break;
    case 0:
      break;
    }
}

static void 
rec_fvec_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  rec_fvec_t *this = (rec_fvec_t *)o;

  this->index = this->begin;
  this->mode = mode_rec;
}

static void 
rec_fvec_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  rec_fvec_t *this = (rec_fvec_t *)o;

  rec_fvec_set(o, 0, 0, ac, at);
  rec_fvec_bang(o, 0, 0, 0, 0);
}

static void 
rec_fvec_rec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  rec_fvec_t *this = (rec_fvec_t *)o;

  switch(this->mode)
    {
    case mode_stop:
      this->index = this->begin;
    case mode_pause:
      this->mode = mode_rec;
    default:
      break;
    }
}

static void
rec_fvec_pause(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  rec_fvec_t *this = (rec_fvec_t *)o;

  this->mode = mode_pause;
}

static void
rec_fvec_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  rec_fvec_t *this = (rec_fvec_t *)o;

  this->mode = mode_stop;
}

/************************************************************
 *
 *  dsp
 *
 */

static void
rec_fvec_reset(rec_fvec_t *this, int n_tick, double sr)
{
  double sp = 1000. / sr;

  this->sp = sp;
  this->sr = 0.001 * sr;
}

static void
rec_fvec_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  rec_fvec_t *this = (rec_fvec_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_ptr(at);
  int n_tick = fts_dsp_get_input_size(dsp, 0);
  double sr = fts_dsp_get_input_srate(dsp, 0);
  fts_atom_t a[3];

  rec_fvec_reset(this, n_tick, sr);

  fts_set_ptr(a + 0, this);
  fts_set_symbol(a + 1, fts_dsp_get_input_name(dsp, 0));
  fts_set_int(a + 2, n_tick);
  
  dsp_add_funcall(sym_rec, 3, a);
}

static void
rec_fvec_ftl(fts_word_t *argv)
{
  rec_fvec_t *this = (rec_fvec_t *) fts_word_get_ptr(argv + 0);
  float *in = (float *) fts_word_get_ptr(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  fvec_t *fvec = this->fvec;
  float *buf = fvec_get_ptr(fvec);
  int size = fvec_get_size(fvec);
  int begin = this->begin;
  int end = this->end;
  int index = this->index;

  /* clip begin and end into size */
  if(begin >= size)
    begin = size;
  if(end > size)
    end = size;

  /* turn movement into range */
  if(index > end)
    index = end;
  else if(index  < begin)
    index = begin;

  if(this->mode >= mode_rec)
    {
      int end_index = index + n_tick;

      if(end_index <= end)
	{
	  int i;
	  
	  for(i=0; i<n_tick; i++)
	    {
	      buf[index] = in[i];
	      index++;
	    }
	  
	  this->index = end_index;
	}
      else
	{
	  int i;
	  
	  for(i=0; i<n_tick; i++)
	    {
	      if(this->mode == mode_rec)
		{
		  /* increment */
		  buf[index] = in[i];
		  index++;
		  
		  if(index >= end)
		    {
		      /* end reached */
		      fts_alarm_set_delay(&this->alarm, 0.0);
		      
		      index = end;
		      
		      this->mode = mode_stop;
		    }
		}
	    }

	  this->index = index;  
	}
    }
}

/************************************************************
 *
 *  class
 *
 */

static void
rec_fvec_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  rec_fvec_t *this = (rec_fvec_t *)o;

  ac--;
  at++;

  dsp_list_insert(o);

  this->index = 0.0;

  this->mode = mode_stop;

  this->fvec = 0;
  this->begin = 0.0;
  this->end = MAX_INT;

  this->sr = 0.001 * fts_dsp_get_sample_rate();
  this->sp = 1000. / fts_dsp_get_sample_rate();

  /* init output alarm */
  fts_alarm_init(&this->alarm, 0, rec_fvec_bang_at_end, this);    

  if(ac > 0 && fvec_atom_is(at))
    rec_fvec_set(o, 0, 0, ac, at);
  else
    fts_object_set_error(o, "First argument of fvec required");
}

static void
rec_fvec_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  rec_fvec_t *this = (rec_fvec_t *)o;

  fts_alarm_reset(&this->alarm);
  dsp_list_remove(o);
}

static fts_status_t
rec_fvec_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int i;

  fts_class_init(cl, sizeof(rec_fvec_t), 3, 1, 0); 
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, rec_fvec_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, rec_fvec_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, rec_fvec_put);

  fts_method_define_varargs(cl, 0, fts_s_bang, rec_fvec_bang);
  fts_method_define_varargs(cl, 0, fts_new_symbol("rec"), rec_fvec_rec);
  fts_method_define_varargs(cl, 0, fts_new_symbol("pause"), rec_fvec_pause);
  fts_method_define_varargs(cl, 0, fts_new_symbol("stop"), rec_fvec_stop);

  fts_method_define_varargs(cl, 0, fts_new_symbol("begin"), rec_fvec_set_begin);
  fts_method_define_varargs(cl, 0, fts_new_symbol("end"), rec_fvec_set_end);

  fts_method_define_varargs(cl, 0, fts_s_set, rec_fvec_set);
  fts_method_define_varargs(cl, 0, fts_s_list, rec_fvec_list);

  fts_method_define_varargs(cl, 0, fvec_symbol, rec_fvec_set_fvec);
  
  fts_method_define_varargs(cl, 1, fts_s_int, rec_fvec_set_begin);
  fts_method_define_varargs(cl, 1, fts_s_float, rec_fvec_set_begin);

  fts_method_define_varargs(cl, 2, fts_s_int, rec_fvec_set_end);
  fts_method_define_varargs(cl, 2, fts_s_float, rec_fvec_set_end);

  dsp_sig_inlet(cl, 0);
  
  return fts_Success;
}

void
signal_rec_fvec_config(void)
{
  sym_rec = fts_new_symbol("rec~");

  fts_metaclass_install(sym_rec, rec_fvec_instantiate, fts_arg_type_equiv);

  dsp_declare_function(sym_rec, rec_fvec_ftl);
}
