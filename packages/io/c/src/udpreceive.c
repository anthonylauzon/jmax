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

#include <string.h>

#include <stdio.h>

#include <fts/fts.h>


/* ********************************************************************** */
/* udpreceive object                                                      */
/* ********************************************************************** */


#define UDP_PACKET_SIZE 512

typedef struct {
  fts_object_t o;
  fts_bytestream_t* udp_stream;
  fts_binary_protocol_t* binary_protocol;
} udpreceive_t;

#define MAXATOMS 1024

static void udpreceive_receive( fts_object_t *o, int size, const unsigned char* buffer)
{
  udpreceive_t *self = (udpreceive_t *)o;
  fts_symbol_t selector = NULL;
  int argc;
  fts_atom_t *argv;
  fts_binary_protocol_t* binary_protocol = self->binary_protocol;

  if ( size <= 0)
    return;

  if (fts_binary_protocol_decode(binary_protocol, size, buffer) != size)
  {
    fts_log("[udpreceive] error in protocol decoding \n");
    fts_object_error(o, "error in protocol decoding");
    return;
  }

  argc = fts_stack_size( &binary_protocol->input_args);
  argv = (fts_atom_t *)fts_stack_base( &binary_protocol->input_args);

  fts_outlet_send( (fts_object_t *)self, 0, 0, argc, argv);
  fts_stack_clear(&binary_protocol->input_args);
}


static void udpreceive_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  udpreceive_t *self = (udpreceive_t *)o;

  /* create binary protocol decoder */
  self->binary_protocol = (fts_binary_protocol_t*)fts_object_create(fts_binary_protocol_type, 0, NULL);
  fts_object_refer((fts_object_t*)self->binary_protocol);

  /* check number of argument */
  if ((ac == 1) 
      && fts_is_object(at)
      && (fts_get_class(at) == fts_udpstream_class))
  {
    self->udp_stream = (fts_bytestream_t*)fts_get_object(at);
    fts_object_refer((fts_object_t*)self->udp_stream);
  }
  else
  {
    /* create udp stream */
    self->udp_stream = (fts_bytestream_t*)fts_object_create(fts_udpstream_class, ac, at);
    if (self->udp_stream != NULL)
    {
      fts_object_refer((fts_object_t*)self->udp_stream);
    }
    else
    {
      fts_object_error(o, "Cannot create udp stream component");
    }
  }

  /* 1 outlet for sending received message */
  fts_object_set_outlets_number(o, 1);

  fts_bytestream_add_listener(self->udp_stream, (fts_object_t*)self, udpreceive_receive);
}

static void udpreceive_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  udpreceive_t *self = (udpreceive_t *)o;
  fts_object_release((fts_object_t*)self->binary_protocol);
  fts_object_release((fts_object_t*)self->udp_stream);
}

static void udpreceive_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof( udpreceive_t), udpreceive_init, udpreceive_delete);
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

