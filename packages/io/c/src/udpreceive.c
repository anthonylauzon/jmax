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
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>

#include <stdio.h>

#include <fts/fts.h>

/* ********************************************************************** */
/* Protocol decoder                                                       */
/* ********************************************************************** */

enum decode_state { 
  STATE_IN_TYPE,
  STATE_IN_INT,
  STATE_IN_FLOAT,
  STATE_IN_STRING
};

#define STRING_MAX 256

typedef struct _protodecode_t {
  enum decode_state state;
  int counter;
  int int_value;
  float float_value;
  int string_len;
  char string_value[STRING_MAX];
} protodecode_t;

typedef enum {
  INT_TOKEN,
  FLOAT_TOKEN,
  STRING_TOKEN,
  EOM_TOKEN,
  RUNNING
} protodecode_status_t;


void protodecode_init( protodecode_t *pr)
{
  pr->state = STATE_IN_TYPE;
}

protodecode_status_t protodecode_run( protodecode_t *pr, unsigned char b)
{
  int ivalue;

  switch( pr->state) {
  case STATE_IN_TYPE:
    if ( b == INT_CODE)
      {
	pr->state = STATE_IN_INT;
	pr->counter = 4;
	pr->int_value = 0;
	return RUNNING;
      }
    else if ( b == FLOAT_CODE)
      {
	unsigned int zero = 0;

	pr->state = STATE_IN_FLOAT;
	pr->counter = 4;
	pr->float_value = *((float *)&zero);
	return RUNNING;
      }
    else if ( b == STRING_CODE)
      {
	pr->state = STATE_IN_STRING;
	pr->string_len = 0;;
	return RUNNING;
      }
    else if ( b == EOM_CODE)
      return EOM_TOKEN;
    break;

  case STATE_IN_INT:
    pr->int_value = (pr->int_value << 8) | b;

    pr->counter--;
    if (pr->counter == 0)
      {
	pr->state = STATE_IN_TYPE;
	return INT_TOKEN;
      }
    else
      return RUNNING;

  case STATE_IN_FLOAT:
    ivalue = *((unsigned int *)&(pr->float_value));
    ivalue = (ivalue << 8) | b;
    pr->float_value = *((float *)&ivalue);

    pr->counter--;
    if (pr->counter == 0)
      {
	pr->state = STATE_IN_TYPE;
	return FLOAT_TOKEN;
      }
    else
      return RUNNING;

  case STATE_IN_STRING:
    if ( b != STRING_END_CODE)
      {
	/* TODO: should realloc() on buffer overflow */
	if ( pr->string_len < STRING_MAX)
	  {
	    pr->string_value[ pr->string_len] = (char)b;
	    pr->string_len++;
	  }
	
	return RUNNING;
      }
    else 
      {
	pr->string_value[ pr->string_len ] = '\0';
	pr->state = STATE_IN_TYPE;

	return STRING_TOKEN;
      }
  }

  return RUNNING;
}

int protodecode_get_int( protodecode_t *pr)
{
  return pr->int_value;
}

float protodecode_get_float( protodecode_t *pr)
{
  return pr->float_value;
}

char *protodecode_get_string( protodecode_t *pr)
{
  return pr->string_value;
}

/* ********************************************************************** */
/* udpreceive object                                                      */
/* ********************************************************************** */


#define UDP_PACKET_SIZE 512

typedef struct {
  fts_object_t o;
  int socket;
  char buffer[UDP_PACKET_SIZE];
} udpreceive_t;

#define MAXATOMS 1024

static void udpreceive_receive( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  udpreceive_t *this = (udpreceive_t *)o;
  fts_atom_t argv[MAXATOMS];
  fts_symbol_t selector = NULL;
  int size, i, argc, first_token;
  protodecode_t pr;

  size = recvfrom( this->socket, this->buffer, UDP_PACKET_SIZE, 0, NULL, NULL);

  if ( size <= 0)
    return;

  protodecode_init( &pr);
  argc = 0;
  first_token = 0;

  for ( i = 0; i < size; i++)
    {
      protodecode_status_t t;

      t = protodecode_run( &pr, this->buffer[i]);

      if ( t == INT_TOKEN)
	{
	  if (first_token == 0)
	    first_token = 1;

	  fts_set_int( &(argv[argc]), protodecode_get_int( &pr));
	  argc++;
	}
      else if ( t == FLOAT_TOKEN)
	{
	  if (first_token == 0)
	    first_token = 1;

	  fts_set_float( &(argv[argc]), protodecode_get_float( &pr));
	  argc++;
	}
      else if ( t == STRING_TOKEN)
	{
	  if (first_token == 0)
	    {
	      selector = fts_new_symbol( protodecode_get_string( &pr));
	      first_token = 1;
	    }
	  else
	    {
	      fts_set_symbol( &(argv[argc]), fts_new_symbol( protodecode_get_string( &pr)));
	      argc++;
	    }
	}
      else if ( t == EOM_TOKEN)
	break;
    }

  fts_outlet_send( (fts_object_t *)this, 0, selector, argc, argv);
}


static void udpreceive_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  udpreceive_t *this = (udpreceive_t *)o;
  int port = fts_get_int_arg( ac, at, 0, 0);
  struct sockaddr_in addr;

  this->socket = socket(AF_INET, SOCK_DGRAM, 0);

  if (this->socket == -1)
    {
      post( "Cannot open socket\n");
      return;
    }

  memset( &addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);

  if ( bind( this->socket, &addr, sizeof(struct sockaddr_in)) == -1)
    {
      post( "Cannot bind socket\n");
      close( this->socket);
      return;
    }

  fts_sched_add( (fts_object_t *)this, FTS_SCHED_READ, this->socket);
}

static void udpreceive_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  udpreceive_t *this = (udpreceive_t *)o;

  if ( this->socket >= 0)
    {
      fts_sched_remove( (fts_object_t *)this);
      close( this->socket);
    }
}

static void udpreceive_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof( udpreceive_t), udpreceive_init, udpreceive_delete);

  fts_class_message_varargs(cl, fts_s_sched_ready, udpreceive_receive);
}

void udpreceive_config( void)
{
  fts_class_install( fts_new_symbol("udpreceive"), udpreceive_instantiate);
}
