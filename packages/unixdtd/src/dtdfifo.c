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

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <assert.h>

#include "dtdfifo.h"

#define DTD_BASE_DIR "/tmp/ftsdtd"

void dtdfifo_init( dtdfifo_t *fifo)
{
  fifo->read_index = 0;
  fifo->write_index = 0;
  fifo->state = FIFO_INACTIVE;
}

void dtdfifo_set_state( dtdfifo_t *fifo, enum dtdfifo_state state)
{
  fifo->state = state;
}

dtdfifo_t *dtdfifo_new( int fifo_number, int buffer_size)
{
  dtdfifo_t *fifo;
  char filename[32];
  int fd, size;
  struct stat buf;

  if ( stat( DTD_BASE_DIR, &buf) < 0)
    {
      if ( errno != ENOENT)
	{
	  fprintf( stderr, "Cannot stat DTD fifo root directory (%s)\n", strerror( errno));
	  return NULL;
	}
      else
	{
	  sprintf( filename, DTD_BASE_DIR);
	  if ( mkdir( filename, 0755) < 0)
	    {
	      fprintf( stderr, "Cannot create DTD fifo root directory (%s)\n", strerror( errno));
	      return NULL;
	    }
	}
    }

  sprintf( filename, "%s/%d", DTD_BASE_DIR, fifo_number);

  fd = open( filename, O_RDWR | O_CREAT, 0666);

  if ( fd < 0)
    {
      fprintf( stderr, "Cannot open file %s (%s)\n", filename, strerror( errno));
      return NULL;
    }

  size = dtdfifo_compute_size( buffer_size);
  ftruncate( fd, size);

  fifo = (dtdfifo_t *)mmap( 0, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

  if ( fifo == MAP_FAILED)
    {
      fprintf( stderr, "Cannot map file (%s)\n", strerror( errno));
      return NULL;
    }

  close(fd);

  fifo->buffer_size = buffer_size;

  dtdfifo_init( fifo);

  return fifo;
}

int dtdfifo_get_read_level( const dtdfifo_t *fifo)
{
  int n;

  n = fifo->write_index - fifo->read_index;

  if ( n < 0)
    n += fifo->buffer_size;

  assert( n >= 0 && n < fifo->buffer_size);

  if ( n < 0 || n >= fifo->buffer_size )
    return -1;

  return n;
}

int dtdfifo_get_write_level( const dtdfifo_t *fifo)
{
  int n;

  n = fifo->read_index - fifo->write_index - 1;

  if ( n < 0)
    n += fifo->buffer_size;

  assert( n >= 0 && n < fifo->buffer_size);

  if ( n < 0 || n >= fifo->buffer_size )
    return -1;

  return n;
}

void dtdfifo_incr_read_index( dtdfifo_t *fifo, int incr)
{
  int read_index;

  read_index = fifo->read_index;

  read_index += incr;

  if ( read_index >= fifo->buffer_size)
    read_index -= fifo->buffer_size;

  fifo->read_index = read_index;
}

void dtdfifo_incr_write_index( dtdfifo_t *fifo, int incr)
{
  int write_index;

  write_index = fifo->write_index;

  write_index += incr;

  if ( write_index >= fifo->buffer_size)
    write_index -= fifo->buffer_size;

  fifo->write_index = write_index;
}

#ifdef DEBUG
static char *dtdfifo_printable_state( dtdfifo_t *fifo)
{
  switch( fifo->state) {
  case FIFO_ACTIVE:
    return "ACT";
  case FIFO_EOF:
    return "EOF";
  case FIFO_INACTIVE:
    return "INA";
  }

  return "UNKNOWN";
}

void dtdfifo_debug( dtdfifo_t *fifo, const char *msg)
{
  int read_index, write_index, read_level, write_level;

  read_index = dtdfifo_get_read_index(fifo);
  write_index = dtdfifo_get_write_index(fifo);
  read_level = dtdfifo_get_read_level(fifo);
  write_level = dtdfifo_get_write_level(fifo);

  fprintf( stderr, "%s fifo 0x%lx 0x%08x [%s] index R 0x%08x W 0x%08x level R 0x%08x W 0x%08x\n", 
	   msg, 
	   (unsigned long)fifo, 
	   fifo->buffer_size,
	   dtdfifo_printable_state( fifo),
	   read_index, 
	   write_index,
	   read_level,
	   write_level);
}
#endif

