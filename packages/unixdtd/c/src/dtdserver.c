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

#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
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

/*
 * The default settings for the server.
 * These arguments are passed as command line arguments to the forked server.
 */
#define DEFAULT_BLOCK_FRAMES  32768
#define DEFAULT_MAX_CHANNELS  8
#define DEFAULT_FIFO_BLOCKS  3
#define DEFAULT_PRELOAD_FRAMES  2048
#define DEFAULT_LOOP_MILLISECONDS 100

struct _dtdserver_t {
  fts_object_t head;

  int server_pid;
  int server_socket;
  int server_port;

  FILE *server_stdout;

  int block_frames;
  int max_channels;
  int fifo_blocks;
  int preload_frames;
  int loop_milliseconds;

  char *base_name;

  int number_of_objects;
  int number_of_fifos;

  dtdfifo_t *fifo_table[DTD_MAX_FIFOS];
};

static fts_class_t *dtdserver_type;
static dtdserver_t *default_instance;

dtdserver_t *dtdserver_get_default_instance( void)
{
  if (!default_instance)
    default_instance = (dtdserver_t *)fts_object_create(dtdserver_type, 0, 0);
  
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

  r = sendto( server->server_socket, command, strlen( command)+1, 0, (const struct sockaddr *)&my_addr, sizeof(my_addr));

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
      int fifo_size;
      dtdfifo_t *fifo;

      strcpy( filename, server->base_name);
      sprintf( filename + strlen( server->base_name), "-%d", server->number_of_fifos);

      fifo_size = server->block_frames * server->max_channels * server->fifo_blocks * sizeof( float);
      fifo = dtdfifo_new( filename, fifo_size);
      if ( !fifo)
	return;

      sprintf( buffer, "mmap %d %s", server->number_of_fifos, filename);
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
    char path[512];
  char buffer[1024];
  int id;
  dtdfifo_t *fifo;

  fifo = dtdserver_allocate_fifo( server, &id);

  if (fifo)
    {
      sprintf( buffer, "openread %d %s %s %d", id, 
	       filename, 
	       fts_make_absolute_path(NULL,filename, path, 512), 
	       n_channels);
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
      double sr;

      sr = (double)fts_dsp_get_sample_rate();

      sprintf( buffer, "openwrite %d %s %s %f %d", id, filename, fts_project_get_dir(), sr, n_channels);
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

static void dtdserver_select( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
#define LEN 2048
  dtdserver_t *this = (dtdserver_t *)o;
  char buff[LEN];

  if (fgets( buff, LEN, this->server_stdout) == NULL)
    {
      fprintf( stderr, "[fts] error reading DTD server stdout\n");
      return;
    }

  post( "%s", buff);
}

static void dtdserver_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dtdserver_t *this = (dtdserver_t *)o;
  int from_child_pipe[2];
  const char *base_name;
  int len;

  this->block_frames = fts_get_int_arg( ac, at, 0, DEFAULT_BLOCK_FRAMES);
  this->max_channels = fts_get_int_arg( ac, at, 1, DEFAULT_MAX_CHANNELS);
  this->fifo_blocks = fts_get_int_arg( ac, at, 2, DEFAULT_FIFO_BLOCKS);
  this->preload_frames = fts_get_int_arg( ac, at, 3, DEFAULT_PRELOAD_FRAMES);
  this->loop_milliseconds = fts_get_int_arg( ac, at, 4, DEFAULT_LOOP_MILLISECONDS);

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
      char buff1[32], buff2[32], buff3[32], buff4[32], buff5[32];
      fts_package_t *package;

      close( from_child_pipe[0]);
      if ( dup2( from_child_pipe[1], 1) < 0)
	{
	  fprintf( stderr, "[dtdserver]: dup2() failed (%s)\n", strerror( errno));
	  exit( 1);
	}
      close( from_child_pipe[1]);

      package = fts_package_get( fts_new_symbol( "unixdtd"));
      sprintf( exe, "%s/c/dtd", fts_package_get_dir( package));
      sprintf( buff1, "%d", this->block_frames);
      sprintf( buff2, "%d", this->max_channels);
      sprintf( buff3, "%d", this->fifo_blocks);
      sprintf( buff4, "%d", this->preload_frames);
      sprintf( buff5, "%d", this->loop_milliseconds);

      if ( execl( exe, exe, buff1, buff2, buff3, buff4, buff5, NULL) < 0)
	{
	  fprintf( stderr, "[dtdserver]: execl() failed (%s)\n", strerror( errno));
	}

      exit( 1);
    }

  /* Read the port number from server */
  read( from_child_pipe[0], &this->server_port, 4);

  fts_sched_add( (fts_object_t *)this, FTS_SCHED_READ, from_child_pipe[0]);

  this->server_stdout = fdopen( from_child_pipe[0], "r");

  /* Open the socket */
  if ( (this->server_socket = socket( PF_INET, SOCK_DGRAM, 0)) < 0)
    post( "[dtdserver] cannot open socket (%s)\n", strerror( errno));

  base_name = fts_get_symbol_arg( ac, at, 3, fts_new_symbol( DEFAULT_BASE_DIR));
  len = strlen( base_name);
  /* 
     append the process id of the server to the base name, so that you can have
     several servers on the same machine
  */
  this->base_name = strcpy( malloc( len+32), base_name);
  sprintf( this->base_name + len, "-%d", this->server_pid);

  this->number_of_objects = 0;
  this->number_of_fifos = 0;
}

static void dtdserver_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dtdserver_t *this = (dtdserver_t *)o;
  int id;

/*    fprintf( stderr, "Killing DTD server (pid = %d)\n", this->server_pid); */

  fts_sched_remove( (fts_object_t *)this);

  /* stop the server */
  kill( this->server_pid, SIGKILL);

  for ( id = 0; id < DTD_MAX_FIFOS; id++)
    {
      if (this->fifo_table[ id])
	dtdfifo_delete( this->fifo_table[ id]);
    }
}

static void dtdserver_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(dtdserver_t), dtdserver_init, dtdserver_delete);

  fts_class_message_varargs(cl, fts_s_sched_ready, dtdserver_select);
}

void dtdserver_config( void)
{
  fts_symbol_t s = fts_new_symbol( "dtdserver");

  dtdserver_type = fts_class_install( s, dtdserver_instantiate);
}


