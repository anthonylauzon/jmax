/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
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


/* Allocate and zero delbuf object (called by dewrite_init) */
extern del_buf_t *delbuf_new(float raw_size, fts_symbol_t unit);

/* init delbuf object (allocate delay line if necsssary) */
extern int delbuf_init(del_buf_t *buf, float sr, long n_tick);

/* ask if delbuf was already initialized (during current DSP compilation) */
extern int delbuf_is_init(del_buf_t *buf);

/* clear init flag (called by delay_table_..., when all delay objects are scheduled) */
extern void delbuf_clear_is_init_flag(del_buf_t *buf);

extern void delbuf_delete_delayline(del_buf_t *buf); /* free delayline */
extern void delbuf_clear_delayline(del_buf_t *buf); /* clear delayline (fill with 0.0) */


extern void delbuf_set_size(del_buf_t *buf, float raw_size, fts_symbol_t unit);

#define delbuf_get_tick_size(buf) ((buf)->n_tick)
#define delbuf_get_size_in_samples(buf) ((buf)->size)

#endif /* _DELBUF_H_ */
