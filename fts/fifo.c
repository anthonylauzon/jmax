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
#include <assert.h>

void fts_fifo_init( fts_fifo_t *fifo, void *buffer, int size)
{
  fifo->read_index = 0;
  fifo->write_index = 0;
  fifo->buffer = buffer;
  fifo->size = size;
}

int fts_fifo_read_level( fts_fifo_t *fifo)
{
  int n;

  n = fifo->write_index - fifo->read_index;

  if ( n < 0)
    n += fifo->size;

  assert( n >= 0 && n < fifo->size);

  return n;
}

int fts_fifo_write_level( fts_fifo_t *fifo)
{
  int n;

  n = fifo->read_index - fifo->write_index - 1;

  if ( n < 0)
    n += fifo->size;

  assert( n >= 0 && n < fifo->size);

  return n;
}

void fts_fifo_incr_read( fts_fifo_t *fifo, int incr)
{
  int read_index;

  read_index = fifo->read_index;

  read_index += incr;

  if ( read_index >= fifo->size)
    read_index -= fifo->size;

  fifo->read_index = read_index;
}

void fts_fifo_incr_write( fts_fifo_t *fifo, int incr)
{
  int write_index;

  write_index = fifo->write_index;

  write_index += incr;

  if ( write_index >= fifo->size)
    write_index -= fifo->size;

  fifo->write_index = write_index;
}

