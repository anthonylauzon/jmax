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

#include "fts.h"

typedef struct {
  fts_object_t o;
  int socket;
  struct sockaddr_in my_addr;
  protoencode_t encoder;
} udpsend_t;

static void udpsend_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  udpsend_t *this = (udpsend_t *)o;
  int port;
  struct hostent *h;
  const char *host;

  host = fts_symbol_name( fts_get_symbol_arg( ac, at, 1, 0));
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

  protoencode_put_string( &this->encoder, fts_symbol_name( s) );

  while (ac--)
    {
      if (fts_is_int( at))
	protoencode_put_int( &this->encoder, fts_get_int( at));
      else if (fts_is_float( at))
	protoencode_put_float( &this->encoder, fts_get_float( at));
      else if (fts_is_symbol( at))
	protoencode_put_string( &this->encoder, fts_symbol_name( fts_get_symbol( at)) );

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

static fts_status_t udpsend_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_type_t t[3];

  fts_class_init( cl, sizeof( udpsend_t), 1, 0, 0);

  t[0] = fts_t_symbol;
  t[1] = fts_t_symbol;
  t[2] = fts_t_int;

  fts_method_define(cl, fts_SystemInlet, fts_s_init, udpsend_init, 3, t);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, udpsend_delete, 0, 0);

  fts_method_define_varargs(cl, 0, fts_s_anything, udpsend_anything);

  return fts_Success;
}

void udpsend_config( void)
{
  fts_class_install( fts_new_symbol("udpsend"), udpsend_instantiate);
}
