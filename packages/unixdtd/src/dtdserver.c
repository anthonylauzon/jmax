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

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "dtddefs.h"
#include "dtdfifo.h"

static FILE *server_stdin = NULL;

static struct dtd_handle {
  dtdfifo_t *fifo;
  int used;
} dtd_handle_table[N_FIFOS];

static void dtdserver_create_fifos( void)
{
  int n;

  /* For now, the number of fifos, the block size and the number of blocks are fixed */
  for ( n = 0; n < N_FIFOS; n++)
    {
      dtd_handle_table[n].fifo = dtdfifo_new( n, BLOCK_FRAMES * BLOCK_MAX_CHANNELS * BLOCKS_PER_FIFO * sizeof( float));
      dtd_handle_table[n].used = 0;
    }
}

/*
 * (HACK)
 * These functions are actually defined in fts/src/tiles/startup.c.
 * They should be defined by settings.
 */
extern char *fts_get_root_dir( void);
extern char *fts_get_arch( void);
extern char *fts_get_mode( void);

static void dtdserver_fork( void)
{
  int to_child_pipe[2];
  int from_child_pipe[2];
  int child_pid;

  if ( pipe( to_child_pipe) < 0)
    {
      fprintf( stderr, "pipe() failed (%s)\n", strerror( errno));
      exit( 1);
    }

  if ( pipe( from_child_pipe) < 0)
    {
      fprintf( stderr, "pipe() failed (%s)\n", strerror( errno));
      exit( 1);
    }

  child_pid = fork();
  if (child_pid < 0)
    {
      fprintf( stderr, "fork() failed\n");
      exit(1);
    }
  else if ( !child_pid)
    {
      char dtdserver_exec_name[256];
      char *argv[2];

      close( to_child_pipe[1]);
      if ( dup2( to_child_pipe[0], 0) < 0)
	{
	  fprintf( stderr, "Cannot dup2 (%s)\n", strerror( errno));
	  return;
	}
      close( to_child_pipe[0]);

      close( from_child_pipe[0]);
      if ( dup2( from_child_pipe[1], 1) < 0)
	{
	  fprintf( stderr, "Cannot dup2 (%s)\n", strerror( errno));
	  return;
	}
      close( from_child_pipe[1]);

      sprintf( dtdserver_exec_name, "%s/packages/unixdtd/bin/%s/%s/dtd", 
	       fts_get_root_dir(), fts_get_arch(), fts_get_mode());

      argv[0] = dtdserver_exec_name;
      argv[1] = NULL;

      if ( execv( dtdserver_exec_name, argv) < 0)
	{
	  fprintf( stderr, "Cannot exec dtd server (%s)\n", strerror( errno));
	  return;
	}

      exit( 1);
    }

  server_stdin = fdopen( to_child_pipe[1], "a");

#ifdef DEBUG
  fprintf( stderr, "[fts] started dtd server (pid = %d)\n", child_pid);
#endif
}


void dtdserver_init( void)
{
  dtdserver_create_fifos();
  dtdserver_fork();
}

static void dtdserver_send_command( const char *command)
{
  fprintf( server_stdin, "%s\n", command);
  fflush( server_stdin);
}

dtdfifo_t *dtdserver_new( int n_channels)
{
  int n;

  for ( n = 0; n < N_FIFOS; n++)
    {
      if ( !dtd_handle_table[n].used )
	{
	  char buffer[256];

	  dtd_handle_table[n].used = 1;

	  sprintf( buffer, "%d new %d", n, n_channels);
	  dtdserver_send_command( buffer);

	  return dtd_handle_table[n].fifo;
	}
    }

  return NULL;
}

static int dtdserver_fifo_get( dtdfifo_t *fifo)
{
  int n;

  for ( n = 0; n < N_FIFOS; n++)
    {
      if ( dtd_handle_table[n].fifo == fifo)
	return n;
    }

  assert( n != N_FIFOS);

  return -1;
}

void dtdserver_free( dtdfifo_t *fifo)
{
  int n;

  n = dtdserver_fifo_get( fifo);

  dtd_handle_table[n].used = 0;

  /* Server command ? */
}

void dtdserver_open( dtdfifo_t *fifo, const char *filename)
{
  int n;
  char buffer[256];

  n = dtdserver_fifo_get( fifo);

  sprintf( buffer, "%d open %s", n, filename);
  dtdserver_send_command( buffer);
}

void dtdserver_close( dtdfifo_t *fifo)
{
  int n;
  char buffer[256];

  n = dtdserver_fifo_get( fifo);

  sprintf( buffer, "%d close", n);
  dtdserver_send_command( buffer);
}



