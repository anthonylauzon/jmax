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
 * Based on Max/ISPW by Miller Puckette.
 *
 *
 */

#include <string.h>
#include <stdio.h>

#include <fts/fts.h>

#include "jmax25_protocol.h"

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
    if ( b == JMAX25_INT_CODE)
      {
	pr->state = STATE_IN_INT;
	pr->counter = 4;
	pr->int_value = 0;
	return RUNNING;
      }
    else if ( b == JMAX25_FLOAT_CODE)
      {
	unsigned int zero = 0;

	pr->state = STATE_IN_FLOAT;
	pr->counter = 4;
	pr->float_value = *((float *)&zero);
	return RUNNING;
      }
    else if ( b == JMAX25_STRING_CODE)
      {
	pr->state = STATE_IN_STRING;
	pr->string_len = 0;;
	return RUNNING;
      }
    else if ( b == JMAX25_EOM_CODE)
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
    if ( b != JMAX25_STRING_END_CODE)
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
  fts_bytestream_t* udp_stream;
  char buffer[UDP_PACKET_SIZE];
} udpreceive_t;

#define MAXATOMS 1024

static void udpreceive_receive( fts_object_t *o, int size, const unsigned char* buffer)
{
  udpreceive_t *self = (udpreceive_t *)o;
  fts_atom_t argv[MAXATOMS];
  fts_symbol_t selector = 0;
  int i, argc, first_token;
  protodecode_t pr;

/*   size = recvfrom( self->socket, self->buffer, UDP_PACKET_SIZE, 0, NULL, NULL); */

  if ( size <= 0)
    return;

  protodecode_init( &pr);
  argc = 0;
  first_token = 0;

  for ( i = 0; i < size; i++)
  {
    protodecode_status_t t;
    
    t = protodecode_run( &pr, buffer[i]);
    
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
	/* 
	   jMax 2.5.x use stype symbol as first selector, so we need to remove 
	   int and float symbl before sending it 
	*/
	if ((selector == fts_s_int)
	    || (selector == fts_s_float)
	    || (selector == fts_s_list))
	{
	  selector = NULL;
	}
	
	if (selector == fts_s_bang)
	{
	  selector = NULL;
	  fts_set_void(&argv[argc]);
	  argc++;
	}
	first_token = 1;
      }
      else
      {
	fts_set_symbol( &(argv[argc]), fts_new_symbol(protodecode_get_string( &pr)));
	argc++;
      }
    }
    else if ( t == EOM_TOKEN)
      break;
  }
  
  fts_outlet_send( (fts_object_t *)self, 0, selector, argc, argv);
}


static void udpreceive_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  udpreceive_t *self = (udpreceive_t *)o;
  fts_bytestream_t* udp_stream;

  /* check number of argument */
  if ((ac == 1) 
      && fts_is_object(at)
      && (fts_get_class(at) == fts_udpstream_class))
  {
    udp_stream = (fts_bytestream_t*)fts_get_object(at);
  }
  else
  {
    /* create udp stream */
    udp_stream = (fts_bytestream_t*)fts_object_create(fts_udpstream_class, ac, at);
    if (udp_stream == NULL)
    {
      fts_object_error(o, "Cannot create udp stream component (%s)", fts_get_error());
      return;
    }
  }

  /* check if bytestream is an input one */
  if (!fts_bytestream_is_input(udp_stream))
  {
    fts_object_error(o, "udpreceive need an input udpstream");
    return;
  }
  self->udp_stream = udp_stream;
  fts_object_refer((fts_object_t*)self->udp_stream);

  /* 1 outlet for sending received message */
  fts_object_set_outlets_number(o, 1);

  fts_bytestream_add_listener(self->udp_stream, (fts_object_t*)self, udpreceive_receive);

}

static void udpreceive_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  udpreceive_t *self = (udpreceive_t *)o;

  if (self->udp_stream != NULL)
  {
    fts_bytestream_remove_listener(self->udp_stream, o);
    fts_object_release((fts_object_t*)self->udp_stream);
  }
}

static void udpreceive_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(udpreceive_t), udpreceive_init, udpreceive_delete);
}

void udpreceive_config( void)
{
  fts_class_install( fts_new_symbol("udpreceive"), udpreceive_instantiate);
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
