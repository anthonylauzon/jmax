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

#include "fts.h"
#include "delbuf.h"
#include "vd.h"

void 
ftl_delwrite(fts_word_t *argv)
{
  float * restrict in = (float *)fts_word_get_ptr(argv + 0);
  del_buf_t * restrict buf = (del_buf_t *)fts_word_get_ptr(argv + 1);
  int n_tick = fts_word_get_long(argv + 2);
  int i;
  int phase = buf->phase;

  for (i=0; i<n_tick; i++)
    buf->delay_line[phase + i] = in[i];

  if (buf->phase >= buf->ring_size) /* ring buffer wrap around */
    {
      buf->phase = n_tick;

      for (i = 0; i < n_tick ; i++)
	buf->delay_line[i] = in[i];
    }
  else
    buf->phase += n_tick;
}

void 
ftl_delread(fts_word_t *argv)
{
  float * restrict out = (float *) fts_word_get_ptr(argv + 0);
  del_buf_t * restrict buf = (del_buf_t *) fts_word_get_ptr(argv + 1);
  int n_tick = fts_word_get_long(argv + 2);
  int *del_time = (int *)fts_word_get_ptr(argv + 3);
  int del = *del_time; 
  int i;

  int phase;
  
  phase = buf->phase - del;
  
  if (phase < 0)
    phase += buf->ring_size; /* ring buffer wrap around */

  for (i=0; i<n_tick; i++)
    out[i] = buf->delay_line[phase + i];
}

/* local */

#define vd_lagrange(ret, ptr, float_index)     \
{                                         \
  float one_plus_f = 1.0f + float_index;  \
  float one_minus_f = 1.0f - float_index; \
  float two_minus_f = 2.0f - float_index; \
                                           \
  ret = (0.5f * one_plus_f * two_minus_f * (one_minus_f * ptr[2] + float_index * ptr[1])            \
	  - 0.1666667f * float_index * one_minus_f * (two_minus_f * ptr[3] + one_plus_f * ptr[0]));  \
}

void ftl_vd(fts_word_t *argv)
{
  float * restrict in = (float *)fts_word_get_ptr(argv);
  float * restrict out = (float *)fts_word_get_ptr(argv + 1);
  del_buf_t * restrict buf = (del_buf_t *) fts_word_get_ptr(argv + 2);
  ftl_vd_t * restrict ftl = (ftl_vd_t *) fts_word_get_ptr(argv + 3);
  int n = fts_word_get_long(argv + 4);
  float read_tick = n;
  float conv = ftl->conv;
  float write_advance = ftl->write_advance;
  float write_tick = buf->n_tick;
  float min_delay = write_tick - write_advance + 2.0f;
  float max_delay = (float)buf->size;
  float ratio = write_tick / read_tick;
  float start = in[0] * conv;
  float target = in[n - 1] * conv;
  float span;

  if (start < min_delay)
    start = min_delay;
  else if (start >= max_delay)
    start = max_delay;

  if (target < min_delay)
    target = min_delay;
  else if (target >= max_delay)
    target = max_delay;

  target -= (write_tick - ratio); /* target delay -> target onset */
  span = target - start;

  if (span <= ftl->max_span && span >= -ftl->max_span)
    {
      float incr = (target - start) / (read_tick - 1.0f);
      float onset = start + write_advance;
      float *delay_line = buf->delay_line;
      float *write_ptr = delay_line + buf->phase - 2; /* well, actually write pointer - 2 */
      int i_onset = (int)onset;
      float d;
      float * read_ptr = write_ptr - i_onset;
  
      if (read_ptr >= delay_line && incr <= 0.0f)
	{
	  int i;

	  for(i=0; i<n-1; i++)
	    {
	      d = onset - (float)i_onset;
	      vd_lagrange(out[i], read_ptr, d);
	  
	      onset += incr;

	      i_onset = onset;
	      read_ptr = write_ptr - i_onset;
	    }

	  d = onset - (float)i_onset;
	  vd_lagrange(out[i], read_ptr, d);
	}
      else
	{
	  int i;

	  for(i=0; i<n-1; i++)
	    {
	      if(read_ptr < delay_line)
		read_ptr += buf->ring_size;

	      d = onset - (float)i_onset;
	      vd_lagrange(out[i], read_ptr, d);
	  
	      onset += incr;
	  
	      i_onset = onset;
	      read_ptr = write_ptr - i_onset;
	    }

	  if(read_ptr < delay_line)
	    read_ptr += buf->ring_size;

	  d = onset - (float)i_onset;
	  vd_lagrange(out[i], read_ptr, d);
	}
    }
  else
    fts_vec_fzero(out, n);
}


/* In place version */

void ftl_vd_inplace(fts_word_t *argv)
{
  float * restrict inout = (float *)fts_word_get_ptr(argv);
  del_buf_t * restrict buf = (del_buf_t *) fts_word_get_ptr(argv + 1);
  ftl_vd_t * restrict ftl = (ftl_vd_t *) fts_word_get_ptr(argv + 2);
  int n = fts_word_get_long(argv + 3);
  float read_tick = n;
  float conv = ftl->conv;
  float write_advance = ftl->write_advance;
  float write_tick = buf->n_tick;
  float min_delay = write_tick - write_advance + 2.0f;
  float max_delay = (float)buf->size;
  float ratio = write_tick / read_tick;
  float start = inout[0] * conv;
  float target = inout[n - 1] * conv;
  float span;

  if (start < min_delay)
    start = min_delay;
  else if (start >= max_delay)
    start = max_delay;

  if (target < min_delay)
    target = min_delay;
  else if (target >= max_delay)
    target = max_delay;

  target -= (write_tick - ratio); /* target delay -> target onset */
  span = target - start;

  if (span <= ftl->max_span && span >= -ftl->max_span)
    {
      float incr = (target - start) / (read_tick - 1.0f);
      float onset = start + write_advance;
      float *delay_line = buf->delay_line;
      float *write_ptr = delay_line + buf->phase - 2; /* well, actually write pointer - 2 */
      int i_onset = (int)onset;
      float d;
      float * read_ptr = write_ptr - i_onset;
  
      if (read_ptr >= delay_line && incr <= 0.0f)
	{
	  int i;

	  for(i=0; i<n-1; i++)
	    {
	      d = onset - (float)i_onset;
	      vd_lagrange(inout[i], read_ptr, d);
	  
	      onset += incr;

	      i_onset = onset;
	      read_ptr = write_ptr - i_onset;
	    }

	  d = onset - (float)i_onset;
	  vd_lagrange(inout[i], read_ptr, d);
	}
      else
	{
	  int i;

	  for(i=0; i<n-1; i++)
	    {
	      if(read_ptr < delay_line)
		read_ptr += buf->ring_size;

	      d = onset - (float)i_onset;
	      vd_lagrange(inout[i], read_ptr, d);
	  
	      onset += incr;
	  
	      i_onset = onset;
	      read_ptr = write_ptr - i_onset;
	    }

	  if(read_ptr < delay_line)
	    read_ptr += buf->ring_size;

	  d = onset - (float)i_onset;
	  vd_lagrange(inout[i], read_ptr, d);
	}
    }
  else
    fts_vec_fzero(inout, n);
}

/* ISPW bug compatible version */
void
ftl_vd_miller(fts_word_t *argv)
{
  float *in = (float *)fts_word_get_ptr(argv);
  float *out = (float *)fts_word_get_ptr(argv + 1);
  del_buf_t * restrict buf = (del_buf_t *) fts_word_get_ptr(argv + 2);
  ftl_vd_t * restrict ftl = (ftl_vd_t *) fts_word_get_ptr(argv + 3);
  int n = fts_word_get_long(argv + 4);
  float write_tick = fts_word_get_float(argv + 5);
  float conv = ftl->conv;
  float write_advance = ftl->write_advance;
  float min_delay = write_tick - write_advance + 2.0f;
  float max_delay = (float)buf->size;
  float start = in[0] * conv;
  float target = in[n - 1] * conv;
  float span;

  if(start < min_delay)
    start = min_delay;
  else if(start >= max_delay)
    start = max_delay;

  if(target < min_delay)
    target = min_delay;
  else if(target >= max_delay)
    target = max_delay;

  target -= (write_tick - 1.0f); /* target delay -> target onset (BUG!!!!) */
  span = target - start;

  if(span <= ftl->max_span && span >= -ftl->max_span)
    {
      float incr = (target - start) / ((float)n - 1.0f);
      float onset = start + write_advance;
      float *delay_line = buf->delay_line;
      float *write_ptr = delay_line + buf->phase - 2; /* well, actually write pointer - 2 */
      int i_onset = (int)onset;
      float d;
      float * read_ptr = write_ptr - i_onset;
  
      if (read_ptr >= delay_line && incr <= 0.0f)
	{
	  int i;

	  for(i=0; i<n-1; i++)
	    {
	      d = onset - (float)i_onset;
	      vd_lagrange(out[i], read_ptr, d);
	  
	      onset += incr;

	      i_onset = onset;
	      read_ptr = write_ptr - i_onset;
	    }

	  d = onset - (float)i_onset;
	  vd_lagrange(out[i], read_ptr, d);
	}
      else
	{
	  int i;

	  for(i=0; i<n-1; i++)
	    {
	      if(read_ptr < delay_line)
		read_ptr += buf->ring_size;

	      d = onset - (float)i_onset;
	      vd_lagrange(out[i], read_ptr, d);
	  
	      onset += incr;
	  
	      i_onset = onset;
	      read_ptr = write_ptr - i_onset;
	    }

	  if(read_ptr < delay_line)
	    read_ptr += buf->ring_size;

	  d = onset - (float)i_onset;
	  vd_lagrange(out[i], read_ptr, d);
	}
    }
  else
    fts_vec_fzero(out, n);
}

