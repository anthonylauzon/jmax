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
 */

#include <string.h>
#include <stdio.h>

#include <fts/fts.h>

#include "jmax25_protocol.h"

/* ********************************************************************** */
/* Protocol encoder                                                       */
/* ********************************************************************** */

typedef struct _protoencode_t {
  unsigned char *buffer;
  int buffer_size;
  int current;
} protoencode_t;

#define DEFAULT_BUFFER_SIZE 1024

void protoencode_init( protoencode_t *pr)
{
  pr->buffer_size = DEFAULT_BUFFER_SIZE;

  pr->buffer = (unsigned char *)fts_malloc( pr->buffer_size);

  pr->current = 0;
}

void protoencode_destroy( protoencode_t *pr)
{
  fts_free( pr->buffer);
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

  pr->buffer = (unsigned char *)fts_realloc( pr->buffer, pr->buffer_size);

  pr->buffer_size = newsize;
}

void protoencode_end( protoencode_t *pr)
{
  if ( pr->current + 1 >= pr->buffer_size)
    protoencode_realloc_buffer( pr);

  pr->buffer[ pr->current ] = JMAX25_EOM_CODE;
  pr->current++;
}

void protoencode_put_int( protoencode_t *pr, int value)
{
  unsigned char *p;

  if (pr->current + (1 + 4) >= pr->buffer_size)
    protoencode_realloc_buffer( pr);

  p = pr->buffer + pr->current;

  *p++ = JMAX25_INT_CODE;
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

  *p++ = JMAX25_FLOAT_CODE;
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

  *p++ = JMAX25_STRING_CODE;

  while (*s)
    {
      *p = *s;
      p++;
      s++;
    }

  *p++ = JMAX25_STRING_END_CODE;

  pr->current += (2 + len);
}

void protoencode_put_tuple(protoencode_t *pr, fts_tuple_t* tup)
{
  int i;
  int size = fts_tuple_get_size(tup);
  fts_atom_t* at;
  protoencode_put_string(pr, fts_s_list);
  for (i = 0; i < size; i++)
  {
    at = fts_tuple_get_element(tup, i);
    if (fts_is_int(at))
    {
      protoencode_put_int(pr, fts_get_int(at));
    }
    if (fts_is_float(at))
    {
      protoencode_put_float(pr, fts_get_float(at));
    }
    if (fts_is_symbol(at))
    {
      protoencode_put_string(pr, fts_get_symbol(at));
    }
  }  

}
/* ********************************************************************** */
/* udpsend object                                                         */
/* ********************************************************************** */


typedef struct {
  fts_object_t o;
  fts_bytestream_t* udp_stream;
  protoencode_t encoder;
} udpsend_t;

static void udpsend_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  udpsend_t *self = (udpsend_t *)o;
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
  
  /* check if bytestream is an output one */
  if (!fts_bytestream_is_output(udp_stream))
  {
    fts_object_error(o, "udpsend need an output udpstream");
    return;
  }

  self->udp_stream = udp_stream;
  fts_object_refer((fts_object_t*)self->udp_stream);

  protoencode_init( &self->encoder);
}

static void udpsend_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  udpsend_t *self = (udpsend_t *)o;

  if (self->udp_stream != NULL)
  {
    fts_object_release((fts_object_t*)self->udp_stream);
  }

  protoencode_destroy( &self->encoder);
}

static void udpsend_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int i, size, r;
  udpsend_t *self = (udpsend_t *)o;
  char *buff;
  fts_symbol_t selector = s;
  protoencode_start( &self->encoder);

  if (NULL != selector)
  {
    protoencode_put_string( &self->encoder, selector);
  }
  else if (0 == ac)
  {
    /* this is a bang */
    protoencode_put_string(&self->encoder, fts_s_bang);
  }

  while (ac--)
  {
    if (fts_is_int( at))
    {
      if (selector == NULL)
      {
	selector = fts_s_int;
	protoencode_put_string(&self->encoder, selector);
      }
      protoencode_put_int( &self->encoder, fts_get_int( at));
    }
    else if (fts_is_float( at))
    {
      if (selector == NULL)
      {
	selector = fts_s_float;
	protoencode_put_string(&self->encoder, selector);
      }
      protoencode_put_float( &self->encoder, fts_get_float( at));
    }
    else if (fts_is_symbol( at))
    {
      protoencode_put_string( &self->encoder, fts_get_symbol( at));
    }
    if (fts_is_tuple(at))
    {
      protoencode_put_tuple(&self->encoder, (fts_tuple_t*)fts_get_object(at));
    }
    at++;
  }
  
  protoencode_end( &self->encoder);
  
  buff = protoencode_get_mess( &self->encoder);
  size = protoencode_get_size( &self->encoder);
  
  fts_bytestream_output(self->udp_stream, size, buff);
}

static void
udpsend_instantiate(fts_class_t *cl)
{
  fts_class_init( cl, sizeof( udpsend_t), udpsend_init, udpsend_delete);

  fts_class_input_handler(cl, udpsend_anything);
}

void udpsend_config( void)
{
  fts_class_install( fts_new_symbol("udpsend"), udpsend_instantiate);
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
