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
 * Authors: Norbert Schnell.
 *
 */

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
fifo_input_atom(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
      fts_fifo_reinit(&this->fifo, buffer, bytes);
      
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
fifo_input_atoms(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fifo_t *this = (fifo_t *)o;
  
  if(ac == 1)
    fifo_input_atom(o, 0, 0, 1, at);
  else if(ac > 1)
    {
      fts_tuple_t *tuple = (fts_tuple_t *)fts_object_create(fts_tuple_metaclass, ac, at);
      fts_atom_t a;
      
      fts_set_object(&a, (fts_object_t *)tuple);
      fifo_input_atom(o, 0, 0, 1, &a);
    }
}

static void
fifo_input_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(ac == 1 && s == fts_get_selector(at))
    fifo_input_atom(o, 0, 0, ac, at);
  else
    fts_object_signal_runtime_error(o, "Don't understand message %s", s);
}

static void
fifo_next(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
fifo_flush(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
fifo_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
fifo_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
fifo_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  fifo_t *this = (fifo_t *)o;

  fifo_clear(o, 0, 0, 0, 0);
  fts_free(this->buffer);
}

static fts_status_t
fifo_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fifo_t), 1, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fifo_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, fifo_delete);

  fts_method_define_varargs(cl, 0, fts_s_int, fifo_input_atom);
  fts_method_define_varargs(cl, 0, fts_s_float, fifo_input_atom);
  fts_method_define_varargs(cl, 0, fts_s_symbol, fifo_input_atom);
  fts_method_define_varargs(cl, 0, fts_s_list, fifo_input_atoms);
  fts_method_define_varargs(cl, 0, fts_s_anything, fifo_input_anything);

  fts_method_define_varargs(cl, 0, fts_s_bang, fifo_next);
  fts_method_define_varargs(cl, 0, fts_new_symbol("next"), fifo_next);
  fts_method_define_varargs(cl, 0, fts_s_flush, fifo_flush);
  fts_method_define_varargs(cl, 0, fts_s_clear, fifo_clear);

  return fts_Success;
}

void
fifo_config(void)
{
  fts_class_install(fts_new_symbol("fifo"), fifo_instantiate);
}
