/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
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
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <fts/fts.h>
#include <ftsconfig.h>

#include <string.h>

#ifdef WIN32
#include <windows.h>

extern int win_close(int socket);

#define CLOSESOCKET  win_close
#define READSOCKET(S,B,L) recv(S,B,L,0)
#define WRITESOCKET(S,B,L) send(S,B,L,0)
#define SOCKET_ERROR_MESSAGE (WSAGetLastError())

typedef unsigned int socklen_t;
typedef SOCKET socket_t;

#else

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <fcntl.h>
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NETINET_TCP_H
#include <netinet/tcp.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
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

#endif

fts_class_t *fts_socketstream_class = 0;
fts_class_t *fts_udpstream_class = 0;

/***********************************************************************
 *
 * Socket bytestream
 * (the object that implements a bidirectional byte stream over a TCP/IP socket) 
 *
 */
#if !defined(__POWERPC__) || (defined(__APPLE__) && defined(__MACH__))
typedef struct _fts_socketstream_t {
  fts_bytestream_t bytestream;
  socket_t socket;
} fts_socketstream_t;

static void fts_socketstream_output(fts_bytestream_t *stream, int n, const unsigned char *c)
{
  int r;
  fts_socketstream_t *this = (fts_socketstream_t *) stream;

  if (this->socket == INVALID_SOCKET) 
    return;

  r = WRITESOCKET(this->socket, c, n);

  if ((r == SOCKET_ERROR) || (r == 0)) 
  {
    CLOSESOCKET(this->socket);
    this->socket = INVALID_SOCKET;
  }
}

static void fts_socketstream_output_char(fts_bytestream_t *stream, unsigned char c)
{
  fts_socketstream_output(stream, 1, &c);
}

static void fts_socketstream_flush(fts_bytestream_t *stream)
{
#if WIN32
  fts_socketstream_t *this = (fts_socketstream_t *) stream;
  FlushFileBuffers((HANDLE) this->socket);
#endif
}

/* Argument: <INT> socket */
static void fts_socketstream_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_socketstream_t *this = (fts_socketstream_t *) o;

  fts_bytestream_init((fts_bytestream_t *) this);
  
  this->socket = fts_get_int_arg( ac, at, 0, INVALID_SOCKET);

  if (this->socket == INVALID_SOCKET) 
  {
    fts_object_error( (fts_object_t *)this, "bad arguments");
    return;
  }

  fts_bytestream_set_input( (fts_bytestream_t *) this);

  fts_bytestream_set_output((fts_bytestream_t *) this, 
			    fts_socketstream_output,
			    fts_socketstream_output_char,
			    fts_socketstream_flush);

  fts_sched_add( (fts_object_t *)this, FTS_SCHED_READ, this->socket);
}

static void fts_socketstream_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_socketstream_t *this = (fts_socketstream_t *) o;

  fts_sched_remove( (fts_object_t *)this);

  if (this->socket != INVALID_SOCKET) 
  {
    CLOSESOCKET(this->socket);
    this->socket = INVALID_SOCKET;
  }  

  fts_bytestream_destroy((fts_bytestream_t *) this);
}

static void fts_socketstream_receive(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
#define NN 1024
  fts_socketstream_t *this = (fts_socketstream_t *) o;
  unsigned char buffer[NN];
  int size;

  size = READSOCKET( this->socket, buffer, NN);

  fts_bytestream_input((fts_bytestream_t *) this, size, buffer);
}

static void fts_socketstream_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_socketstream_t), fts_socketstream_init, fts_socketstream_delete);
  fts_bytestream_class_init(cl);

  fts_class_message_varargs(cl, fts_s_sched_ready, fts_socketstream_receive);
}
#endif

/***********************************************************************
 *
 * UDP bytestream
 * (the object that implements a bidirectional byte stream over a TCP/IP socket) 
 *
 */
#if !defined(__POWERPC__) || (defined(__APPLE__) && defined(__MACH__))
 
#define FTS_UDP_DEFAULT_PORT 2023

typedef struct _fts_udpstream_t 
{
  fts_bytestream_t bytestream;
  int socket;
  int in_port;
  fts_symbol_t out_host;
  int out_port;
  struct sockaddr_in out_addr;
} fts_udpstream_t;


static void fts_udpstream_output(fts_bytestream_t* stream, int n, const unsigned char* c)
{
  int r;
  fts_udpstream_t* self = (fts_udpstream_t*)stream;
  
  if (self->socket == INVALID_SOCKET)
    return;

  r = sendto(self->socket, c, n, 0, (const struct sockaddr*)&(self->out_addr), sizeof(struct sockaddr_in));
  if (r == SOCKET_ERROR)
  {
    CLOSESOCKET(self->socket);
    self->socket = INVALID_SOCKET;
  }
}


static void fts_udpstream_output_char(fts_bytestream_t *stream, unsigned char c)
{
  fts_udpstream_output(stream, 1, &c);
}

static void fts_udpstream_flush(fts_bytestream_t *stream)
{
#if WIN32
  fts_socketstream_t *this = (fts_socketstream_t *) stream;
  FlushFileBuffers((HANDLE) this->socket);
#endif
}


int fts_udpstream_get_input_port(fts_udpstream_t* stream)
{
  return stream->in_port;
}

int fts_udpstream_get_output_port(fts_udpstream_t* stream)
{
  return stream->out_port;
}


/* 
   <input>: port (int)
   <output>: host (symbol) port (int)
   
   udpstream <input>          : can receive
   udpstream <input> <output> : can receive and send
   udpstream <->     <output> : can send  
*/
static void fts_udpstream_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_udpstream_t* self = (fts_udpstream_t*)o;
  struct sockaddr_in addr;


  /* parent constructor */
  fts_bytestream_init((fts_bytestream_t*)self);

  /* socket creation */
  self->socket = socket(AF_INET, SOCK_DGRAM, 0);
  if (self->socket == INVALID_SOCKET)
  {
    fts_object_error(o, "Cannot create socket");
    return;
  }
  
  /*
    case:
    udp <input>
    udp <input> <output>

    RECEIVE CASE
  */
  if (fts_is_int(at))
  {
    self->in_port = fts_get_int(at);
    
    
    memset((char*)&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(self->in_port);
    
    if (bind(self->socket, (const struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
    {
      fts_object_error(o, "Cannot bind socket on port: %d", self->in_port);
      CLOSESOCKET(self->socket);
      return;
    }
    fts_bytestream_set_input((fts_bytestream_t*)self);
    fts_sched_add( (fts_object_t *)self, FTS_SCHED_READ, self->socket);
  }
  else
  {
    if (!fts_is_symbol(at)
	|| (fts_get_symbol(at) != fts_s_unconnected))
    {
      fts_object_error(o, "First argument must be int or \"-\"");
      return;
    }
  }
  /*
    case:
    udp <input> <output>
    udp   -     <output>

    SEND CASE
  */
  if (3 == ac)
  {
    /* check argument */
    if (fts_is_symbol(at + 1) && fts_is_int(at + 2))
    {
      struct hostent* hostptr;
      self->out_host = fts_get_symbol(at + 1);
      self->out_port = fts_get_int(at + 2);

      /* create output address */
      memset(&self->out_addr, 0, sizeof(self->out_addr));
      self->out_addr.sin_family = AF_INET;

      hostptr = gethostbyname(self->out_host);
      
      if ( !hostptr)
      {
	fts_object_error(o, "Unknown host %s", self->out_host);
	return;
      }
      
      memcpy( &(self->out_addr.sin_addr), (struct in_addr *)*(hostptr->h_addr_list), sizeof( struct in_addr));
      self->out_addr.sin_port = htons(self->out_port);

      /* set output function */
      fts_bytestream_set_output((fts_bytestream_t*)self,
				fts_udpstream_output,
				fts_udpstream_output_char,
				fts_udpstream_flush);
    }
    else
    {
      /* error wrong argument */
      fts_object_error(o, "wrong arguments types for output setting ");
    }
  }

  fts_log( "[udpstream]: created with port %d \n", self->in_port);
}

static void fts_udpstream_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_udpstream_t* self = (fts_udpstream_t*)o;
  
  /* remove from scheduler */
  fts_sched_remove(o);

  /* close socket */
  if (self->socket != INVALID_SOCKET) 
  {
    CLOSESOCKET(self->socket);
    self->socket = INVALID_SOCKET;
  }  
  
  fts_bytestream_destroy((fts_bytestream_t*)self);
}

static void fts_udpstream_receive(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
#define NN 1024
  fts_socketstream_t* self = (fts_socketstream_t *) o;
  unsigned char buffer[NN];
  int size;

  /* read available buffer */
  size = recvfrom(self->socket, buffer, NN, 0, NULL, NULL);
  fts_bytestream_input((fts_bytestream_t *) self, size, buffer);
}

static void fts_udpstream_instantiate(fts_class_t* cl)
{
  fts_class_init(cl, sizeof(fts_udpstream_t), fts_udpstream_init, fts_udpstream_delete);
  fts_bytestream_class_init(cl);

  /* name support */
  fts_class_message_varargs(cl, fts_s_name, fts_object_name); 
  fts_class_message_varargs(cl, fts_s_sched_ready, fts_udpstream_receive);
}

#endif

/***********************************************************************
 *
 * Initialization
 *
 */

void fts_kernel_socketstream_init(void)
{
  fts_socketstream_class = fts_class_install( fts_new_symbol("socketstream"), fts_socketstream_instantiate);
  fts_udpstream_class = fts_class_install( fts_new_symbol("udpstream"), fts_udpstream_instantiate);
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
