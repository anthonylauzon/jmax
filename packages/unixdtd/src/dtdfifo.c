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

#include "dtddefs.h"
#include "dtdfifo.h"

typedef struct {
  dtdfifo_t *fifo;
  char *full_path;
  void *user_data;
} dtdfifo_entry_t;

static dtdfifo_entry_t *table = 0;
static int table_size = 0;

/* ********************************************************************** */
/* Table handling for mapping ids to pointers                             */
/* ********************************************************************** */

static void grow_table( int id)
{
  int i, old_size;

  old_size = table_size;

  if (table_size == 0)
    table_size = 32;

  while ( table_size <= id )
    table_size *= 2;

  table = realloc( table, table_size * sizeof( dtdfifo_entry_t ));

  for ( i = old_size; i < table_size; i ++)
    {
      table[i].fifo = 0;
      table[i].user_data = 0;
    }
}

dtdfifo_t *dtdfifo_get( int id)
{
  if ( id < 0 || id >= table_size)
    return 0;

  return table[id].fifo;
}

static void dtdfifo_put( int id, dtdfifo_t *fifo)
{
  if (id < 0)
    return;

  if (id >= table_size)
    grow_table( id);
    
  table[id].fifo = fifo;
}

void *dtdfifo_get_user_data( int id)
{
  if ( id < 0 || id >= table_size)
    return 0;

  return table[id].user_data;
}

void dtdfifo_put_user_data( int id, void *user_data)
{
  if (id < 0)
    return;

  if (id >= table_size)
    grow_table( id);
    
  table[id].user_data = user_data;
}


/* ********************************************************************** */
/* Allocation/Deallocation                                                */
/* ********************************************************************** */

int dtdfifo_get_number_of_fifos( void)
{
  int id, n;

  n = 0;

  for ( id = 0; id < table_size; id++)
    {
      if ( table[id].fifo)
	n++;
    }

  return n;
}

int dtdfifo_allocate( int side)
{
  int id;

  for ( id = 0; id < table_size; id++)
    {
      if ( table[id].fifo 
	   && ! dtdfifo_is_used( table[id].fifo, 0)
	   && ! dtdfifo_is_used( table[id].fifo, 1))
	{
	  dtdfifo_set_read_index( table[id].fifo, 0);
	  dtdfifo_set_write_index( table[id].fifo, 0);

	  fprintf( stderr, "allocate: fifo %d DTD %d FTS %d\n", id, dtdfifo_is_used(table[id].fifo, DTD_SIDE), dtdfifo_is_used(table[id].fifo, FTS_SIDE));

	  dtdfifo_set_used( table[id].fifo, side, 1);

	  return id;
	}
    }

  return -1;
}

/* ********************************************************************** */
/* Creation                                                               */
/* ********************************************************************** */

static int create_base_dir( const char *dirname)
{
  struct stat buf;

  if ( stat( dirname, &buf) < 0)
    {
      if ( errno != ENOENT)
	{
	  fprintf( stderr, "Cannot stat DTD fifo root directory %s (%s)\n", dirname, strerror( errno));
	  return -1;
	}
      else
	{
	  if ( mkdir( dirname, 0777) < 0)
	    {
	      fprintf( stderr, "Cannot create DTD fifo root directory %s (%s)\n", dirname, strerror( errno));
	      return -1;
	    }
	}
    }

  return 1;
}

int dtdfifo_new( int id, const char *dirname, int buffer_size)
{
  static int new_id = 0;
  dtdfifo_t *fifo;
  int fd, size;
  char *full_path;
  char tmp[32];

  if ( id == 0 )
    id = ++new_id;

  if ( create_base_dir( dirname) < 0 )
    return -1;

  full_path = (char *)malloc( strlen( dirname) + 32);
  strcpy( full_path, dirname);
  strcat( full_path, "/");
  sprintf( tmp, "%d", id);
  strcat( full_path, tmp);

  fd = open( full_path, O_RDWR | O_CREAT, 0666);

  if ( fd < 0)
    {
      fprintf( stderr, "Cannot open file %s (%s)\n", full_path, strerror( errno));
      return -1;
    }

  size = DTDFIFO_SIZE( buffer_size);

  ftruncate( fd, size);

  fifo = (dtdfifo_t *)mmap( 0, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

  if ( (void *)fifo == MAP_FAILED)
    {
      fprintf( stderr, "Cannot map file %s (%d,%s)\n", full_path, errno, strerror( errno));
      return -1;
    }

  close(fd);

  fifo->buffer_size = buffer_size;
  fifo->used[ 0] = 0;
  fifo->used[ 1] = 0;
  fifo->read_index = 0;
  fifo->write_index = 0;

  dtdfifo_put( id, fifo);

  table[id].full_path = full_path;

  return id;
}

void dtdfifo_delete( int id)
{
  int size;
  dtdfifo_t *fifo;

  fifo = dtdfifo_get( id);

  if (unlink( table[id].full_path) < 0)
    fprintf( stderr, "Cannot unlink %s (%d,%s)\n", table[id].full_path, errno, strerror( errno));

  free( table[id].full_path);
  table[id].full_path = 0;

  size = DTDFIFO_SIZE( fifo->buffer_size);

  if ( munmap( fifo, size) < 0)
    fprintf( stderr, "Cannot unmap fifo %d (%d,%s)\n", id, errno, strerror( errno));

  table[id].fifo = 0;
  table[id].user_data = 0;
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
/* Apply to all registered fifos                                          */
/* ********************************************************************** */

void dtdfifo_apply( void (*fun)( int id, dtdfifo_t *, void *))
{
  int id;

  if ( !table)
    return;

  for ( id = 0; id < table_size; id++)
    {
      if (table[id].fifo)
	(*fun)( id, table[id].fifo, table[id].user_data);
    }
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


