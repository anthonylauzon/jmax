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
#include <string.h>
#include <ctype.h>

#include <stdio.h>

#include <fts/fts.h>


/* ********************************************************************** */
/* udpreceive object                                                      */
/* ********************************************************************** */


#define UDP_PACKET_SIZE 512

typedef struct {
  fts_object_t o;
  int socket;
  char buffer[UDP_PACKET_SIZE];
  fts_binary_protocol_t* binary_protocol;
} udpreceive_t;

#define MAXATOMS 1024

static void udpreceive_receive( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  udpreceive_t *self = (udpreceive_t *)o;
  fts_symbol_t selector = NULL;
  int argc;
  fts_atom_t *argv;
  fts_binary_protocol_t* binary_protocol = self->binary_protocol;
  int size;

  size = recvfrom( self->socket, self->buffer, UDP_PACKET_SIZE, 0, NULL, NULL);

  if ( size <= 0)
    return;

  if (fts_binary_protocol_decode(binary_protocol, size, self->buffer) != size)
  {
    fts_log("[udpreceive] error in protocol decoding \n");
    fts_object_error(o, "error in protocol decoding");
    return;
  }

  argc = fts_stack_size( &binary_protocol->input_args);
  argv = (fts_atom_t *)fts_stack_base( &binary_protocol->input_args);

  selector = fts_get_symbol( argv+1);
  argc -= 2;
  argv += 2;

  fts_outlet_send( (fts_object_t *)self, 0, selector, argc, argv);
}


static void udpreceive_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  udpreceive_t *self = (udpreceive_t *)o;
  int port = fts_get_int_arg( ac, at, 0, 0);
  struct sockaddr_in addr;

  self->binary_protocol = (fts_binary_protocol_t*)fts_object_create(fts_binary_protocol_type, 0, NULL);
  fts_object_refer((fts_object_t*)self->binary_protocol);

  self->socket = socket(AF_INET, SOCK_DGRAM, 0);

  if (self->socket == -1)
    {
      post( "Cannot open socket\n");
      return;
    }

  memset( &addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);

  if ( bind( self->socket, (const struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1)
    {
      post( "Cannot bind socket\n");
      close( self->socket);
      return;
    }

  /* 1 outlet for sending received message */
  fts_object_set_outlets_number(o, 1);

  fts_sched_add( (fts_object_t *)self, FTS_SCHED_READ, self->socket);
}

static void udpreceive_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  udpreceive_t *self = (udpreceive_t *)o;
  fts_object_release((fts_object_t*)self->binary_protocol);

  if ( self->socket >= 0)
    {
      fts_sched_remove( (fts_object_t *)self);
      close( self->socket);
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


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */

