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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * (rewritten by Francois Dechelle)
 *
 */

#include <string.h>
#include <stdio.h>

#include "protocol.h"
#include "sys.h"
#include "lang.h"

/*
 * Uncommenting INCOMING_DEBUG_TRACE will produce a trace
 * of the incoming messages on the standard error
 */
/*  #define INCOMING_DEBUG_TRACE  */

static void (* mess_dispatch_table[256])(int, const fts_atom_t *);

#define S_CMD                      (256+1)
#define S_ARG                      (256+2)
#define S_ERROR                    (256+3)
#define S_SYMBOL_AND_DEF_CODE_1    (256+4)

#define BUFFER_INITIAL_SIZE        1024
#define AT_INITIAL_SIZE            256
#define SYMBOL_CACHE_INITIAL_SIZE  512

static int state = S_CMD;
static int cmd = 0;
static int ivalue = 0;
static int count = 0;
static int shift_table[4] = { 24, 16, 8, 0};

static fts_symbol_t *symbol_cache = 0;
static int symbol_cache_size = 0;

static int ac = 0;
static fts_atom_t *at = 0;
static int at_size = 0;

static int buffer_fill_p = 0;
static char *buffer = 0;
static int buffer_size = 0;

static void add_char( char c)
{
  if (buffer_fill_p >= buffer_size)
    {
      buffer_size = (buffer_size == 0) ? BUFFER_INITIAL_SIZE : 2 * buffer_size;
      buffer = (char *)fts_realloc( buffer, buffer_size);
    }

  buffer[ buffer_fill_p++ ] = c;
}

static void add_arg( fts_atom_t *a)
{
  if (ac >= at_size) 
    {
      at_size = (at_size == 0) ? AT_INITIAL_SIZE : 2 * at_size;
      at = (fts_atom_t *)fts_realloc( at, sizeof(fts_atom_t) * at_size);
    }

  at[ ac++ ] = *a;
}

static void reallocate_symbol_cache( int index)
{
  if ( symbol_cache_size != 0)
    {
      while ( index >= symbol_cache_size )
	symbol_cache_size = 2 * symbol_cache_size;
    }
  else
    symbol_cache_size = SYMBOL_CACHE_INITIAL_SIZE;

  symbol_cache = (fts_symbol_t *)fts_realloc( symbol_cache, symbol_cache_size);
}

static void protocol_error( char c, int state)
{
  fprintf( stderr, "Error in protocol : got %d ('%c') in state %d\n", c, (c < 32) ? '?' : c, state);
}

void fts_client_parse_char( char c)
{
  fts_atom_t a;

  switch (state) {

  case S_CMD:
    cmd = c;
    ac = 0;
    state = S_ARG;
    break;

  case S_ARG:
    count = 0;
    ivalue = 0;
    buffer_fill_p = 0;
    if ( c >= STRING_CODE && c < EOM_CODE)
      state = c;
    else if ( c == EOM_CODE)
      {
	if (mess_dispatch_table[ cmd ])
	  {
#ifdef INCOMING_DEBUG_TRACE
	    fprintf( stderr, "Dispatching '%s' ", protocol_printable_cmd( cmd));
	    fprintf_atoms( stderr, ac, at);
	    fprintf( stderr, "\n");
#endif

	    (*mess_dispatch_table[ cmd ])( ac, at);
	  }

	state = S_CMD;
      }
    else
      {
	protocol_error( c, state);
	state = S_ERROR;
      }
    break;

  case STRING_CODE:
    if ( c == STRING_END_CODE)
      {
	add_char( '\0');
	/* For now, we transform strings to symbols */
/*  	fts_set_string( &a, strcpy( fts_malloc( buffer_fill_p), buffer) ); */
	fts_set_symbol( &a, fts_new_symbol_copy( buffer));

	add_arg( &a);
	state = S_ARG;
      }
    else
      add_char( c);
    break;

  case INT_CODE:
    ivalue += ((int)c & 0xff) << shift_table[ count++ ];
    if ( count == 4)
      {
	fts_set_long( &a, ivalue);
	add_arg( &a);
	state = S_ARG;
      }
    break;

  case FLOAT_CODE:
    ivalue += ((int)c & 0xff) << shift_table[ count++ ];
    if ( count == 4)
      {
	fts_set_float( &a, *((float *)&ivalue) );
	add_arg( &a);
	state = S_ARG;
      }
    break;

  case SYMBOL_CACHED_CODE:
    ivalue += ((int)c & 0xff) << shift_table[ count++ ];
    if ( count == 4)
      {
	fts_set_symbol( &a, symbol_cache[ ivalue]);
	add_arg( &a);
	state = S_ARG;
      }
    break;

  case SYMBOL_AND_DEF_CODE:
    ivalue += ((int)c & 0xff) << shift_table[ count++ ];
    if ( count == 4)
      state = S_SYMBOL_AND_DEF_CODE_1;
    break;

  case S_SYMBOL_AND_DEF_CODE_1:
    if ( c == STRING_END_CODE)
      {
	add_char( '\0');

	if (ivalue >= symbol_cache_size)
	  reallocate_symbol_cache( ivalue);

	symbol_cache[ ivalue ] = fts_new_symbol_copy( buffer);

	fts_set_symbol( &a, symbol_cache[ ivalue]);

	add_arg( &a);
	state = S_ARG;
      }
    else
      add_char( c);
    break;

  case SYMBOL_CODE:
    if ( c == STRING_END_CODE)
      {
	add_char( '\0');
	fts_set_symbol( &a, fts_new_symbol_copy( buffer));
	add_arg( &a);
	state = S_ARG;
      }
    else
      add_char( c);
    break;

  case OBJECT_CODE:
    ivalue += ((int)c & 0xff) << shift_table[ count++ ];
    if ( count == 4)
      {
	fts_set_object( &a, fts_object_table_get( ivalue));
	add_arg( &a);
	state = S_ARG;
      }
    break;

  case CONNECTION_CODE:
    ivalue += ((int)c & 0xff) << shift_table[ count++ ];
    if ( count == 4)
      {
	fts_set_connection(&a, fts_connection_table_get( ivalue));
	add_arg( &a);
	state = S_ARG;
      }
    break;

  case DATA_CODE:
    ivalue += ((int)c & 0xff) << shift_table[ count++ ];
    if ( count == 4)
      {
	fts_set_data( &a, fts_data_id_get( ivalue));
	add_arg( &a);
	state = S_ARG;
      }
    break;

  default:
    protocol_error( c, state);
    if ( c == EOM_CODE)
      state = S_CMD;
    break;
  }
}

void fts_client_install(char type, void (* fun) (int, const fts_atom_t *))
{
  mess_dispatch_table[(int) type] = fun;
}
