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
 *
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <fts/runtime/client/protocol.h>
#include <fts/runtime/client/protcodec.h>

/* ********************************************************************** */
/*                                                                        */
/* Protocol encoder                                                       */
/*                                                                        */
/* ********************************************************************** */

#define DEFAULT_BUFFER_SIZE 1024

void protoencode_init( protoencode_t *pr)
{
  pr->buffer_size = DEFAULT_BUFFER_SIZE;

  pr->buffer = (unsigned char *)malloc( pr->buffer_size);
  assert( pr->buffer != 0);

  pr->current = 0;
}

void protoencode_destroy( protoencode_t *pr)
{
  free( pr->buffer);
  pr->buffer = 0;
  pr->current = 0;
  pr->buffer_size = 0;
}

unsigned char *protoencode_get_mess( protoencode_t *pr)
{
  return pr->buffer;
}

int protoencode_get_size( protoencode_t *pr)
{
  return pr->current;
}

void protoencode_start( protoencode_t *pr)
{
  pr->current = 0;
}

static void protoencode_realloc_buffer( protoencode_t *pr)
{
  int newsize = 2 * pr->buffer_size;

  pr->buffer = (unsigned char *)realloc( pr->buffer, pr->buffer_size);
  assert( pr->buffer != 0);

  pr->buffer_size = newsize;
}

void protoencode_end( protoencode_t *pr)
{
  if ( pr->current + 1 >= pr->buffer_size)
    protoencode_realloc_buffer( pr);

  pr->buffer[ pr->current ] = EOM_CODE;
  pr->current++;
}

void protoencode_put_int( protoencode_t *pr, int value)
{
  unsigned char *p;

  if (pr->current + (1 + 4) >= pr->buffer_size)
    protoencode_realloc_buffer( pr);

  p = pr->buffer + pr->current;

  *p++ = INT_CODE;
  *p++ = (unsigned char) (((unsigned int) value >> 24) & 0xff);
  *p++ = (unsigned char) (((unsigned int) value >> 16) & 0xff);
  *p++ = (unsigned char) (((unsigned int) value >> 8) & 0xff);
  *p++ = (unsigned char) (((unsigned int) value >> 0) & 0xff);

  pr->current += 5;
}

void protoencode_put_float( protoencode_t *pr, float value)
{
  unsigned char *p;
  unsigned int ivalue;

  if (pr->current + (1 + 4) >= pr->buffer_size)
    protoencode_realloc_buffer( pr);

  p = pr->buffer + pr->current;
  ivalue = *((unsigned int *)&value);

  *p++ = FLOAT_CODE;
  *p++ = (unsigned char) ((ivalue >> 24) & 0xff);
  *p++ = (unsigned char) ((ivalue >> 16) & 0xff);
  *p++ = (unsigned char) ((ivalue >> 8) & 0xff);
  *p++ = (unsigned char) ((ivalue >> 0) & 0xff);

  pr->current += 5;
}

void protoencode_put_string( protoencode_t *pr, const char *s)
{
  int len = strlen( s);
  unsigned char *p;

  if (pr->current + (1 + len + 1) >= pr->buffer_size)
    protoencode_realloc_buffer( pr);

  p = pr->buffer + pr->current;

  *p++ = STRING_CODE;

  while (*s)
    {
      *p = *s;
      p++;
      s++;
    }

  *p++ = STRING_END_CODE;

  pr->current += (2 + len);
}




/* ********************************************************************** */
/*                                                                        */
/* Protocol decoder                                                       */
/*                                                                        */
/* ********************************************************************** */

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

