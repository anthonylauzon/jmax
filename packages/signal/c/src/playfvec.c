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
#include <utils.h>
#include "fvec.h"
#include "play.h"

static fts_symbol_t play_fvec_symbol = 0;

typedef struct _play_fvec_
{
  signal_play_t play;
} play_fvec_t;


static void
play_fvec_set_period_prop(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  play_fvec_t *this = (play_fvec_t *)o;
  double period = fts_get_number_float(value);

  signal_play_set_conv_position(&this->play, 1. / period);
  signal_play_set_conv_step(&this->play, 1000.0 / (fts_dsp_get_sample_rate() * period));
}

/************************************************************
 *
 *  dsp
 *
 */

static void
play_fvec_reset(play_fvec_t *this, int n_tick, double sr)
{
  signal_play_set_conv_step(&this->play, 1000.0 * signal_play_get_conv_position(&this->play) / sr);
}

static void
play_fvec_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_fvec_t *this = (play_fvec_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_ptr(at);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  double sr = fts_dsp_get_output_srate(dsp, 0);
  fts_atom_t a[3];

  play_fvec_reset(this, n_tick, sr);

  fts_set_ptr(a + 0, &this->play);
  fts_set_symbol(a + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(a + 2, n_tick);
  
  fts_dsp_add_function(play_fvec_symbol, 3, a);
}

static void
play_fvec_ftl(fts_word_t *argv)
{
  signal_play_t *this = (signal_play_t *) fts_word_get_ptr(argv + 0);
  float *out = (float *) fts_word_get_ptr(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  fvec_t *fvec = (fvec_t *)this->object;
  float *buf = fvec_get_ptr(fvec);
  double position = this->position;
  fts_idefix_t index;
      
  /* convert to idefix representation */
  fts_idefix_set_float(&index, position);
  
  if(this->mode <= mode_pause)
    {
      /* stop or pause */
      float f;
      int i;

      fts_cubic_interpolate(buf, index, &f);

      for(i=0; i<n_tick; i++)
	out[i] = f;
    }
  else
    {
      double size = fvec_get_size(fvec);
      int invert = (this->mode == mode_cycle && this->cycle_direction);
      double begin = (invert)? this->end: this->begin;
      double end = (invert)? this->begin: this->end;
      double step = (end > begin)? this->step: -this->step;
      double tick_end_position = position + n_tick * step;
      fts_idefix_t incr;

      fts_idefix_set_float(&incr, step);

      /* clip begin and end into size */
      if(begin >= size)
	begin = size;
      if(end > size)
	end = size;

      if((end - tick_end_position) * step >= 0.0)
	{
	  int i;
	  
	  for(i=0; i<n_tick; i++)
	    {
	      fts_cubic_interpolate(buf, index, out + i);
	      fts_idefix_incr(&index, incr);
	    }
	  
	  this->position = tick_end_position;
	}
      else
	{
	  int i;
	  
	  for(i=0; i<n_tick; i++)
	    {
	      fts_cubic_interpolate(buf, index, out + i);
	      
	      if(step != 0.0)
		{
		  /* increment */
		  fts_idefix_incr(&index, incr);
		  position += step;
		  
		  if((position - end) * step >= 0.0)
		    {
		      /* end reached */
		      switch (this->mode)
			{
			case mode_loop:
			  {
			    /* repeat from loop begin */
			    position += begin - end;

			    if((position - end) * step > 0.0)
			      {
				position = end;
				step = 0.0;
			      }
			    
			    fts_idefix_set_float(&index, position);
			  }
			  break;
			  
			case mode_cycle:
			  {
			    /* turn and loop back */
			    double old_begin = begin;

			    /* mirror position at end */ 
			    position = 2.0 * end - position;

			    if((position - begin) * step < 0.0)
			      {
				position = begin;
				step = 0.0;
			      }
			    else
			      {
				/* turn */
				step = -step;
				fts_idefix_negate(&incr);
				
				begin = end;
				end = old_begin;
			  
				/* swap direction */
				this->cycle_direction = 1 - this->cycle_direction;
			      }
			    
			    fts_idefix_set_float(&index, position);
			  }
			  break;
			  
			default:
			  {
			    /* reached end of playing */
			    position = end;
			    step = 0.0;
			    
			    fts_idefix_set_float(&index, position);
			    this->mode = mode_stop;

			    fts_timebase_add_call(fts_get_timebase(), (fts_object_t *)this, signal_play_bang_at_end, 0, 0.0);
			  }
			}
		    }
		}
	    }

	  this->position = position;  
	}
    }
}

/************************************************************
 *
 *  class
 *
 */

static void
play_fvec_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  play_fvec_t *this = (play_fvec_t *)o;

  ac--;
  at++;

  signal_play_init(&this->play, ac, at);

  signal_play_set_conv_position(&this->play, 0.001 * fts_dsp_get_sample_rate());
}

static void
play_fvec_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  play_fvec_t *this = (play_fvec_t *)o;

  signal_play_delete(&this->play);
}

static fts_status_t
play_fvec_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  signal_play_class_init(cl, fvec_symbol);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, play_fvec_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, play_fvec_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, play_fvec_put);

  fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("period"), play_fvec_set_period_prop);
  
  return fts_Success;
}

void
signal_play_fvec_config(void)
{
  signal_play_class_register(fvec_symbol, play_fvec_instantiate);

  play_fvec_symbol = fts_new_symbol("play~ <fvec>");
  fts_dsp_declare_function(play_fvec_symbol, play_fvec_ftl);
}
