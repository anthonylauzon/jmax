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

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>

#include <fts/fts.h>

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
/* udpsend object                                                         */
/* ********************************************************************** */


typedef struct {
  fts_object_t o;
  int socket;
  struct sockaddr_in my_addr;
  protoencode_t encoder;
} udpsend_t;

static void udpsend_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  udpsend_t *this = (udpsend_t *)o;

  if(ac > 0 && fts_is_symbol(at))
    {
      int port;
      struct hostent *h;
      const char *host;
      
      host = fts_get_symbol_arg( ac, at, 1, 0);
      port = fts_get_int_arg( ac, at, 2, 0);
      
      this->socket = socket(AF_INET, SOCK_DGRAM, 0);
      
      if (this->socket == -1)
	{
	  post( "Cannot open socket (%s)\n", strerror( errno));
	  return;
	}
      
      memset( &this->my_addr, 0, sizeof( this->my_addr));
      
      this->my_addr.sin_family = AF_INET;
      
      h = gethostbyname( host);
      
      if ( !h)
	{
	  post( "udpsend: cannot find host %s\n", host);
	  
	  close( this->socket);
	  this->socket = -1;
	  
	  return;
	}
      
      this->my_addr.sin_addr.s_addr = ((struct in_addr *)h->h_addr_list[0])->s_addr;
      this->my_addr.sin_port = htons( port);
      
      protoencode_init( &this->encoder);
    }
  else
    fts_object_set_error(o, "hostname argument required");
}

static void udpsend_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  udpsend_t *this = (udpsend_t *)o;

  if ( this->socket >= 0)
    {
      close( this->socket);
    }

  protoencode_destroy( &this->encoder);
}

static void udpsend_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int i, size, r;
  udpsend_t *this = (udpsend_t *)o;
  char *buff;

  protoencode_start( &this->encoder);

  protoencode_put_string( &this->encoder, s);

  while (ac--)
    {
      if (fts_is_int( at))
	protoencode_put_int( &this->encoder, fts_get_int( at));
      else if (fts_is_float( at))
	protoencode_put_float( &this->encoder, fts_get_float( at));
      else if (fts_is_symbol( at))
	protoencode_put_string( &this->encoder, fts_get_symbol( at) );

      at++;
    }

  protoencode_end( &this->encoder);

  buff = protoencode_get_mess( &this->encoder);
  size = protoencode_get_size( &this->encoder);

  r = sendto( this->socket, buff, size, 0, &this->my_addr, sizeof(this->my_addr));

  if ( r < 0)
    {
      post( "udpsend: error sending (%s)\n", strerror( errno));
    }
}

static void 
udpsend_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof( udpsend_t), udpsend_init, udpsend_delete);

  /*fts_class_message_varargs(cl, fts_s_anything, udpsend_anything);*/
}

void udpsend_config( void)
{
  fts_class_install( fts_new_symbol("udpsend"), udpsend_instantiate);
}
