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

#include "ftsconfig.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#if HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#if HAVE_NETDB_H
#include <netdb.h>
#endif
#include <errno.h>
#include <string.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_WINDOWS_H
#include <windows.h>
#endif

#include <fts/fts.h>
#include <ftsprivate/OLDclient.h>
#include <ftsprivate/OLDftsdata.h>
#include <ftsprivate/objtable.h>
#include <ftsprivate/abstraction.h>
#include <ftsprivate/connection.h>
#include <ftsprivate/errobj.h>
#include <ftsprivate/object.h>
#include <ftsprivate/patcher.h>
#include <ftsprivate/loader.h>
#include <ftsprivate/patparser.h>
#include <ftsprivate/bmaxhdr.h>
#include <ftsprivate/saver.h>
#include <ftsprivate/symbol.h>
#include <ftsprivate/template.h>

/***********************************************************************
 *
 * Forward declarations
 *
 */

static void fts_client_parse_char( char c);


/***********************************************************************
 *
 * Emulation of old client behaviour, using scheduler callback
 *
 */

#define UDP_PACKET_SIZE 2048

typedef struct _oldclient_t {
  fts_object_t head;
  /* Socket */
#ifdef WIN32
  SOCKET socket;
#else
  int socket;
#endif
  struct sockaddr_in client_addr;
  /* Connection type */
  int stream;
  /* Input */
  char input_buffer[UDP_PACKET_SIZE];
  /* Output */
  char sequence;
  fts_stack_t output_buffer;
} oldclient_t;

static void oldclient_flush( oldclient_t *this);

static oldclient_t *oldclient;

static void
oldclient_receive( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  oldclient_t *this = (oldclient_t *)o;
  int r, i;

  if ( fts_get_int_arg( ac, at, 0, -1) < 0)
    {
      oldclient_flush( this);
      return;
    }

  if (this->stream) 
    {
#if WIN32
      if ((r = recv( this->socket, this->input_buffer, UDP_PACKET_SIZE, 0)) < 0)
#else
      if ((r = read( this->socket, this->input_buffer, UDP_PACKET_SIZE)) < 0)
#endif
	{
	  post( "[client] error in reading message (error %d)\n", errno);
	  return;
	}
    }
  else
    {
      if ((r = recvfrom( this->socket, this->input_buffer, UDP_PACKET_SIZE, 0, 0, 0)) < 0)
	{
	  post( "[client] error in reading message (%s)\n", strerror( errno));
	  return;
	}
    }

  for ( i = 0; i < r; i++)
    fts_client_parse_char( this->input_buffer[i]);
}


static void
oldclient_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  oldclient_t *this = (oldclient_t *)o;
  fts_atom_t a;
  struct sockaddr_in my_addr;
  char *address;
  unsigned short port = 0;
  const char *host = "127.0.0.1";
  struct hostent *hostptr;
  fts_symbol_t value;

  /*
   * Get values from command line args
   */
  if ((value = fts_cmd_args_get( fts_new_symbol( "port"))))
    port = atoi( fts_symbol_name( value));

  if ((value = fts_cmd_args_get( fts_new_symbol( "host"))))
    host = fts_symbol_name( value);

  this->stream = (fts_cmd_args_get( fts_new_symbol( "tcp")) != NULL);

  hostptr = gethostbyname( host);

  if ( !hostptr)
    {
      fprintf( stderr, "Unknown host: %s\n", host);
      return;
    }

  fts_stack_init( &this->output_buffer, unsigned char);

  if (this->stream) 
    {
      if ( (this->socket = socket( AF_INET, SOCK_STREAM, 0) ) == -1)
	{
	  fprintf( stderr, "[oldclient] error opening socket (%s)\n", strerror( errno));
	  return;
	}
      
      /* Bind the socket to an arbitrary available port  */
      memset( &my_addr, 0, sizeof(struct sockaddr_in));
      my_addr.sin_family = AF_INET;
      my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
      my_addr.sin_port = 0;
      
      if (bind( this->socket, &my_addr, sizeof(struct sockaddr_in)) == -1)
	{
	  fprintf( stderr, "[oldclient] cannot bind socket (%s)\n", strerror( errno));
	  return;
	}
      
      memset( &this->client_addr, 0, sizeof(this->client_addr));
      this->client_addr.sin_family = AF_INET;
      this->client_addr.sin_addr = *(struct in_addr *)hostptr->h_addr_list[0];
      this->client_addr.sin_port = htons(port);

      /* Connect the socket to the client */
      if ( connect(this->socket, &this->client_addr, sizeof(this->client_addr)) < 0) 
	{
	  fprintf( stderr, "[oldclient] cannot connect (%s)\n", strerror( errno));
	  return;
	}
    } 
  else 
    {
      if ( (this->socket = socket( AF_INET, SOCK_DGRAM, 0) ) == -1)
	{
	  fprintf( stderr, "[oldclient] error opening socket (%s)\n", strerror( errno));
	  return;
	}
      
      /* Bind the socket to an arbitrary available port  */
      memset( &my_addr, 0, sizeof(struct sockaddr_in));
      my_addr.sin_family = AF_INET;
      my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
      my_addr.sin_port = 0;
      
      if (bind( this->socket, &my_addr, sizeof(struct sockaddr_in)) == -1)
	{
	  fprintf( stderr, "[oldclient] cannot bind socket (%s)\n", strerror( errno));
	  return;
	}
      
      memset( &this->client_addr, 0, sizeof(this->client_addr));
      this->client_addr.sin_family = AF_INET;
      this->client_addr.sin_addr = *(struct in_addr *)hostptr->h_addr_list[0];
      this->client_addr.sin_port = htons(port);
      
      /* Send an init packet: empty content, just the packet */
      if ( sendto( this->socket, "init", 4, 0, &this->client_addr, sizeof( this->client_addr)) < 0)
	{
	  fprintf( stderr, "[oldclient] cannot send init packet (%s)\n", strerror( errno));
	  return;
	}

      fts_stack_push( &this->output_buffer, unsigned char, 0);
      fts_stack_push( &this->output_buffer, unsigned char, 0);
      fts_stack_push( &this->output_buffer, unsigned char, 0);
    }

  fts_sched_add( (fts_object_t *)this, FTS_SCHED_READ, this->socket);
  fts_sched_add( (fts_object_t *)this, FTS_SCHED_ALWAYS);
}

static void
oldclient_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  oldclient_t *this = (oldclient_t *)o;

  fts_sched_remove( (fts_object_t *)this);

#ifdef WIN32
  if (this->socket != INVALID_SOCKET) {
    int r;
    char buf[1024];

    /* call WSAAsyncSelect ??? */
    shutdown(this->socket, 0x02);
    while (1) {
      r = recv(this->socket, buf, 1024, 0);
      if ((r == 0) || (r == SOCKET_ERROR)) {
	break;
      }
    }
    closesocket(this->socket);
  }
#else
  close( this->socket);
#endif
}

static fts_status_t oldclient_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init( cl, sizeof( oldclient_t), 0, 0, 0);

  fts_method_define_varargs( cl, fts_SystemInlet, fts_s_init, oldclient_init);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, oldclient_delete, 0, 0);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_sched_ready, oldclient_receive);

  return fts_Success;
}

void fts_oldclient_start( void)
{
  fts_symbol_t s;
  fts_atom_t a[1];

  s = fts_new_symbol( "oldclient");
  fts_class_install( s, oldclient_instantiate);

  fts_set_symbol( a, s);
  fts_object_new_to_patcher( fts_get_root_patcher(), 1, a, (fts_object_t **)&oldclient);
}


#define UDP_PACKET_SIZE 2048

static void oldclient_put_char( oldclient_t *this, unsigned char c)
{
  if (!this)
    return;

  fts_stack_push( &this->output_buffer, unsigned char, c);

  if (fts_stack_get_size( &this->output_buffer) >= UDP_PACKET_SIZE)
    oldclient_flush( this);
}

static void oldclient_flush( oldclient_t *this)
{
  int r, len;
  unsigned char *p;

  if (!this)
    return;

  len = fts_stack_get_size( &this->output_buffer);

  if (this->stream) 
    {
      if (len <= 0)
	return;

      p = (unsigned char *)fts_stack_get_ptr( &this->output_buffer);
      
      /* Send an init packet: empty content, just the packet */
#if WIN32
      if ( send( this->socket, p, len, 0) < 0)
#else
      if ( write( this->socket, p, len) < 0)
#endif
	{
	  fprintf( stderr, "[oldclient] cannot send init packet (%s)\n", strerror( errno));
	  return;
	}
      
      fts_stack_clear( &this->output_buffer);
    } 
  else
    {
      if (len <= 3)
	return;

      p = (unsigned char *)fts_stack_get_ptr( &this->output_buffer);
      
      p[0] = this->sequence;
      
      this->sequence = (this->sequence + 1) % 128;
      
      p[1] = len / 256;
      p[2] = len % 256;
      
#if 0
      {
	int i;
	
	fprintf( stderr, "Sending %d bytes\n", len);
	
#define NN 32
	for ( i = 0; i < len; i++)
	  {
	    if ( i % NN == 0)
	      fprintf( stderr, "[%03d]", i);
	    
	    if (p[i] >= ' ')
	      fprintf( stderr, " %2c", p[i]);
	    else
	      fprintf( stderr, " %02x", (int)p[i]);
	    
	    if ( i % NN == (NN-1))
	      fprintf( stderr, "\n");
	  }
	
	if ( i % NN != (NN-1))
	  fprintf( stderr, "\n");
      }
#endif
      
      r = sendto( this->socket, p, len, 0, &this->client_addr, sizeof( this->client_addr));

      if ( r != len)
	fprintf( stderr, "[oldclient] error sending (%s)\n", strerror( errno));

      fts_stack_clear( &this->output_buffer);
      fts_stack_push( &this->output_buffer, unsigned char, 0);
      fts_stack_push( &this->output_buffer, unsigned char, 0);
      fts_stack_push( &this->output_buffer, unsigned char, 0);
    }
}


/***********************************************************************
 * 
 * incoming.c
 *
 */

/*
 * Defining INCOMING_DEBUG_TRACE will produce a trace
 * of the incoming messages on the standard error
 */
/*  #define INCOMING_DEBUG_TRACE */

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

static void fts_client_parse_char( char c)
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
	    fprintf( stderr, "Dispatching '%c' ", cmd);
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
	fts_set_int( &a, ivalue);
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


/***********************************************************************
 *
 * outgoing.c
 *
 */


/* Functions to open a message, close a message, send a valur, send a list of atoms.
   Messages to the client; there is only one client at this level, so we don't have
   any client argument (a part for the coding of the client id in the stream).

   We send the values as we get them; the device implement buffering
   if it need it.
*/

/*
 * Defining OUTGOING_DEBUG_TRACE will produce a trace
 * of the outgoing messages on the standard error
 */

/*  #define OUTGOING_DEBUG_TRACE       */

static void fts_client_send_string(const char *msg)
{
  int i;

  for (i = 0; msg[i] != '\0' ; i++)
    oldclient_put_char( oldclient, msg[i]);
}

static void fts_client_send_int(int value)
{
  oldclient_put_char( oldclient, (unsigned char) (((unsigned int) value >> 24) & 0xff));
  oldclient_put_char( oldclient, (unsigned char) (((unsigned int) value >> 16) & 0xff));
  oldclient_put_char( oldclient, (unsigned char) (((unsigned int) value >> 8) & 0xff));
  oldclient_put_char( oldclient, (unsigned char) (((unsigned int) value) & 0xff));
}

void fts_client_start_msg( int type)
{
  oldclient_put_char( oldclient, (char)type);

#ifdef OUTGOING_DEBUG_TRACE      
  fprintf(stderr, "Sending '%c' ", (char)type);
#endif
}

void fts_client_start_clientmess(void)
{
  fts_client_start_msg(CLIENTMESS_CODE);
}

void fts_client_add_int(int value)
{
#ifdef OUTGOING_DEBUG_TRACE      
  fprintf( stderr, "%d ", value);
#endif

  oldclient_put_char( oldclient, INT_CODE);
  fts_client_send_int(value);
}


void fts_client_add_data( fts_data_t *data)
{
#ifdef OUTGOING_DEBUG_TRACE      
  fprintf_data( stderr, data);
#endif

  oldclient_put_char( oldclient, DATA_CODE);
  fts_client_send_int( data ? fts_data_get_id(data) : 0);
}

void fts_client_add_object(fts_object_t *obj)
{  
#ifdef OUTGOING_DEBUG_TRACE      
  fprintf_object( stderr, obj);
#endif

  if (obj && fts_object_get_id(obj) == FTS_NO_ID) {
    int dummy = 0;
  }

  oldclient_put_char( oldclient, OBJECT_CODE);
  fts_client_send_int( obj ? fts_object_get_id(obj) : 0);
}


void fts_client_add_connection(fts_connection_t *c)
{
  oldclient_put_char( oldclient, CONNECTION_CODE);
  fts_client_send_int(c ? fts_connection_get_id(c) : 0);

}


void fts_client_add_float(float value)
{
  oldclient_put_char( oldclient, FLOAT_CODE);
  fts_client_send_int( *((unsigned int *)&value) );
}

/*
  Symbol cache handling

  The symbol cache transforms a symbol in an integer id,
  that will be send other the client connection.

  The first time a symbol is send to the client, it is
  allocated an id and this id together with its name is sent
  to the client.
  The next time it is sent to the client, the id only is
  sent.

  The cache has a limited size: if there is no more
  cache id available, a symbol is sent as a string.
  (i.e. the symbol cache does not have a LRU-like replacement 
  algorithm).

*/

#define MAX_CACHE_INDEX 512

static int first_unused_symbol_cache_index = 0;

static int cache_symbol( fts_symbol_t s)
{
  if (first_unused_symbol_cache_index < MAX_CACHE_INDEX)
    {
      fts_symbol_set_cache_index( s, first_unused_symbol_cache_index++);
      return 1;
    }

  return 0;
}

void fts_client_add_symbol(fts_symbol_t s)
{
#ifdef OUTGOING_DEBUG_TRACE      
  fprintf( stderr, "%s ", fts_symbol_name(s) );
#endif

  if ( fts_symbol_get_cache_index(s) >= 0 )   /* Is symbol cached ? */
    {
      oldclient_put_char( oldclient, SYMBOL_CACHED_CODE);
      fts_client_send_int( fts_symbol_get_cache_index(s));
    }
  else if (cache_symbol(s))   /* Try to cache it and if succeeded, send a cache definition */
    {
      oldclient_put_char( oldclient, SYMBOL_AND_DEF_CODE);
      fts_client_send_int( fts_symbol_get_cache_index(s));
      fts_client_send_string( fts_symbol_name(s));
      oldclient_put_char( oldclient, STRING_END_CODE);
    }
  else   /* Send it as string, but with a SYMBOL_CODE */
    {
      oldclient_put_char( oldclient, SYMBOL_CODE);
      fts_client_send_string(fts_symbol_name(s));
      oldclient_put_char( oldclient, STRING_END_CODE);
    }
}

void fts_client_add_string(const char *s)
{
#ifdef OUTGOING_DEBUG_TRACE      
  fprintf( stderr, "%s ", s);
#endif

  oldclient_put_char( oldclient, STRING_CODE);
  fts_client_send_string(s);
  oldclient_put_char( oldclient, STRING_END_CODE);
}


static void 
fts_client_add_atom(const fts_atom_t *atom)
{
  if (fts_is_int( atom))
    fts_client_add_int( fts_get_int(atom));
  else  if (fts_is_float( atom))
    fts_client_add_float( fts_get_float(atom));
  else  if (fts_is_symbol( atom))
    fts_client_add_symbol( fts_get_symbol(atom));
  else  if (fts_is_string( atom))
    fts_client_add_string( fts_get_string(atom));
  else  if (fts_is_object( atom))
    fts_client_add_object( fts_get_object(atom));
  else  if (fts_is_data( atom))
    fts_client_add_data( fts_get_data( atom) );
  else
    fprintf(stderr, "Wrong atom type in fts_client_add_atoms: %lx\n", (unsigned long) fts_get_type(atom));
}

void 
fts_client_add_atoms(int ac, const fts_atom_t *args)
{
  int i;
  
  for (i=0; i<ac; i++)
    fts_client_add_atom(&args[i]);
}


void 
fts_client_done_msg(void)
{
  /*  Add the eom code  */

  oldclient_put_char( oldclient, (char) EOM_CODE);

#ifdef OUTGOING_DEBUG_TRACE      
  fprintf(stderr, "<EOM>\n");
#endif
}

/* 
   Utility to send a message to an client object;
   put here for the convenience of object programmers
   
   CLIENTMESS (obj)obj (symbol)selector [(atom)<args>]* 
*/

void fts_client_send_message(fts_object_t *obj, fts_symbol_t selector, int argc, const fts_atom_t *args)
{
  fts_client_start_msg(CLIENTMESS_CODE);
  fts_client_add_object(obj);
  fts_client_add_symbol(selector);
  fts_client_add_atoms(argc, args);
  fts_client_done_msg();
}

/* (nos:) This is a new upload function, which is part of a generic object creation client/server API.
 * Other than "fts_client_upload_object" it is called by an object in order to upload itself.
 * In the creation of an object from the client this happens inside the "upload" method, which this way
 * has quite a different semantic than the same message send by "fts_client_upload_object".
 */
void fts_client_upload(fts_object_t *obj, fts_symbol_t classname, int ac, const fts_atom_t *at)
{
  if (!fts_object_has_id(obj))
    fts_object_table_register(obj);

  fts_client_start_msg(NEW_OBJECT_CODE);

  /* this is to be compatible with the NEW_OBJECT_CODE */
  fts_client_add_object((fts_object_t *)0);
  fts_client_add_data((fts_data_t *)0);

  fts_client_add_int(fts_object_get_id(obj));
  fts_client_add_symbol(classname);
  fts_client_add_atoms(ac, at);
  fts_client_done_msg();

  /*fts_object_send_properties(obj);*/
}

void fts_client_upload_object(fts_object_t *obj)
{
  int do_var = 0;

  if (!fts_object_has_id(obj))
    fts_object_table_register(obj);

  /* First, check if the parent has been uploaded; if it is not,
     upload it; recursively, this will upload all the chain up
     to the root */

  if (obj->patcher && !fts_object_has_id((fts_object_t *)obj->patcher))
    fts_client_upload_object((fts_object_t *) obj->patcher);

  /* 
     NEW_OBJECT_VAR_CODE (obj)parent (dta) data (int)new-id (symbol) var [<args>]+
     NEW_OBJECT_CODE (obj)parent (dta) data (int)new-id [<args>]+
     */

  if(fts_object_description_defines_variable(obj->argc, obj->argv))
    do_var = 1;
  
  if (do_var)
    fts_client_start_msg(NEW_OBJECT_VAR_CODE);
  else
    fts_client_start_msg(NEW_OBJECT_CODE);

  fts_client_add_object((fts_object_t *) obj->patcher);
  
  if (obj->patcher)
    fts_client_add_data((fts_data_t *) obj->patcher->data);
  else
    fts_client_add_data((fts_data_t *) 0);

  fts_client_add_int(fts_object_get_id(obj));
  
  if (do_var)
    {
      fts_client_add_symbol(fts_get_symbol(&obj->argv[0]));

      fts_client_add_atoms(obj->argc - 2, obj->argv + 2);
    }
  else
    fts_client_add_atoms(obj->argc, obj->argv);

  fts_client_done_msg();

  fts_object_send_properties(obj);

  /* Also, send to the object the message upload, in the case the object have more data/initialization to do */
  fts_message_send(obj, fts_SystemInlet, fts_s_upload, 0, 0);
}


void fts_client_upload_connection(fts_connection_t *c)
{
  /* CONNECT (obj)from (int)outlet (obj)to (int)inlet */

  if (c->id == FTS_NO_ID)
    fts_connection_table_register(c);

  fts_client_start_msg(NEW_CONNECTION_CODE);

  if (c->src->patcher)
    fts_client_add_data((fts_data_t *) c->src->patcher->data);
  else
    fts_client_add_data((fts_data_t *) 0);

  fts_client_add_int(c->id);
  fts_client_add_object(c->src);
  fts_client_add_int(c->woutlet);
  fts_client_add_object(c->dst);
  fts_client_add_int(c->winlet);
  fts_client_add_int(c->type);
  fts_client_done_msg();
}


/* Handling of connections and object release and refine */

void fts_client_release_connection(fts_connection_t *c)
{
  fts_client_start_msg(CONNECTION_RELEASE_CODE);
  fts_client_add_connection(c);
  fts_client_done_msg();
}


void fts_client_redefine_connection(fts_connection_t *c)
{
  fts_client_start_msg(REDEFINE_CONNECTION_CODE);
  fts_client_add_connection(c);
  fts_client_add_object(c->src);
  fts_client_add_int(c->woutlet);
  fts_client_add_object(c->dst);
  fts_client_add_int(c->winlet);
  fts_client_add_int(c->type);
  fts_client_done_msg();
}

void fts_client_release_object(fts_object_t *obj)
{
  fts_client_start_msg(OBJECT_RELEASE_CODE);
  fts_client_add_object(obj);;
  fts_client_done_msg();
}

void fts_client_release_object_data(fts_object_t *obj)
{
  fts_client_start_msg(OBJECT_RELEASE_DATA_CODE);
  fts_client_add_object(obj);;
  fts_client_done_msg();
}

/***********************************************************************
 *
 * sync.c
 *
 */

/* The Sync subsystem; just answer to message 
   SYNC_CODE with a message SYNC_DONE_CODE; it is used for syncronization;
   A client can send a number of requests, and then a PING; at the reception
   of the PONG, it is sure that all the answerback invoked by the previous 
   requests has been processed.
 */

static void fts_sync_dispatch(int ac, const fts_atom_t *av)
{
  fts_client_updates_sync();

  fts_client_start_msg(SYNC_DONE_CODE);
  fts_client_done_msg();
}

static void fts_client_sync_init(void)
{
  fts_client_install(SYNC_CODE, fts_sync_dispatch);
}



/***********************************************************************
 *
 * updates.c
 *
 */
/* 
   This file define the update generation scheduling mechanism for objects.

   NEW version for FTS 2.0.

   In FTS 2.0, the flow control is gone; updates are sent directly as
   property values; we allow update-per-ticks updates per ticks, where
   the update-per-ticks parameter can be set with a UCS command (for the moment),
   and default to 100.

   The unidirectional flow control is gone because we estimate that
   on a UNIX machine, a more "statistical" control may be more
   meaningfull; also, the whole determinism of the flow control make
   little or no sense compared to the complete undeterminism of the
   load given by a control patch.

   The update scheduler use the property change registration mechanism:
   when scheduled, it get the first N property change in the change fifo,
   and send them to the client.

   The scheduler is started by an alarm; the first property change start
   the alarm with a small time (actually 0) to have the maximal reactivity,
   and then the alarm is automatically rearmed iff there are elements in the fifo.
   In this way for burst of events with a period larger than the alarm we answer
   in real-time.
   */

/* #define UPDATE_TRACE  */

/* Default values are for 400 updates per seconds,
 */

static int fts_updates_per_ticks = 40;
static int fts_update_period = 10; /* every how many milliseconds we do an update */

static fts_alarm_t update_alarm;

struct changes
{
  fts_symbol_t property;
  fts_object_t *obj;
  struct changes *next;
};

static fts_heap_t *changes_heap;
static struct changes *changes_queue_head = 0;


static void fts_client_send_property(fts_object_t *obj, fts_symbol_t name)
{
  fts_atom_t a;

  if (obj && name)
    {
      fts_object_get_prop(obj, name, &a);

      if (fts_is_data(&a))
	{
	  /* If the property value is an fts_data, we
	     export the data needed  */

	  fts_data_t *d;
	  
	  d = fts_get_data(&a);

	  if (! fts_data_is_exported(d))
	    fts_data_export(d);
	}
      else if (fts_is_object(&a))
	{
	  /* If the property is an fts_object and is not 
	     uploaded, upload it; note that this will shortly
	     be unified with fts_data model */

	  fts_object_t *obj;

	  obj = fts_get_object(&a);

	  if (!fts_object_has_id(obj))
	    fts_client_upload_object(obj);
	}

      /* If the value is void, send a null value only
	 if the property is a special registered property;
	 for the moment, only fts_s_data; it should be done
	 better */

      if (fts_is_void(&a))
	{
	  if (name == fts_s_data)
	    fts_set_data(&a, (fts_data_t *) 0);
	  else
	    return;
	}

#ifdef UPDATE_TRACE 
      {
	fprintf(stderr, "Sending property %s value " , fts_symbol_name(name));
	fprintf_atoms(stderr, 1, &a);
	fprintf(stderr, " for object ");
	fprintf_object(stderr, obj);
	fprintf(stderr, "\n");
      }
#endif

      fts_client_start_msg(CLIENTPROP_CODE);
      fts_client_add_object(obj);
      fts_client_add_symbol(name);
      fts_client_add_atoms(1, &a);
      fts_client_done_msg();
    }
}


static void update_group_start(void)
{
  fts_client_start_msg(UPDATE_GROUP_START_CODE);
  fts_client_done_msg();
}


static void update_group_end(void)
{
  fts_client_start_msg(UPDATE_GROUP_END_CODE);
  fts_client_done_msg();
}


static void fts_client_updates_sched(fts_alarm_t *alarm, void *arg)
{
  fts_object_t *obj;
  fts_symbol_t property;
  int update_count;

  if (changes_queue_head)
    {
      update_group_start();

      for (update_count = 0;
	   (update_count < fts_updates_per_ticks) && changes_queue_head ;
	   update_count++)
	{
	  struct changes *p;

	  p = changes_queue_head;
	  changes_queue_head = p->next;

	  fts_client_send_property(p->obj, p->property);
	  fts_heap_free(p, changes_heap);
	}

      update_group_end();

      fts_alarm_set_delay(&update_alarm, fts_update_period);
    }
}

/* Sync empty the change fifo; this is used to introduce a sequence
   point between the client and the server, with the sync protocol,
   under request from the server; it do not touch the alarm.
   */

void fts_client_updates_sync(void)
{
  fts_object_t *obj;
  fts_symbol_t property;

  if (changes_queue_head)
    {
      update_group_start();

      while (changes_queue_head)
	{
	  struct changes *p;

	  p = changes_queue_head;
	  changes_queue_head = p->next;

	  /* Note that fts_client_send_property may
	     recursively call this function; this
	     is why changes_queue_head *must* be updated
	     before the call */

	  fts_client_send_property(p->obj, p->property);
	  fts_heap_free(p, changes_heap);
	}

      update_group_end();
    }
}


void fts_updates_set_updates_per_ticks(int upt)
{
  if (upt > 0)
    fts_updates_per_ticks = upt;
}

void fts_updates_set_update_period(int upt)
{
  if (upt > 0)
    fts_update_period = upt;
}



/* This function send a property value asynchroniously to the client,
   when some bandwith is available; please note that the function
   fts_client_send_property can be used to send the property
   sychroniusly, in  contexts like uploading */


void fts_object_property_changed(fts_object_t *obj, fts_symbol_t property)
{
  struct changes *p;
  struct changes *last = 0;

  /* check if the object is already in the evsched list */

  for (p = changes_queue_head; p; last = p, p = p->next)
    if ((p->obj == obj) && p->property == property)
      return;

  /* 
     Here, if last is not null, is the last element of the list;
     if it is null, there are no element in the list.
   */

  p = (struct changes *)fts_heap_alloc(changes_heap);

  p->property = property;
  p->obj = obj;
  p->next = 0;

  /* add the new queue element to the end of the list */

  if (last)
    last->next = p;
  else
    changes_queue_head = p;

  /* If the update timer is not armed, arm it with a zero period,
     so it will fire immediately */

  if (! fts_alarm_is_armed(&update_alarm))
    {
      fts_alarm_set_delay(&update_alarm, 0);
    }
}



/* By using the fts_object_ui_property_changed function, an object declare
   that the changed property is a User Interface related matter, and this
   allow optimization like not registering the property change if the corresponding
   patcher is not open, or if there are no editors open for this object ...
   (the latter is to be implemented).
 */


void fts_object_ui_property_changed(fts_object_t *obj, fts_symbol_t property)
{
  if (fts_patcher_is_open( fts_object_get_patcher( obj)))
      fts_object_property_changed(obj, property);
}


/* To reset to signal that an object  shouldn't be considered changed anymore.
   It is called before deleting an object, to keep the change list
   consistent.
*/

void fts_object_reset_changed(fts_object_t *obj)
{
  struct changes **pp;		/* indirect precursor  */
  struct changes *p;		/* found element */

  pp = &changes_queue_head;

  while (*pp)
    {
      if ((*pp)->obj == obj)
	{
	  p = (*pp);

	  (*pp) = (*pp)->next;

	  fts_heap_free((char *)p, changes_heap);
	}
      else
	pp = &((*pp)->next);
    }
}


void fts_client_updates_init(void)
{
  changes_heap = fts_heap_new(sizeof(struct changes));
  fts_alarm_init(&update_alarm, 0, fts_client_updates_sched, 0);
}


/***********************************************************************
 *
 * messtile.c
 *
 */

/* 
   Client message functions; use the prefix fts_mess_client.

   The FTS 2.0 protocol support objects in the atoms; so the
   messages include objects, not id, when possible (i.e. a part
   from the new message).

   NEW protocol; the old 1.x protocol is dropped, in favour of the following:
   All the fts_mess_client messages are atom strings; the message
   selector is a command, as before.
   The accepted messages are 

   not yet implemented, but soon !!!

   NEW  (obj) p (int)id [<args>]*
   REDEFINE_PATCHER (obj)patcher <name> <ins> <outs>
   REDEFINE_OBJECT (obj)object [<args>]*
   DELETE_OBJECT (obj)obj
   DELETE_CONNECTION (conn)c
   CONNECT (obj)from (int)outlet (obj)to (int)inlet
   DISCONNECT (obj)cid

   This two are temporary: they will be substituted with special
   protected mechanisms .. or extended with protection mechanism.

   MESS (obj)obj (int)inlet (symbol)selector [<args>]*

   Message handling

   PUTPROP (obj)o (symbol)name (any) value
   GETPROP (obj)o (symbol)name

   Get property ask a message back; the message back have
   exactly the PUTPROP format.

   These messages are s sent *to* the client; here we provide
   convenience functions to do it

   CLIENTPROP (obj)o (symbol)name (atom) value
   CLIENTMESS (obj)obj (symbol)selector [(atom)<args>]*

   REMOTE_CALL <key> <args> * 

   FTS_SHUTDOWN_CODE  quit fts.

   Other messages will be added ...
 */

static fts_symbol_t fts_s_download;
static fts_symbol_t fts_s_load_init;
static fts_symbol_t fts_s_setDescription;

/******************************************************************************/
/*                                                                            */
/*                INIT and Utilities                                          */
/*                                                                            */
/******************************************************************************/

/* Debug  message log, post and on the standard error */

static void
printf_mess(const char *msg, int ac, const fts_atom_t *av)
{
  fprintf(stderr, "%s (%d args): ", msg, ac);
  fprintf_atoms(stderr, ac, av);
  fprintf(stderr, "\n");
}


#define INIT_TRACE 0

static int do_mess_trace = INIT_TRACE;

void fts_set_mess_trace(int b)
{
  do_mess_trace = b;
}


void
trace_mess(const char *msg, int ac, const fts_atom_t *av)
{
  if (do_mess_trace)
    {
      fprintf(stderr, "%s: ", msg); 
      fprintf_atoms(stderr, ac, av);
      fprintf(stderr, "\n");
    }
}


/******************************************************************************/
/*                                                                            */
/*                  FOS message functions                                     */
/*                                                                            */
/******************************************************************************/

/*    SAVE_PATCHER_BMAX  (obj)p (sym)filename

      Save the patcher in a bmax binary file.
 */

static void
fts_mess_client_save_patcher_bmax(int ac, const fts_atom_t *av)
{
  trace_mess("Received save patcher bmax ", ac, av);

  if (ac == 2 && fts_is_object(&av[0]) && fts_is_symbol(&av[1]))
    {
      fts_object_t *patcher;
      fts_symbol_t filename;

      patcher = (fts_object_t *) fts_get_object(&av[0]);
      filename = fts_get_symbol(&av[1]);

      if (patcher)
	fts_save_patcher_as_bmax(filename, patcher);
      else
	printf_mess("System Error in FOS message SAVE PATCHER BMAX: null patcher", ac, av);
    }
  else
    printf_mess("System Error in FOS message SAVE PATCHER BMAX: bad args", ac, av);
}


/*    LOAD_PATCHER_BMAX  (obj)p (sym)filename

      Save the patcher in a bmax binary file.
 */

static void
fts_mess_client_load_patcher_bmax(int ac, const fts_atom_t *av)
{
  trace_mess("Received load patcher bmax ", ac, av);

  if (ac == 3 && fts_is_object(&av[0]) && fts_is_int(&av[1]) && fts_is_symbol(&av[2]))
    {
      fts_object_t *parent;
      int id;
      fts_symbol_t filename;

      parent = (fts_object_t *) fts_get_object(&av[0]);
      id = fts_get_int(&av[1]);
      filename = fts_get_symbol(&av[2]);

      if (parent)
	{
	  fts_object_t *patcher;

	  patcher = fts_binary_file_load(fts_symbol_name(filename), parent, 0, 0, 0);

	  if (patcher == 0)
	    {
	      post("Cannot read jmax file %s\n", fts_symbol_name(filename));
	      return;
	    }

	  if (id != FTS_NO_ID)
	    fts_object_set_id(patcher, id);

	  /* Save the file name, for future autosaves and other services */

	  {
	    fts_atom_t a;

	    fts_set_symbol(&a, filename);
	    fts_object_put_prop(patcher, fts_s_filename, &a);
	  }

	  /* activate the post-load init, like loadbangs */	  
	  fts_message_send(patcher, fts_SystemInlet, fts_new_symbol("load_init"), 0, 0);
	}
      else
	printf_mess("System Error in FOS message LOAD PATCHER BMAX: null patcher", ac, av);

    }
  else
    printf_mess("System Error in FOS message LOAD PATCHER BMAX: bad args", ac, av);
}


/*    LOAD_PATCHER_DPAT  (obj)p (sym)filename

      Save the patcher in a bmax binary file.
 */

static void
fts_mess_client_load_patcher_dpat(int ac, const fts_atom_t *av)
{
  trace_mess("Received load patcher dpat", ac, av);

  if (ac == 3 && fts_is_object(&av[0]) && fts_is_int(&av[1]) && fts_is_symbol(&av[2]))
    {
      fts_object_t *parent;
      int id;
      fts_symbol_t filename;

      parent  = (fts_object_t *) fts_get_object(&av[0]);
      id       = fts_get_int(&av[1]);
      filename = fts_get_symbol(&av[2]);

      if (parent)
	{
	  fts_object_t *patcher;

	  patcher = fts_load_dotpat_patcher(parent, filename);

	  if (patcher == 0)
	    {
	      post("Cannot read .pat file %s\n", fts_symbol_name(filename));
	      return;
	    }

	  if (id != FTS_NO_ID)
	    fts_object_set_id(patcher, id);
	}

      else
	printf_mess("System Error in FOS message LOAD PATCHER DPAT: null patcher", ac, av);

    }
  else
    printf_mess("System Error in FOS message LOAD PATCHER DPAT: bad args", ac, av);
}

static void
fts_mess_client_download_object(int ac, const fts_atom_t *av)
{
  trace_mess("Received download object ", ac, av);

  if (ac == 1 && fts_is_object(&av[0]))
    {
      fts_object_t *object;
      fts_object_t *p;

      object = (fts_object_t *) fts_get_object(&av[0]);

      if (! object)
	{
	  return;
	}

      fts_client_upload_object(object);
    }
  else
    printf_mess("System Error in FOS message DOWNLOAD OBJECT: bad args", ac, av);
}


/*    DOWNLOAD_CONNECTION   (obj)p

      Send to the connection the message "download". (system inlet)
      Do the actual work here; this is not good; but: it cannot be
      in the mess module because use the runtime !!
 */

static void
fts_mess_client_download_connection(int ac, const fts_atom_t *av)
{
  trace_mess("Received download connection ", ac, av);

  if (ac == 1 && fts_is_connection(&av[0]))
    {
      fts_connection_t *connection;
      fts_object_t *p;

      connection = (fts_connection_t *) fts_get_connection(&av[0]);

      if (! connection)
	{
	  return;
	}

      fts_client_upload_connection(connection);
    }
  else
    printf_mess("System Error in FOS message DOWNLOAD CONNECTION: bad args", ac, av);
}


/*    NEW  (obj)pid (int)new-id [<args>]* or
      NEW  (int)new-id [<args>]*

      Create a new object, in the parent patcher, with 
      local id new-lid, and description args.

      Now new accept an empty argument list; in this case
      fts_eval_object_description produce an error object.
 */

static void
fts_mess_client_new(int ac, const fts_atom_t *av)
{
  trace_mess("Received new", ac, av);

  if (ac >= 2 && fts_is_object(&av[0]) && fts_is_int(&av[1]))
    {
      fts_object_t *parent = fts_get_object(&av[0]);
      
      if(parent && fts_object_get_class_name(parent) == fts_s_patcher)
	{
	  /* new object in patcher */
	  int id = fts_get_int(&av[1]);
	  fts_object_t *obj = fts_eval_object_description((fts_patcher_t *)parent, ac - 2, av + 2);
	  
	  fts_object_set_id(obj, id);
	}
      else
	printf_mess("System Error in FOS message NEW:  parent not found", ac, av);
    }
  else
    printf_mess("System Error in FOS message NEW: bad args", ac, av);
}


/*
   REDEFINE_PATCHER (obj)old [<args>]*
   
   Redefine the patcher in place; for the moment, the
   accepted arguments are what the user type in the box
   (ie.  [<var name> : ] <args>*) without the class name,
   and to the translation to the real format here;
   in the future it will be the client to do the work.
   */

static void
fts_mess_client_redefine_patcher(int ac, const fts_atom_t *av)
{
  trace_mess("Received redefine patcher", ac, av);

  if (ac >= 1)
    {
      fts_atom_t argv[512];
      int argc;
      fts_patcher_t  *patcher;

      patcher = (fts_patcher_t *) fts_get_object(&av[0]);

      if (! patcher)
	{
	  printf_mess("System Error in FOS message REDEFINE PATCHER: redefining a non existing patcher", ac, av);
	  return;
	}

      if (fts_object_description_defines_variable(ac - 1, av + 1))
	{
	  /* Variable syntax */
	  /* argv[0] = av[1]; */
	  /* argv[1] = av[2]; */
	  fts_set_symbol(&argv[0], fts_s_patcher); /* "jpatcher" */

	  /* copy arguments (ignoring variable) */
	  for (argc = 1; (argc < ac - 2) && (argc < 512) ; argc++) 
	    argv[argc] = av[argc + 2];
	}
      else
	{
	  /* Plain syntax */
	  fts_set_symbol(&argv[0], fts_s_patcher);

	  for (argc = 1; (argc < ac) && (argc < 512) ; argc++)
	    argv[argc] = av[argc];
	}

      fts_patcher_redefine(patcher, argc, argv);

      fts_client_send_message((fts_object_t *)patcher, fts_s_setDescription, argc - 1, argv + 1);
    }
  else
    printf_mess("System Error in FOS message REDEFINE PATCHER: bad args", ac, av);
}

/*
   REDEFINE_OBJECT (obj)object (int) new_id [<args>]*
   
   Redefine an object (not a patcher, use in re-editing).
   */

static void
fts_mess_client_redefine_object(int ac, const fts_atom_t *av)
{
  trace_mess("Received redefine object", ac, av);

  if (ac >= 1 && fts_is_object(&av[0]))
    {
      fts_object_t  *object;
      int new_id;

      object = fts_get_object(&av[0]);
      new_id = fts_get_int(&av[1]);
      fts_object_redefine(object, new_id, ac - 2, av + 2);
    }
  else
    printf_mess("System Error in FOS message REDEFINE OBJECT: bad args", ac, av);
}


/* 
   DELETE_OBJECT (obj)obj

   Free (destroy) the object identified by id.
*/


static void
fts_mess_client_delete_object(int ac, const fts_atom_t *av)
{
  trace_mess("delete object", ac, av);

  if (ac == 1 && fts_is_object(&av[0]))
    {
      fts_object_t *obj;

      obj = fts_get_object(&av[0]);

      if (! obj)
	{
	  printf_mess("System Error in FOS message DELETE_OBJECT: deleting a non existing object", ac, av);
	  return;
	}

      fts_object_delete_from_patcher(obj);
    }
  else
    printf_mess("System Error in FOS message DELETE_OBJECT: bad args", ac, av);
}





/* CONNECT (obj)from (int)outlet (obj)to (int)inlet

  Connect the outlet of a from object (identified by the from-id )
  to an inlet of a to object (identified by the to-id) */


static void
fts_mess_client_new_connection(int ac, const fts_atom_t *av)
{
  trace_mess("Received connect", ac, av);

  if ((ac == 5) &&
      fts_is_int(&av[0]) &&
      fts_is_object(&av[1]) &&
      fts_is_int(&av[2]) &&
      fts_is_object(&av[3]) &&
      fts_is_int(&av[4]))
    {
      int inlet, outlet;
      int id;
      fts_object_t *from, *to;
      fts_status_t ret;

      id     = fts_get_int(&av[0]);
      from   = fts_get_object(&av[1]);
      outlet = fts_get_int(&av[2]);
      to     = fts_get_object(&av[3]);
      inlet  = fts_get_int(&av[4]);

      if (to && from)
	{
	  fts_connection_new( id, from, outlet, to, inlet);
	}
      else
	printf_mess("System Error in FOS message CONNECT: Error trying to connect non existing objects", ac, av);
    }
  else
    printf_mess("System Error in FOS message CONNECT: bad args", ac, av);
}


/*
  DELETE_CONNECTION (conn)connection

  Disconnect the outlet of a from object (identified by the from-id )
  to an inlet of a to object  (identified by the to-id)
  */


static void
fts_mess_client_delete_connection(int ac, const fts_atom_t *av)
{
  trace_mess("Received delete connection", ac, av);

  if ((ac == 1) && fts_is_connection(&av[0]))
    {
      fts_connection_t *c;

      c = fts_get_connection(&av[0]);

      if (c)
	fts_connection_delete(c);
      else
	printf_mess("System Error in FOS message DELETE CONNECTION: disconnecting non existing connection", ac, av);
    }
  else
    printf_mess("System Error in FOS message DELETE CONNECTION: bad args", ac, av);
}


/*
   MESS (obj)o (int)inlet (symbol)selector [<args>]*

   Send to an object,
   in the inlet "inlet", the message specified by the selector and its arguments.
   */

static void 
fts_mess_client_mess(int ac, const fts_atom_t *av)
{
  trace_mess("Received mess", ac, av);

  if ((ac >= 3) &&
      fts_is_object(&av[0]) &&
      fts_is_int(&av[1]) &&
      fts_is_symbol(&av[2]))
    {
      int inlet;
      fts_object_t *obj;
      fts_symbol_t selector;

      obj    = fts_get_object(&av[0]);
      inlet  = fts_get_int(&av[1]);
      selector = fts_get_symbol(&av[2]);

      if (! obj)
	{
	  printf_mess("System Error in FOS message MESS: message to a non existing object", ac, av);
	  return;
	}

      /* Ignore failure; it is normal that the UI interface try to send
	 messages that have no methods defined
	 */

      fts_message_send(obj, inlet, selector, ac - 3, av + 3);
    }
  else
    printf_mess("System Error in FOS message MESS: bad args", ac, av);
}

/*
   PUTPROP (obj)o (symbol)name (any) value

   Send to an object  a new value for a specified property.
   */

static void 
fts_mess_client_put_prop(int ac, const fts_atom_t *av)
{
  trace_mess("Received put prop", ac, av);

  if ((ac == 3) &&
      fts_is_object(&av[0]) &&
      fts_is_symbol(&av[1]))
    {
      fts_object_t *obj;
      fts_symbol_t name;

      obj  = fts_get_object(&av[0]);
      name = fts_get_symbol(&av[1]);

      fts_object_put_prop(obj, name, &av[2]);
    }
  else
    printf_mess("System Error in FOS message PUTPROP: bad args", ac, av);
}


/*
   GETPROP (obj)o (symbol)name

   Ask an object to Send the client a property value.
   */

static void 
fts_mess_client_get_prop(int ac, const fts_atom_t *av)
{
  trace_mess("Received get prop", ac, av);

  if ((ac == 2) &&
      fts_is_object(&av[0]) &&
      fts_is_symbol(&av[1]))
    {
      fts_object_t *obj;
      fts_symbol_t name;

      obj  = fts_get_object(&av[0]);
      name = fts_get_symbol(&av[1]);

      fts_object_property_changed(obj, name);
    }
  else
    printf_mess("System Error in FOS message GETPROP: bad args", ac, av);
}


/*
   GETALLPROP (obj)o (symbol)name

   Ask a patcher object to Send the client a property value for
   all the objects inside the patcher.
   */

static void 
fts_mess_client_get_all_prop(int ac, const fts_atom_t *av)
{
  trace_mess("Received get prop", ac, av);

  if ((ac == 2) &&
      fts_is_object(&av[0]) &&
      fts_is_symbol(&av[1]))
    {
      fts_patcher_t *patcher;
      fts_object_t *obj;
      fts_symbol_t name;

      patcher  = (fts_patcher_t *) fts_get_object(&av[0]);
      name = fts_get_symbol(&av[1]);

      for (obj = patcher->objects ; obj; obj = obj->next_in_patcher)
	fts_object_property_changed(obj, name);
    }
  else
    printf_mess("System Error in FOS message GETPROP: bad args", ac, av);
}

/*
  REMOTE_CALL <key> <args> * 

   Get a fts data function call .
   */


static void 
fts_mess_client_remote_call(int ac, const fts_atom_t *av)
{
  trace_mess("Received remote call", ac, av);

  if ((ac >= 2) &&
      fts_is_data(&av[0]) &&
      fts_is_int(&av[1]))
    {
      fts_data_t *data;
      int key;

      data = fts_get_data(&av[0]);
      key  = fts_get_int(&av[1]);

      fts_data_call(data, key, ac - 2, av + 2);
    }
  else
    printf_mess("System Error in FOS message REMOTE_CALL: bad args", ac, av);
}

/*
  RECOMPUTE_ERRORS

  Ask to recompute all the errors.
  */

static void 
fts_mess_client_recompute_errors(int ac, const fts_atom_t *av)
{
  fts_do_recompute_errors();
}

/*
   SHUTDOWN

   Quit FTS
   */


static void 
fts_mess_client_shutdown(int ac, const fts_atom_t *av)
{
  trace_mess("Received shutdown", ac, av);

  fts_sched_halt();
}


static void fts_messtile_init(void)
{
  fts_s_download = fts_new_symbol("download");
  fts_s_load_init = fts_new_symbol("load_init");
  fts_s_setDescription = fts_new_symbol("setDescription");

  fts_client_install(SAVE_PATCHER_BMAX_CODE, fts_mess_client_save_patcher_bmax);

  fts_client_install(LOAD_PATCHER_BMAX_CODE, fts_mess_client_load_patcher_bmax);
  fts_client_install(LOAD_PATCHER_DPAT_CODE, fts_mess_client_load_patcher_dpat);

/*    fts_client_install(DECLARE_ABSTRACTION_CODE, fts_mess_client_declare_abstraction); */
/*    fts_client_install(DECLARE_ABSTRACTION_PATH_CODE, fts_mess_client_declare_abstraction_path); */

/*    fts_client_install(DECLARE_TEMPLATE_CODE, fts_mess_client_declare_template); */
/*    fts_client_install(DECLARE_TEMPLATE_PATH_CODE, fts_mess_client_declare_template_path); */

  fts_client_install(DOWNLOAD_OBJECT_CODE, fts_mess_client_download_object);
  fts_client_install(DOWNLOAD_CONNECTION_CODE, fts_mess_client_download_connection);

  fts_client_install(NEW_OBJECT_CODE,  fts_mess_client_new);
  fts_client_install(REDEFINE_PATCHER_CODE,  fts_mess_client_redefine_patcher);
  fts_client_install(REDEFINE_OBJECT_CODE,  fts_mess_client_redefine_object);
  fts_client_install(DELETE_OBJECT_CODE,  fts_mess_client_delete_object);

  fts_client_install(NEW_CONNECTION_CODE,  fts_mess_client_new_connection);
  fts_client_install(DELETE_CONNECTION_CODE, fts_mess_client_delete_connection);

  fts_client_install(MESSAGE_CODE, fts_mess_client_mess);
  fts_client_install(PUTPROP_CODE,  fts_mess_client_put_prop);
  fts_client_install(GETPROP_CODE,  fts_mess_client_get_prop);
  fts_client_install(GETALLPROP_CODE,  fts_mess_client_get_all_prop);
  fts_client_install(REMOTE_CALL_CODE,  fts_mess_client_remote_call);
  fts_client_install(RECOMPUTE_ERRORS_CODE, fts_mess_client_recompute_errors);
  fts_client_install(FTS_SHUTDOWN_CODE,  fts_mess_client_shutdown);
}


/***********************************************************************
 *
 * Initialization and shutdown
 *
 */

void fts_kernel_oldclient_init( void)
{
  fts_client_sync_init();
  fts_client_updates_init();
  fts_messtile_init();
}

void fts_kernel_oldclient_shutdown(void)
{
/*    if (client_dev) */
/*      { */
/*        fts_dev_close(client_dev); */
/*        client_dev = 0; */
/*      } */
}
