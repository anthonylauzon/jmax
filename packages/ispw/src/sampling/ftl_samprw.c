/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include "fts.h"
#include "sampbuf.h"
#include "sampfilt.h"
#include "sampwrite.h"
#include "sampread.h"


void ftl_sampwrite(fts_word_t *argv)
{
  float *in = (float *)fts_word_get_ptr(argv + 0);
  sampwrite_ctl_t *ctl = (sampwrite_ctl_t *)fts_word_get_ptr(argv + 1);
  long n_tick = fts_word_get_long(argv + 2);
  long onset = ctl->onset;
  sampbuf_t *buf = ctl->buf;
  float *xp = buf->samples + onset;
  long n_left = buf->size + GUARDPTS - onset;

  if(n_left <= 0) return;
  if(n_left >= n_tick){
     fts_vecx_fcpy(in, buf->samples + onset, n_tick);
  }else{
     fts_vec_fcpy(in, buf->samples + onset, n_left);
  }
  ctl->onset = onset + n_tick;
}


void ftl_sampread(fts_word_t *argv)
{
  float *in = (float *)fts_word_get_ptr(argv);
  float *out = (float *)fts_word_get_ptr(argv + 1);
  sampread_ctl_t * restrict ctl = (sampread_ctl_t *)fts_word_get_ptr(argv + 2);
  long n_tick = fts_word_get_long(argv + 3);
  sampbuf_t *buf = ctl->buf;
  float f1, f2, extent;
  long index, incr, n1;
  float *onset;
  int i;

  f1 = ctl->last_in;
  f2 = in[n_tick-1] * ctl->conv;
  ctl->last_in = f2;

  extent = f2 - f1;

  if(extent > 0.0f && f1 >= 0.0f && f2 < buf->size - 4 && extent <= ctl->max_extent)
    n1 = f1;
  else if(extent < 0.0f && f2 >= 0 && f1 < buf->size - 4 && extent >= -ctl->max_extent)
    n1 = f2 - 0.5f;
  else
    {
      for(i=0; i<n_tick; i++)
	out[i] = 0.0f;

      return;
    }
  
  onset = buf->samples + n1;
  index = (f1 - n1) * 65536.0f;
  incr = 65536.0f * extent * ctl->inv_n;
  
  for(i=0; i<n_tick; i++)
    {
      sampfilt_t * restrict sampfilt = sampfilt_tab + ((index >> 8) & 255);
      float * restrict fp = onset + (index >> 16);
      out[i] = sampfilt->f1 * fp[0] + sampfilt->f2 * fp[1] + sampfilt->f3 * fp[2] + sampfilt->f4 * fp[3];
      index += incr;
    }
}
