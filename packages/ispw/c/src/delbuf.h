/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#ifndef _DELBUF_H_
#define _DELBUF_H_

#define VD_TAIL 4

typedef struct {
  float *delay_line;		/* pointer to delay line */
  long phase;			/* current ring buffer phase */
  long ring_size;		/* size of ring buffer */
  long size;			/* virtual delay line size in samples */
  long malloc_size;		/* size (NOT real size) of biggest alloc */
  float raw_size;		/* delay line size given for delwrite~ in unit */
  fts_symbol_t unit;		/* unit of raw_size given for delwrite~ */
  long n_tick;			/* tick size for delay line accesses */
  int is_init;			/* flag if init was called for current DSP compiling */
} del_buf_t;

#define delbuf_get_tick_size(b) ((b)->n_tick)
#define delbuf_get_size_in_samples(b) ((b)->size)
#define delbuf_get_ring_size(b) ((b)->ring_size)
#define delbuf_get_phase(b) ((b)->phase)
#define delbuf_get_ptr(b) ((b)->delay_line)

/* Allocate and zero delbuf object (called by dewrite_init) */
extern del_buf_t *delbuf_new(float raw_size, fts_symbol_t unit);

/* init delbuf object (allocate delay line if necsssary) */
extern int delbuf_init(del_buf_t *buf, float sr, int n_tick);

/* ask if delbuf was already initialized (during current DSP compilation) */
extern int delbuf_is_init(del_buf_t *buf);

/* clear init flag (called by delay_table_..., when all delay objects are scheduled) */
extern void delbuf_clear_is_init_flag(del_buf_t *buf);

extern void delbuf_delete_delayline(del_buf_t *buf); /* free delayline */
extern void delbuf_clear_delayline(del_buf_t *buf); /* clear delayline (fill with 0.0) */

extern void delbuf_set_size(del_buf_t *buf, float raw_size, fts_symbol_t unit);

#endif
