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

/***************************************************
 *
 *  event fifo
 *
 */
void
fts_eventfifo_init(fts_eventfifo_t *eventfifo, int size)
{
  int bytes = sizeof(fts_fifoevent_t) * size;

  fts_fifo_init(&eventfifo->fifo, fts_malloc(bytes), bytes);

  eventfifo->size = size;
  eventfifo->delta = 0.0;
}

void
fts_eventfifo_destroy(fts_eventfifo_t *eventfifo, int size)
{
  fts_free(eventfifo->fifo.buffer);
}

/* read next fifo entry into time base (returns pointer to atom of newly allocated event) */
fts_fifoevent_t *
fts_eventfifo_get_read(fts_eventfifo_t *eventfifo)
{
  if(fts_fifo_read_level(&eventfifo->fifo) >= sizeof(fts_fifoevent_t)) {
    fts_fifoevent_t *event = (fts_fifoevent_t *)fts_fifo_read_pointer(&eventfifo->fifo);
    double time = event->time - eventfifo->delta;
    double now = fts_get_time();

    /* resync fifo */
    if(time < now)
      eventfifo->delta = event->time - now;

    /* adjust event time and return (void value can serve for sync) */
    if(!fts_is_void(&event->atom)) {
      event->time = time;
      return event;
    }
  }

  return NULL;
}

fts_fifoevent_t *
fts_eventfifo_get_write(fts_eventfifo_t *eventfifo)
{
  if(fts_fifo_write_level(&eventfifo->fifo) >= sizeof(fts_fifoevent_t *))
    return *((fts_fifoevent_t **)fts_fifo_write_pointer(&eventfifo->fifo));
  else
    return NULL;
}

void
fts_eventfifo_incr_read(fts_eventfifo_t *eventfifo)
{
  fts_fifo_incr_write(&eventfifo->fifo, sizeof(fts_fifoevent_t *));
}

void
fts_eventfifo_incr_write(fts_eventfifo_t *eventfifo)
{
  fts_fifo_incr_write(&eventfifo->fifo, sizeof(fts_fifoevent_t *));
}

/***************************************************
*
*  call fifo
*
*/
void
fts_callfifo_init(fts_callfifo_t *callfifo, int size)
{
  int bytes = sizeof(fts_fifocall_t) * size;

  fts_fifo_init(&callfifo->fifo, fts_malloc(bytes), bytes);

  callfifo->size = size;
  callfifo->delta = 0.0;
}

void
fts_callfifo_destroy(fts_callfifo_t *callfifo, int size)
{
  fts_free(callfifo->fifo.buffer);
}

/* read next fifo entry into time base (returns pointer to atom of newly allocated call) */
fts_fifocall_t *
fts_callfifo_get_read(fts_callfifo_t *callfifo)
{
  if(fts_fifo_read_level(&callfifo->fifo) >= sizeof(fts_fifocall_t)) {
    fts_fifocall_t *call = (fts_fifocall_t *)fts_fifo_read_pointer(&callfifo->fifo);
    double time = call->time - callfifo->delta;
    double now = fts_get_time();

    /* resync fifo */
    if(time < now)
      callfifo->delta = call->time - now;

    /* adjust call time and return (void value can serve for sync) */
    if(!fts_is_void(&call->argument)) {
      call->time = time;
      return call;
    }
  }

  return NULL;
}

fts_fifocall_t *
fts_callfifo_get_write(fts_callfifo_t *callfifo)
{
  if(fts_fifo_write_level(&callfifo->fifo) >= sizeof(fts_fifocall_t *))
    return *((fts_fifocall_t **)fts_fifo_write_pointer(&callfifo->fifo));
  else
    return NULL;
}

void
fts_callfifo_incr_read(fts_callfifo_t *callfifo)
{
  fts_fifo_incr_write(&callfifo->fifo, sizeof(fts_fifocall_t *));
}

void
fts_callfifo_incr_write(fts_callfifo_t *callfifo)
{
  fts_fifo_incr_write(&callfifo->fifo, sizeof(fts_fifocall_t *));
}

