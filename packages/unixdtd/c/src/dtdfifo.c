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

/* ********************************************************************** */
/* Creation                                                               */
/* ********************************************************************** */

dtdfifo_t *dtdfifo_mmap( const char *filename, int buffer_size)
{
  dtdfifo_t *fifo;
  int fd, size;

  fd = open( filename, O_RDWR | O_CREAT, 0666);

  if ( fd < 0)
    {
      fprintf( stderr, "Cannot open file %s (%s)\n", filename, strerror( errno));
      return 0;
    }

  size = DTDFIFO_SIZE( buffer_size);

  ftruncate( fd, size);

  fifo = (dtdfifo_t *)mmap( 0, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

  if ( (void *)fifo == MAP_FAILED)
    {
      fprintf( stderr, "Cannot map file %s (%d,%s)\n", filename, errno, strerror( errno));
      return 0;
    }

  close(fd);

  return fifo;
}

dtdfifo_t *dtdfifo_new( const char *filename, int buffer_size)
{
  dtdfifo_t *fifo;

  fifo = dtdfifo_mmap( filename, buffer_size);

  if (!fifo)
    return 0;

  fifo->buffer_size = buffer_size;
  fifo->used[0] = 0;
  fifo->used[1] = 0;
  fifo->read_index = 0;
  fifo->write_index = 0;
  strcpy( fifo->filename, filename);

  return fifo;
}


void dtdfifo_delete( dtdfifo_t *fifo)
{
  int size;

  if (unlink( fifo->filename) < 0)
    fprintf( stderr, "Cannot unlink %s (%d,%s)\n", fifo->filename, errno, strerror( errno));

  if ( munmap( fifo, DTDFIFO_SIZE( fifo->buffer_size)) < 0)
    fprintf( stderr, "Cannot unmap fifo (%d,%s)\n", errno, strerror( errno));
}

/* ********************************************************************** */
/* Read/write index                                                       */
/* ********************************************************************** */

int dtdfifo_get_read_level( const dtdfifo_t *fifo)
{
  int n;

  n = fifo->write_index - fifo->read_index;

  if ( n < 0)
    n += fifo->buffer_size;

  assert( n >= 0 && n < fifo->buffer_size);

  return n;
}

int dtdfifo_get_write_level( const dtdfifo_t *fifo)
{
  int n;

  n = fifo->read_index - fifo->write_index - 1;

  if ( n < 0)
    n += fifo->buffer_size;

  assert( n >= 0 && n < fifo->buffer_size);

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


/* ********************************************************************** */
/* Debug code                                                             */
/* ********************************************************************** */

#ifdef DEBUG

void dtdfifo_debug( dtdfifo_t *fifo, const char *msg)
{
  int read_index, write_index, read_level, write_level;

  read_index = dtdfifo_get_read_index(fifo);
  write_index = dtdfifo_get_write_index(fifo);
  read_level = dtdfifo_get_read_level(fifo);
  write_level = dtdfifo_get_write_level(fifo);

  fprintf( stderr, "%s fifo 0x%lx 0x%08x index R 0x%08x W 0x%08x level R 0x%08x W 0x%08x\n", 
	   msg, 
	   (unsigned long)fifo, 
	   fifo->buffer_size,
	   read_index, 
	   write_index,
	   read_level,
	   write_level);
}
#endif


