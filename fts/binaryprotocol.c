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

#include <fts/fts.h>

#include <ftsprivate/tokenizer.h>
#include <ftsprivate/protocol.h>

#include <string.h>

#define SYMBOL_CACHE_SIZE 1031

fts_class_t* fts_binary_protocol_type = NULL;

/*----------------------------------------------------------------------
 * Symbol cache
 */
static void symbol_cache_init( fts_symbol_cache_t *cache)
{
  int i;

  cache->length = SYMBOL_CACHE_SIZE;
  cache->symbols = (fts_symbol_t *)fts_malloc( cache->length * sizeof( fts_symbol_t));

  for ( i = 0; i < cache->length; i++)
    cache->symbols[i] = 0;
}

static void symbol_cache_destroy( fts_symbol_cache_t *cache)
{
  fts_free( cache->symbols);
}

static void symbol_cache_put( fts_symbol_cache_t *cache, fts_symbol_t s, int index)
{
  if (index >= cache->length)
  {
    int i, old_length = cache->length;

    cache->length = index+1;
    cache->symbols = fts_realloc( cache->symbols, cache->length * sizeof( fts_symbol_t));

    for ( i = old_length; i < cache->length; i++)
      cache->symbols[i] = 0;
  }

  cache->symbols[index] = s;
}

/*----------------------------------------------------------------------
 * Finite state automata for protocol decoding
 */

/* Actions */

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

static void clear_action( unsigned char input, fts_binary_protocol_t *binary_protocol)
{
  fts_stack_clear( &binary_protocol->input_buffer);
}

static void shift_action( unsigned char input, fts_binary_protocol_t *binary_protocol)
{
  fts_stack_push( &binary_protocol->input_buffer, unsigned char, input);
}

static void end_int_action( unsigned char input, fts_binary_protocol_t *binary_protocol)
{
  fts_atom_t a;

  fts_stack_push( &binary_protocol->input_buffer, unsigned char, input);
  fts_set_int( &a, get_int_from_bytes( (unsigned char *)fts_stack_base( &binary_protocol->input_buffer)));
  fts_stack_push( &binary_protocol->input_args, fts_atom_t, a);
} 

static void end_float_action( unsigned char input, fts_binary_protocol_t *binary_protocol)
{
  fts_atom_t a;

  fts_stack_push( &binary_protocol->input_buffer, unsigned char, input);
  fts_set_float( &a, get_double_from_bytes( (unsigned char *)fts_stack_base( &binary_protocol->input_buffer)));
  fts_stack_push( &binary_protocol->input_args, fts_atom_t, a);
}

static void end_symbol_index_action( unsigned char input, fts_binary_protocol_t *binary_protocol)
{
  fts_atom_t a;
  int index;

  fts_stack_push( &binary_protocol->input_buffer, unsigned char, input);
  index = get_int_from_bytes( (unsigned char *)fts_stack_base( &binary_protocol->input_buffer));
  fts_set_symbol( &a, binary_protocol->input_cache.symbols[ index]);
  fts_stack_push( &binary_protocol->input_args, fts_atom_t, a);
}

static void end_symbol_cache_action( unsigned char input, fts_binary_protocol_t *binary_protocol)
{
  int index;
  fts_symbol_t s;
  fts_atom_t a;

  index = get_int_from_bytes( (unsigned char *)fts_stack_base( &binary_protocol->input_buffer));

  fts_stack_push( &binary_protocol->input_buffer, unsigned char, '\0');
  s = fts_new_symbol( (char *)fts_stack_base( &binary_protocol->input_buffer) + sizeof( int));

  symbol_cache_put( &binary_protocol->input_cache, s, index);

  fts_set_symbol( &a, s);
  fts_stack_push( &binary_protocol->input_args, fts_atom_t, a);
}

static void end_string_action( unsigned char input, fts_binary_protocol_t *binary_protocol)
{
  fts_atom_t a;

  fts_stack_push( &binary_protocol->input_buffer, unsigned char, '\0');
  fts_set_string( &a, strdup( (char *)fts_stack_base( &binary_protocol->input_buffer)));
  fts_stack_push( &binary_protocol->input_args, fts_atom_t, a);
}

static void end_raw_string_action( unsigned char input, fts_binary_protocol_t *binary_protocol)
{
  fts_atom_t a;
  fts_tokenizer_t tokenizer;
  char *p;
  int l;

  fts_stack_push( &binary_protocol->input_buffer, unsigned char, '\0');
  fts_stack_push( &binary_protocol->input_buffer, unsigned char, '\0');

  p = (char *)fts_stack_base( &binary_protocol->input_buffer);
  l = fts_stack_size( &binary_protocol->input_buffer);
  fts_tokenizer_init_buffer( &tokenizer, p, l);

  while ( fts_tokenizer_next( &tokenizer, &a) != 0)
    fts_stack_push( &binary_protocol->input_args, fts_atom_t, a);

  fts_tokenizer_destroy( &tokenizer);
}

static void end_object_action( unsigned char input, fts_binary_protocol_t *binary_protocol)
{
  fts_object_t *obj = 0;
  fts_atom_t v;
  int id;


  /* call registered callback */
  /* @@@@@ example of client specific code @@@@@ */
/*   fts_stack_push( &binary_protocol->input_buffer, unsigned char, input); */
/*   id = get_int_from_bytes( fts_stack_base( &binary_protocol->input_buffer)); */

/*   obj = binary_protocol_get_object( binary_protocol, id); */

/*   if (obj == NULL) */
/*   { */
/*     fts_log( "[binary_protocol] invalid object id: %d\n", id); */
/*     fts_set_void( &v); */
/*   } */
/*   else */
/*     fts_set_object( &v, obj); */

/*   fts_stack_push( &binary_protocol->input_args, fts_atom_t, v); */
}

static void end_message_action( unsigned char input, fts_binary_protocol_t *binary_protocol)
{
  fts_object_t *target;
  fts_symbol_t selector;
  int argc;
  fts_atom_t *argv;

  return;
  /* call registered callback */
  /* @@@@@ example of client specific code @@@@@ */
/*   argc = fts_stack_size( &client->input_args); */
/*   argv = (fts_atom_t *)fts_stack_base( &client->input_args); */

/*   selector = fts_get_symbol( argv+1); */

/*   if ( fts_is_void( argv)) */
/*   { */
/*     fts_log( "[client] message %s to null object\n", selector); */
/*     goto skipped; */
/*   } */
    
/*   target = fts_get_object( argv); */
/*   argc -= 2; */
/*   argv += 2; */

/* #ifdef CLIENT_LOG */
/*   fts_log( "[client]: Received message dest=0x%x selector=%s args=", target, selector); */
/*   fts_log_atoms( argc, argv); */
/*   fts_log( "\n"); */
/* #endif */

/*   /\* Client messages are sent to the system inlet *\/ */
/*   if (target) */
/*     fts_send_message( target, selector, argc, argv); */

/* skipped: */
/*   fts_stack_clear( &client->input_args); */
}

static void state_next( fts_binary_protocol_t *binary_protocol, unsigned char input)
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

#define moveto( NEWSTATE, ACTION) (binary_protocol->state = NEWSTATE, ACTION( input, binary_protocol))

  switch( binary_protocol->state) {
  case 0:
    /* try to skip till end of message */
    if ( input == FTS_PROTOCOL_END_OF_MESSAGE)
      binary_protocol->state = q_initial;
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
      binary_protocol->state = 0;
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

#define put_byte(B,N) fts_stack_push( &(B)->output_buffer, unsigned char, (N))

#define put_int(B,N) \
	put_byte(B, (unsigned char) (((N) >> 24) & 0xff)), \
	put_byte(B, (unsigned char) (((N) >> 16) & 0xff)), \
	put_byte(B, (unsigned char) (((N) >> 8) & 0xff)), \
	put_byte(B, (unsigned char) (((N) >> 0) & 0xff))

void fts_binary_protocol_add_int(fts_binary_protocol_t* binary_protocol, int v)
{
  put_byte(binary_protocol, FTS_PROTOCOL_INT);
  put_int(binary_protocol, v);
}

void fts_binary_protocol_add_float(fts_binary_protocol_t* binary_protocol, double v)
{
  unsigned char* p;
  int i;

  put_byte(binary_protocol, FTS_PROTOCOL_FLOAT);
  
  p = (unsigned char*)&v;
#ifndef WORDS_BIGENDIAN
  swap_bytes(p, sizeof(double));
#endif
  
  for (i = 0; i < sizeof(double); ++i)
    put_byte(binary_protocol, p[i]);
}

void fts_binary_protocol_add_symbol(fts_binary_protocol_t* binary_protocol, fts_symbol_t s)
{
  unsigned int index;
  fts_symbol_cache_t* cache = &binary_protocol->output_cache;

#ifdef CACHE_REPORT
  cache->nbaccess++;
#endif

  index = (unsigned int)s % cache->length;

  if (cache->symbols[index] == s)
  {
#ifdef CACHE_REPORT
    cache->nhit++;
#endif
    /* just send the index */
    put_byte(binary_protocol, FTS_PROTOCOL_SYMBOL_INDEX);
    put_int(binary_protocol, index);
  }
  else
  {
    const char* p = s;
    cache->symbols[index] = s;

    /* send both cache index and the symbol */
    put_byte(binary_protocol, FTS_PROTOCOL_SYMBOL_CACHE);
    put_int(binary_protocol, index);

    while (*p)
      put_byte(binary_protocol, (unsigned char)*p++);

    put_byte(binary_protocol, 0);
  }
#ifdef CACHE_REPORT
  if (cache->naccess % 1024 == 0)
  {
    fts_log("[binary_protocol] output symbol cache hit: %6.2f%%\n", ((100.0 * cache->nhit) / cache->naccess));
  }
#endif 
}

void fts_binary_protocol_add_string(fts_binary_protocol_t* binary_protocol, const char* s)
{
  put_byte(binary_protocol, FTS_PROTOCOL_STRING);
  
  while (*s)
    put_byte(binary_protocol, (unsigned char)*s++);

  put_byte(binary_protocol, 0);
}


void fts_binary_protocol_add_object(fts_binary_protocol_t* binary_protocol, fts_object_t* obj)
{
  put_byte(binary_protocol, FTS_PROTOCOL_OBJECT);
  put_int(binary_protocol, OBJECT_ID_OBJ(fts_object_get_id(obj)));
}


void fts_binary_protocol_add_atoms( fts_binary_protocol_t *binary_protocol, int ac, const fts_atom_t *at)
{
  while (ac--)
  {
    if ( fts_is_int( at))
      fts_binary_protocol_add_int( binary_protocol, fts_get_int( at));
    else if ( fts_is_float( at))
      fts_binary_protocol_add_float( binary_protocol, fts_get_float( at));
    else if ( fts_is_symbol( at))
      fts_binary_protocol_add_symbol( binary_protocol, fts_get_symbol( at));
    else if ( fts_is_string( at))
      fts_binary_protocol_add_string( binary_protocol, fts_get_string( at));
    else if ( fts_is_object( at))
      fts_binary_protocol_add_object( binary_protocol, fts_get_object( at));

    at++;
  }
}

void fts_binary_protocol_start_message(fts_binary_protocol_t* binary_protocol, fts_object_t* obj, fts_symbol_t selector)
{
  fts_binary_protocol_add_object(binary_protocol, obj);
  fts_binary_protocol_add_symbol(binary_protocol, selector);
}


void fts_binary_protocol_end_message(fts_binary_protocol_t* binary_protocol)
{
  put_byte(binary_protocol, FTS_PROTOCOL_END_OF_MESSAGE);
}

int fts_binary_protocol_decode(fts_binary_protocol_t* binary_protocol, int size, const unsigned char* buffer)
{
  int i;
  for (i = 0; i < size; ++i)
  {
    state_next(binary_protocol, buffer[i]);
    if (binary_protocol->state == 0)
    {
      fts_log("[bianry_protocol] protocol error \n");
      return -1;
    }   
  }
  return size;
}


static void 
binary_protocol_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_binary_protocol_t* self = (fts_binary_protocol_t*) o;

  /* output protocol encoder */
  fts_stack_init( &self->output_buffer, unsigned char);
  symbol_cache_init( &self->output_cache);

  /* input protocol decoder */
  self->state = q_initial;
  fts_stack_init( &self->input_args, fts_atom_t);
  fts_stack_init( &self->input_buffer, unsigned char);
  symbol_cache_init( &self->input_cache);
}

static void
binary_protocol_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_binary_protocol_t* self = (fts_binary_protocol_t*) o;
  
  /* output protocol encoder */
  fts_stack_destroy( &self->output_buffer);
  symbol_cache_destroy( &self->output_cache);

  /* input protocol decoder */
  fts_stack_destroy( &self->input_args);
  fts_stack_destroy( &self->input_buffer);
}

static void
binary_protocol_instantiate(fts_class_t* cl)
{
  fts_class_init(cl, sizeof(fts_binary_protocol_t), binary_protocol_init, binary_protocol_delete);
}

void fts_binary_protocol_config(void)
{
  fts_binary_protocol_type = fts_class_install(NULL, binary_protocol_instantiate);
}


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */

