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

#include <float.h>
#include <fts/fts.h>
#include <fts/packages/utils/utils.h>
#include <fts/packages/data/data.h>

static fts_symbol_t play_symbol = 0;

/***************************************************
 *
 *  segment
 *
 */

static fts_heap_t *segment_heap = NULL;

typedef struct _segment_
{
  fvec_t *fvec; /* sample */
  double begin; /* begin position */
  double end; /* end position */
  double speed; /* playing speed */
  struct _segment_ *next; /* next segment */
} segment_t;

static segment_t *
segment_new(void)
{
  segment_t *seg = (segment_t *)fts_heap_alloc(segment_heap);
  
  seg->fvec = NULL;
  seg->begin = 0.0;
  seg->end = DBL_MAX;
  seg->speed = 1.0;

  return seg;
}

static segment_t *
segment_copy(segment_t *org)
{
  segment_t *seg = (segment_t *)fts_heap_alloc(segment_heap);
  
  seg->fvec = org->fvec;
  seg->begin = org->begin;
  seg->end = org->end;
  seg->speed = org->speed;

  if(seg->fvec)
    fts_object_refer(seg->fvec);

  return seg;
}

static void 
segment_set_begin(segment_t *seg, const fts_atom_t *at)
{
  if(fts_is_number(at))
    {
      double begin = fts_get_number_float(at);
      
      if(begin > 0.0)
	seg->begin = begin * 0.001 * fts_dsp_get_sample_rate();
      else
	seg->begin = 0.0;
    }
}

static void 
segment_set_end(segment_t *seg, const fts_atom_t *at)
{
  if(fts_is_number(at))
    {
      double end = fts_get_number_float(at);
      
      if(end > 0.0)
	seg->end = end * 0.001 * fts_dsp_get_sample_rate();
      else
	seg->end = 0.0;
    }
}

static void 
segment_set_speed(segment_t *seg, const fts_atom_t *at)
{
  if(fts_is_number(at))
    {
      double speed = fts_get_number_float(at);
      
      if(speed > 0.0)
	seg->speed = speed;
      else
	seg->speed = 0.0;
    }
}

static void 
segment_set_duration(segment_t *seg, const fts_atom_t *at)
{
  if(fts_is_number(at))
    {
      double dur = fts_get_number_float(at);
      double begin = seg->begin;
      double end = seg->end;

      if(seg->fvec != NULL && end > fvec_get_size(seg->fvec))
	end = fvec_get_size(seg->fvec);
      
      if(dur > 0.0)
	{
	  double conv = 0.001 * fts_dsp_get_sample_rate();

	  if(end > begin)
	    seg->speed = (end - begin) / (dur * conv);
	  else if(begin > end)
	    seg->speed = (begin - end) / (dur * conv);
	}
    }
}

static segment_t *
segment_set(segment_t *seg, int ac, const fts_atom_t *at)
{
  switch (ac)
    {
    default:
    case 4:
      segment_set_speed(seg, at + 3);
    case 3:
      segment_set_end(seg, at + 2);
    case 2:
      segment_set_begin(seg, at + 1);
    case 1:
      {
	fvec_t *fvec;

	if(seg->fvec)
	  fts_object_release(seg->fvec);

	if(fts_is_object(at) && fts_get_class(at) == fvec_type)
	  fvec = (fvec_t *)fts_get_object(at);
	else
	  fvec = (fvec_t *)fts_object_create(fvec_type, 0, 0);
	
	seg->fvec = fvec;	  
	fts_object_refer(fvec);
      }
      break;
    case 0:
      break;
    }

  return seg;
}

static void
segment_destroy(segment_t *seg)
{
  if(seg->fvec)
    fts_object_release(seg->fvec);  

  fts_heap_free(seg, segment_heap);
}

/***************************************************
 *
 *  play~
 *
 */

typedef struct
{
  fts_dsp_object_t o;

  segment_t *segment;
  segment_t *next;
  double position; /* current position */

  enum play_mode {mode_stop, mode_pause, mode_play, mode_loop} mode;

} play_t;


static void 
play_reset_next(play_t *this)
{
  segment_destroy(this->next);
  this->next = NULL;
}


static void 
play_bang_at_end(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_bang((fts_object_t *)o, 1);
}

static void 
play_set_begin(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_t *this = (play_t *)o;
  
  segment_set_begin(this->segment, at);
}

static void 
play_set_end(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_t *this = (play_t *)o;

  segment_set_end(this->segment, at);
}

static void 
play_set_speed(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_t *this = (play_t *)o;

  segment_set_speed(this->segment, at);
}

static void 
play_set_duration(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_t *this = (play_t *)o;

  segment_set_duration(this->segment, at);
}

static void 
play_play(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_t *this = (play_t *)o;

  this->mode = mode_play;

  if(this->next != NULL)
    play_reset_next(this);
}

static void
play_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_t *this = (play_t *)o;

  this->position = this->segment->begin;
  this->mode = mode_stop;

  if(this->next != NULL)
    play_reset_next(this);
}

static void 
play_loop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_t *this = (play_t *)o;

  this->mode = mode_loop;
}

static void
play_jump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_t *this = (play_t *)o;
  
  if(fts_is_number(at))
    {
      double position = fts_get_number_float(at);

      this->position = position * 0.001 * fts_dsp_get_sample_rate();;
    }
}

static void
play_pause(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_t *this = (play_t *)o;

  if(this->mode != mode_stop)
    this->mode = mode_pause;
}

static void 
play_segment(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_t *this = (play_t *)o;

  segment_set(this->segment, ac, at);

  this->position = this->segment->begin;
  this->mode = mode_play;
}

static void 
play_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_t *this = (play_t *)o;

  segment_set(this->segment, ac, at);
}

static void 
play_next(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_t *this = (play_t *)o;

  if(this->mode >= mode_play)
    {
      if(this->next == NULL)
	this->next = segment_copy(this->segment);
      
      segment_set(this->next, ac, at);
    }
  else
    segment_set(this->segment, ac, at);    
}

/************************************************************
 *
 *  dsp
 *
 */

static void
play_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  play_t *this = (play_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  double sr = fts_dsp_get_output_srate(dsp, 0);
  fts_atom_t a[3];

  fts_set_pointer(a + 0, this);
  fts_set_symbol(a + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(a + 2, n_tick);
  
  fts_dsp_add_function(play_symbol, 3, a);
}

static void
play_ftl(fts_word_t *argv)
{
  play_t *this = (play_t *) fts_word_get_pointer(argv + 0);
  float *out = (float *) fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  fvec_t *fvec = this->segment->fvec;
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

      fts_cubic_idefix_interpolate(buf, index, &f);

      for(i=0; i<n_tick; i++)
	out[i] = f;
    }
  else
    {
      double size = fvec_get_size(fvec);
      double begin = this->segment->begin;
      double end = this->segment->end;
      double speed = (end > begin)? this->segment->speed: -this->segment->speed;
      double tick_end_position = position + n_tick * speed;
      fts_idefix_t incr;

      fts_idefix_set_float(&incr, speed);

      /* clip begin and end into size */
      if(begin >= size)
	begin = size;
      if(end > size)
	end = size;

      if((end - tick_end_position) * speed >= 0.0)
	{
	  int i;
	  
	  /* play straight during current tick */
	  for(i=0; i<n_tick; i++)
	    {
	      fts_cubic_idefix_interpolate(buf, index, out + i);
	      fts_idefix_incr(&index, incr);
	    }
	  
	  this->position = tick_end_position;
	}
      else
	{
	  int i;
	  
	  /* play close to end */
	  for(i=0; i<n_tick; i++)
	    {
	      fts_cubic_idefix_interpolate(buf, index, out + i);
	      
	      if(speed != 0.0)
		{
		  /* increment */
		  fts_idefix_incr(&index, incr);
		  position += speed;
		  
		  if((position - end) * speed >= 0.0)
		    {
		      if(this->next != NULL)
			{
			  segment_t *next = this->next;
			  
			  /* replace current segment by next*/
			  segment_destroy(this->segment);
			  this->segment = next;
			  this->next = NULL;
			  
			  begin = next->begin;
			  end = next->end;
			  speed = (end > begin)? next->speed: -next->speed;
			  
			  position = begin;
			}
		      else
			{
			  if(this->mode == mode_loop)
			    {
			      position += begin - end;

			      /* stop if segment is too short */
			      if((position - end) * speed >= 0.0)
				{
				  position = end;
				  speed = 0.0;
				}
			    }
			  else
			    {
			      /* end of playing */
			      speed = 0.0;
			      position = begin;
			      
			      /* output bang */
			      fts_timebase_add_call(fts_get_timebase(), (fts_object_t *)this, play_bang_at_end, 0, 0.0);
			      this->mode = mode_stop;
			    }
			}

		      fts_idefix_set_float(&index, begin);
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
play_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  play_t *this = (play_t *)o;

  if(ac > 0 && fts_is_object(at) && fts_get_class(at) == fvec_type)
    {
      this->mode = mode_stop;
      this->segment = segment_new();
      this->position = this->segment->begin;

      segment_set(this->segment, ac, at);
    }
  else
    fts_object_error((fts_object_t *)this, "fvec required as first argument");

  fts_dsp_object_init((fts_dsp_object_t *)(fts_object_t *)this);
}

static void
play_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

static void
play_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(play_t), play_init, play_delete);

  fts_class_message_varargs(cl, fts_s_put, play_put);

  /* transport */
  fts_class_message_varargs(cl, fts_new_symbol("play"), play_play);
  fts_class_message_varargs(cl, fts_new_symbol("stop"), play_stop);
  fts_class_message_varargs(cl, fts_new_symbol("loop"), play_loop);
  fts_class_message_varargs(cl, fts_new_symbol("pause"), play_pause);
  fts_class_message_varargs(cl, fts_new_symbol("jump"), play_jump);
  fts_class_message_varargs(cl, fts_new_symbol("duration"), play_set_duration);

  fts_class_message_varargs(cl, fts_new_symbol("begin"), play_set_begin);
  fts_class_message_varargs(cl, fts_new_symbol("end"), play_set_end);
  fts_class_message_varargs(cl, fts_new_symbol("speed"), play_set_speed);

  /* parameters */
  fts_class_message_varargs(cl, fts_s_set, play_set);
  fts_class_message_varargs(cl, fts_s_next, play_next);

  fts_class_inlet_varargs(cl, 0, play_segment);
  fts_class_inlet_number(cl, 1, play_set_begin);
  fts_class_inlet_number(cl, 2, play_set_end);
  fts_class_inlet_number(cl, 3, play_set_speed);

  fts_class_outlet_bang(cl, 1);

  fts_dsp_declare_function(play_symbol, play_ftl);
  fts_dsp_declare_outlet(cl, 0);
}

void
signal_play_config(void)
{
  play_symbol = fts_new_symbol("play~");

  segment_heap = fts_heap_new(sizeof(segment_t));

  fts_class_install(play_symbol, play_instantiate);
}
