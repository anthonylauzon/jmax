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
#include "bpf.h"
#include "play.h"

static fts_symbol_t play_bpf_symbol = 0;

typedef struct _play_bpf_
{
  signal_play_t play;
  int index;
} play_bpf_t;


/************************************************************
 *
 *  methods
 *
 */

static int
play_bpf_advance(bpf_t *bpf, int index, double position)
{
  /* position must be > 0.0 */
  if(position >= bpf_get_time(bpf, index + 1))
    {
      index++;
      
      while(position > bpf_get_time(bpf, index + 1))
	index++;
    }
  else if(position < bpf_get_time(bpf, index))
    {
      index--;
      
      while(position < bpf_get_time(bpf, index))
	index--;
    }
  else if(bpf_get_time(bpf, index) == bpf_get_time(bpf, index + 1))
    index++;

  return index;
}

/************************************************************
 *
 *  dsp
 *
 */

static void
play_bpf_reset(play_bpf_t *this, int n_tick, double sr)
{
  signal_play_set_conv_step(&this->play, 1000.0 / sr);
}

static void
play_bpf_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_bpf_t *this = (play_bpf_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_ptr(at);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  double sr = fts_dsp_get_output_srate(dsp, 0);
  fts_atom_t a[3];

  play_bpf_reset(this, n_tick, sr);

  fts_set_ptr(a + 0, this);
  fts_set_symbol(a + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(a + 2, n_tick);
  
  fts_dsp_add_function(play_bpf_symbol, 3, a);
}

static void
play_bpf_ftl(fts_word_t *argv)
{
  play_bpf_t *this = (play_bpf_t *)fts_word_get_ptr(argv + 0);
  float *out = (float *) fts_word_get_ptr(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  signal_play_t *play = &this->play;
  bpf_t *bpf = (bpf_t *)play->object;
  int size = bpf_get_size(bpf);
  double duration = bpf_get_duration(bpf);
  double position = play->position;
  int invert = (play->mode == mode_cycle && play->cycle_direction);
  double begin = (invert)? play->end: play->begin;
  double end = (invert)? play->begin: play->end;
  double step = (end > begin)? play->step: -play->step;
  double curr_position, curr_value;
  double slope;
  int index;
  
  if(size < 2)
    {
      step = 0.0;

      position = 0.0;
      index = 0;

      curr_position = bpf_get_time(bpf, index);
      curr_value = bpf_get_value(bpf, index);

      slope = 0.0;
    }
  else
    {
      if(position >= duration)
	{
	  position = duration;
	  index = size - 2;
	}
      else if(this->index > size - 2)
	index = play_bpf_advance(bpf, size - 2, position);
      else
	index = play_bpf_advance(bpf, this->index, position);
      
      curr_position = bpf_get_time(bpf, index);
      curr_value = bpf_get_value(bpf, index);

      slope = (bpf_get_value(bpf, index + 1) - curr_value) / (bpf_get_time(bpf, index + 1) - curr_position);
    }
  
  if(play->mode <= mode_pause)
    {
      float value = curr_value + slope * (position - curr_position);
      int i;
      
      for(i=0; i<n_tick; i++)
	out[i] = value;
    }
  else
    {
      double next_position = bpf_get_time(bpf, index + 1);
      double tick_end_position = position + step * n_tick;

      /* clip begin and end into duration */
      if(begin >= duration)
	begin = duration;
      if(end > duration)
	end = duration;
  
      if(tick_end_position >= curr_position && tick_end_position <= next_position && (end - tick_end_position) * step >= 0.0)
	{
	  double dposition = position - curr_position;
	  int i;
	  
	  for(i=0; i<n_tick; i++)
	    {
	      out[i] = curr_value + slope * dposition;
	      dposition += step;
	    }
	  
	  position = tick_end_position;
	}
      else 
	{
	  int new_index;
	  int i;
      
	  for(i=0; i<n_tick; i++)
	    {
	      out[i] = curr_value + slope * (position - curr_position);
	  
	      if(step != 0.0)
		{
		  /* increment */
		  position += step;
	      
		  if((position - end) * step >= 0.0)
		    {
		      /* end reached */
		      switch (play->mode)
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
				step = -step;;
				
				begin = end;
				end = old_begin;
			  
				/* swap direction */
				play->cycle_direction = 1 - play->cycle_direction;
			      }
			  }
			  break;
			  
			default:
			  {
			    /* reached end of playing */
			    position = end;
			    step = 0.0;
			    
			    play->mode = mode_stop;
			    
			    fts_timer_set_delay(play->timer, 0.0, 0);
			  }
			}
		    }
		  
		  new_index = play_bpf_advance(bpf, index, position);

		  if(new_index != index)
		    {
		      curr_position = bpf_get_time(bpf, new_index);
		      curr_value = bpf_get_value(bpf, new_index);
		      
		      slope = (bpf_get_value(bpf, new_index + 1) - curr_value) / (bpf_get_time(bpf, new_index + 1) - curr_position);

		      index = new_index;
		    }
		}
	    }
	}
    }

  this->index = index;
  play->position = position;
}

/************************************************************
 *
 *  class
 *
 */

/************************************************************
 *
 *  class
 *
 */

static void
play_bpf_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  play_bpf_t *this = (play_bpf_t *)o;

  ac--;
  at++;

  signal_play_init(&this->play, ac, at);
  signal_play_set_conv_step(&this->play, 1000.0 / fts_dsp_get_sample_rate());

  this->index = 0;
}

static void
play_bpf_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  play_bpf_t *this = (play_bpf_t *)o;

  signal_play_delete(&this->play);
}

static fts_status_t
play_bpf_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  signal_play_class_init(cl, bpf_symbol);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, play_bpf_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, play_bpf_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, play_bpf_put);

  return fts_Success;
}

void
signal_play_bpf_config(void)
{
  signal_play_class_register(bpf_symbol, play_bpf_instantiate);

  play_bpf_symbol = fts_new_symbol("play~ <bpf>");
  fts_dsp_declare_function(play_bpf_symbol, play_bpf_ftl);
}
