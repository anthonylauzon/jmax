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
 */

#include <fts/fts.h>
#include "delbuf.h"
#include "sampunit.h"

/*
  Consistency requirements for delwrite, delread to work: the logical length
  of the buffer (buf->size) and the phase (buf->phase) must be multiples of the
  vector size.  The actual delay buffer has vecsize elements at the end which are
  copies of the first vecsize elements.  Thus you can read vecsize consecutive
  samples starting anywhere as long as they don't cross the write pointer.  The
  phase can't be zero but can equal the logical length.
*/
/*
  ???
  The length of a delay line MUST be a multiple of the vector length
  The last buffer in the delay line
  (i.e. the buffer starting at adress delay->samples + length - vectorLength)
  is duplicated at beginning of delay line.
*/

/* clear delayline (fill with 0.0) */
void 
delbuf_clear_delayline(del_buf_t *buf)
{
  int i;
      
  for(i=0; i<buf->ring_size + buf->n_tick; i++)
    buf->delay_line[i] = 0.0;

  buf->phase = buf->ring_size;
}

/* free delayline */
void 
delbuf_delete_delayline(del_buf_t *buf)
{
  if(buf->delay_line)
    fts_free((void *)(buf->delay_line)); 

  buf->delay_line = 0;
  buf->phase = 0;
  buf->ring_size = 0;
  buf->size = 0;
  buf->malloc_size = 0;
  buf->n_tick = 0;
}

del_buf_t * 
delbuf_new(float raw_size, fts_symbol_t unit)
{
  del_buf_t *buf;

  buf = (del_buf_t *)fts_malloc(sizeof(del_buf_t));

  if(!buf) return (0);

  buf->delay_line = 0;
  buf->phase = 0;
  buf->ring_size = 0;
  buf->size = 0;
  buf->raw_size = raw_size;
  buf->unit = unit;
  buf->malloc_size = 0;
  buf->n_tick = 0;
  buf->is_init = 0;

  return (buf);
}

/* init delbuf object (allocate delbuf if necsssary) */
int 
delbuf_init(del_buf_t *buf, float sr, int n_tick)
{
  int size, ring_size;

  if(!buf)
    return(0);
  
  size = samples_unit_convert(buf->unit, buf->raw_size, sr);
  if(size < n_tick) size = n_tick;
  
  /* check if new size matches old size */
  if(size > buf->malloc_size) /* need to reallocate */
    {
      if(buf->malloc_size != 0) 
	delbuf_delete_delayline(buf); /* delete to reallocate */  
      
      /* allocate delayline */

      ring_size = size + n_tick + VD_TAIL;
      ring_size += (-ring_size) & (n_tick - 1);
      buf->delay_line = (float *)fts_malloc((ring_size + n_tick) * sizeof(float));

      /* init delbuf with new delayline or exit */
      if(!buf->delay_line){
	buf->phase = 0;
	buf->ring_size = 0;
	buf->size = 0;
	buf->malloc_size = 0;
	buf->n_tick = 0;
	fts_post("error: delay line: out of memory\n");
	return(0);
      }
      else{
	buf->ring_size = ring_size;
	buf->size = size;
	buf->malloc_size = size;
      }
    }
  else if(size < buf->malloc_size) /* no allocation */
    {
      ring_size = size + n_tick + VD_TAIL;
      ring_size += (-ring_size) & (n_tick - 1);

      buf->ring_size = ring_size;
      buf->size = size;
    }
    
  buf->n_tick = n_tick;
  delbuf_clear_delayline(buf);
  buf->is_init = 1;
  return(1);
}

void delbuf_set_size(del_buf_t *buf, float raw_size, fts_symbol_t unit)
{
  if(raw_size > 0)
    {
      buf->raw_size = raw_size;
      buf->unit = unit;
    }
}

int 
delbuf_is_init(del_buf_t *buf)
{
  return (buf->is_init);
}

void
delbuf_clear_is_init_flag(del_buf_t *buf)
{
  buf->is_init = 0;
}

