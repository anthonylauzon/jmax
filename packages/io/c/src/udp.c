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

#include <stdio.h>

#include <fts/fts.h>

#define UDP_PACKET_SIZE 512

typedef struct {
    fts_object_t o;
    int socket;
    char buffer[UDP_PACKET_SIZE];
} udp_t;

static void udp_receive( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  udp_t *this = (udp_t *)o;
  int size;

  size = recvfrom( this->socket, this->buffer, UDP_PACKET_SIZE, 0, NULL, NULL);
  if ( size > 0)
    {
      int i;

      for ( i = 0; i < size; i++)
	fts_outlet_int( (fts_object_t *)this, 0, this->buffer[i]);
    }
}

static void udp_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  udp_t *this = (udp_t *)o;
  int port;
  struct sockaddr_in addr;

  port = fts_get_int_arg( ac, at, 1, 0);

  post( "Created UDP object on port %d\n", port);

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

static void udp_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  udp_t *this = (udp_t *)o;

  if ( this->socket >= 0)
    {
      fts_sched_remove( (fts_object_t *)this);

      close( this->socket);
    }
}

static fts_status_t udp_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_type_t t[2];

  fts_class_init( cl, sizeof( udp_t), 0, 1, 0);

  t[0] = fts_t_symbol;
  t[1] = fts_t_int;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, udp_init, 2, t);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, udp_delete, 0, 0);
  fts_method_define_varargs( cl, fts_SystemInlet, fts_s_sched_ready, udp_receive);

  t[0] = fts_t_int;
  fts_outlet_type_define( cl, 0, fts_type_get_selector(fts_t_int), 1, t);

  return fts_Success;
}

void udp_config( void)
{
  fts_class_install( fts_new_symbol("udp"), udp_instantiate);
}


