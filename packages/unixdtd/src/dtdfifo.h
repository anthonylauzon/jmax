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
 */

/*
 * This file's authors: Francois Dechelle.
 */

#ifndef _FTS_DTDFIFO_H_
#define _FTS_DTDFIFO_H_

typedef enum { FIFO_LEFT, FIFO_RIGHT} dtdfifo_side_t;
#define OTHER_SIDE(S) (((S)==FIFO_LEFT) ? FIFO_RIGHT : FIFO_LEFT)

typedef struct {
  volatile int used[2];
  volatile int eof;
  volatile int read_index;
  volatile int write_index;
  int buffer_size;
  volatile double buffer[1]; /* double for alignement */
} dtdfifo_t;

#define DTDFIFO_SIZE(BS) ((BS) + sizeof( dtdfifo_t) - sizeof( double))

/*
 * Macros 
 */
#define dtdfifo_get_buffer_size(F) ((F)->buffer_size)
#define dtdfifo_get_buffer(F) ((F)->buffer)

#define dtdfifo_get_read_index(F) ((F)->read_index)
#define dtdfifo_set_read_index(F,I) ((F)->read_index = (I))

#define dtdfifo_get_write_index(F) ((F)->write_index)
#define dtdfifo_set_write_index(F,I) ((F)->write_index = (I))

#define dtdfifo_get_read_pointer(F) (((volatile char *)((F)->buffer)) + (F)->read_index)
#define dtdfifo_get_write_pointer(F) (((volatile char *)((F)->buffer)) + (F)->write_index)

#define dtdfifo_is_eof(F) ((F)->eof)
#define dtdfifo_set_eof(F,E) ((F)->eof = (E))

#define dtdfifo_is_used(F,W) ((F)->used[W])
#define dtdfifo_set_used(F,W,U) ((F)->used[W] = (U))


/*
 * Functions
 */

extern int dtdfifo_get_number_of_fifos( void);

extern int dtdfifo_allocate( dtdfifo_side_t side);
extern void dtdfifo_deallocate( dtdfifo_side_t side, int id);

extern int dtdfifo_new( int id, const char *dirname, int buffer_size);
extern void dtdfifo_delete( int id);

extern dtdfifo_t *dtdfifo_get( int id);

extern void *dtdfifo_get_user_data( int id);
extern void dtdfifo_put_user_data( int id, void *user_data);

extern int dtdfifo_get_read_level( const dtdfifo_t *fifo);
extern int dtdfifo_get_write_level( const dtdfifo_t *fifo);

extern void dtdfifo_incr_read_index( dtdfifo_t *fifo, int incr);
extern void dtdfifo_incr_write_index( dtdfifo_t *fifo, int incr);

extern void dtdfifo_apply( void (*fun)( int id, dtdfifo_t *, void *));

#ifdef DEBUG
extern void dtdfifo_debug( dtdfifo_t *fifo, const char *msg);
#endif

#endif
