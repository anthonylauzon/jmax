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

#define UDP_PACKET_SIZE 512


typedef struct
{
  fts_object_t head;
  fts_bytestream_t* udp_stream;
} udp_t;


static void
udp_receive(fts_object_t* o, int size, const unsigned char* buffer)
{
  udp_t* self = (udp_t*)o;
  fts_atom_t a;
  
  fts_set_symbol(&a, fts_new_symbol(buffer));
  fts_outlet_send(o, 0, NULL, 1, &a);
}

static void
udpout_symbol(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  udp_t* self = (udp_t*)o;

  /* here we have s=0x0, ac = 1 and fts_is_symbol(at) is true */
  fts_bytestream_output(self->udp_stream, strlen(fts_get_symbol(at)) + 1, fts_get_symbol(at));
  fts_bytestream_flush(self->udp_stream);
}

static void
udpout_bang(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  udp_t* self = (udp_t*)o;
  fts_bytestream_output(self->udp_stream, strlen(fts_s_bang), fts_s_bang);
  fts_bytestream_flush(self->udp_stream);
}

static void
udpout_send(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  udp_t* self = (udp_t*)o;
  fts_symbol_t mess_to_send;
  if (!fts_is_symbol(at))
  {
    fts_object_error(o,"send message requires a symbol as argument");
    return;
  }

  mess_to_send = fts_get_symbol(at);
  fts_bytestream_output(self->udp_stream, strlen(mess_to_send) + 1, mess_to_send);
  fts_bytestream_flush(self->udp_stream);
}


static void
udpin_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  udp_t* self = (udp_t*)o;

  if (self->udp_stream != NULL)
  {
    fts_bytestream_remove_listener(self->udp_stream, (fts_object_t*)self);
    fts_object_release((fts_object_t*)self->udp_stream);
  }
}

static void
udpin_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  udp_t *self = (udp_t *)o;
  fts_bytestream_t* udp_stream;
  /* check number of argument */
  if ((ac == 1) 
      && fts_is_object(at)
      && (fts_get_class(at) == fts_udpstream_class))
  {
    udp_stream = (fts_bytestream_t*)fts_get_object(at);
    if (!fts_bytestream_is_input(udp_stream))
    {
      fts_object_error(o, "udpin need an input udpstream as argument");
      return;
    }
    else
    {
      self->udp_stream = udp_stream;
      fts_object_refer((fts_object_t*)self->udp_stream);
    }
  }
  else
  {
    /* create udp stream */
    udp_stream = (fts_bytestream_t*)fts_object_create(fts_udpstream_class, ac, at);
    if (udp_stream != NULL)
    {
      if (!fts_bytestream_is_input(udp_stream))
      {
	fts_object_error(o, "cannot create an input udpstream with given arguments");
      }
      else
      {
	self->udp_stream = udp_stream;
	fts_object_refer((fts_object_t*)self->udp_stream);
      }
    }
    else
    {
      fts_object_error(o, "Cannot create udp stream component");
      return;
    }
  }

  fts_object_set_outlets_number(o, 1);
  
  fts_bytestream_add_listener(self->udp_stream, (fts_object_t*)self, udp_receive);
}

static void
udpout_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  udp_t *self = (udp_t *)o;
  if (self->udp_stream != NULL)
  {
    fts_object_release((fts_object_t*)self->udp_stream);
  }
}


static void
udpout_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  udp_t *self = (udp_t *)o;
  fts_bytestream_t* udp_stream;

  /* check number of argument */
  if ((ac == 1) 
      && fts_is_object(at)
      && (fts_get_class(at) == fts_udpstream_class))
  {
    udp_stream = (fts_bytestream_t*)fts_get_object(at);
    if (!fts_bytestream_is_output(udp_stream))
    {
      fts_object_error(o,"udpout need an output udpstream as arguments");
      return;
    }
    else
    {
      self->udp_stream = udp_stream;
      fts_object_refer((fts_object_t*)self->udp_stream);
    }
  }
  else
  {
    /* create udp stream */
    udp_stream = (fts_bytestream_t*)fts_object_create(fts_udpstream_class, ac, at);
    if (udp_stream != NULL)
    {
      if (!fts_bytestream_is_output(udp_stream))
      {
	fts_object_error(o, "cannot create an output udpstream with given arguments");
      }
      else
      {
	self->udp_stream = udp_stream;
	fts_object_refer((fts_object_t*)self->udp_stream);
      }
    }
    else
    {
      fts_object_error(o, "Cannot create udp stream component");
      return;
    }
  }
}


static void
udpin_instantiate(fts_class_t* cl)
{
  fts_class_init(cl, sizeof(udp_t), udpin_init, udpin_delete);

  /* name support */
  fts_class_message_varargs(cl, fts_s_name, fts_name_set_method); 
  fts_class_message_varargs(cl, fts_s_update_gui, fts_name_gui_method); 
  fts_class_message_varargs(cl, fts_s_dump, fts_name_dump_method); 
}

static void
udpout_instantiate(fts_class_t* cl)
{
  fts_class_init(cl, sizeof(udp_t), udpout_init, udpout_delete);

  fts_class_message_varargs(cl, fts_s_send, udpout_send);

  fts_class_inlet_symbol(cl, 0, udpout_symbol);
  fts_class_inlet_bang(cl, 0, udpout_bang);   

  /* name support */
  fts_class_message_varargs(cl, fts_s_name, fts_name_set_method); 
  fts_class_message_varargs(cl, fts_s_update_gui, fts_name_gui_method); 
  fts_class_message_varargs(cl, fts_s_dump, fts_name_dump_method); 
}


void
udp_config( void)
{
  fts_class_install( fts_new_symbol("udpin"), udpin_instantiate);
  fts_class_install( fts_new_symbol("udpout"), udpout_instantiate);
}


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
