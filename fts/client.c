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

#define HACK_FOR_CRASH_ON_EXIT_WITH_PIPE_CONNECTION

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
#define SOCKET_ERROR_MESSAGE (WSAGetLastError())

typedef unsigned int socklen_t;
typedef SOCKET socket_t;

#else

#include <sys/types.h>
#include <fcntl.h>
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
#define INVALID_PIPE -1
#define SOCKET_ERROR -1
typedef int socket_t;
#define SOCKET_ERROR_MESSAGE (strerror( errno))

#endif

#define client_error(mess)  post("%s (error %d)\n", mess, SOCKET_ERROR_MESSAGE)

#include <string.h>
#include <stdlib.h>

#include <fts/fts.h>
#include "ftsprivate/connection.h"
#include "ftsprivate/patcher.h"
#include "ftsprivate/client.h"
#include "ftsprivate/protocol.h"

/* Forward decls */
typedef struct _client_t client_t;
static fts_symbol_t s_client;
static fts_symbol_t s_client_manager;

/***********************************************************************
 *
 * client table handling
 *
 */

static fts_stack_t client_table;

static int client_table_add( client_t *client)
{
  fts_stack_push( &client_table, client_t *, client);

  return fts_stack_get_top( &client_table) - 1;
}

#define client_table_get(ID) ((client_t **)fts_stack_get_base( &client_table))[(ID)]

static void client_table_remove( int id)
{
  ((client_t **)fts_stack_get_base( &client_table))[id] = 0;
}

static void client_table_init( void)
{
  fts_stack_init( &client_table, client_t *);

  client_table_add( 0); /* so that first client will have id 1 */
}


/***********************************************************************
 *
 * client_manager object
 * (the object that listens for client connections) 
 *
 */

typedef struct {
  fts_object_t head;
  socket_t socket;
  fts_class_t *socketstream_class;
} client_manager_t;


static void client_manager_select( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_manager_t *this = (client_manager_t *)o;
  socket_t new_socket;
  fts_atom_t argv[3];
  fts_object_t *client_object;
  fts_object_t *socket_stream; 

  new_socket = accept( this->socket, NULL, NULL);

  if (new_socket == INVALID_SOCKET)
    {
      client_error( "Cannot accept() connection");
      return;
    }

  fts_set_int( argv, new_socket);
  socket_stream = fts_object_create( this->socketstream_class, 1, argv);

  fts_set_symbol( argv, s_client);
  fts_set_object( argv+1, socket_stream);

  fts_object_new_to_patcher( fts_get_root_patcher(), 3, argv, (fts_object_t **)&client_object);

  if (!client_object)
    {
      fprintf( stderr, "[client_manager] internal error (cannot create client object)\n");
      return;
    }
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

  this->socketstream_class = fts_class_get_by_name( fts_new_symbol("socketstream"));

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
 * client object (the object that is created on new client connections)
 *
 */

typedef void (*transition_action_t)( unsigned char input, void *data);
typedef struct _state_t state_t;

typedef struct _transition_t {
  unsigned char input;
  state_t *target_state;
  transition_action_t action;
  struct _transition_t *next;
} transition_t;

struct _state_t {
  const char *name;
  transition_t *transition;
  transition_t *default_transition;
};

#define SYMBOL_CACHE_SIZE 512

typedef struct {
  int length;
  fts_symbol_t *symbols;
} symbol_cache_t;

/* The protocol decoder state */
typedef struct {
  fts_object_t head;
  /* Automata state */
  state_t *state;
  /* Input decoding */
  int ival;
  fts_stack_t args;
  fts_stack_t buffer;
  /* Symbol caches */
  symbol_cache_t from_client_cache;
  struct _client_t *client;
} protocol_decoder_t;

typedef struct {  
  /* Output buffer */
  fts_stack_t buffer;
  /* Symbol caches */
  symbol_cache_t to_client_cache;
  fts_bytestream_t *stream;
} protocol_encoder_t;

struct _client_t {
  fts_object_t head;
  /* Client id */
  int client_id;
  /* Object table */
  fts_hashtable_t object_table;
  /* Root patcher */
  fts_object_t *root_patcher;
  /* Protocol stream */
  fts_bytestream_t *stream;
  /* Input protocol decoder */
  protocol_decoder_t decoder;
  /* Output protocol encoder */
  protocol_encoder_t encoder;
};

/*----------------------------------------------------------------------
 * Object table
 */
static void client_put_object( client_t *this, int id, fts_object_t *object)
{
  fts_atom_t k, v;

  fts_set_int( &k, id);
  fts_set_object( &v, object);
  fts_hashtable_put( &this->object_table, &k, &v);
}

static fts_object_t *client_get_object( client_t *this, int id)
{
  fts_atom_t k, v;

  fts_set_int( &k, id);

  if ( !fts_hashtable_get( &this->object_table, &k, &v))
    {
      fprintf( stderr, "invalid object in protocol %d\n", id);
      return 0;
    }

  return fts_get_object( &v);
}

/*----------------------------------------------------------------------
 * Symbol cache
 */
static void symbol_cache_init( symbol_cache_t *cache)
{
  cache->length = SYMBOL_CACHE_SIZE;
  cache->symbols = (fts_symbol_t *)fts_malloc( cache->length * sizeof( fts_symbol_t));
}

static void symbol_cache_destroy( symbol_cache_t *cache)
{
  fts_free( cache->symbols);
}

static void symbol_cache_put( symbol_cache_t *cache, fts_symbol_t s, int index)
{
  if (index >= cache->length)
    cache->symbols = fts_realloc( cache->symbols, index+1);

  cache->symbols[index] = s;
}

/*----------------------------------------------------------------------
 * Binary protocol encoder
 */

#define push_char(E,C) fts_stack_push( &(E)->buffer, unsigned char, (C))
#define push_int(E,N) \
	push_char(E, (unsigned char) (((N) >> 24) & 0xff)), \
	push_char(E, (unsigned char) (((N) >> 16) & 0xff)), \
	push_char(E, (unsigned char) (((N) >> 8) & 0xff)), \
	push_char(E, (unsigned char) (((N) >> 0) & 0xff))

static void protocol_encoder_write_int( protocol_encoder_t *encoder, int n)
{
  push_char( encoder, FTS_PROTOCOL_INT);
  push_int( encoder, n);
}

static void protocol_encoder_write_float( protocol_encoder_t *encoder, float value)
{
  float f = value;

  push_char( encoder, FTS_PROTOCOL_FLOAT);
  push_int( encoder, *((unsigned int *)&f));
}

static void protocol_encoder_write_symbol( protocol_encoder_t *encoder, fts_symbol_t s)
{
  int index;
  symbol_cache_t *cache = &encoder->to_client_cache;

  index = (int)s % cache->length;

  if (cache->symbols[index] == s)
    {
      /* just send the index */
      push_char( encoder, FTS_PROTOCOL_SYMBOL_INDEX);
      push_int( encoder, index);
    }
  else 
    {
      const char *p = s;

      cache->symbols[index] = s;

      /* send both the cache index and the symbol */
      push_char( encoder, FTS_PROTOCOL_SYMBOL_CACHE);
      push_int( encoder, index);

      while (*p)
	push_char( encoder, (unsigned char)*p++);

      push_char( encoder, 0);
    }
}

static void protocol_encoder_string( protocol_encoder_t *encoder, const char *s)
{
  push_char( encoder, FTS_PROTOCOL_STRING);

  while (*s)
    push_char( encoder, (unsigned char)*s++);

  push_char( encoder, 0);
}

static void protocol_encoder_write_object( protocol_encoder_t *encoder, fts_object_t *obj)
{
  push_char( encoder, FTS_PROTOCOL_OBJECT);
  push_int( encoder, OBJECT_ID_OBJ( fts_object_get_id( obj)));
}

static void protocol_encoder_write_atoms( protocol_encoder_t *encoder, int ac, const fts_atom_t *at)
{
  while (ac--)
    {
      if ( fts_is_int( at))
	protocol_encoder_write_int( encoder, fts_get_int( at));
      else if ( fts_is_float( at))
	protocol_encoder_write_float( encoder, fts_get_float( at));
      else if ( fts_is_symbol( at))
	protocol_encoder_write_symbol( encoder, fts_get_symbol( at));
      else if ( fts_is_object( at))
	protocol_encoder_write_object( encoder, fts_get_object( at));

      at++;
    }
}

static int protocol_encoder_flush( protocol_encoder_t *encoder)
{
  push_char( encoder, FTS_PROTOCOL_END_OF_MESSAGE);
  
  fts_bytestream_output( encoder->stream, fts_stack_get_top( &encoder->buffer), fts_stack_get_base( &encoder->buffer));

  fts_stack_clear( &encoder->buffer);

  return 0;
}

static void protocol_encoder_init( protocol_encoder_t *encoder, fts_bytestream_t *stream)
{
  fts_stack_init( &encoder->buffer, unsigned char);
  symbol_cache_init( &encoder->to_client_cache);
  encoder->stream = stream;
}

static void protocol_encoder_destroy( protocol_encoder_t *encoder)
{
  fts_stack_destroy( &encoder->buffer);
  symbol_cache_destroy( &encoder->to_client_cache);
}

/*----------------------------------------------------------------------
 * Finite state automata for protocol decoding
 */

static transition_t *transition_new( unsigned char input, state_t *target_state, transition_action_t action, transition_t *next)
{
  transition_t *t = (transition_t *)fts_malloc( sizeof( transition_t));
  t->input = input;
  t->target_state = target_state;
  t->action = action;
  t->next = next;
  return t;
}

static state_t *state_new( const char *name)
{
  state_t *s = (state_t *)fts_malloc( sizeof( state_t));
  s->name = name;
  s->transition = 0;
  s->default_transition = 0;
  return s;
}

static void state_add_transition( state_t *state, unsigned char input, state_t *target_state, transition_action_t action)
{
  state->transition = transition_new( input, target_state, action, state->transition);
}

static void state_add_default_transition( state_t *state, state_t *target_state, transition_action_t action)
{
  state->default_transition = transition_new( 0, target_state, action, 0);
}

static state_t *state_next( state_t *state, unsigned char input, void *data)
{
  transition_t *transition = state->transition;

  while (transition)
    {
      if (transition->input == input)
	{
	  (*transition->action)( input, data);
	  return transition->target_state;
	}

      transition = transition->next;
    }

  if (state->default_transition)
    {
      (*state->default_transition->action)( input, data);
      return state->default_transition->target_state;
    }

  return 0;
}

/*----------------------------------------------------------------------
 * Protocol decoding
 */

/* Actions */

static void clear_action( unsigned char input, void *data)
{
  protocol_decoder_t *decoder = (protocol_decoder_t *)data;

  decoder->ival = 0;
}

static void shift_action( unsigned char input, void *data)
{
  protocol_decoder_t *decoder = (protocol_decoder_t *)data;

  decoder->ival = decoder->ival << 8 | input;
}

static void buffer_clear_action( unsigned char input, void *data)
{
  protocol_decoder_t *decoder = (protocol_decoder_t *)data;

  fts_stack_clear( &decoder->buffer);
}

static void buffer_shift_action( unsigned char input, void *data)
{
  protocol_decoder_t *decoder = (protocol_decoder_t *)data;

  fts_stack_push( &decoder->buffer, unsigned char, input);
}

static void end_int_action( unsigned char input, void *data)
{
  protocol_decoder_t *decoder = (protocol_decoder_t *)data;
  fts_atom_t a;

  decoder->ival = (decoder->ival << 8) | input;
  fts_set_int( &a, decoder->ival);
  fts_stack_push( &decoder->args, fts_atom_t, a);
} 

static void end_float_action( unsigned char input, void *data)
{
  protocol_decoder_t *decoder = (protocol_decoder_t *)data;
  fts_atom_t a;

  decoder->ival = (decoder->ival << 8) | input;
  fts_set_float( &a, *((float *)&decoder->ival));
  fts_stack_push( &decoder->args, fts_atom_t, a);
}

static void end_symbol_index_action( unsigned char input, void *data)
{
  protocol_decoder_t *decoder = (protocol_decoder_t *)data;
  fts_atom_t a;

  decoder->ival = (decoder->ival << 8) | input;
  fts_set_symbol( &a, decoder->from_client_cache.symbols[ decoder->ival]);
  fts_stack_push( &decoder->args, fts_atom_t, a);
}

static void end_symbol_cache_action( unsigned char input, void *data)
{
  protocol_decoder_t *decoder = (protocol_decoder_t *)data;
  fts_symbol_t s = fts_new_symbol_copy( fts_stack_get_base( &decoder->buffer));
  fts_atom_t a;

  symbol_cache_put( &decoder->from_client_cache, s, decoder->ival);
  fts_set_symbol( &a, s);
  fts_stack_push( &decoder->args, fts_atom_t, a);
}

static void end_string_action( unsigned char input, void *data)
{
  protocol_decoder_t *decoder = (protocol_decoder_t *)data;
  fts_atom_t a;

  fts_stack_push( &decoder->buffer, unsigned char, '\0');
  fts_set_symbol( &a, fts_new_symbol_copy( fts_stack_get_base( &decoder->buffer)));
  fts_stack_push( &decoder->args, fts_atom_t, a);
}

static void end_object_action( unsigned char input, void *data)
{
  protocol_decoder_t *decoder = (protocol_decoder_t *)data;
  fts_object_t *obj = 0;
  fts_atom_t v;

  decoder->ival = (decoder->ival << 8) | input;
  obj = client_get_object( decoder->client, decoder->ival);
  fts_set_object( &v, obj);
  fts_stack_push( &decoder->args, fts_atom_t, v);
}

static void end_message_action( unsigned char input, void *data)
{
  protocol_decoder_t *decoder = (protocol_decoder_t *)data;
  fts_object_t *target;
  fts_symbol_t selector;
  int argc;
  fts_atom_t *argv;

  argc = fts_stack_get_top( &decoder->args);
  argv = (fts_atom_t *)fts_stack_get_base( &decoder->args);
  target = fts_get_object( argv);
  selector = fts_get_symbol( argv+1);
  argc -= 2;
  argv += 2;

  fts_log( "[client]: Received message dest=0x%x selector=%s args=", target, selector);
  fts_log_atoms( argc, argv);
  fts_log( "\n");

  /* Client messages are sent to the system inlet */
  if (target)
    fts_send_message( target, fts_SystemInlet, selector, argc, argv);

  fts_stack_clear( &decoder->args);
}

static void protocol_error_action( unsigned char input, void *data)
{
  protocol_decoder_t *decoder = (protocol_decoder_t *)data;

  fprintf( stderr, "Protocol error: state %s incoming %d\n", decoder->state->name, input);
}

static state_t *build_state_machine( void)
{
  static state_t *state_machine_single_instance = 0;

  state_t *q_initial;

  state_t *q_int0;
  state_t *q_int1;
  state_t *q_int2;
  state_t *q_int3;

  state_t *q_float0;
  state_t *q_float1;
  state_t *q_float2;
  state_t *q_float3;

  state_t *q_string;

  state_t *q_object0;
  state_t *q_object1;
  state_t *q_object2;
  state_t *q_object3;

  state_t *q_symbol_index0;
  state_t *q_symbol_index1;
  state_t *q_symbol_index2;
  state_t *q_symbol_index3;

  state_t *q_symbol_cache0;
  state_t *q_symbol_cache1;
  state_t *q_symbol_cache2;
  state_t *q_symbol_cache3;
  state_t *q_symbol_cache4;

  if (state_machine_single_instance)
    return state_machine_single_instance;

  q_initial = state_new( "Initial");

  q_int0 = state_new( "Int0");
  q_int1 = state_new( "Int1");
  q_int2 = state_new( "Int2");
  q_int3 = state_new( "Int3");

  q_float0 = state_new( "Float0");
  q_float1 = state_new( "Float1");
  q_float2 = state_new( "Float2");
  q_float3 = state_new( "Float3");

  q_string = state_new( "String");

  q_object0 = state_new( "Object0");
  q_object1 = state_new( "Object1");
  q_object2 = state_new( "Object2");
  q_object3 = state_new( "Object3");

  q_symbol_index0 = state_new( "SymbolIndex0");
  q_symbol_index1 = state_new( "SymbolIndex1");
  q_symbol_index2 = state_new( "SymbolIndex2");
  q_symbol_index3 = state_new( "SymbolIndex3");

  q_symbol_cache0 = state_new( "SymbolCache0");
  q_symbol_cache1 = state_new( "SymbolCache1");
  q_symbol_cache2 = state_new( "SymbolCache2");
  q_symbol_cache3 = state_new( "SymbolCache3");
  q_symbol_cache4 = state_new( "SymbolCache4");

  state_add_transition( q_initial, FTS_PROTOCOL_INT, q_int0, clear_action);
  state_add_transition( q_initial, FTS_PROTOCOL_FLOAT, q_float0, clear_action);
  state_add_transition( q_initial, FTS_PROTOCOL_SYMBOL_INDEX, q_symbol_index0, clear_action);
  state_add_transition( q_initial, FTS_PROTOCOL_SYMBOL_CACHE, q_symbol_cache0, clear_action);
  state_add_transition( q_initial, FTS_PROTOCOL_STRING, q_string, buffer_clear_action);
  state_add_transition( q_initial, FTS_PROTOCOL_OBJECT, q_object0, clear_action);
  state_add_transition( q_initial, FTS_PROTOCOL_END_OF_MESSAGE, q_initial, end_message_action);

  state_add_default_transition( q_int0, q_int1, shift_action);
  state_add_default_transition( q_int1, q_int2, shift_action);
  state_add_default_transition( q_int2, q_int3, shift_action);
  state_add_default_transition( q_int3, q_initial, end_int_action);

  state_add_default_transition( q_float0, q_float1, shift_action);
  state_add_default_transition( q_float1, q_float2, shift_action);
  state_add_default_transition( q_float2, q_float3, shift_action);
  state_add_default_transition( q_float3, q_initial, end_float_action);

  state_add_default_transition( q_symbol_index0, q_symbol_index1, shift_action);
  state_add_default_transition( q_symbol_index1, q_symbol_index2, shift_action);
  state_add_default_transition( q_symbol_index2, q_symbol_index3, shift_action);
  state_add_default_transition( q_symbol_index3, q_initial, end_symbol_index_action);

  state_add_default_transition( q_symbol_cache0, q_symbol_cache1, shift_action);
  state_add_default_transition( q_symbol_cache1, q_symbol_cache2, shift_action);
  state_add_default_transition( q_symbol_cache2, q_symbol_cache3, shift_action);
  state_add_default_transition( q_symbol_cache3, q_symbol_cache4, shift_action);
  state_add_transition( q_symbol_cache4, 0, q_initial, end_symbol_cache_action);
  state_add_default_transition( q_symbol_cache4, q_symbol_cache4, buffer_shift_action);

  state_add_transition( q_string, 0, q_initial, end_string_action);
  state_add_default_transition( q_string, q_string, buffer_shift_action);

  state_add_default_transition( q_object0, q_object1, shift_action);
  state_add_default_transition( q_object1, q_object2, shift_action);
  state_add_default_transition( q_object2, q_object3, shift_action);
  state_add_default_transition( q_object3, q_initial, end_object_action);

  state_machine_single_instance = q_initial;

  return state_machine_single_instance;
}

static void protocol_decoder_run(  protocol_decoder_t *decoder, int size, const unsigned char *buffer)
{
  int i;

  for ( i = 0; i < size; i++)
    {
      decoder->state = state_next( decoder->state, buffer[i], decoder);
      if (decoder->state == 0)
	fts_log( "[client] protocol error\n");
    }
}

static void protocol_decoder_init( protocol_decoder_t *decoder, client_t *client)
{
  decoder->state = 0;
  decoder->ival = 0;

  fts_stack_init( &decoder->args, fts_atom_t);
  fts_stack_init( &decoder->buffer, unsigned char);

  decoder->client = client;
  decoder->state = build_state_machine();
}

static void protocol_decoder_destroy( protocol_decoder_t *decoder)
{
  fts_stack_destroy( &decoder->args);
  fts_stack_destroy( &decoder->buffer);
}

/*----------------------------------------------------------------------
 * Client object methods
 */

/* Bytestream listener */
static void client_receive( fts_object_t *o, int size, const unsigned char* buffer)
{
  client_t *this = (client_t *)o;

  if ( size < 0)
    {
      client_error( "[client] error in reading message, client stopped");
      fts_log( "[client] error in reading message, client stopped\n");
      fts_object_delete_from_patcher( (fts_object_t *)this);
      return;
    }
  else if (size == 0)
    {
      fts_log( "[client] client stopped\n");
      fts_object_delete_from_patcher( (fts_object_t *)this);
      return;
    }

  protocol_decoder_run( &this->decoder, size, buffer);
}

static void client_new_object( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_t *this = (client_t *)o;
  int id;
  fts_object_t *parent;
  fts_object_t *newobj;

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

  client_put_object( this, id, newobj);

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

  this->stream = (fts_bytestream_t *)fts_get_object_arg( ac, at, 0, NULL);

  if (this->stream == NULL || !fts_bytestream_check( (fts_object_t *)this->stream) || !fts_bytestream_is_input( this->stream) || !fts_bytestream_is_output( this->stream))
    {
      fts_object_set_error( (fts_object_t *)this, "Invalid stream");
      return;
    }

  this->client_id = client_table_add( this);

  fts_hashtable_init( &this->object_table, FTS_HASHTABLE_INT, FTS_HASHTABLE_MEDIUM);

#ifdef HACK_FOR_CRASH_ON_EXIT_WITH_PIPE_CONNECTION
  fts_set_int( &k, 0);
  fts_set_object( &v, (fts_object_t *)fts_get_root_patcher());
  fts_hashtable_put( &this->object_table, &k, &v);
#else
  fts_set_symbol( a, fts_s_patcher);
  fts_object_new_to_patcher( fts_get_root_patcher(), 1, a, &this->root_patcher);

  if ( !this->root_patcher)
    {
      fts_object_set_error( (fts_object_t *)this, "Cannot create client root patcher");
      return;
    }

  fts_object_refer( this->root_patcher);

  fts_set_int( &k, 0);
  fts_set_object( &v, this->root_patcher);
  fts_hashtable_put( &this->object_table, &k, &v);
#endif

  fts_set_int( &k, 1);
  fts_set_object( &v, (fts_object_t *)this);
  fts_hashtable_put( &this->object_table, &k, &v);

  protocol_encoder_init( &this->encoder, this->stream);
  protocol_decoder_init( &this->decoder, this);

  fts_bytestream_add_listener(this->stream, (fts_object_t *) this, client_receive);

  fts_log( "[client]: Accepted client connection on socket\n");
}

static void client_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_t *this = (client_t *)o;

#ifndef HACK_FOR_CRASH_ON_EXIT_WITH_PIPE_CONNECTION
  fts_object_release( this->root_patcher);
/*    fts_object_delete_from_patcher( this->root_patcher); */
#endif

  client_table_remove( this->client_id);

  fts_hashtable_destroy( &this->object_table);

  fts_bytestream_remove_listener(this->stream, (fts_object_t *) this);

  protocol_encoder_destroy( &this->encoder);
  protocol_decoder_destroy( &this->decoder);

  fts_log( "[client]: Released client connection on socket\n");
}

static fts_status_t client_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init( cl, sizeof( client_t), 1, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, client_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, client_delete);

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
  fts_symbol_t name;
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

  name = fts_get_symbol( at);

  v = fts_variable_get_value( fts_object_get_patcher(o), name);
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

  fts_variable_add_user( fts_object_get_patcher( (fts_object_t *)this), name, (fts_object_t *)this);

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

  fts_log( "[client]: Created controller on %s %s channel %d\n", target_class_name, fts_object_get_variable( target), channel_number);
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

  fts_log( "[client]: Sending \"%s ", s);
  fts_log_atoms( ac, at);
  fts_log( "\"\n");

  fts_client_send_message( o, s, ac, at);
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
 * Client message sending
 *
 */
void fts_client_send_message( fts_object_t *o, fts_symbol_t selector, int ac, const fts_atom_t *at)
{
  client_t *client = client_table_get( OBJECT_ID_CLIENT( fts_object_get_id( o)) );

  if (!client)
    return;

  protocol_encoder_write_object( &client->encoder, o);
  protocol_encoder_write_symbol( &client->encoder, selector);
  protocol_encoder_write_atoms( &client->encoder, ac, at);
  protocol_encoder_flush( &client->encoder);
}



/***********************************************************************
 *
 * Initialization
 *
 */
static void client_tcp_manager_install( void)
{
  int argc = 0;
  fts_atom_t argv[2];
  fts_object_t *client_manager_object;
  fts_symbol_t s;

  fts_set_symbol( argv, s_client_manager);
  argc++;
    
  if ((s = fts_cmd_args_get( fts_new_symbol( "listen-port"))))
    {
      fts_set_int( argv+1, atoi( s));
      argc++;
    }
    
  fts_object_new_to_patcher( fts_get_root_patcher(), argc, argv, &client_manager_object);
  
  if ( !client_manager_object)
    fprintf( stderr, "[client] cannot create client manager\n");
}

static void client_pipe_install( void)
{
  fts_atom_t argv[3];
  fts_object_t *client_object;
  fts_object_t *pipe_stream; 

  pipe_stream = fts_object_create(fts_class_get_by_name( fts_new_symbol("pipestream")), 0, 0);
  
  fts_set_symbol( argv, s_client);
  fts_set_object( argv+1, pipe_stream);
  fts_set_int( argv+2, 1);
  
  fts_object_new_to_patcher( fts_get_root_patcher(), 3, argv, (fts_object_t **)&client_object);
  
  if (!client_object)
    {
      fprintf( stderr, "[client_manager] internal error (cannot create client object)\n");
      return;
    }
}

void fts_client_config( void)
{
  client_table_init();

  s_client_manager = fts_new_symbol("client_manager");
  fts_class_install( s_client_manager, client_manager_instantiate);

  s_client = fts_new_symbol("client");
  fts_class_install( s_client, client_instantiate);

  fts_class_install( fts_new_symbol("client_controller"), client_controller_instantiate);

  /* check whether we should use a piped connection thru the stdio file handles */
  if ( fts_cmd_args_get( fts_new_symbol( "stdio")) != NULL ) 
    client_pipe_install();
  else
    client_tcp_manager_install();
}

