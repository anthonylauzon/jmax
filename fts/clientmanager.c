/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

/* **********************************************************************
 * 
 * client_manager object
 * (the object that listens for client connections)
 *
 */
#include <fts/fts.h>
#include <ftsconfig.h>

#include <string.h>
#include <stdlib.h>

#include "ftsprivate/client.h"
#include "ftsprivate/clientmanager.h"
#include "ftsprivate/patcherobject.h"
#include "ftsprivate/object.h"
#include "ftsprivate/patcher.h"

#define FTS_CLIENT_DEFAULT_PORT 2023
#define MAX_CLIENTS (1<<FTS_OBJECT_BITS_CLIENT)
#ifdef WIN32

#include <windows.h>

extern int win_close(int socket);

#define CLOSESOCKET  win_close
#define READSOCKET(S,B,L) recv(S,B,L,0)
#define WRITESOCKET(S,B,L) send(S,B,L,0)
#define SOCKET_ERROR_MESSAGE (WSAGetLastError())

typedef unsigned int socklen_t;
typedef SOCKET socket_t;

#else /* #ifdef WIN32 */

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <fcntl.h>
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#if HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#if HAVE_NETINET_TCP_H
#include <netinet/tcp.h>
#endif
#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>

#define CLOSESOCKET  close
#define READSOCKET(S,B,L) read(S,B,L)
#define WRITESOCKET(S,B,L) write(S,B,L)
#define INVALID_SOCKET -1
#define INVALID_PIPE -1
#define SOCKET_ERROR -1
typedef int socket_t;
#define SOCKET_ERROR_MESSAGE (strerror( errno))

#endif /* #ifdef WIN32 */

#define client_error(mess)  fts_post("%s (error %d)\n", mess, SOCKET_ERROR_MESSAGE)

typedef struct {
  fts_object_t base;
  socket_t socket;
} client_manager_t;

/* system byte stream implementations */
extern fts_class_t *fts_socketstream_class;
extern fts_class_t *fts_udpstream_class;
extern fts_class_t *fts_pipestream_class;

static fts_class_t *client_manager_class;

static void 
client_manager_select( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_manager_t *self = (client_manager_t *)o;
  socket_t new_socket;
  fts_atom_t a;
  fts_object_t *client_object;
  fts_object_t *socket_stream;

  new_socket = accept( self->socket, NULL, NULL);

  if (new_socket == INVALID_SOCKET)
    {
      client_error( "Client manager: cannot accept connection");
      return;
    }

  fts_set_int( &a, new_socket);
  socket_stream = fts_object_create( fts_socketstream_class, 1, &a);

  fts_set_object( &a, socket_stream);
  client_object = fts_object_create_in_patcher( fts_client_class, fts_get_root_patcher(), 1, &a);
  fts_patcher_add_object(fts_get_root_patcher(), client_object);

  if (!client_object)
    {
      fts_log( "[client_manager] internal error (cannot create client object)\n");
      return;
    }
}

static void
client_manager_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_manager_t *self = (client_manager_t *)o;
  int port;
  struct sockaddr_in addr;
  static int objects_count = 0;

  if ( ++objects_count > 1)
    {
      fts_object_error( o, "object client_manager already exists");
      return;
    }

  port = fts_get_int_arg( ac, at, 0, FTS_CLIENT_DEFAULT_PORT);

  self->socket = socket( AF_INET, SOCK_STREAM, 0);
  if (self->socket == INVALID_SOCKET)
    {
      client_error( "Cannot create socket");
      return;
    }

  memset( (char *)&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);

  if ( bind( self->socket, (const struct sockaddr *) &addr, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
    {
      client_error( "Cannot bind socket");
      CLOSESOCKET( self->socket);
      self->socket = INVALID_SOCKET;

      return;
    }

  if ( listen( self->socket, MAX_CLIENTS) == SOCKET_ERROR)
    {
      client_error( "Cannot listen on socket");
      CLOSESOCKET( self->socket);
      self->socket = INVALID_SOCKET;

      return;
    }

  fts_sched_add( (fts_object_t *)self, FTS_SCHED_READ, self->socket);

  fts_log( "[client]: Listening on port %d\n", port);
}

static void
client_manager_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_manager_t *self = (client_manager_t *)o;

  if ( self->socket == INVALID_SOCKET)
    return;

  fts_sched_remove( (fts_object_t *)self);
  CLOSESOCKET( self->socket);
}

static void 
client_manager_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof( client_manager_t), client_manager_init, client_manager_delete);
  fts_class_message_varargs(cl, fts_s_sched_ready, client_manager_select);
}

/***********************************************************************
 *
 * Initialization
 *
 */

void
fts_client_manager_tcp_start( void )
{
  int ac = 0;
  fts_atom_t at[1];
  fts_object_t *client_manager_object;
  fts_symbol_t s;

  if ((s = fts_cmd_args_get( fts_new_symbol( "listen-port"))))
    {
      fts_set_int( at, atoi( s));
      ac++;
    }
    
  client_manager_object = fts_object_create_in_patcher( client_manager_class, fts_get_root_patcher(), ac, at);
  fts_patcher_add_object(fts_get_root_patcher(), client_manager_object);
  
  if ( !client_manager_object)
    {
      fts_log( "[client] cannot create client manager\n");
    }
}

void
fts_client_manager_pipe_start( void)
{
  fts_atom_t a;
  fts_object_t *client_object;
  fts_object_t *pipe_stream; 

  pipe_stream = fts_object_create( fts_pipestream_class, 0, 0);
  
  fts_set_object( &a, pipe_stream);
  client_object = fts_object_create_in_patcher( fts_client_class, fts_get_root_patcher(), 1, &a);
  fts_patcher_add_object(fts_get_root_patcher(), client_object);
  
  if (!client_object)
    {
      fts_log( "[client_manager] internal error (cannot create client object)\n");
    }
}

/***********************************************************************
*
* Initialization
*
*/

void
fts_kernel_client_manager_init( void)
{
  client_manager_class = fts_class_install( NULL, client_manager_instantiate);
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */

