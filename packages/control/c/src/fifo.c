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

#include <fts/fts.h>

#define FIFO_BLOCK_SIZE 256

typedef struct 
{
  fts_object_t o;
  void *buffer;
  fts_fifo_t fifo;
  int size;
} fifo_t;

/************************************************************
 *
 *  user methods
 *
 */

static void
fifo_reinit( fts_fifo_t *fifo, void *buffer, int size)
{
  if(buffer != NULL)
    {
      if(fifo->write_index >= fifo->read_index)
	{
	  int fill = fifo->write_index - fifo->read_index;

	  if(fill > size)
	    fill = size;

	  memcpy(buffer, (void *)fifo->buffer, fill);

	  fifo->read_index = 0;
	  fifo->write_index = fill;
	}
      else
	{
	  int head = fifo->size - fifo->read_index;
	  int tail;

	  if(head > size)
	    head = size;

	  memcpy(buffer, (char *)fifo->buffer + fifo->read_index, head);

	  tail = fifo->write_index;
	  if(head + tail > size)
	    tail = size - head;

	  memcpy((char *)buffer + head, (void *)fifo->buffer, tail);

	  fifo->read_index = 0;
	  fifo->write_index = head + tail;
	}

      fifo->buffer = buffer;
      fifo->size = size;
    }    
  else
    { 
      fifo->read_index = 0;
      fifo->write_index = 0;
    }
}

static void
fifo_input(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fifo_t *this = (fifo_t *)o;

  /* make sure that there is enough space */
  if(fts_fifo_write_level(&this->fifo) < sizeof(fts_atom_t))
    {
      int size = this->size + FIFO_BLOCK_SIZE;
      int bytes = size * sizeof(fts_atom_t);
      void *buffer = fts_malloc(bytes);
      int i;

      /* reset fifo to new buffer */
      fifo_reinit(&this->fifo, buffer, bytes);
      
      /* free old buffer */
      fts_free(this->buffer);

      for(i=this->size-1; i<size; i++)
	fts_set_void(((fts_atom_t *)buffer) + i);

      this->buffer = buffer;
      this->size = size;
    }

  fts_atom_assign((fts_atom_t *)fts_fifo_write_pointer(&this->fifo), at);
  fts_fifo_incr_write(&this->fifo, sizeof(fts_atom_t));  
}

static void
fifo_next(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fifo_t *this = (fifo_t *)o;
  int n = 1;
  int i;

  if(ac > 0 && fts_is_number(at))
    {
      n = fts_get_number_int(at);
      
      if(n < 0)
	n = 0;
    }

  for(i=0; i<n && fts_fifo_read_level(&this->fifo) >= sizeof(fts_atom_t); i++)
    {
      fts_atom_t *atom = (fts_atom_t *)fts_fifo_read_pointer(&this->fifo);

      fts_outlet_atom(o, 0, atom);
      fts_atom_void(atom);

      fts_fifo_incr_read(&this->fifo, sizeof(fts_atom_t));
    }
}

static void
fifo_flush(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fifo_t *this = (fifo_t *)o;

  while(fts_fifo_read_level(&this->fifo) >= sizeof(fts_atom_t))
    {
      fts_atom_t *atom = (fts_atom_t *)fts_fifo_read_pointer(&this->fifo);
      
      fts_outlet_atom(o, 0, atom);
      fts_atom_void(atom);
      
      fts_fifo_incr_read(&this->fifo, sizeof(fts_atom_t));
    }
}

static void
fifo_clear(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fifo_t *this = (fifo_t *)o;

  while(fts_fifo_read_level(&this->fifo) >= sizeof(fts_atom_t))
    {
      fts_atom_void((fts_atom_t *)fts_fifo_read_pointer(&this->fifo));      
      fts_fifo_incr_read(&this->fifo, sizeof(fts_atom_t));
    }
}

/************************************************************
 *
 *  class
 *
 */
static void
fifo_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{ 
  fifo_t *this = (fifo_t *)o;
  int size = 0;
  int bytes;
  fts_atom_t *atoms;
  int i;
  
  if(ac > 0 && fts_is_number(at))
    size = fts_get_number_int(at);

  if(size <= 0)
    size = FIFO_BLOCK_SIZE;

  bytes = size * sizeof(fts_atom_t);
  this->buffer = fts_malloc(bytes);

  fts_fifo_init(&this->fifo, this->buffer, bytes);
  atoms = (fts_atom_t *)fts_fifo_get_buffer(&this->fifo);

  for(i=0; i<size; i++) 
    fts_set_void(atoms + i);

  this->size = size;
}

static void
fifo_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{ 
  fifo_t *this = (fifo_t *)o;

  fifo_clear(o, 0, 0, 0, 0);
  fts_free(this->buffer);
}

static void
fifo_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fifo_t), fifo_init, fifo_delete);

  fts_class_message_void(cl, fts_s_next, fifo_next);
  fts_class_message_void(cl, fts_s_flush, fifo_flush);
  fts_class_message_void(cl, fts_s_clear, fifo_clear);

  fts_class_inlet_bang(cl, 0, fifo_next);
  fts_class_inlet_atom(cl, 1, fifo_input);

  fts_class_outlet_atom(cl, 0);
}

void
fifo_config(void)
{
  fts_class_install(fts_new_symbol("fifo"), fifo_instantiate);
}
