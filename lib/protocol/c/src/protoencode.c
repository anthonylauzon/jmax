/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */

#include <stdlib.h>
#include <assert.h>

#include "protodefs.h"
#include "protoencode.h"

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

  *p++ = STRING_START_CODE;

  while (*s)
    {
      *p = *s;
      p++;
      s++;
    }

  *p++ = STRING_END_CODE;

  pr->current += (2 + len);
}
