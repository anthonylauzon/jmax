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
 */

#include <fts/fts.h>
#include <ftsconfig.h>

#include "fourpoint.h"
#include "sampbuf.h"
#include "sampwrite.h"
#include "sampread.h"

void ftl_sampwrite(fts_word_t *argv)
{
  float *in = (float *)fts_word_get_pointer(argv + 0);
  sampwrite_ctl_t *ctl = (sampwrite_ctl_t *)fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  int onset = ctl->onset;
  sampbuf_t *buf = ctl->buf;
  int n_left = buf->size + GUARDPTS - onset;
  int i;

  if(n_left <= 0) 
    return;

  if(n_left > n_tick)
    n_left = n_tick;

  for(i=0; i<n_left; i++)
    buf->samples[onset + i] = in[i];

  ctl->onset = onset + n_tick;
}


void ftl_sampread(fts_word_t *argv)
{
  float *in = (float *)fts_word_get_pointer(argv);
  float *out = (float *)fts_word_get_pointer(argv + 1);
  sampread_ctl_t * restrict ctl = (sampread_ctl_t *)fts_word_get_pointer(argv + 2);
  int n_tick = fts_word_get_int(argv + 3);
  sampbuf_t *buf = ctl->buf;
  float begin, end, length;
  float max = (float)(buf->size - 4);
  int index, incr, start;
  float *onset;
  int i;

  begin = ctl->last_in;
  end = in[n_tick - 1] * ctl->conv;
  ctl->last_in = end;

  length = end - begin;

  if(length > 0.0f && begin >= 0.0f && end < max && length <= ctl->max_extent)
    {
      /* end > begin: forward */
      start = begin;
    }
  else if(length < 0.0f && end >= 0 && begin < max && length >= -ctl->max_extent)
    {
      /* end < begin: backwards */
      start = end - 0.5f; 
    }
  else
    {
      for(i=0; i<n_tick; i++)
	out[i] = 0.0f;

      return;
    }
  
  onset = buf->samples + start;
  index = fts_fourpoint_index_scale(begin - (float)start);
  incr = fts_fourpoint_index_scale(length * ctl->inv_n);
  
  for(i=0; i<n_tick; i++)
    {
      fts_fourpoint_interpolate(onset + 1, index, out + i);
      index += incr;
    }
}
