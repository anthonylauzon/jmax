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

#include <fts/fts.h>

/***********************************************************************
 *
 * Generic bytestream implementation
 *
 */

static fts_symbol_t fts_s_bytestream = 0;

fts_class_t *fts_memorystream_class = 0;

void fts_bytestream_init(fts_bytestream_t *stream)
{
  stream->input = 0;
  stream->listeners = 0;

  stream->output = 0;
  stream->output_char = 0;
  stream->flush = 0;
}

void fts_bytestream_destroy(fts_bytestream_t *stream)
{
  fts_bytestream_listener_t *l = stream->listeners;
  fts_bytestream_listener_t *n;

  while (l) {
    n = l->next;
    fts_free(l);
    l = n;
  }
}

void fts_bytestream_set_input(fts_bytestream_t *stream)
{
  stream->input = 1;
}

void fts_bytestream_set_output(fts_bytestream_t *stream, fts_bytestream_output_t write, fts_bytestream_output_char_t put, fts_bytestream_flush_t flush)
{
  stream->output = write;
  stream->output_char = put;
  stream->flush = flush;
}

static void
fts_bytestream_bytestream(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
}

void fts_bytestream_class_init(fts_class_t *cl)
{
  fts_class_message_varargs(cl, fts_s_bytestream, fts_bytestream_bytestream);
}

int fts_bytestream_check(fts_object_t *obj)
{
  fts_class_t *class = fts_object_get_class(obj);
  fts_method_t method = fts_class_get_method_varargs(class, fts_s_bytestream);

  return (method != NULL);
}

void fts_bytestream_input(fts_bytestream_t *stream, int n, const unsigned char *c)
{
  fts_bytestream_listener_t *l = stream->listeners;

  while(l)
    {
      fts_bytestream_listener_t *next = l->next;

      l->callback(l->listener, n, c);
      l = next;
    }
}

void fts_bytestream_add_listener(fts_bytestream_t *stream, fts_object_t *listener, fts_bytestream_callback_t fun)
{
  fts_bytestream_listener_t *l = (fts_bytestream_listener_t *)fts_malloc(sizeof(fts_bytestream_listener_t));

  l->callback = fun;
  l->listener = listener;
  l->next = stream->listeners;
  
  stream->listeners = l;      
}

void fts_bytestream_remove_listener(fts_bytestream_t *stream, fts_object_t *listener)
{
  fts_bytestream_listener_t *l = stream->listeners;

  if(l)
    {
      fts_bytestream_listener_t *freeme = 0;
      
      if(l && l->listener == listener)
	{
	  freeme = l;
	  stream->listeners = l->next;
	}
      else
	{
	  while(l->next)
	    {
	      if(l->next->listener == listener)
		{
		  freeme = l->next;
		  l->next = l->next->next;
		  
		  break;
		}
	      
	      l = l->next;
	    }
	}
      
      /* free removed listener */
      if(freeme)
	fts_free(freeme);
    }
}

/***********************************************************************
 *
 * Memory bytestream
 * (the object that implements an output stream writing into an array of bytes)
 *
 */

struct _fts_memorystream_t {
  fts_bytestream_t bytestream;
  fts_stack_t output_buffer;
  int input_size;
};

static void fts_memorystream_output(fts_bytestream_t *stream, int n, const unsigned char *c)
{
  fts_memorystream_t *this = (fts_memorystream_t *) stream;
  int i;

  for ( i = 0; i < n; i++)
    fts_stack_push( &this->output_buffer, unsigned char, c[i]);
}

static void fts_memorystream_output_char(fts_bytestream_t *stream, unsigned char c)
{
  fts_memorystream_output(stream, 1, &c);
}

static void fts_memorystream_flush(fts_bytestream_t *stream)
{
}

static void fts_memorystream_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_memorystream_t *this = (fts_memorystream_t *) o;

  fts_bytestream_init((fts_bytestream_t *) this);

  fts_bytestream_set_output((fts_bytestream_t *) this, 
			    fts_memorystream_output,
			    fts_memorystream_output_char,
			    fts_memorystream_flush);
  
  fts_stack_init( &this->output_buffer, unsigned char);
}

static void fts_memorystream_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_memorystream_t *this = (fts_memorystream_t *) o;

  fts_bytestream_destroy((fts_bytestream_t *) this);

  fts_stack_destroy( &this->output_buffer);
}

static void fts_memorystream_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_memorystream_t), fts_memorystream_init, fts_memorystream_delete);
  fts_bytestream_class_init(cl);
}

unsigned char *fts_memorystream_get_bytes( fts_memorystream_t *stream)
{
  return (unsigned char *)fts_stack_base( &stream->output_buffer);
}

void fts_memorystream_reset( fts_memorystream_t *stream)
{
  fts_stack_clear( &stream->output_buffer);
}


/***********************************************************************
 *
 * Initialization
 *
 */

void fts_kernel_bytestream_init( void)
{
  fts_s_bytestream = fts_new_symbol("bytestream");

  fts_memorystream_class = fts_class_install( fts_new_symbol("memorystream"), fts_memorystream_instantiate);
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
