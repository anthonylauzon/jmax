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
 * Authors: Francois Dechelle.
 *
 */

#ifndef _FTS_DTDFIFO_H_
#define _FTS_DTDFIFO_H_

enum dtdfifo_state {FIFO_ACTIVE, FIFO_EOF, FIFO_INACTIVE};

typedef struct {
  volatile int read_index;
  volatile int write_index;
  volatile enum dtdfifo_state state;
  int buffer_size;
  volatile double buffer[1]; /* double for alignement */
} dtdfifo_t;

#define dtdfifo_compute_size(BUFFER_SIZE) ( sizeof( dtdfifo_t) - sizeof( double) + (BUFFER_SIZE))

#define dtdfifo_get_buffer(F) ((F)->buffer)
#define dtdfifo_get_buffer_size(F) ((F)->buffer_size)
#define dtdfifo_get_read_index(F) ((F)->read_index)
#define dtdfifo_get_write_index(F) ((F)->write_index)
#define dtdfifo_get_read_pointer(F) (((volatile char *)((F)->buffer)) + (F)->read_index)
#define dtdfifo_get_write_pointer(F) (((volatile char *)((F)->buffer)) + (F)->write_index)

extern void dtdfifo_init( dtdfifo_t *fifo);

extern dtdfifo_t *dtdfifo_new( int fifo_number, int buffer_size);

extern void dtdfifo_set_state( dtdfifo_t *fifo, enum dtdfifo_state state);
#define dtdfifo_get_state(F) ((F)->state)

extern int dtdfifo_get_read_level( const dtdfifo_t *fifo);
extern int dtdfifo_get_write_level( const dtdfifo_t *fifo);

extern void dtdfifo_incr_read_index( dtdfifo_t *fifo, int incr);
extern void dtdfifo_incr_write_index( dtdfifo_t *fifo, int incr);

#ifdef DEBUG
extern void dtdfifo_debug( dtdfifo_t *fifo, const char *msg);
#endif

#endif
