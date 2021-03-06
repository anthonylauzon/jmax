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

#define HACK_FOR_CRASH_ON_EXIT_WITH_PIPE_CONNECTION
/* Define this if you want logs of symbol cache hit */
/* #define CACHE_REPORT */
/* #define CLIENT_LOG */

#include <fts/fts.h>
#include <ftsconfig.h>

#if HAVE_ALLOCA_H
#include <alloca.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <ftsprivate/sched.h>
#include <ftsprivate/bmaxfile.h>
#include <ftsprivate/patcherobject.h>
#include <ftsprivate/object.h>
#include <ftsprivate/patfile.h>
#include <ftsprivate/package.h>
#include <ftsprivate/client.h>
#include <ftsprivate/errobj.h>
#include <ftsprivate/audio.h>
#include <ftsprivate/audioconfig.h> /* requires audiolabel.h */
#include <ftsprivate/midi.h>
#include <ftsprivate/config.h> /* requires audioconfig.h and midi.h */
#include <ftsprivate/loader.h>

#include <string.h>
#include <stdlib.h>

#include <ftsprivate/connection.h>
#include <ftsprivate/patcher.h>
#include <ftsprivate/client.h>
#include <ftsprivate/protocol.h>

#ifdef WIN32
#include <direct.h> /* Win32 _chdir() */
#endif

fts_class_t *fts_client_class;

static fts_symbol_t s_package_loaded;
static fts_symbol_t s_remove_object;
static fts_symbol_t s_show_message;

/***********************************************************************
 *
 * client table handling
 *
 */

static fts_stack_t client_table;

static int 
client_table_add( client_t *client)
{
  fts_stack_push( &client_table, client_t *, client);

  return fts_stack_top( &client_table);
}

static void 
client_table_remove( int id)
{
  ((client_t **)fts_stack_base( &client_table))[id] = 0;
}

static void 
client_table_init(void)
{
  fts_stack_init( &client_table, client_t *);

  client_table_add( 0); /* so that first client will have id 1 */
}

#define client_table_get(I) ((client_t **)fts_stack_base( &client_table))[(I)]

client_t *
object_get_client( fts_object_t *obj)
{
  int index = fts_object_get_client_id( obj );

  if (index < 0 || index >= fts_stack_size( &client_table))
    return NULL;

  return client_table_get(index);
}

/***********************************************************************
 *
 * client object (the object that is created on new client connections)
 *
 */

#define SYMBOL_CACHE_SIZE 1031

typedef struct {
  int length;
  fts_symbol_t *symbols;
#ifdef CACHE_REPORT
  int naccess, nhit;
#endif
} symbol_cache_t;

struct _client_t 
{
  fts_object_t head;
  int client_id;
  fts_hashtable_t object_table;
  int object_id_count;
  fts_object_t *root_patcher;
  fts_bytestream_t *stream;

  /* Input protocol decoder */
  int state;
  fts_stack_t input_args;
  fts_stack_t input_buffer;
  symbol_cache_t input_cache;

  /* Output protocol encoder */
  fts_stack_t output_buffer;
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

  fts_set_int( &k, fts_object_get_id( object));
  fts_hashtable_remove( &this->object_table, &k);

  fts_object_set_client_id( object, FTS_NO_ID);
}

static void 
client_register_object( client_t *this, fts_object_t *object)
{
  int object_id = fts_object_get_id(object);
  fts_atom_t k, v;

  if(object_id < 0)
  {
    object_id = this->object_id_count;
    this->object_id_count += 2;

    fts_object_set_id( object, object_id);
  }
  
  fts_set_int( &k, object_id);
  fts_set_object( &v, object);
  fts_hashtable_put( &this->object_table, &k, &v);

  fts_object_set_client_id( object, this->client_id);
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
  fts_free((void*)cache->symbols);
}

static void symbol_cache_put( symbol_cache_t *cache, fts_symbol_t s, int index)
{
  if (index >= cache->length)
  {
    int i, old_length = cache->length;

    cache->length = index+1;
    cache->symbols = fts_realloc((void*)cache->symbols, cache->length * sizeof( fts_symbol_t));

    for ( i = old_length; i < cache->length; i++)
      cache->symbols[i] = 0;
  }

  cache->symbols[index] = s;
}


/*----------------------------------------------------------------------
 * Finite state automata for protocol decoding
 */

/* Actions */

#ifndef WORDS_BIGENDIAN
static void swap_bytes( unsigned char *p, int n)
{
  int i, tmp;

  for ( i = 0; i < n/2; i++)
  {
    tmp = p[i];
    p[i] = p[n-i-1];
    p[n-i-1] = tmp;
  }
}
#endif

static int get_int_from_bytes( unsigned char *p)
{
#ifndef WORDS_BIGENDIAN
  swap_bytes( p, sizeof( int));
#endif

  return *(int *)p;
}

static double get_double_from_bytes( unsigned char *p)
{
#ifndef WORDS_BIGENDIAN
  swap_bytes( p, sizeof( double));
#endif

  return *(double *)p;
}

static void clear_action( unsigned char input, client_t *client)
{
  fts_stack_clear( &client->input_buffer);
}

static void shift_action( unsigned char input, client_t *client)
{
  fts_stack_push( &client->input_buffer, unsigned char, input);
}

static void end_int_action( unsigned char input, client_t *client)
{
  fts_atom_t a;

  fts_stack_push( &client->input_buffer, unsigned char, input);
  fts_set_int( &a, get_int_from_bytes( (unsigned char *)fts_stack_base( &client->input_buffer)));
  fts_stack_push( &client->input_args, fts_atom_t, a);
} 

static void end_float_action( unsigned char input, client_t *client)
{
  fts_atom_t a;

  fts_stack_push( &client->input_buffer, unsigned char, input);
  fts_set_float( &a, get_double_from_bytes( (unsigned char *)fts_stack_base( &client->input_buffer)));
  fts_stack_push( &client->input_args, fts_atom_t, a);
}

static void end_symbol_index_action( unsigned char input, client_t *client)
{
  fts_atom_t a;
  int index;

  fts_stack_push( &client->input_buffer, unsigned char, input);
  index = get_int_from_bytes( (unsigned char *)fts_stack_base( &client->input_buffer));
  fts_set_symbol( &a, client->input_cache.symbols[ index]);
  fts_stack_push( &client->input_args, fts_atom_t, a);
}

static void end_symbol_cache_action( unsigned char input, client_t *client)
{
  int index;
  fts_symbol_t s;
  fts_atom_t a;

  index = get_int_from_bytes( (unsigned char *)fts_stack_base( &client->input_buffer));

  fts_stack_push( &client->input_buffer, unsigned char, '\0');
  s = fts_new_symbol( (char *)fts_stack_base( &client->input_buffer) + sizeof( int));

  symbol_cache_put( &client->input_cache, s, index);

  fts_set_symbol( &a, s);
  fts_stack_push( &client->input_args, fts_atom_t, a);
}

static void end_string_action( unsigned char input, client_t *client)
{
  fts_atom_t a;

  fts_stack_push( &client->input_buffer, unsigned char, '\0');
  fts_set_string( &a, strdup( (char *)fts_stack_base( &client->input_buffer)));
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

  p = (char *)fts_stack_base( &client->input_buffer);
  l = fts_stack_size( &client->input_buffer);
  fts_tokenizer_init_buffer( &tokenizer, p, l);

  while ( fts_tokenizer_next( &tokenizer, &a) != 0)
    fts_stack_push( &client->input_args, fts_atom_t, a);

  fts_tokenizer_destroy( &tokenizer);
}

static void end_object_action( unsigned char input, client_t *client)
{
  fts_object_t *obj = 0;
  fts_atom_t v;
  int id;

  fts_stack_push( &client->input_buffer, unsigned char, input);
  id = get_int_from_bytes( (unsigned char *)fts_stack_base( &client->input_buffer));

  obj = client_get_object( client, id);

  if (obj == NULL)
  {
    fts_log( "[client] invalid object id: %d\n", id);
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

  argc = fts_stack_size( &client->input_args);
  argv = (fts_atom_t *)fts_stack_base( &client->input_args);

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
    fts_send_message( target, selector, argc, argv, fts_nix);

skipped:
  fts_stack_clear( &client->input_args);
}

static void state_next( client_t *client, unsigned char input)
{
#define q_initial            1

#define q_int0               10
#define q_int1               11
#define q_int2               12
#define q_int3               13

#define q_float0             20
#define q_float1             21
#define q_float2             22
#define q_float3   	     23
#define q_float4             24
#define q_float5             25
#define q_float6             26
#define q_float7   	     27

#define q_string             30

#define q_object0            40
#define q_object1            41
#define q_object2            42
#define q_object3            43

#define q_symbol_index0      50
#define q_symbol_index1      51
#define q_symbol_index2      52
#define q_symbol_index3      53

#define q_symbol_cache0      60
#define q_symbol_cache1      61
#define q_symbol_cache2      62
#define q_symbol_cache3      63
#define q_symbol_cache4      64

#define q_raw_string         70

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
    moveto( q_float4, shift_action);
    break;
  case q_float4:
    moveto( q_float5, shift_action);
    break;
  case q_float5:
    moveto( q_float6, shift_action);
    break;
  case q_float6:
    moveto( q_float7, shift_action);
    break;
  case q_float7:
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
      moveto( q_symbol_cache4, shift_action);
    break;

  case q_string:
    if ( input == 0)
      moveto( q_initial, end_string_action);
    else
      moveto( q_string, shift_action);
    break;

  case q_raw_string:
    if ( input == 0)
      moveto( q_initial, end_raw_string_action);
    else
      moveto( q_raw_string, shift_action);
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
    fts_log( "[client] error in reading message, client stopped\n");

    fts_bytestream_remove_listener(this->stream, (fts_object_t *) this);
    fts_bytestream_destroy( this->stream);
    this->stream = NULL;
    fts_patcher_remove_object(fts_get_root_patcher(), (fts_object_t *) this);

    return;
  }

  for ( i = 0; i < size; i++)
  {
    state_next( this, buffer[i]);
    if (this->state == 0)
      fts_log( "[client] protocol error\n");
  }
}

static void client_new_object(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  client_t *this = (client_t *)o;
  int id;
  fts_object_t *parent;
  fts_object_t *newobj;

  parent = fts_get_object_arg( ac, at, 0, 0);
  id = fts_get_int_arg( ac, at, 1, -1);

  if (!parent || id < 0)
  {
    fts_post( "client: invalid arguments\n");
    return;
  }

  newobj = fts_eval_object_description( (fts_patcher_t *)parent, ac - 2, at + 2);

  if (!newobj || fts_object_is_error( newobj))
  {
    fts_post( "error in object instantiation (");
    fts_post_atoms( ac - 2, at + 2);
    fts_post( ")\n");
    return;
  }

  fts_object_set_id(newobj, id);
  client_register_object( this, newobj);
}

static void client_set_object_property(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
    fts_object_set_dirty(obj);
  }
  else
    fts_log("[client]: System Error set_object_property: bad args\n");
}

static void client_connect_object(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_object_t *src, *dst;
  int src_outlet, dst_inlet;

  src = fts_get_object_arg( ac, at, 0, 0);
  src_outlet = fts_get_int_arg( ac, at, 1, -1);
  dst = fts_get_object_arg( ac, at, 2, 0);
  dst_inlet = fts_get_int_arg( ac, at, 3, -1);

  if ( !src || src_outlet < 0 || !dst || dst_inlet < 0)
    return;

  fts_connection_new(src, src_outlet, dst, dst_inlet, fts_c_anything);
}

#ifdef GENERIC_OPEN_FILE
/* (fd)
   This would be a generic "open", which would be the same to open a patcher, a project, a package,
   a configuration...
   But it is too complicated on the client side yet, so I give up..
*/
static void client_open_file(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  client_t *this = (client_t *)o;
  fts_object_t *object;
  fts_atom_t a[3];
  client_t *client;
  char *dir_name;
  fts_object_t *parent = (fts_object_t *)fts_get_root_patcher();
  fts_symbol_t file_name;
  fts_status_t status;

  file_name = fts_get_symbol( at);

  /*
   * We change the working directory so that it is the directory of the opened file.
   */
  dir_name = (char *)alloca( strlen( file_name) + 1);

  strcpy( dir_name, file_name);
  fts_dirname( dir_name);
  chdir( dir_name);

  if ((status = fts_file_load( file_name, parent, 0, 0, &object)) != fts_ok)
  {
    /* FIXME: signal the error */
    fts_log("[client]: cannot open file %s\n", file_name);
  }

  /* Inform the object that it has been loaded from a file and tell it the file name */
  fts_set_symbol( a, file_name);
  fts_send_message( object, fts_s_loaded, 1, a, fts_nix);

  /* upload the object to the client */
  client_register_object( this, object);

  fts_send_message( object, fts_s_upload, 0, 0, fts_nix);

  /* open the editor */
  fts_send_message( object, fts_new_symbol("openEditor"), 0, 0, fts_nix);
}
#endif


static void client_load_patcher_file(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  client_t *this = (client_t *)o;
  fts_client_load_patcher( fts_get_symbol( at), this->client_id);
}

fts_patcher_t *
fts_client_load_patcher(fts_symbol_t file_name, int client_id)
{
  fts_patcher_t *patcher = 0;
  int type = 1;
  fts_atom_t a[3];
  client_t *client;
  char *dir_name;
  fts_object_t *parent = (fts_object_t *)fts_get_root_patcher();

  client = client_table_get(client_id);

  fts_log("[client]: Load patcher %s\n", file_name);

  /*
   * This is to find the templates that are in the same directory
   * as the patch we are opening.
   * We change the working directory so that it is the directory of the
   * opened patch.
   */
  dir_name = (char *)alloca( strlen( file_name) + 1);

  strcpy( dir_name, file_name);
  fts_dirname( dir_name);

#ifdef WIN32
  _chdir(dir_name);
#else
  chdir(dir_name);
#endif

  if (fts_bmax_file_load( file_name, parent, 0, 0, (fts_object_t **)&patcher) != fts_ok)
  {
    fts_log("[patcher]: Cannot read file %s\n", file_name);
    return 0;
  }

  client_register_object( client, (fts_object_t *)patcher);

  /* Save the file name, for future autosaves and other services */
  fts_patcher_set_file_name(patcher, file_name);

  /* activate the post-load init, like loadbangs */   
  fts_send_message( (fts_object_t *)patcher, fts_new_symbol("load_init"), 0, 0, fts_nix);

  fts_set_int(a, fts_object_get_id((fts_object_t *)patcher));
  fts_set_symbol(a+1, file_name);
  fts_set_int(a+2, type);
  fts_client_send_message( (fts_object_t *)client, fts_new_symbol( "patcher_loaded"), 3, a);

  /* upload the patcher to the client */
  fts_send_message( (fts_object_t *)patcher, fts_s_upload, 0, 0, fts_nix);

  fts_patcher_set_dirty( patcher, 0);

  fts_send_message( (fts_object_t *)patcher, fts_s_openEditor, 0, 0, fts_nix);

  fts_log("[patcher]: Finished loading patcher %s\n", file_name);

  return patcher;
}

static void client_load_project(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  client_t *this = (client_t *)o;
  fts_package_t* project;
  fts_atom_t a[1];
  char message[1024];

  fts_symbol_t project_file = fts_get_symbol( at);
  
  project = fts_project_open(project_file);

  if(project == NULL || project->state == fts_package_corrupt) 
  {
    snprintf(message, sizeof(message), "Invalid project file: \n%s\n", project_file);
    fts_set_symbol( a, message);
    fts_client_send_message( o, s_show_message, 1, a);
  }  
  else
  {
    fts_project_set( project);

    client_register_object( this, (fts_object_t *)project);

    fts_set_int(a, fts_object_get_id( (fts_object_t *)project));
    fts_client_send_message(o, fts_s_project, 1, a);
      
    fts_send_message( (fts_object_t *)project, fts_s_upload, 0, 0, fts_nix);
  }
}

static void client_load_package(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  client_t *this = (client_t *)o;
  fts_package_t* package;
  fts_atom_t a[1];
  char message[1024];

  fts_symbol_t package_name = fts_get_symbol( at);
  fts_symbol_t package_file = fts_get_symbol( at+1);
  
  package = fts_package_load_from_file(package_name, package_file);

  if(package == NULL || package->state == fts_package_corrupt) 
  {
    snprintf(message, sizeof(message), "Invalid package file: \n%s\n", package_file);
    fts_set_symbol( a, message);
    fts_client_send_message( o, s_show_message, 1, a);
  }  
  else
  {
    if (fts_object_has_client( (fts_object_t *)package) == 0)
    {
      client_register_object( this, (fts_object_t *)package);

      fts_set_int(a, fts_object_get_id( (fts_object_t *)package));
      fts_client_send_message(o, fts_s_package, 1, a);    
      fts_send_message( (fts_object_t *)package, fts_s_upload, 0, 0, fts_nix);
    }
    fts_send_message( (fts_object_t *)package, fts_s_openEditor, 0, 0, fts_nix);
  }
}

static void client_load_summary(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  client_t *this = (client_t *)o;  
  fts_symbol_t file_name = fts_get_symbol( at);
  fts_patcher_t *summary = fts_patcher_get_by_file_name(file_name);
  
  if( summary)
    fts_client_send_message((fts_object_t *)summary, fts_s_openEditor, 0, 0);
  else
    fts_client_load_patcher( file_name, this->client_id);
}

static void client_get_project(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_atom_t a[1];
  fts_object_t *project = (fts_object_t *)fts_project_get();

  if (fts_object_has_client( project) == 0)
    client_register_object( (client_t *)o, project);

  fts_set_int(a, fts_object_get_id( project));
  fts_client_send_message(o, fts_s_project, 1, a);
  
  fts_send_message( project, fts_s_upload, 0, 0, fts_nix);
}

static void client_get_config(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_object_t *config = fts_config_get();
  
  if (config != NULL) 
  {
    fts_atom_t a;
    
    if (fts_object_has_client( config) == 0)
      client_register_object((client_t *)o, config);
    
    fts_set_int(&a, fts_object_get_id(config));
    fts_client_send_message(o, fts_s_config, 1, &a);
    
    fts_send_message(config, fts_s_upload, 0, 0, fts_nix);
  }
}

static void client_shutdown(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_sched_halt();
}

static void client_predefine_objects( client_t *this)
{
#ifdef HACK_FOR_CRASH_ON_EXIT_WITH_PIPE_CONNECTION
  fts_object_set_id((fts_object_t *)fts_get_root_patcher(), FTS_OBJECT_ID_ROOT);
  client_register_object( this, (fts_object_t *)fts_get_root_patcher());
#else
  this->root_patcher = fts_object_create_in_patcher( patcher_class, fts_get_root_patcher(), 0, 0);
  fts_object_set_id((fts_object_t *)this->root_patcher, FTS_OBJECT_ID_ROOT);

  if (!this->root_patcher)
  {
    fts_object_error( (fts_object_t *)this, "cannot create client root patcher");
    return;
  }

  fts_patcher_add_object(fts_get_root_patcher(), this->root_patcher);
  fts_object_refer( this->root_patcher);

  client_register_object( this, this->root_patcher);
#endif

  fts_object_set_id((fts_object_t *)this, FTS_OBJECT_ID_CLIENT);
  client_register_object( this, (fts_object_t *)this);
}

static void client_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  client_t *this = (client_t *)o;

  this->stream = (fts_bytestream_t *)fts_get_object_arg( ac, at, 0, NULL);

  if ( this->stream == NULL 
       || !fts_bytestream_check( (fts_object_t *)this->stream) 
       || !fts_bytestream_is_input( this->stream) 
       || !fts_bytestream_is_output( this->stream))
  {
    fts_object_error( (fts_object_t *)this, "invalid stream");
    return;
  }

  this->client_id = client_table_add( this);

  /* output protocol encoder */
  fts_stack_init( &this->output_buffer, unsigned char);
  symbol_cache_init( &this->output_cache);

  /* input protocol decoder */
  this->state = q_initial;
  fts_stack_init( &this->input_args, fts_atom_t);
  fts_stack_init( &this->input_buffer, unsigned char);
  symbol_cache_init( &this->input_cache);

  fts_bytestream_add_listener( this->stream, (fts_object_t *) this, client_receive);

  fts_hashtable_init( &this->object_table, FTS_HASHTABLE_MEDIUM);
  this->object_id_count = 17;

  client_predefine_objects( this);

  fts_log( "[client]: Accepted client connection\n");
}

static void client_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  client_t *this = (client_t *)o;

#ifndef HACK_FOR_CRASH_ON_EXIT_WITH_PIPE_CONNECTION
  fts_object_release( this->root_patcher);
  /* fts_patcher_remove_object(fts_get_root_patcher(), client_object); */
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

static void client_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof( client_t), client_init, client_delete);

  fts_class_message_varargs(cl, fts_new_symbol( "get_project"), client_get_project);
  fts_class_message_varargs(cl, fts_s_config, client_get_config);

  fts_class_message_varargs(cl, fts_new_symbol( "new_object"), client_new_object);
  fts_class_message_varargs(cl, fts_new_symbol( "set_object_property"), client_set_object_property);

#ifdef GENERIC_OPEN_FILE
  fts_class_message_varargs(cl, fts_new_symbol( "open_file"), client_open_file);
#endif

  fts_class_message_varargs(cl, fts_new_symbol( "load"), client_load_patcher_file);
  fts_class_message_varargs(cl, fts_new_symbol( "load_package"), client_load_package);
  fts_class_message_varargs(cl, fts_new_symbol( "load_project"), client_load_project);
  fts_class_message_varargs(cl, fts_new_symbol( "load_summary"), client_load_summary);

  /* why send to the client and not to the patcher ??? */
  fts_class_message_varargs(cl, fts_new_symbol( "connect_object"), client_connect_object);

  fts_class_message_varargs(cl, fts_new_symbol( "shutdown"), client_shutdown);
}

/***********************************************************************
 *
 * Client message sending
 *
 */

#define put_byte(C,N) fts_stack_push( &(C)->output_buffer, unsigned char, (N))

#define put_int(C,N) \
	put_byte(C, (unsigned char) (((N) >> 24) & 0xff)), \
	put_byte(C, (unsigned char) (((N) >> 16) & 0xff)), \
	put_byte(C, (unsigned char) (((N) >> 8) & 0xff)), \
	put_byte(C, (unsigned char) (((N) >> 0) & 0xff))

static void client_write_int( client_t *client, int v)
{
  put_byte( client, FTS_PROTOCOL_INT);
  put_int( client, v);
}

static void client_write_float( client_t *client, double v)
{
  unsigned char *p;
  int i;

  put_byte( client, FTS_PROTOCOL_FLOAT);

  p = (unsigned char *)&v;

#ifndef WORDS_BIGENDIAN
  swap_bytes( p, sizeof( double));
#endif

  for ( i = 0; i < sizeof( double); i++)
    put_byte( client, p[i]);
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
    put_byte( client, FTS_PROTOCOL_SYMBOL_INDEX);
    put_int( client, index);
  }
  else 
  {
    const char *p = s;

    cache->symbols[index] = s;

    /* send both the cache index and the symbol */
    put_byte( client, FTS_PROTOCOL_SYMBOL_CACHE);
    put_int( client, index);

    while (*p)
      put_byte( client, (unsigned char)*p++);

    put_byte( client, 0);
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
  put_byte( client, FTS_PROTOCOL_STRING);

  while (*s)
    put_byte( client, (unsigned char)*s++);

  put_byte( client, 0);
}

static void client_write_object( client_t *client, fts_object_t *obj)
{
  put_byte( client, FTS_PROTOCOL_OBJECT);
  put_int( client, fts_object_get_id( obj));
}

void fts_client_start_message( fts_object_t *obj, fts_symbol_t selector)
{
  client_t *client = object_get_client( obj);

  if (client != NULL && client->stream != NULL)
  {
    client_write_object( client, obj);
    client_write_symbol( client, selector);
  }
}

void fts_client_add_int( fts_object_t *obj, int v)
{
  client_t *client = object_get_client( obj);

  if ( !client || client->stream == NULL)
    return;

  client_write_int( client, v);
}

void fts_client_add_float( fts_object_t *obj, float v)
{
  client_t *client = object_get_client( obj);

  if ( !client || client->stream == NULL)
    return;

  client_write_float( client, v);
}

void fts_client_add_symbol( fts_object_t *obj, fts_symbol_t v)
{
  client_t *client = object_get_client( obj);

  if ( !client || client->stream == NULL)
    return;

  client_write_symbol( client, v);
}

void fts_client_add_string( fts_object_t *obj, const char *v)
{
  client_t *client = object_get_client( obj);

  if ( !client || client->stream == NULL)
    return;

  client_write_string( client, v);
}

void fts_client_add_object( fts_object_t *obj, fts_object_t *v)
{
  client_t *client = object_get_client( obj);

  if ( !client || client->stream == NULL)
    return;

  client_write_object( client, v);
}

void fts_client_add_atoms( fts_object_t *obj, int ac, const fts_atom_t *at)
{
  client_t *client = object_get_client( obj);

  if ( !client || client->stream == NULL)
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

  if ( !client || client->stream == NULL)
    return;

  put_byte( client, FTS_PROTOCOL_END_OF_MESSAGE);
  
  fts_bytestream_output( client->stream, fts_stack_size( &client->output_buffer), fts_stack_base( &client->output_buffer));

  fts_stack_clear( &client->output_buffer);
}

void fts_client_send_message( fts_object_t *obj, fts_symbol_t selector, int ac, const fts_atom_t *at)
{
  if(fts_object_has_client(obj))
  {
    client_t *client = object_get_client( obj);
    
    if (client->stream == NULL)
      return;

#ifdef CLIENT_LOG
    fts_log( "[client]: Send message dest=0x%x selector=%s ac=%d args=", obj, selector, ac);
    fts_log_atoms( ac, at);
    fts_log( "\n");
#endif
    
    fts_client_start_message( obj, selector);
    fts_client_add_atoms( obj, ac, at);
    fts_client_done_message( obj);
  }
}

void fts_client_register_object(fts_object_t *obj, int client_id)
{
  client_t *client;

  if(client_id <= FTS_NO_ID)
    client_id = fts_object_get_client_id((fts_object_t *)fts_object_get_patcher(obj));
  
  client = client_table_get(client_id);

  if(!client)
  {
    fts_log("[client] fts_client_register_object: cannot get id\n");      
    return;
  }

  client_register_object( client, obj);
}

void fts_client_release_object(fts_object_t *obj)
{
  if(fts_object_has_client(obj))
  {
    int client_id;
    client_t *client;
    fts_atom_t a[1];
    fts_object_t *patcher;
    
    client_id = fts_object_get_client_id( obj);
    client = client_table_get(client_id);
    
    if ( !client)
    {
      fts_log("[client] fts_client_release_object: Cannot release object\n");      
      return;
    }
    
    patcher = (fts_object_t *)fts_object_get_patcher(obj);
    if(patcher != NULL && fts_object_has_client(patcher))
    {
      fts_set_object(a, obj);
      fts_client_send_message( patcher, s_remove_object, 1, a);
    }
    
    client_release_object( client, obj);
  }
}

void fts_client_unregister_object(fts_object_t *obj)
{
  if(fts_object_has_client(obj))
  {
    int client_id = fts_object_get_client_id( obj);
    client_t *client = client_table_get(client_id);
    
    if ( !client)
    {
      fts_log("[client] fts_client_release_object: Cannot release object\n");      
      return;
    }
    
    fts_send_message_varargs( obj, fts_s_closeEditor, 0, 0);
    client_release_object( client, obj);
  }
}

/***************************************************************************/
static fts_memorystream_t *client_upload_memory_stream = NULL;

void 
fts_client_create_object(fts_object_t *obj, int client_id)
{
  if(!fts_object_has_client(obj))
  {
    fts_memorystream_t *stream = client_upload_memory_stream;
    fts_object_t *client = client_get_object( client_table_get(client_id), FTS_OBJECT_ID_CLIENT);
    fts_class_t *cl = fts_object_get_class(obj);
    fts_array_t array;
    fts_atom_t *at;
    int ac;
    int i;
    
    fts_client_register_object(obj, client_id);	
    
    fts_array_init(&array, 0, 0);  
    fts_array_append_int(&array, fts_object_get_id(obj));            
    
    /* description arguments */
    (*fts_class_get_description_function(cl))(obj, &array);
    
    /* description (post) string */
    fts_memorystream_reset(stream);
    fts_spost_object((fts_bytestream_t *)stream, obj);
    fts_bytestream_output_char((fts_bytestream_t *)stream,'\0');
    fts_array_append_symbol(&array, fts_new_symbol((char *)fts_memorystream_get_bytes( stream)));
    
    ac = fts_array_get_size(&array);
    at = fts_array_get_atoms(&array);
    
    for(i=1; i<ac-1; i++)
    {
      if(fts_is_object(at + i))
        fts_client_upload_object(fts_get_object(at + i), client_id);
    }
    
    fts_client_send_message(client, fts_s_register_object, ac, at);      
    fts_array_destroy(&array);
    
    fts_send_message(obj, fts_s_update_gui, 0, 0, fts_nix);
  }
}

void 
fts_client_upload_object(fts_object_t *obj, int client_id)
{
  if(!fts_object_has_client(obj))
  {
    fts_object_t *container = (fts_object_t *)fts_object_get_container(obj);
    
    if(container != NULL)
      fts_object_upload(obj);
    else
      fts_client_create_object(obj, client_id);
  }  
}
/***********************************************************************
*
* Initialization
*
*/

FTS_MODULE_INIT(client)
{
  client_table_init();

  s_package_loaded = fts_new_symbol( "package_loaded");
  s_remove_object = fts_new_symbol( "removeObject");
  s_show_message = fts_new_symbol( "showMessage");

  client_upload_memory_stream = (fts_memorystream_t *)fts_object_create(fts_memorystream_class, 0, 0);
  
  fts_client_class = fts_class_install( NULL, client_instantiate);
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */

