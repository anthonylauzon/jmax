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

typedef struct {
  fts_object_t head;

  int server_pid;

  int server_socket;
  int server_port;

  int fifo_size;

  int loop_milliseconds;

  const char *base_dir;
} dtdserver_t;

static fts_class_t *dtdserver_class;
static dtdserver_t *single_instance;

void dtdserver_start( void)
{
  single_instance = (dtdserver_t *)fts_object_create( dtdserver_class, 0, 0);
}

void dtdserver_exit( void)
{
  fts_object_destroy( (fts_object_t *)single_instance);
}

static void dtdserver_send_command( const char *command)
{
  int r;
  struct sockaddr_in my_addr;

  if ( single_instance->server_socket <= 0)
    return;

  memset( &my_addr, 0, sizeof( my_addr));
  my_addr.sin_family = AF_INET;
  my_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
  my_addr.sin_port = htons( single_instance->server_port);

  r = sendto( single_instance->server_socket, command, strlen( command)+1, 0, &my_addr, sizeof(my_addr));

  if ( r < 0)
    {
      fprintf( stderr, "[fts] error sending (%d,%s)\n", errno, strerror( errno));
    }
}

void dtdserver_new_fifo( void)
{
  int id;
  char buffer[1024];

  id = dtdfifo_new( 0, single_instance->base_dir, single_instance->fifo_size);

  sprintf( buffer, "new %d %s %d", id, single_instance->base_dir, single_instance->fifo_size);
  dtdserver_send_command( buffer);
}

void dtdserver_open( int id, const char *filename, const char *path, int n_channels)
{
  char buffer[1024];

  sprintf( buffer, "open %d %s %s %d", id, filename, path, n_channels);
  dtdserver_send_command( buffer);
}

void dtdserver_close( int id)
{
  char buffer[128];

  sprintf( buffer, "close %d", id);
  dtdserver_send_command( buffer);
}

void dtdserver_quit( void)
{
  dtdserver_send_command( "quit");
}

static void dtdserver_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dtdserver_t *this = (dtdserver_t *)o;
  int from_child_pipe[2];
  const char *base_dir;
  char *d;

  if (single_instance)
    {
      post( "Internal error: instanciating second DTD object");
      return;
    }

  this->fifo_size = fts_get_int_arg( ac, at, 0, DEFAULT_BLOCK_FRAMES);
  this->loop_milliseconds = fts_get_int_arg( ac, at, 1, DEFAULT_LOOP_MILLISECONDS);

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
  base_dir = fts_symbol_name( fts_get_symbol_arg( ac, at, 2, fts_new_symbol( DEFAULT_BASE_DIR)));

  d = strcpy( malloc( strlen( base_dir+1+10)), base_dir);
  sprintf( d  + strlen( d), "/%d", this->server_pid);

  this->base_dir = d;

  single_instance = this;
}

static void dtdserver_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dtdserver_t *this = (dtdserver_t *)o;

/*    fprintf( stderr, "Killing DTD server (pid = %d)\n", this->server_pid); */

  /* stop the server */
  kill( this->server_pid, SIGKILL);

  dtdfifo_delete_all();

  if ( rmdir( this->base_dir) < 0)
    fprintf( stderr, "Cannot remove directory %s (%d,%s)\n", this->base_dir, errno, strerror( errno));

  single_instance = 0;
}

static fts_status_t dtdserver_instantiate( fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_type_t a[3];

  fts_class_init( cl, sizeof(dtdserver_t), 0, 0, 0);

  a[0] = fts_t_int;
  a[1] = fts_t_int;
  a[2] = fts_t_int;
  fts_method_define( cl, fts_SystemInlet, fts_s_init, dtdserver_init, 3, a);

  fts_method_define( cl, fts_SystemInlet, fts_s_delete, dtdserver_delete, 0, 0);

  return fts_Success;
}

void dtdserver_config( void)
{
  fts_symbol_t s = fts_new_symbol( "dtdserver");

  fts_class_install( s, dtdserver_instantiate);
  dtdserver_class = fts_class_get_by_name( s);
}


