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

/* #define one of these to get a server that reads commands via udp or on a pipe */
#define USE_UDP
#undef USE_PIPE

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <signal.h>
#ifdef USE_UDP
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "fts.h"
#include "dtddefs.h"
#include "dtdfifo.h"

static int server_pid;

static FILE *server_stdin = NULL;
static FILE *server_stdout = NULL;
#ifdef USE_UDP
static int server_socket = 0;
static int server_port = 0;
#endif

static void dtdserver_fork( void)
{
  int to_child_pipe[2];
  int from_child_pipe[2];

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

  server_pid = fork();
  if (server_pid < 0)
    {
      fprintf( stderr, "fork() failed\n");
      exit(1);
    }
  else if ( !server_pid)
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
  server_stdout = fdopen( from_child_pipe[0], "r");
}


#ifdef USE_UDP
static int dtdserver_create_socket( void)
{
  int sock;

  if ( (sock = socket( PF_INET, SOCK_DGRAM, 0)) == -1)
    fprintf( stderr, "Cannot open socket\n");

  return sock;
}
#endif

void dtdserver_init( void)
{
  dtdserver_fork();

#ifdef USE_UDP
  server_socket = dtdserver_create_socket();

  /* Read the port number from server */
  fscanf( server_stdout, "%d", &server_port);
#endif
}

void dtdserver_exit( void)
{
  fprintf( stderr, "Killing DTD server (pid = %d)\n", server_pid);
  kill( server_pid, SIGKILL);

  /* Delete all fifos */
  /* To Be Written */
}

#ifdef USE_UDP
static void dtdserver_send_command( const char *command)
{
  int r;
  struct sockaddr_in my_addr;

  memset( &my_addr, 0, sizeof( my_addr));
  my_addr.sin_family = AF_INET;
  my_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
  my_addr.sin_port = htons( server_port);

  r = sendto( server_socket, command, strlen( command)+1, 0, &my_addr, sizeof(my_addr));

  if ( r < 0)
    {
      fprintf( stderr, "[fts] error sending (%d,%s)\n", errno, strerror( errno));
    }
}
#endif

#ifdef USE_PIPE
static void dtdserver_send_command( const char *command)
{
  fprintf( server_stdin, "%s\n", command);
  fflush( server_stdin);
}
#endif

void dtdserver_new( int id, const char *dirname, int buffer_size)
{
  char buffer[1024];

  sprintf( buffer, "new %d %s %d", id, dirname, buffer_size);
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
  char buffer[128];

  sprintf( buffer, "quit");
  dtdserver_send_command( buffer);
}
