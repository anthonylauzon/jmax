/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
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
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include "ftsconfig.h"

#ifdef WIN32
#include <windows.h>

int win_close(int socket)
{
  int r;
  char buf[1024];
  if (socket != INVALID_SOCKET) {
    shutdown(socket, 0x02);
    while (1) {
      r = recv(socket, buf, 1024, 0);
      if ((r == 0) || (r == SOCKET_ERROR)) {
	break;
      }
    }
    closesocket(socket);
  }
  return 0;
}

#define CLOSESOCKET  win_close
#define READSOCKET(S,B,L) recv(S,B,L,0)
#define WRITESOCKET(S,B,L) send(S,B,L,0)
#define client_error(mess)  post("%s (error %d)\n", mess, WSAGetLastError())

typedef unsigned int socklen_t;
typedef SOCKET socket_t;

#else

#include <sys/types.h>
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#if HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#include <netinet/tcp.h>
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
#define SOCKET_ERROR -1
typedef int socket_t;
#define client_error(mess)  post("%s (%s)\n", mess, strerror( errno))

#endif

#include <string.h>
#include <stdlib.h>

#include <fts/fts.h>
#include "ftsprivate/connection.h"
#include "ftsprivate/patcher.h"
#include "ftsprivate/client.h"
#include "ftsprivate/protocol.h"


/*
 * Client object
 * (the object that is created on new client connections)
 */
typedef struct _client_t {
  fts_object_t head;
  socket_t socket;

  /* Output buffer */
  fts_stack_t send_sb;

  /* Automata state */
  unsigned char incoming;
  int state;
  int ival;

  /* Input decoding */
  fts_object_t *dest_object;
  fts_symbol_t selector;
  fts_stack_t receive_args;
  fts_stack_t receive_sb;

  /* Object table */
  fts_hashtable_t object_table;

  /* Client id */
  int client_id;

  /* Root patcher */
  fts_object_t *root_patcher;
} client_t;

/*
 * client_manager object
 * (the object that listens for client connections) 
 */
typedef struct {
  fts_object_t head;
  socket_t socket;
} client_manager_t;

static client_t *client_table[MAX_CLIENTS];


/***********************************************************************
 *
 * client_manager object
 *
 */

static void client_manager_select( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_manager_t *this = (client_manager_t *)o;
  socket_t new_socket;
  int new_client_id;
  fts_atom_t argv[3];
  fts_object_t *client_object;

  for ( new_client_id = 0; new_client_id < MAX_CLIENTS; new_client_id++)
    if (!client_table[new_client_id])
      break;

  if (new_client_id >= MAX_CLIENTS)
    {
      post( "No more client connections available (max %d)\n", MAX_CLIENTS);
      return;
    }

  new_socket = accept( this->socket, NULL, NULL);

  if (new_socket == INVALID_SOCKET)
    {
      client_error( "Cannot accept() connection");
      return;
    }

  fts_set_symbol( argv, fts_new_symbol("client"));
  fts_set_int( argv+1, new_socket);
  /* Client id is index in table + 1, so that no client can have id 0 */
  fts_set_int( argv+2, new_client_id+1);

  fts_object_new_to_patcher( fts_get_root_patcher(), 3, argv, (fts_object_t **)&client_object);

  if (!client_object)
    {
      fprintf( stderr, "[client_manager] internal error (cannot create client object)\n");
      return;
    }

  client_table[new_client_id] = (struct _client_t *)client_object;
}

static void client_manager_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_manager_t *this = (client_manager_t *)o;
  int port;
  struct sockaddr_in addr;
  static int objects_count = 0;

  if ( ++objects_count > 1)
    {
      fts_object_set_error( o, "Object client_manager already exists");
      return;
    }

  ac--;
  at++;

  port = fts_get_int_arg( ac, at, 0, CLIENT_DEFAULT_PORT);

  this->socket = socket( AF_INET, SOCK_STREAM, 0);
  if (this->socket == INVALID_SOCKET)
    {
      client_error( "Cannot create socket");
      return;
    }

  memset( (char *)&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);

  if ( bind( this->socket, (const struct sockaddr *) &addr, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
    {
      client_error( "Cannot bind socket");
      CLOSESOCKET( this->socket);
      this->socket = INVALID_SOCKET;
      return;
    }

  if ( listen( this->socket, MAX_CLIENTS) == SOCKET_ERROR)
    {
      client_error( "Cannot listen on socket");
      CLOSESOCKET( this->socket);
      this->socket = INVALID_SOCKET;
      return;
    }

  fts_sched_add( (fts_object_t *)this, FTS_SCHED_READ, this->socket);

  fts_log( "[client]: Listening on port %d\n", port);
}

static void client_manager_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_manager_t *this = (client_manager_t *)o;

  if ( this->socket == INVALID_SOCKET)
    return;

  fts_sched_remove( (fts_object_t *)this);
  CLOSESOCKET( this->socket);
}

static fts_status_t client_manager_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init( cl, sizeof( client_manager_t), 0, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, client_manager_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, client_manager_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_sched_ready, client_manager_select);

  return fts_Success;
}

/***********************************************************************
 *
 *  client object
 *
 */

/***********************************************************************
 * 
 * Messages to client
 *
 */

static void client_start_message( client_t *this)
{
  fts_stack_clear( &this->send_sb);
}

/* Used by other functions */
static void client_put( client_t *this, int n)
{
  fts_stack_push( &this->send_sb, unsigned char, (unsigned char) ((n >> 24) & 0xff));
  fts_stack_push( &this->send_sb, unsigned char, (unsigned char) ((n >> 16) & 0xff));
  fts_stack_push( &this->send_sb, unsigned char, (unsigned char) ((n >> 8) & 0xff));
  fts_stack_push( &this->send_sb, unsigned char, (unsigned char) ((n >> 0) & 0xff));
}

static void client_put_int( client_t *this, int n)
{
  fts_stack_push( &this->send_sb, unsigned char, FTS_PROTOCOL_INT);
  client_put( this, n);
}

static void client_put_float( client_t *this, float value)
{
  float f = value;

  fts_stack_push( &this->send_sb, unsigned char, FTS_PROTOCOL_FLOAT);
  client_put( this, *((unsigned int *)&f));
}

static void client_put_symbol( client_t *this, fts_symbol_t s)
{
  const char *p = fts_symbol_name( s);

  fts_stack_push( &this->send_sb, unsigned char, FTS_PROTOCOL_STRING);

  while (*p)
    fts_stack_push( &this->send_sb, unsigned char, (unsigned char)*p++);

  fts_stack_push( &this->send_sb, unsigned char, FTS_PROTOCOL_STRING_END);
}

static void client_put_object( client_t *this, fts_object_t *obj)
{
  fts_stack_push( &this->send_sb, unsigned char, FTS_PROTOCOL_OBJECT);
  client_put( this, OBJECT_ID_OBJ( fts_object_get_id( obj)));
}

static void client_put_atoms( client_t *this, int ac, const fts_atom_t *at)
{
  while (ac--)
    {
      if ( fts_is_int( at))
	client_put_int( this, fts_get_int( at));
      else if ( fts_is_float( at))
	client_put_float( this, fts_get_float( at));
      else if ( fts_is_symbol( at))
	client_put_symbol( this, fts_get_symbol( at));
      else if ( fts_is_object( at))
	client_put_object( this, fts_get_object( at));

      at++;
    }
}

static int client_end_message( client_t *this)
{
  int r;

  fts_stack_push( &this->send_sb, unsigned char, FTS_PROTOCOL_END_OF_MESSAGE);

  if ( ( r = WRITESOCKET( this->socket, fts_stack_get_ptr( &this->send_sb), fts_stack_get_size( &this->send_sb)) == SOCKET_ERROR))
    {
      client_error( "[client] error in sending message");
      return -1;
    }

  return 0;
}


/***********************************************************************
 * 
 * Finite state automata for protocol decoding
 *
 */

static void a_end_dest_object( client_t *this)
{
  fts_atom_t k, v;

  this->ival = (this->ival << 8) | this->incoming;

  fts_set_int( &k, this->ival);
  if ( !fts_hashtable_get( &this->object_table, &k, &v))
    {
      fprintf( stderr, "invalid object in protocol %d\n", this->ival);
      return;
    }

  this->dest_object = fts_get_object( &v);
}

static void a_end_selector( client_t *this)
{
  fts_stack_push( &this->receive_sb, unsigned char, '\0');

  this->selector = fts_new_symbol_copy( fts_stack_get_ptr( &this->receive_sb));
}

static void a_end_int_arg( client_t *this)
{
  fts_atom_t a;

  this->ival = (this->ival << 8) | this->incoming;
  fts_set_int( &a, this->ival);
  fts_stack_push( &this->receive_args, fts_atom_t, a);
} 

static void a_end_float_arg( client_t *this)
{
  fts_atom_t a;

  this->ival = (this->ival << 8) | this->incoming;
  fts_set_float( &a, *((float *)&this->ival));
  fts_stack_push( &this->receive_args, fts_atom_t, a);
}

static void a_end_string_arg( client_t *this)
{
  fts_atom_t a;

  fts_stack_push( &this->receive_sb, unsigned char, '\0');
  fts_set_symbol( &a, fts_new_symbol_copy( fts_stack_get_ptr( &this->receive_sb)));
  fts_stack_push( &this->receive_args, fts_atom_t, a);
}

static void a_end_object_arg( client_t *this)
{
  fts_atom_t k, v;
  fts_object_t *obj = 0;

  this->ival = (this->ival << 8) | this->incoming;

  fts_set_int( &k, this->ival);
  if ( !fts_hashtable_get( &this->object_table, &k, &v))
    {
      fprintf( stderr, "invalid object in protocol %d\n", this->ival);
      return;
    }

  fts_stack_push( &this->receive_args, fts_atom_t, v);
}

static void a_end_message( client_t *this)
{
  int argc;
  fts_atom_t *argv;

  argc = fts_stack_get_size( &this->receive_args);
  argv = (fts_atom_t *)fts_stack_get_ptr( &this->receive_args);

  fts_log( "[client]: Received message dest=0x%x selector=%s args=", this->dest_object, fts_symbol_name(this->selector));
  fts_log_atoms( argc, argv);
  fts_log( "\n");

  /* Client messages are sent to the system inlet */
  if (this->dest_object)
    fts_send_message( this->dest_object, fts_SystemInlet, this->selector, argc, argv);

  fts_stack_clear( &this->receive_args);

  this->dest_object = 0;
}

static void a_protocol_error( client_t *this)
{
  fprintf( stderr, "Protocol error: state %d incoming %d\n", this->state, this->incoming);
}

static void client_receive( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_t *this = (client_t *)o;
  int i, size;
#define NN 1024
  unsigned char buffer[NN];

  size = READSOCKET( this->socket, buffer, NN);

  if ( size < 0)
    {
      client_error( "[client]: Error in reading message, client stopped");
      fts_log( "[client]: Error in reading message, client stopped\n");
      fts_object_delete_from_patcher( (fts_object_t *)this);
      return;
    }
  else if (size == 0)
    {
      fts_log( "[client]: Client stopped\n");
      fts_object_delete_from_patcher( (fts_object_t *)this);
      return;
    }

  for ( i = 0; i < size; i++)
    {
      this->incoming = buffer[i];

#define MOVE( VALUE, STATE, ACTION) if ( this->incoming==(VALUE)) { this->state = STATE; ACTION; break; }
#define UMOVE( STATE, ACTION) this->state = STATE; ACTION; break;

      switch( this->state) {
      case 0:
	MOVE( FTS_PROTOCOL_OBJECT, 1, this->ival = 0);
	UMOVE( 0, a_protocol_error( this));
      case 1:
	UMOVE( 2, this->ival = this->ival << 8 | this->incoming);
      case 2:
	UMOVE( 3, this->ival = this->ival << 8 | this->incoming);
      case 3:
	UMOVE( 4, this->ival = this->ival << 8 | this->incoming);
      case 4:
	UMOVE( 5, a_end_dest_object(this) );
      case 5:
	MOVE( FTS_PROTOCOL_STRING, 6, fts_stack_clear( &this->receive_sb));
	UMOVE( 0, a_protocol_error(this) );
      case 6:
	MOVE( FTS_PROTOCOL_STRING_END, 10, a_end_selector( this));
	UMOVE( 6, fts_stack_push( &this->receive_sb, unsigned char, this->incoming) );
      case 10:
	MOVE( FTS_PROTOCOL_INT, 20, this->ival = 0);
	MOVE( FTS_PROTOCOL_FLOAT, 30, this->ival = 0);
	MOVE( FTS_PROTOCOL_STRING, 40, fts_stack_clear( &this->receive_sb));
	MOVE( FTS_PROTOCOL_OBJECT, 50, this->ival = 0);
	MOVE( FTS_PROTOCOL_END_OF_MESSAGE, 0, a_end_message(this));
	UMOVE( 0, a_protocol_error( this));
      case 20:
	UMOVE( 21, this->ival = this->ival << 8 | this->incoming);
      case 21:
	UMOVE( 22, this->ival = this->ival << 8 | this->incoming);
      case 22:
	UMOVE( 23, this->ival = this->ival << 8 | this->incoming);
      case 23:
	UMOVE( 10, a_end_int_arg(this) );
      case 30:
	UMOVE( 31, this->ival = this->ival << 8 | this->incoming);
      case 31:
	UMOVE( 32, this->ival = this->ival << 8 | this->incoming);
      case 32:
	UMOVE( 33, this->ival = this->ival << 8 | this->incoming);
      case 33:
	UMOVE( 10, a_end_float_arg(this) );
      case 40:
	MOVE( FTS_PROTOCOL_STRING_END, 10, a_end_string_arg( this));
	UMOVE( 40, fts_stack_push( &this->receive_sb, unsigned char, this->incoming) );
      case 50:
	UMOVE( 51, this->ival = this->ival << 8 | this->incoming);
      case 51:
	UMOVE( 52, this->ival = this->ival << 8 | this->incoming);
      case 52:
	UMOVE( 53, this->ival = this->ival << 8 | this->incoming);
      case 53:
	UMOVE( 10, a_end_object_arg( this));
      default:
	fprintf( stderr, "dans un drôle d'état %d\n", this->state);
	break;
      }
    }
}

/***********************************************************************
 * 
 * Client object methods
 *
 */

static void client_new_object( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_t *this = (client_t *)o;
  int id;
  fts_object_t *parent;
  fts_object_t *newobj;
  fts_atom_t k, v;

  parent = fts_get_object_arg( ac, at, 0, 0);
  id = fts_get_int_arg( ac, at, 1, -1);

  if (!parent || id < 0)
    {
      post( "client: invalid arguments\n");
      return;
    }

  newobj = fts_eval_object_description( (fts_patcher_t *)parent, ac-2, at+2);

  if (!newobj || fts_object_is_error( newobj))
    {
      post( "Error in object instantiation (");
      post_atoms( ac-2, at+2);
      post( ")\n");
      return;
    }

  fts_set_int( &k, id);
  fts_set_object( &v, newobj);
  fts_hashtable_put( &this->object_table, &k, &v);

  newobj->head.id = OBJECT_ID( id, this->client_id);
}

static void client_connect_object( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_t *this = (client_t *)o;
  fts_object_t *src, *dst;
  int src_outlet, dst_inlet;

  src = fts_get_object_arg( ac, at, 0, 0);
  src_outlet = fts_get_int_arg( ac, at, 1, -1);
  dst = fts_get_object_arg( ac, at, 2, 0);
  dst_inlet = fts_get_int_arg( ac, at, 3, -1);

  if ( !src || src_outlet < 0 || !dst || dst_inlet < 0)
    return;

  fts_connection_new( FTS_NO_ID, src, src_outlet, dst, dst_inlet);
}

static void client_delete_object( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_t *this = (client_t *)o;
  fts_object_t *obj;
  fts_atom_t k;

  obj = fts_get_object( at);

  if (!obj)
    return;

  fts_object_delete_from_patcher( obj);

  fts_set_int( &k, OBJECT_ID_OBJ( fts_object_get_id( obj)) );
  fts_hashtable_remove( &this->object_table, &k);
}

static void client_shutdown( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_sched_halt();
}

static void client_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_t *this = (client_t *)o;
  fts_atom_t k, v;
  fts_atom_t a[1];

  ac--;
  at++;

  this->socket = fts_get_int_arg( ac, at, 0, INVALID_SOCKET);
  this->client_id = fts_get_int_arg( ac, at, 1, -1);

  if (this->socket == INVALID_SOCKET || this->client_id < 0)
    {
      fts_object_set_error( (fts_object_t *)this, "Invalid arguments");
      CLOSESOCKET( this->socket);
      return;
    }

  fts_set_symbol( a, fts_s_patcher);
  fts_object_new_to_patcher( fts_get_root_patcher(), 1, a, &this->root_patcher);

  if ( !this->root_patcher)
    {
      fts_object_set_error( (fts_object_t *)this, "Cannot create client root patcher");
      CLOSESOCKET( this->socket);
      return;
    }

  fts_stack_init( &this->send_sb, unsigned char);

  this->state = 0;
  this->dest_object = 0;
  fts_stack_init( &this->receive_args, fts_atom_t);

  fts_stack_init( &this->receive_sb, unsigned char);

  fts_hashtable_init( &this->object_table, FTS_HASHTABLE_INT, FTS_HASHTABLE_MEDIUM);

  fts_set_int( &k, 0);
  fts_set_object( &v, this->root_patcher);
  fts_hashtable_put( &this->object_table, &k, &v);

  fts_set_int( &k, 1);
  fts_set_object( &v, (fts_object_t *)this);
  fts_hashtable_put( &this->object_table, &k, &v);

  fts_sched_add( (fts_object_t *)this, FTS_SCHED_READ, this->socket);

  fts_log( "[client]: Accepted client connection on socket %d\n", this->socket);
}

static void client_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_t *this = (client_t *)o;

  fts_object_delete_from_patcher( this->root_patcher);

  client_table[ this->client_id - 1] = 0;

  fts_stack_destroy( &this->send_sb);
  fts_stack_destroy( &this->receive_args);
  fts_stack_destroy( &this->receive_sb);
  fts_hashtable_destroy( &this->object_table);

  fts_sched_remove( (fts_object_t *)this);
  CLOSESOCKET( this->socket);

  fts_log( "[client]: Released client connection on socket %d\n", this->socket);
}

static fts_status_t client_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init( cl, sizeof( client_t), 1, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, client_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, client_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_sched_ready, client_receive);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol( "new_object"), client_new_object);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol( "connect_object"), client_connect_object);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol( "delete_object"), client_delete_object);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol( "shutdown"), client_shutdown);

  return fts_Success;
}

/***********************************************************************
 *
 *  client controller object
 *
 */

typedef struct {
  fts_object_t head;
  int gate;
  int echo;
} client_controller_t;

static void client_controller_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_controller_t *this = (client_controller_t *)o;
  fts_object_t *target;
  fts_object_t *from;
  fts_object_t *to;
  fts_atom_t *v;
  fts_symbol_t target_class_name;
  int channel_number;
  fts_symbol_t s_bus = fts_new_symbol( "bus");
  fts_symbol_t s_label = fts_new_symbol( "label");

  this->gate = 0;

  /* By default, we don't echo the message coming from the client to the client */
  this->echo = 0;

  ac--;
  at++;

  if ( !((ac == 1 || ac == 2) && fts_is_symbol( at) && ( (ac==2) ? fts_is_int( at+1) : 1)) )
    {
      fts_object_set_error( (fts_object_t *)this, "Invalid arguments (symbol [int])");
      return;
    }

  if ( !fts_is_symbol( at))
    {
      fts_object_set_error( (fts_object_t *)this, "First argument must be a symbol");
      return;
    }

  v = fts_variable_get_value( fts_object_get_patcher(o), fts_get_symbol( at));
  if (!v || !fts_is_object(v))
    {
      fts_object_set_error( (fts_object_t *)this, "First argument does not refer to an object");
      return;
    }

  target = fts_get_object( v);
  target_class_name = fts_object_get_class_name( target);
 
  if (target_class_name != s_bus && target_class_name != s_label)
    {
      fts_object_set_error( (fts_object_t *)this, "First argument must be a \"bus\" or a \"label\"");
      return;
    }

  channel_number = fts_get_int_arg( ac, at, 1, 0);

  if ( target_class_name == s_bus)
    {
      fts_atom_t a[3];

      fts_set_symbol( a, fts_new_symbol( "catch"));
      fts_set_object_with_type( a+1, target, target_class_name);
      fts_set_int( a+2, channel_number);
      fts_object_new_to_patcher( fts_object_get_patcher( (fts_object_t *)this), 3, a, &from);

      fts_set_symbol( a, fts_new_symbol( "throw"));
      fts_object_new_to_patcher( fts_object_get_patcher( (fts_object_t *)this), 3, a, &to);
    }
  else
    {
      fts_atom_t a[2];

      fts_set_symbol( a, fts_new_symbol( "inlet"));
      fts_set_object_with_type( a+1, target, target_class_name);
      fts_object_new_to_patcher( fts_object_get_patcher( (fts_object_t *)this), 2, a, &from);

      fts_set_symbol( a, fts_new_symbol( "outlet"));
      fts_object_new_to_patcher( fts_object_get_patcher( (fts_object_t *)this), 2, a, &to);
    }

  if( !from || !to)
    {
      fts_object_set_error( (fts_object_t *)this, "Cannot create connection objects");
      return;
    }

  fts_connection_new( FTS_NO_ID, from, 0, (fts_object_t *)this, 0);
  fts_connection_new( FTS_NO_ID, (fts_object_t *)this, 0, to, 0);

  fts_log( "[client]: Created controller on %s %s channel %d\n", fts_symbol_name( target_class_name), fts_symbol_name( fts_object_get_variable( target)), channel_number);
}

static void client_controller_delete_dummy(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
}

static void client_controller_anything_fts(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_controller_t *this = (client_controller_t *)o;
  client_t *client;

  /* If we don't echo and the gate is set, do nothing */
  if ( !this->echo && this->gate )
    return;

  client = client_table[OBJECT_ID_CLIENT( fts_object_get_id( o))-1];

  if (client != NULL)
    {
      fts_log( "[client]: Sending \"%s ", fts_symbol_name(s));
      fts_log_atoms( ac, at);
      fts_log( "\"\n");

      client_start_message( client);
      client_put_object( client, o);
      client_put_symbol( client, s);
      client_put_atoms( client, ac, at);
      client_end_message( client);
    }
}

static void client_controller_anything_client(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_controller_t *this = (client_controller_t *)o;

  this->gate = 1;

  fts_log( "[client]: Received \"");
  fts_log_atoms( ac, at);
  fts_log( "\"\n");

  fts_outlet_send( o, 0, s, ac, at);

  this->gate = 0;
}

static void client_controller_set_echo(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  client_controller_t *this = (client_controller_t *)obj;

  if ( fts_is_symbol(value) )
    this->echo = (fts_get_symbol(value) == fts_s_yes);
}

static fts_status_t client_controller_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(client_controller_t), 1, 1, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, client_controller_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, client_controller_delete_dummy);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_anything, client_controller_anything_client);
  fts_method_define_varargs(cl, 0, fts_s_anything, client_controller_anything_fts);

  fts_class_add_daemon( cl, obj_property_put, fts_new_symbol("echo"), client_controller_set_echo);

  return fts_Success;
}

/***********************************************************************
 *
 * Initialization
 *
 */
void fts_client_config( void)
{
  int ac = 0;
  fts_atom_t at[2];
  fts_symbol_t s;
  fts_symbol_t s_client_manager;
  fts_object_t *obj;

  s_client_manager = fts_new_symbol("client_manager");

  fts_class_install( s_client_manager, client_manager_instantiate);
  fts_class_install( fts_new_symbol("client"), client_instantiate);
  fts_class_install( fts_new_symbol("client_controller"), client_controller_instantiate);

  fts_set_symbol( at, s_client_manager);
  ac++;

  if ((s = fts_cmd_args_get( fts_new_symbol( "listen-port"))))
    {
      fts_set_int( at+1, atoi( fts_symbol_name( s)));
      ac++;
    }

  fts_object_new_to_patcher( fts_get_root_patcher(), ac, at, &obj);

  if ( !obj)
    post( "cannot create client manager\n");
}

