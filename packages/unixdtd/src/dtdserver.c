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

#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <fts/fts.h>
#include "dtddefs.h"
#include "dtdfifo.h"
#include "dtdserver.h"

struct _dtdserver_t {
  fts_object_t head;

  int server_pid;

  int server_socket;
  int server_port;

  int fifo_size;

  int loop_milliseconds;

  const char *base_dir;

  int number_of_objects;
  int number_of_fifos;

  dtdfifo_t *fifo_table[DTD_MAX_FIFOS];
};

static fts_class_t *dtdserver_class;
static dtdserver_t *default_instance;

dtdserver_t *dtdserver_get_default_instance( void)
{
  if (!default_instance)
    default_instance = (dtdserver_t *)fts_object_create( dtdserver_class, 0, 0);

  return default_instance;
}

void dtdserver_stop( void)
{
  if ( default_instance)
    fts_object_destroy( (fts_object_t *)default_instance);
}

/* ********************************************************************** */
/* Functions called by the DTD objects                                    */
/* ********************************************************************** */

static void dtdserver_send_command( dtdserver_t *server, const char *command)
{
  int r;
  struct sockaddr_in my_addr;

  if ( server->server_socket <= 0)
    return;

  memset( &my_addr, 0, sizeof( my_addr));
  my_addr.sin_family = AF_INET;
  my_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
  my_addr.sin_port = htons( server->server_port);

  r = sendto( server->server_socket, command, strlen( command)+1, 0, &my_addr, sizeof(my_addr));

  if ( r < 0)
    {
      fprintf( stderr, "[fts] error sending (%d,%s)\n", errno, strerror( errno));
    }
}

void dtdserver_add_object( dtdserver_t *server, void *object)
{
  server->number_of_objects++;

#define X 2
  /* 
   * Create enough fifos in order that there is at least X*number_of_objects 
   * fifos, so that, when you open a file, you don't need to create the fifo, 
   * i.e. create the file and mmap it, which would block FTS
   */

  while ( server->number_of_fifos < X * server->number_of_objects )
    {
      char filename[1024];
      char buffer[1024];
      dtdfifo_t *fifo;

      strcpy( filename, server->base_dir);
      sprintf( filename + strlen( server->base_dir), "%d", server->number_of_fifos);

      fifo = dtdfifo_new( filename, server->fifo_size);
      if ( !fifo)
	return;

      sprintf( buffer, "mmap %d %s %d", server->number_of_fifos, filename, server->fifo_size);
      dtdserver_send_command( server, buffer);

      server->fifo_table[ server->number_of_fifos ] = fifo;

      server->number_of_fifos++;
    }
}

void dtdserver_remove_object( dtdserver_t *server, void *object)
{
  server->number_of_objects--;
}

static dtdfifo_t *dtdserver_allocate_fifo( dtdserver_t *server, int *pid)
{
  int id;

  for ( id = 0; id < DTD_MAX_FIFOS; id++)
    {
      dtdfifo_t *fifo = server->fifo_table[id];

      if ( fifo && ! dtdfifo_is_used( fifo, 0) && ! dtdfifo_is_used( fifo, 1))
	{
	  dtdfifo_set_read_index( fifo, 0);
	  dtdfifo_set_write_index( fifo, 0);

	  dtdfifo_set_used( fifo, FTS_SIDE, 1);

	  *pid = id;
	  return fifo;
	}

    }

  return 0;
}

dtdfifo_t *dtdserver_open_read( dtdserver_t *server, const char *filename, int n_channels)
{
  char buffer[1024];
  int id;
  dtdfifo_t *fifo;

  fifo = dtdserver_allocate_fifo( server, &id);

  if (fifo)
    {
      sprintf( buffer, "openr %d %s %s %d", id, filename, fts_symbol_name(fts_get_search_path()), n_channels);
      dtdserver_send_command( server, buffer);
    }

  return fifo;
}

dtdfifo_t *dtdserver_open_write( dtdserver_t *server, const char *filename, int n_channels)
{
  char buffer[1024];
  int id;
  dtdfifo_t *fifo;

  fifo = dtdserver_allocate_fifo( server, &id);

  if (fifo)
    {
      sprintf( buffer, "openw %d %s %d", id, filename, n_channels);
      dtdserver_send_command( server, buffer);
    }

  return fifo;
}


void dtdserver_close( dtdserver_t *server, dtdfifo_t *fifo)
{
  char buffer[128];
  int id;

  dtdfifo_set_used( fifo, FTS_SIDE, 0);

  for ( id = 0; id < DTD_MAX_FIFOS; id++)
    if (server->fifo_table[ id] == fifo)
      {
	sprintf( buffer, "close %d", id);
	dtdserver_send_command( server, buffer);
      }
}


/* ********************************************************************** */
/* Methods                                                                */
/* ********************************************************************** */

static int create_base_dir( const char *base_dir)
{
  struct stat buf;
  char *p;

  p = index( base_dir, '/') + 1;

  do
    {
      p = index( p, '/');

      if (!p)
	break;

      *p = '\0';
      if ( stat( base_dir, &buf) < 0)
	{
	  if ( errno != ENOENT)
	    {
	      fprintf( stderr, "Cannot stat DTD fifo root directory %s (%s)\n", base_dir, strerror( errno));
	      return -1;
	    }

	  if ( mkdir( base_dir, 0777) < 0)
	    {
	      fprintf( stderr, "Cannot create DTD fifo root directory %s (%s)\n", base_dir, strerror( errno));
	      return -1;
	    }
	}
      *p = '/';

      p++;
    }
  while (*p);

  return 1;
}


static void dtdserver_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dtdserver_t *this = (dtdserver_t *)o;
  int from_child_pipe[2];
  const char *base_dir;
  char *d;
  int len, fifo_size;

  fifo_size = DEFAULT_BLOCK_FRAMES * DEFAULT_BLOCK_MAX_CHANNELS * DEFAULT_FIFO_BLOCKS * sizeof( float);
  this->fifo_size = fts_get_int_arg( ac, at, 1, fifo_size);
  this->loop_milliseconds = fts_get_int_arg( ac, at, 2, DEFAULT_LOOP_MILLISECONDS);

  /* fork the server */
  if ( pipe( from_child_pipe) < 0)
    {
      post( "[dtdserver]: pipe() failed (%s)\n", strerror( errno));
      return;
    }

  if ( (this->server_pid = fork()) < 0)
    {
      post( "[dtdserver]: fork() failed (%s)\n", strerror( errno));
      return;
    }
  else if ( !this->server_pid)
    {
      char exe[256];

      close( from_child_pipe[0]);
      if ( dup2( from_child_pipe[1], 1) < 0)
	{
	  fprintf( stderr, "[dtdserver]: dup2() failed (%s)\n", strerror( errno));
	  exit( 1);
	}
      close( from_child_pipe[1]);

      sprintf( exe, "%s/packages/unixdtd/bin/%s/%s/dtd", fts_get_root_dir(), fts_get_arch(), fts_get_mode());

      if ( execl( exe, exe, NULL) < 0)
	{
	  fprintf( stderr, "[dtdserver]: execl() failed (%s)\n", strerror( errno));
	}

      exit( 1);
    }

  /* Read the port number from server */
  read( from_child_pipe[0], &this->server_port, 4);

  /* Open the socket */
  if ( (this->server_socket = socket( PF_INET, SOCK_DGRAM, 0)) < 0)
    post( "[dtdserver] cannot open socket (%s)\n", strerror( errno));

  /* 
     append the process id of the server to the base directory, so that you can have
     several servers on the same machine
  */
  base_dir = fts_symbol_name( fts_get_symbol_arg( ac, at, 3, fts_new_symbol( DEFAULT_BASE_DIR)));

  len = strlen( base_dir);
  d = strcpy( malloc( len+32), base_dir);
  sprintf( d + len, "/%d/", this->server_pid);

  this->base_dir = d;

  create_base_dir( this->base_dir);

  this->number_of_objects = 0;
  this->number_of_fifos = 0;
}

static void dtdserver_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dtdserver_t *this = (dtdserver_t *)o;
  int id;

/*    fprintf( stderr, "Killing DTD server (pid = %d)\n", this->server_pid); */

  /* stop the server */
  kill( this->server_pid, SIGKILL);

  for ( id = 0; id < DTD_MAX_FIFOS; id++)
    {
      if (this->fifo_table[ id])
	dtdfifo_delete( this->fifo_table[ id]);
    }

  if ( rmdir( this->base_dir) < 0)
    fprintf( stderr, "Cannot remove directory %s (%d,%s)\n", this->base_dir, errno, strerror( errno));
}

static fts_status_t dtdserver_instantiate( fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init( cl, sizeof(dtdserver_t), 0, 0, 0);

  fts_method_define_varargs( cl, fts_SystemInlet, fts_s_init, dtdserver_init);

  fts_method_define( cl, fts_SystemInlet, fts_s_delete, dtdserver_delete, 0, 0);

  return fts_Success;
}

void dtdserver_config( void)
{
  fts_symbol_t s = fts_new_symbol( "dtdserver");

  fts_class_install( s, dtdserver_instantiate);
  dtdserver_class = fts_class_get_by_name( s);
}


