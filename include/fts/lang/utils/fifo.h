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

#ifndef _FTS_FIFO_H_
#define _FTS_FIFO_H_

typedef struct {
  volatile int read_index;
  volatile int write_index;
  int buffer_size;
  volatile unsigned char *buffer;
} fts_fifo_t;

#define fts_fifo_get_buffer_size(FIFO) ((FIFO)->buffer_size)
#define fts_fifo_get_buffer(FIFO) ((FIFO)->buffer)

#define fts_fifo_get_read_index(FIFO) ((FIFO)->read_index)
#define fts_fifo_set_read_index(FIFO,INDEX) ((FIFO)->read_index = (INDEX))
#define fts_fifo_get_write_index(FIFO) ((FIFO)->write_index)
#define fts_fifo_set_write_index(FIFO,INDEX) ((FIFO)->write_index = (INDEX))

#define fts_fifo_get_read_pointer(FIFO) ((FIFO)->buffer + (FIFO)->read_index)
#define fts_fifo_get_write_pointer(FIFO) ((FIFO)->buffer + (FIFO)->write_index)

extern void fts_fifo_init( fts_fifo_t *fifo, int buffer_size, void *buffer);

extern int fts_fifo_get_read_level( const fts_fifo_t *fifo);
extern int fts_fifo_get_write_level( const fts_fifo_t *fifo);

extern void fts_fifo_incr_read_index( fts_fifo_t *fifo, int incr);
extern void fts_fifo_incr_write_index( fts_fifo_t *fifo, int incr);

#endif
