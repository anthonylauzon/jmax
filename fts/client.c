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
/* Define this if you want logs of symbol cache hit */
#define CACHE_REPORT
#define CLIENT_LOG

#include <fts/fts.h>
#include <ftsconfig.h>
#include <ftsprivate/loader.h>
#include <ftsprivate/patparser.h>
#include <ftsprivate/package.h>
#include <ftsprivate/tokenizer.h>

#include <string.h>
#include <stdlib.h>

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
#if HAVE_ALLOCA_H
#include <alloca.h>
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

#include "ftsprivate/connection.h"
#include "ftsprivate/patcher.h"
#include "ftsprivate/client.h"
#include "ftsprivate/protocol.h"

/* Forward decls */
typedef struct _client_t client_t;

static fts_symbol_t s_client;
static fts_symbol_t s_client_manager;
static fts_symbol_t s_package_loaded;
static fts_symbol_t s_remove_object;
static fts_symbol_t s_show_message;


/* Predefined ids */
#define FTS_CLIENT_ROOT_OBJECT_ID 0
#define FTS_CLIENT_CLIENT_OBJECT_ID 1


/***********************************************************************
 *
 * client table handling
 *
 */

static fts_stack_t client_table;

static int client_table_add( client_t *client)
{
  fts_stack_push( &client_table, client_t *, client);

  return fts_stack_get_top( &client_table);
}

static void client_table_remove( int id)
{
  ((client_t **)fts_stack_get_base( &client_table))[id] = 0;
}

static void client_table_init( void)
{
  fts_stack_init( &client_table, client_t *);

  client_table_add( 0); /* so that first client will have id 1 */
}

#define client_table_get(I) ((client_t **)fts_stack_get_base( &client_table))[(I)]

client_t *object_get_client( fts_object_t *obj)
{
  int id, index;

  id = fts_object_get_id( obj);

  if (id == FTS_NO_ID)
    return NULL;

  index = OBJECT_ID_CLIENT( id );

  if (index < 0 || index >= fts_stack_get_size( &client_table))
    return NULL;

  return client_table_get(index);
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
  socket_stream = fts_object_create( fts_socketstream_type, 1, argv);

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

#define SYMBOL_CACHE_SIZE 512

typedef struct {
  int length;
  fts_symbol_t *symbols;
#ifdef CACHE_REPORT
  int naccess, nhit;
#endif
} symbol_cache_t;

struct _client_t {
  fts_object_t head;
  /* Client id */
  int client_id;
  /* Object table */
  fts_hashtable_t object_table;
  /* object id count */
  int object_id_count;
  /* Root patcher */
  fts_object_t *root_patcher;
  /* Protocol stream */
  fts_bytestream_t *stream;

  /* Input protocol decoder */
  /* Automata state */
  int state;
  /* Input decoding */
  int input_value;
  fts_stack_t input_args;
  fts_stack_t input_buffer;
  /* Symbol caches */
  symbol_cache_t input_cache;

  /* Output protocol encoder */
  /* Output buffer */
  fts_stack_t output_buffer;
  /* Symbol caches */
  symbol_cache_t output_cache;
};

/*----------------------------------------------------------------------
 * Object table
 */

static fts_object_t *client_get_object( client_t *this, int id)
{
  fts_atom_t k, v;

  fts_set_int( &k, id);

  if ( !fts_hashtable_get( &this->object_table, &k, &v))
    return NULL;

  return fts_get_object( &v);
}

static void client_release_object( client_t *this, fts_object_t *object)
{
  fts_atom_t k;

  fts_set_int( &k, fts_get_object_id( object));
  fts_hashtable_remove( &this->object_table, &k);

  object->head.id = FTS_NO_ID;
}

static void client_register_object( client_t *this, fts_object_t *object, int object_id)
{
  fts_atom_t k, v;

  if (object_id == FTS_NO_ID)
    {
      object_id = this->object_id_count;
      this->object_id_count += 2; 
    }

  fts_set_int( &k, object_id);
  fts_set_object( &v, object);
  fts_hashtable_put( &this->object_table, &k, &v);

  object->head.id = OBJECT_ID( object_id, this->client_id);
}

/*----------------------------------------------------------------------
 * Symbol cache
 */
static void symbol_cache_init( symbol_cache_t *cache)
{
  int i;

  cache->length = SYMBOL_CACHE_SIZE;
  cache->symbols = (fts_symbol_t *)fts_malloc( cache->length * sizeof( fts_symbol_t));

  for ( i = 0; i < cache->length; i++)
    cache->symbols[i] = 0;
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
 * Finite state automata for protocol decoding
 */

/* Actions */

static void clear_action( unsigned char input, client_t *client)
{
  client->input_value = 0;
  fts_stack_clear( &client->input_buffer);
}

static void shift_action( unsigned char input, client_t *client)
{
  client->input_value = client->input_value << 8 | input;
}

static void buffer_shift_action( unsigned char input, client_t *client)
{
  fts_stack_push( &client->input_buffer, unsigned char, input);
}

static void end_int_action( unsigned char input, client_t *client)
{
  fts_atom_t a;

  client->input_value = (client->input_value << 8) | input;
  fts_set_int( &a, client->input_value);
  fts_stack_push( &client->input_args, fts_atom_t, a);
} 

static void end_float_action( unsigned char input, client_t *client)
{
  fts_atom_t a;

  client->input_value = (client->input_value << 8) | input;
  fts_set_float( &a, *((float *)&client->input_value));
  fts_stack_push( &client->input_args, fts_atom_t, a);
}

static void end_symbol_index_action( unsigned char input, client_t *client)
{
  fts_atom_t a;

  client->input_value = (client->input_value << 8) | input;
  fts_set_symbol( &a, client->input_cache.symbols[ client->input_value]);
  fts_stack_push( &client->input_args, fts_atom_t, a);
}

static void end_symbol_cache_action( unsigned char input, client_t *client)
{
  fts_symbol_t s;
  fts_atom_t a;

  fts_stack_push( &client->input_buffer, unsigned char, '\0');
  s = fts_new_symbol_copy( fts_stack_get_base( &client->input_buffer));
  symbol_cache_put( &client->input_cache, s, client->input_value);
  fts_set_symbol( &a, s);
  fts_stack_push( &client->input_args, fts_atom_t, a);
}

static void end_string_action( unsigned char input, client_t *client)
{
  fts_atom_t a;

  fts_stack_push( &client->input_buffer, unsigned char, '\0');
  fts_set_string( &a, strdup( fts_stack_get_base( &client->input_buffer)));
  fts_stack_push( &client->input_args, fts_atom_t, a);
}

static void end_raw_string_action( unsigned char input, client_t *client)
{
  fts_atom_t a;
  fts_tokenizer_t tokenizer;
  char *p;
  int l;

  fts_stack_push( &client->input_buffer, unsigned char, '\0');
  fts_stack_push( &client->input_buffer, unsigned char, '\0');

  p = fts_stack_get_base( &client->input_buffer);
  l = fts_stack_get_size( &client->input_buffer);
  fts_tokenizer_init_buffer( &tokenizer, p, l);

  while ( fts_tokenizer_next( &tokenizer, &a) != 0)
    fts_stack_push( &client->input_args, fts_atom_t, a);

  fts_tokenizer_destroy( &tokenizer);
}

static void end_object_action( unsigned char input, client_t *client)
{
  fts_object_t *obj = 0;
  fts_atom_t v;

  client->input_value = (client->input_value << 8) | input;
  obj = client_get_object( client, client->input_value);

  if (obj == NULL)
    {
      fts_log( "[client] invalid object id: %d\n", client->input_value);
      fts_set_void( &v);
    }
  else
    fts_set_object( &v, obj);

  fts_stack_push( &client->input_args, fts_atom_t, v);
}

static void end_message_action( unsigned char input, client_t *client)
{
  fts_object_t *target;
  fts_symbol_t selector;
  int argc;
  fts_atom_t *argv;

  argc = fts_stack_get_size( &client->input_args);
  argv = (fts_atom_t *)fts_stack_get_base( &client->input_args);

  selector = fts_get_symbol( argv+1);

  if ( fts_is_void( argv))
    {
      fts_log( "[client] message %s to null object\n", selector);
      goto skipped;
    }
    
  target = fts_get_object( argv);
  argc -= 2;
  argv += 2;

#ifdef CLIENT_LOG
  fts_log( "[client]: Received message dest=0x%x selector=%s args=", target, selector);
  fts_log_atoms( argc, argv);
  fts_log( "\n");
#endif

  /* Client messages are sent to the system inlet */
  if (target)
    fts_send_message( target, fts_SystemInlet, selector, argc, argv);

 skipped:
  fts_stack_clear( &client->input_args);
}

static void state_next( client_t *client, unsigned char input)
{
#define q_initial            1
#define q_int0               2
#define q_int1               3
#define q_int2               4
#define q_int3               5
#define q_float0             6
#define q_float1             7
#define q_float2             8
#define q_float3   	     9
#define q_string             10
#define q_object0            11
#define q_object1            12
#define q_object2            13
#define q_object3            14
#define q_symbol_index0      15
#define q_symbol_index1      16
#define q_symbol_index2      17
#define q_symbol_index3      18
#define q_symbol_cache0      19
#define q_symbol_cache1      20
#define q_symbol_cache2      21
#define q_symbol_cache3      22
#define q_symbol_cache4      23
#define q_raw_string         24

#define moveto( NEWSTATE, ACTION) (client->state = NEWSTATE, ACTION( input, client))

  switch( client->state) {
  case 0:
    /* try to skip till end of message */
    if ( input == FTS_PROTOCOL_END_OF_MESSAGE)
      client->state = q_initial;
    break;
  case q_initial:
    if ( input == FTS_PROTOCOL_INT)
      moveto( q_int0, clear_action);
    else if ( input == FTS_PROTOCOL_FLOAT)
      moveto( q_float0, clear_action);
    else if ( input == FTS_PROTOCOL_SYMBOL_INDEX)
      moveto( q_symbol_index0, clear_action);
    else if ( input == FTS_PROTOCOL_SYMBOL_CACHE)
      moveto( q_symbol_cache0, clear_action);
    else if ( input == FTS_PROTOCOL_STRING)
      moveto( q_string, clear_action);
    else if ( input == FTS_PROTOCOL_RAW_STRING)
      moveto( q_raw_string, clear_action);
    else if ( input == FTS_PROTOCOL_OBJECT)
      moveto( q_object0, clear_action);
    else if ( input == FTS_PROTOCOL_END_OF_MESSAGE)
      moveto( q_initial, end_message_action);
    else
      client->state = 0;
    break;
  case q_int0:
    moveto( q_int1, shift_action);
    break;
  case q_int1:
    moveto( q_int2, shift_action);
    break;
  case q_int2:
    moveto( q_int3, shift_action);
    break;
  case q_int3:
    moveto( q_initial, end_int_action);
    break;
  case q_float0:
    moveto( q_float1, shift_action);
    break;
  case q_float1:
    moveto( q_float2, shift_action);
    break;
  case q_float2:
    moveto( q_float3, shift_action);
    break;
  case q_float3:
    moveto( q_initial, end_float_action);
    break;
  case q_symbol_index0:
    moveto( q_symbol_index1, shift_action);
    break;
  case q_symbol_index1:
    moveto( q_symbol_index2, shift_action);
    break;
  case q_symbol_index2:
    moveto( q_symbol_index3, shift_action);
    break;
  case q_symbol_index3:
    moveto( q_initial, end_symbol_index_action);
    break;
  case q_symbol_cache0:
    moveto( q_symbol_cache1, shift_action);
    break;
  case q_symbol_cache1:
    moveto( q_symbol_cache2, shift_action);
    break;
  case q_symbol_cache2:
    moveto( q_symbol_cache3, shift_action);
    break;
  case q_symbol_cache3:
    moveto( q_symbol_cache4, shift_action);
    break;
  case q_symbol_cache4:
    if ( input == 0)
      moveto( q_initial, end_symbol_cache_action);
    else
      moveto( q_symbol_cache4, buffer_shift_action);
    break;
  case q_string:
    if ( input == 0)
      moveto( q_initial, end_string_action);
    else
      moveto( q_string, buffer_shift_action);
    break;
  case q_raw_string:
    if ( input == 0)
      moveto( q_initial, end_raw_string_action);
    else
      moveto( q_raw_string, buffer_shift_action);
    break;
  case q_object0:
    moveto( q_object1, shift_action);
    break;
  case q_object1:
    moveto( q_object2, shift_action);
    break;
  case q_object2:
    moveto( q_object3, shift_action);
    break;
  case q_object3:
    moveto( q_initial, end_object_action);
    break;
  }
}

/*----------------------------------------------------------------------
 * Client object methods
 */

/* Bytestream listener */
static void client_receive( fts_object_t *o, int size, const unsigned char* buffer)
{
  client_t *this = (client_t *)o;
  int i;

  if ( size <= 0)
    {
      client_error( "[client] error in reading message, client stopped");
      fts_log( "[client] error in reading message, client stopped\n");
      fts_bytestream_remove_listener(this->stream, (fts_object_t *) this);
      fts_object_delete_from_patcher( (fts_object_t *)this);
      return;
    }


  for ( i = 0; i < size; i++)
    {
      state_next( this, buffer[i]);
      if (this->state == 0)
	fts_log( "[client] protocol error\n");
    }
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

  client_register_object( this, newobj, id);
}

static void client_set_object_property( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if ((ac == 3) &&
      fts_is_object(&at[0]) &&
      fts_is_symbol(&at[1]))
    {
      fts_object_t *obj;
      fts_symbol_t name;

      obj  = fts_get_object(&at[0]);
      name = fts_get_symbol(&at[1]);

      fts_object_put_prop(obj, name, &at[2]);

      fts_patcher_set_dirty( obj->patcher, 1);
    }
  else
    fts_log("[client]: System Error set_object_property: bad args\n");
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

static void client_load_patcher_file( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_t *this = (client_t *)o;
  fts_client_load_patcher( fts_get_symbol( at), this->client_id);
}

fts_patcher_t *
fts_client_load_patcher(fts_symbol_t file_name, int client_id)
{
  fts_patcher_t *patcher = 0;
  fts_package_t *project;
  int type = 1;
  fts_atom_t a[3];
  client_t *client;
  char *dir_name;
  fts_object_t *parent = (fts_object_t *)fts_get_root_patcher();

  client = client_table_get(client_id);

  fts_log("[client]: Load patcher %s\n", file_name);

  /*
    FIXME
    Hack to get the templates that are in the same directory
    as the patch we are opening.
  */
  dir_name = (char *)alloca( strlen( file_name) + 1);
  strcpy( dir_name, file_name);
  fts_dirname( dir_name);
  fts_package_add_template_path( fts_project_get(), fts_new_symbol_copy( dir_name));
  fts_package_add_abstraction_path( fts_project_get(), fts_new_symbol_copy( dir_name));
  fts_package_add_data_path( fts_project_get(), fts_new_symbol_copy( dir_name));

  /*
    We change the working directory so that it is the directory of the
    opened patch.
  */
  chdir( dir_name);


  /* here finds the file-type if is a jmax_file do 
     fts_binary_file_load( filename, parent, 0, 0)
     else if is a dot_pat file do 
     fts_load_dotpat_patcher(parent, filename)
  */
  
  type = ! fts_is_dotpat_file( file_name);

  if( type)
    patcher = (fts_patcher_t *)fts_binary_file_load( file_name, parent, 0, 0, 0);
  else
    patcher = (fts_patcher_t *)fts_load_dotpat_patcher( parent, file_name);

  if (patcher == 0)
    {
      fts_log("[patcher]: Cannot read file %s\n", file_name);
      return 0;
    }

  client_register_object( client, (fts_object_t *)patcher, FTS_NO_ID);

  /* Save the file name, for future autosaves and other services */
  fts_patcher_set_file_name(patcher, file_name);

  /* activate the post-load init, like loadbangs */   
  fts_send_message( (fts_object_t *)patcher, fts_SystemInlet, fts_new_symbol("load_init"), 0, 0);

  fts_set_int(a, fts_get_object_id((fts_object_t *)patcher));
  fts_set_symbol(a+1, file_name);
  fts_set_int(a+2, type);
  fts_client_send_message( (fts_object_t *)client, fts_new_symbol( "patcher_loaded"), 3, a);

  /* upload the patcher to the client */
  fts_send_message( (fts_object_t *)patcher, fts_SystemInlet, fts_s_upload, 0, 0);
  fts_send_message( (fts_object_t *)patcher, fts_SystemInlet, fts_new_symbol("openEditor"), 0, 0);

  fts_log("[patcher]: Finished loading patcher %s\n", file_name);

  return patcher;
}

static void client_load_project( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_t *this = (client_t *)o;
  fts_package_t* project;
  fts_atom_t a[1];
  char message[1024];

  fts_symbol_t project_file = fts_get_symbol( at);
  
  project = fts_project_open(project_file);

  if( project->state == fts_package_corrupt) 
    {
      sprintf(message, "Invalid project file: \n%s\n", project_file);
      fts_set_symbol( a, message);
      fts_client_send_message( o, s_show_message, 1, a);
    }  
  else
    {
      client_register_object( this, (fts_object_t *)project, FTS_NO_ID);

      fts_set_int(a, fts_get_object_id( (fts_object_t *)project));
      fts_client_send_message(o, fts_s_project, 1, a);
      
      fts_send_message( (fts_object_t *)project, fts_SystemInlet, fts_s_upload, 0, 0);
      fts_send_message( (fts_object_t *)project, fts_SystemInlet, fts_s_openEditor, 0, 0);
    }
}

static void client_shutdown( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_sched_halt();
}

static void client_predefine_objects( client_t *this)
{
  fts_atom_t k, v;
  fts_atom_t a[1];

#ifdef HACK_FOR_CRASH_ON_EXIT_WITH_PIPE_CONNECTION
  client_register_object( this, (fts_object_t *)fts_get_root_patcher(), FTS_CLIENT_ROOT_OBJECT_ID);
#else
  fts_set_symbol( a, fts_s_patcher);
  fts_object_new_to_patcher( fts_get_root_patcher(), 1, a, &this->root_patcher);

  if ( !this->root_patcher)
    {
      fts_object_set_error( (fts_object_t *)this, "Cannot create client root patcher");
      return;
    }

  fts_object_refer( this->root_patcher);

  client_register_object( this, (fts_object_t *)fts_get_root_patcher(), FTS_CLIENT_ROOT_OBJECT_ID);
#endif

  client_register_object( this, (fts_object_t *)this, FTS_CLIENT_CLIENT_OBJECT_ID);
}

static void client_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_t *this = (client_t *)o;

  ac--;
  at++;

  this->stream = (fts_bytestream_t *)fts_get_object_arg( ac, at, 0, NULL);

  if ( this->stream == NULL 
       || !fts_bytestream_check( (fts_object_t *)this->stream) 
       || !fts_bytestream_is_input( this->stream) 
       || !fts_bytestream_is_output( this->stream))
    {
      fts_object_set_error( (fts_object_t *)this, "Invalid stream");
      return;
    }

  this->client_id = client_table_add( this);
  this->object_id_count = 17;

  /* Set my client id */
  this->head.head.id = OBJECT_ID( 1, this->client_id);

  /* output protocol encoder */
  fts_stack_init( &this->output_buffer, unsigned char);
  symbol_cache_init( &this->output_cache);

  /* input protocol decoder */
  this->state = q_initial;
  this->input_value = 0;
  fts_stack_init( &this->input_args, fts_atom_t);
  fts_stack_init( &this->input_buffer, unsigned char);
  symbol_cache_init( &this->input_cache);

  fts_bytestream_add_listener( this->stream, (fts_object_t *) this, client_receive);

  fts_hashtable_init( &this->object_table, FTS_HASHTABLE_INT, FTS_HASHTABLE_MEDIUM);

  client_predefine_objects( this);

  fts_log( "[client]: Accepted client connection\n");
}

static void client_get_project( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t a[1];
  fts_object_t *project = (fts_object_t *)fts_project_get();

  if (!fts_object_has_id( project))
    client_register_object( (client_t *)o, project, FTS_NO_ID);

  fts_set_int(a, fts_get_object_id( project));
  fts_client_send_message(o, fts_s_project, 1, a);
  
  fts_send_message( project, fts_SystemInlet, fts_s_upload, 0, 0);
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

  /* output protocol encoder */
  fts_stack_destroy( &this->output_buffer);
  symbol_cache_destroy( &this->output_cache);

  /* input protocol decoder */
  fts_stack_destroy( &this->input_args);
  fts_stack_destroy( &this->input_buffer);

  fts_log( "[client]: Released client connection on socket\n");
}

static fts_status_t client_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init( cl, sizeof( client_t), 1, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, client_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, client_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol( "get_project"), client_get_project);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol( "new_object"), client_new_object);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol( "set_object_property"), client_set_object_property);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol( "connect_object"), client_connect_object);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol( "delete_object"), client_delete_object);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol( "load"), client_load_patcher_file);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol( "load_project"), client_load_project);

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
      fts_set_object( a+1, target);
      fts_set_int( a+2, channel_number);
      fts_object_new_to_patcher( fts_object_get_patcher( (fts_object_t *)this), 3, a, &from);

      fts_set_symbol( a, fts_new_symbol( "throw"));
      fts_object_new_to_patcher( fts_object_get_patcher( (fts_object_t *)this), 3, a, &to);
    }
  else
    {
      fts_atom_t a[2];

      fts_set_symbol( a, fts_new_symbol( "inlet"));
      fts_set_object( a+1, target);
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

#ifdef CLIENT_LOG
  fts_log( "[client]: Sending \"%s ", s);
  fts_log_atoms( ac, at);
  fts_log( "\"\n");
#endif

  fts_client_send_message( o, s, ac, at);
}

static void client_controller_anything_client(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  client_controller_t *this = (client_controller_t *)o;

  this->gate = 1;

#ifdef CLIENT_LOG
  fts_log( "[client]: Received \"");
  fts_log_atoms( ac, at);
  fts_log( "\"\n");
#endif

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

#define write_char(C,N) fts_stack_push( &(C)->output_buffer, unsigned char, (N))
#define write_int(C,N) \
	write_char(C, (unsigned char) (((N) >> 24) & 0xff)), \
	write_char(C, (unsigned char) (((N) >> 16) & 0xff)), \
	write_char(C, (unsigned char) (((N) >> 8) & 0xff)), \
	write_char(C, (unsigned char) (((N) >> 0) & 0xff))

static void client_write_int( client_t *client, int v)
{
  write_char( client, FTS_PROTOCOL_INT);
  write_int( client, v);
}

static void client_write_float( client_t *client, float v)
{
  float f = v;

  write_char( client, FTS_PROTOCOL_FLOAT);
  write_int( client, *((unsigned int *)&f));
}

static void client_write_symbol( client_t *client, fts_symbol_t s)
{
  unsigned int index;
  symbol_cache_t *cache = &client->output_cache;

#ifdef CACHE_REPORT
  cache->naccess++;
#endif

  index = (unsigned int)s % cache->length;

  if (cache->symbols[index] == s)
    {
#ifdef CACHE_REPORT
      cache->nhit++;
#endif

      /* just send the index */
      write_char( client, FTS_PROTOCOL_SYMBOL_INDEX);
      write_int( client, index);
    }
  else 
    {
      const char *p = s;

      cache->symbols[index] = s;

      /* send both the cache index and the symbol */
      write_char( client, FTS_PROTOCOL_SYMBOL_CACHE);
      write_int( client, index);

      while (*p)
	write_char( client, (unsigned char)*p++);

      write_char( client, 0);
    }

#ifdef CACHE_REPORT
  if (cache->naccess % 1024 == 0)
    {
      fts_log( "[client] output symbol cache hit: %6.2f%%\n", ((100.0 * cache->nhit) / cache->naccess));
    }
#endif
}

static void client_write_string( client_t *client, const char *s)
{
  write_char( client, FTS_PROTOCOL_STRING);

  while (*s)
    write_char( client, (unsigned char)*s++);

  write_char( client, 0);
}

static void client_write_object( client_t *client, fts_object_t *obj)
{
  write_char( client, FTS_PROTOCOL_OBJECT);
  write_int( client, OBJECT_ID_OBJ( fts_object_get_id( obj)));
}

void fts_client_start_message( fts_object_t *obj, fts_symbol_t selector)
{
  client_t *client = object_get_client( obj);

  if ( !client)
    return;

  client_write_object( client, obj);
  client_write_symbol( client, selector);
}

void fts_client_add_int( fts_object_t *obj, int v)
{
  client_t *client = object_get_client( obj);

  if ( !client)
    return;

  client_write_int( client, v);
}

void fts_client_add_float( fts_object_t *obj, float v)
{
  client_t *client = object_get_client( obj);

  if ( !client)
    return;

  client_write_float( client, v);
}

void fts_client_add_symbol( fts_object_t *obj, fts_symbol_t v)
{
  client_t *client = object_get_client( obj);

  if ( !client)
    return;

  client_write_symbol( client, v);
}

void fts_client_add_string( fts_object_t *obj, const char *v)
{
  client_t *client = object_get_client( obj);

  if ( !client)
    return;

  client_write_string( client, v);
}

void fts_client_add_object( fts_object_t *obj, fts_object_t *v)
{
  client_t *client = object_get_client( obj);

  if ( !client)
    return;

  client_write_object( client, v);
}

void fts_client_add_atoms( fts_object_t *obj, int ac, const fts_atom_t *at)
{
  client_t *client = object_get_client( obj);

  if ( !client)
    return;

  while (ac--)
    {
      if ( fts_is_int( at))
	client_write_int( client, fts_get_int( at));
      else if ( fts_is_float( at))
	client_write_float( client, fts_get_float( at));
      else if ( fts_is_symbol( at))
	client_write_symbol( client, fts_get_symbol( at));
      else if ( fts_is_string( at))
	client_write_string( client, fts_get_string( at));
      else if ( fts_is_object( at))
	client_write_object( client, fts_get_object( at));

      at++;
    }
}

void fts_client_done_message( fts_object_t *obj)
{
  client_t *client = object_get_client( obj);

  if ( !client)
    return;

  write_char( client, FTS_PROTOCOL_END_OF_MESSAGE);
  
  fts_bytestream_output( client->stream, fts_stack_get_size( &client->output_buffer), fts_stack_get_base( &client->output_buffer));

  fts_stack_clear( &client->output_buffer);
}

void fts_client_send_message( fts_object_t *obj, fts_symbol_t selector, int ac, const fts_atom_t *at)
{
#ifdef CLIENT_LOG
  fts_log( "[client]: Send message dest=0x%x selector=%s ac=%d args=", obj, selector, ac);
  fts_log_atoms( ac, at);
  fts_log( "\n");
#endif

  fts_client_start_message( obj, selector);
  fts_client_add_atoms( obj, ac, at);
  fts_client_done_message( obj);
}

void fts_client_upload_object(fts_object_t *obj, int client_id)
{
  fts_atom_t a[1];

  fts_client_register_object(obj, client_id);

  fts_set_object( a, obj);
  fts_send_message( (fts_object_t *)fts_object_get_patcher(obj), fts_SystemInlet, fts_s_upload_child, 1, a);  
}

void fts_client_register_object(fts_object_t *obj, int client_id)
{
  client_t *client;
  fts_atom_t a[1];

  if(client_id == FTS_NO_ID)
    client_id = fts_get_client_id((fts_object_t *)fts_object_get_patcher(obj));
  
  client = client_table_get(client_id);

  if ( !client)
    {
      fts_log("[client] fts_client_upload_object: Cannot upload object\n");      
      return;
    }

  client_register_object( client, obj, FTS_NO_ID);
}

void fts_client_release_object(fts_object_t *obj)
{
  int client_id;
  client_t *client;
  fts_atom_t a[1];
  
  client_id = fts_get_client_id( obj);
  client = client_table_get(client_id);

  if ( !client)
    {
      fts_log("[client] fts_client_release_object: Cannot release object\n");      
      return;
    }

  if( obj->patcher)
    {
      fts_set_object(a, obj);
      fts_client_send_message( (fts_object_t *)obj->patcher, s_remove_object, 1, a);
    }

  client_release_object( client, obj);
}

void fts_object_property_changed(fts_object_t *obj, fts_symbol_t property)
{
}

void fts_client_send_property(fts_object_t *obj, fts_symbol_t property)
{
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

  pipe_stream = fts_object_create(fts_pipestream_type, 0, 0);
  
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
  s_client_manager = fts_new_symbol("client_manager");
  s_client = fts_new_symbol("client");
  s_package_loaded = fts_new_symbol( "package_loaded");
  s_remove_object = fts_new_symbol( "removeObject");
  s_show_message = fts_new_symbol( "showMessage");

  client_table_init();

  fts_class_install( s_client_manager, client_manager_instantiate);

  fts_class_install( s_client, client_instantiate);

  fts_class_install( fts_new_symbol("client_controller"), client_controller_instantiate);

  /* check whether we should use a piped connection thru the stdio file handles */
  if ( fts_cmd_args_get( fts_new_symbol( "stdio")) != NULL ) 
    client_pipe_install();
  else
    client_tcp_manager_install();
}
