/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
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

struct _protoencode_t {
  unsigned char *buffer;
  int buffer_size;
  int current;
};


void protoencode_init( protoencode_t *pr, int buffer_size)
{
  pr->buffer = (unsigned char *)malloc( buffer_size);
  assert( pr->buffer != 0);

  pr->buffer_size = buffer_size;
  pr->current = 0;
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

void protoencode_end( protoencode_t *pr)
{
  pr->buffer[ pr->current ] = EOM_CODE;
  pr->current++;
}

void protoencode_put_int( protoencode_t *pr, int value)
{
  if (pr->current + 5 < pr->buffer_size)
    {
      unsigned char *p = pr->buffer + pr->current;

      *p++ = INT_CODE;
      *p++ = (unsigned char) (((unsigned int) value >> 24) & 0xff);
      *p++ = (unsigned char) (((unsigned int) value >> 16) & 0xff);
      *p++ = (unsigned char) (((unsigned int) value >> 8) & 0xff);
      *p++ = (unsigned char) (((unsigned int) value >> 0) & 0xff);

      pr->current += 5;
    }
}

void protoencode_put_string( protoencode_t *pr, const char *s)
{
  int len = strlen( s);

  if (pr->current + 2 + len < pr->buffer_size)
    {
      unsigned char *p = pr->buffer + pr->current;

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
}
