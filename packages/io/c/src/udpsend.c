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
/* udpsend object                                                         */
/* ********************************************************************** */


typedef struct {
  fts_object_t o;
  fts_bytestream_t* udp_stream;
  fts_binary_protocol_t* binary_protocol;
} udpsend_t;


static void
udpsend_input(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  udpsend_t* self = (udpsend_t*)o;
  fts_binary_protocol_t* binary_protocol = self->binary_protocol;

  if (s != NULL)
  {
    /* add selector to message */
    fts_binary_protocol_add_symbol(binary_protocol, s);
  }
  fts_binary_protocol_add_atoms(binary_protocol, ac, at);
  fts_binary_protocol_end_message(binary_protocol);

  fts_bytestream_output(self->udp_stream, 
			fts_stack_size(&binary_protocol->output_buffer), 
			fts_stack_base(&binary_protocol->output_buffer));
  fts_stack_clear(&binary_protocol->output_buffer);
}

static void udpsend_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  udpsend_t *self = (udpsend_t *)o;

  /* create binary protocol */
  self->binary_protocol = (fts_binary_protocol_t*)fts_object_create(fts_binary_protocol_type, 0, NULL);
  if (self->binary_protocol == NULL)
  {
    fts_object_error(o, "Cannot create binary protocol component");
    return;
  }
  fts_object_refer((fts_object_t*)self->binary_protocol);

  /* check number of argument */
  if ((ac == 1) 
      && fts_is_object(at)
      && (fts_get_class(at) == fts_udpstream_class))
  {
    self->udp_stream = (fts_bytestream_t*)fts_get_object(at);
  }
  else
  {
    /* create udp stream */
    self->udp_stream = (fts_bytestream_t*)fts_object_create(fts_udpstream_class, ac, at);
    if (self->udp_stream == NULL)
    {
      fts_object_error(o, "Cannot create udp stream component (%s)", fts_get_error());
      return;
    }
  }
  
  /* check if bytestream is an output one */
  if (!fts_bytestream_is_output(self->udp_stream))
  {
    fts_object_error(o, "udpsend need an output udpstream");
    return;
  }

  fts_object_refer((fts_object_t*)self->udp_stream);
}

static void udpsend_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  udpsend_t *self = (udpsend_t *)o;
  if (self->binary_protocol != NULL)
  {
    fts_object_release((fts_object_t*)self->binary_protocol);
  }
  if (self->udp_stream != NULL)
  {
    fts_object_release((fts_object_t*)self->udp_stream);
  }
}

static void 
udpsend_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof( udpsend_t), udpsend_init, udpsend_delete);

  fts_class_input_handler(cl, udpsend_input);
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

