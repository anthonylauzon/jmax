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
#include "ftsprivate/protocol.h"

#define CLIENT_DEFAULT_PORT 2023

/*
 * The object and client parts of the object ids
 */
#define OBJECT_ID_BITS  6
#define OBJECT_ID_OBJ_MASK  (~0<<OBJECT_ID_BITS)
#define OBJECT_ID_CLIENT_MASK  (~OBJECT_ID_OBJ_MASK)
#define OBJECT_ID_CLIENT(id) ((id)&OBJECT_ID_CLIENT_MASK)
#define OBJECT_ID_OBJ(id) (((id)&OBJECT_ID_OBJ_MASK)>>OBJECT_ID_BITS)
#define OBJECT_ID(o,c) ((o)<<OBJECT_ID_BITS|(c))

#define MAX_CLIENTS (1<<OBJECT_ID_BITS)

/*
 * Client object
 * (the object that is created on new client connections)
 */
typedef struct _client_t {
  fts_object_t head;
  socket_t socket;

  fts_stack_t send_sb;

  /* Automata state */
  unsigned char incoming;
  int state;

  fts_object_t *dest_object;
  fts_symbol_t selector;
  int argc;
  fts_atom_t *argv;
  int argv_len;

  int ival;

  fts_stack_t receive_sb;

  /* Object table */
  fts_hashtable_t object_table;

  /* Client id */
  int client_id;

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
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(struct sockaddr_in);
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

  new_socket = accept( this->socket, (struct sockaddr *) &client_addr, &client_len);
/*    new_socket = accept( this->socket, NULL, NULL); */

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

  post( "Opening client manager on port %d\n", port);

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

static void client_put_int( client_t *this, int n)
{
  fts_stack_push( &this->send_sb, unsigned char, FTS_PROTOCOL_INT);
  fts_stack_push( &this->send_sb, unsigned char, (unsigned char) ((n >> 24) & 0xff));
  fts_stack_push( &this->send_sb, unsigned char, (unsigned char) ((n >> 16) & 0xff));
  fts_stack_push( &this->send_sb, unsigned char, (unsigned char) ((n >> 8) & 0xff));
  fts_stack_push( &this->send_sb, unsigned char, (unsigned char) ((n >> 0) & 0xff));
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
  int n = OBJECT_ID_OBJ( fts_object_get_id( obj));

  fts_stack_push( &this->send_sb, unsigned char, FTS_PROTOCOL_OBJECT);
  fts_stack_push( &this->send_sb, unsigned char, (unsigned char) ((n >> 24) & 0xff));
  fts_stack_push( &this->send_sb, unsigned char, (unsigned char) ((n >> 16) & 0xff));
  fts_stack_push( &this->send_sb, unsigned char, (unsigned char) ((n >> 8) & 0xff));
  fts_stack_push( &this->send_sb, unsigned char, (unsigned char) ((n >> 0) & 0xff));
}

static void client_put_atoms( client_t *this, int ac, const fts_atom_t *at)
{
  while (ac--)
    {
      if ( fts_is_int( at))
	client_put_int( this, fts_get_int( at));
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
static void atoms_ensure_capacity( client_t *this)
{
  if (this->argc >= this->argv_len) 
    {
      this->argv_len *= 2;
      this->argv = (fts_atom_t *)fts_realloc( this->argv, this->argv_len * sizeof( fts_atom_t));
    }
}

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
  this->ival = (this->ival << 8) | this->incoming;
  atoms_ensure_capacity( this);

  fts_set_int( this->argv + this->argc, this->ival);
  this->argc++;
} 

static void a_end_float_arg( client_t *this)
{
  this->ival = (this->ival << 8) | this->incoming;
  atoms_ensure_capacity( this);

  fts_set_float( this->argv + this->argc, *((float *)&this->ival));
  this->argc++;
}

static void a_end_string_arg( client_t *this)
{
  fts_stack_push( &this->receive_sb, unsigned char, '\0');

  atoms_ensure_capacity( this);
  fts_set_symbol( this->argv + this->argc, fts_new_symbol_copy( fts_stack_get_ptr( &this->receive_sb)));
  this->argc++;
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

  atoms_ensure_capacity( this);
  fts_set_object( this->argv + this->argc, fts_get_object( &v));
  this->argc++;
}

static void a_end_message( client_t *this)
{
#ifdef DEBUG
  post( "[client] received message : dest=0x%x selector=%s args=", this->dest_object, fts_symbol_name(this->selector));
  post_atoms( this->argc, this->argv);
  post( "\n");
#endif

  /* on which inlet ???? */
  if (this->dest_object)
    fts_send_message( this->dest_object, 0, this->selector, this->argc, this->argv);

  this->argc = 0;
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
      client_error( "[client] error in reading message");
      return;
    }
  else if (size == 0)
    {
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
	fprintf( stderr, "dans un dr�le d'�tat %d\n", this->state);
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
  fts_object_t *newobj;
  fts_atom_t k, v;

  id = fts_get_int_arg( ac, at, 0, -1);

  newobj = fts_eval_object_description( fts_get_root_patcher(), ac-1, at+1);

  if (!newobj)
    {
      post( "Error in object instantiation (");
      post_atoms( ac-1, at+1);
      post( ")\n");
      return;
    }

  fts_set_int( &k, id);
  fts_set_object( &v, newobj);
  fts_hashtable_put( &this->object_table, &k, &v);

  newobj->head.id = OBJECT_ID( id, this->client_id);
}

static void client_delete_object( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_t *this = (client_t *)o;
  fts_object_t *obj;
  int i;

  obj = fts_get_object( at);

  if (!obj)
    return;

  fts_object_delete_from_patcher( obj);

  /* remove it from object table */
  /* FIXME */
}

static void client_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_t *this = (client_t *)o;
  fts_atom_t k, v;

  ac--;
  at++;

  this->socket = fts_get_int_arg( ac, at, 0, INVALID_SOCKET);
  this->client_id = fts_get_int_arg( ac, at, 1, 0);

  post( "[client] accepting connection (socket=%d)\n", this->socket);

  fts_stack_init( &this->send_sb, unsigned char);

  this->state = 0;
  this->argc = 0;
  this->argv_len = 128;
  this->argv = (fts_atom_t *)fts_malloc( this->argv_len * sizeof( fts_atom_t));

  fts_stack_init( &this->receive_sb, unsigned char);

  fts_hashtable_init( &this->object_table, FTS_HASHTABLE_INT, FTS_HASHTABLE_MEDIUM);

  fts_set_int( &k, 0);
  fts_set_object( &v, (fts_object_t *)this);
  fts_hashtable_put( &this->object_table, &k, &v);

  fts_set_int( &k, 1);
  fts_set_object( &v, (fts_object_t *)fts_get_root_patcher());
  fts_hashtable_put( &this->object_table, &k, &v);

  fts_sched_add( (fts_object_t *)this, FTS_SCHED_READ, this->socket);
}

static void client_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_t *this = (client_t *)o;

  /* First, delete all the objects that this client has created */
  /* Walk through the hash table using an iterator */

  client_table[ this->client_id - 1] = 0;

  fts_stack_destroy( &this->send_sb);
  fts_free( this->argv);
  fts_stack_destroy( &this->receive_sb);
  fts_hashtable_destroy( &this->object_table);

  fts_sched_remove( (fts_object_t *)this);
  CLOSESOCKET( this->socket);

  post( "[client] closing connection (socket=%d)\n", this->socket);
}

static fts_status_t client_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init( cl, sizeof( client_t), 1, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, client_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, client_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_sched_ready, client_receive);

  /* 0 for now */
  fts_method_define_varargs(cl, 0, fts_new_symbol( "new_object"), client_new_object);
  fts_method_define_varargs(cl, 0, fts_new_symbol( "delete_object"), client_delete_object);

  return fts_Success;
}

/***********************************************************************
 *
 *  client send object
 *
 */
typedef struct {
  fts_object_t head;
  fts_object_t *receive;
} client_send_t;

static void client_send_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_send_t *this = (client_send_t *)o;
  int inlet;

  ac--;
  at++;

  if (!ac || !fts_is_object( &at[0]))
    {
      post( "client_send: Wrong argument\n");

      fts_object_set_error( o, "Wrong argument");
      return;
    }

  inlet = fts_get_int_arg(ac, at, 1, 0);
  fts_connection_new( FTS_NO_ID, fts_get_object( at), inlet, (fts_object_t *)this, 0);

  post( "client_send: new connection on inlet %d\n", inlet);
}
  
static void client_send_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_send_t *this = (client_send_t *)o;

  if (this->receive)
    fts_object_delete_from_patcher( this->receive);
}

static void client_send_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_t *client;

  client = client_table[OBJECT_ID_CLIENT( fts_object_get_id( o))-1];
  if (client != NULL) {
    client_start_message( client);
    client_put_object( client, o);
    client_put_symbol( client, s);
    client_put_atoms( client, ac, at);
    client_end_message( client);
  }
}

static fts_status_t client_send_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(client_send_t), 1, 0, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, client_send_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, client_send_delete);
  fts_method_define_varargs(cl, 0, fts_s_anything, client_send_anything);

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
  fts_class_install( fts_new_symbol("client_send"), client_send_instantiate);

  fts_set_symbol( at, s_client_manager);
  ac++;

  if ((s = fts_cmd_args_get( fts_new_symbol( "listen-port"))))
    {
      fts_set_int( at+1, atoi( fts_symbol_name( s)));
      ac++;
    }

  fts_object_new_to_patcher( fts_get_root_patcher(), ac, at, &obj);
}
