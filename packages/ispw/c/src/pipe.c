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
#include <fts/fts.h>

typedef struct _pipe_
{
  fts_object_t o;
  int ac;
  fts_atom_t *at;
  double delay;
  fts_heap_t *heap;
} pipe_t;

static void
pipe_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pipe_t *self = (pipe_t *)o;
  fts_atom_t *atoms = (fts_atom_t *)fts_get_pointer(at);
  int i;

  /* output single atoms of current list */
  for(i=self->ac-1; i>=0; i--)
  {
    fts_outlet_atom(o, i, atoms + i);
    fts_atom_void(atoms + i);
  }
  
  fts_heap_free(atoms, self->heap);
}

static void
pipe_delay_list(pipe_t *self)
{
  fts_atom_t *atoms = (fts_atom_t *)fts_heap_alloc(self->heap);
  fts_atom_t a;
  int i;
  
  for(i=0; i<self->ac; i++)
  {
    /* 
       we don't need to use fts_atom_void since we can't any refered object is 
       heap, so we can safely use fts_set_void instead
       old code:
       fts_atom_void(atoms + i); 
    */
    fts_set_void(atoms + i);
    fts_atom_assign(atoms + i, self->at + i);
  }

  fts_set_pointer(&a, atoms);
  fts_timebase_add_call(fts_get_timebase(), (fts_object_t *)self, pipe_output, &a, self->delay);
}

static void
pipe_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pipe_t *self = (pipe_t *)o;

  pipe_delay_list(self);  
}

static void
pipe_atom_trigger(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pipe_t *self = (pipe_t *)o;

  fts_atom_assign(self->at, at);
  pipe_delay_list(self);
}

static void
pipe_atom_delay(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pipe_t *self = (pipe_t *)o;

  if(fts_is_number(at))
  {
    double delay = fts_get_number_float(at);
      
    if(delay < 0)
      delay = 0.0;
      
    self->delay = delay;
  }
  else
    self->delay = 0.0;
}

static void
pipe_atom_right(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pipe_t *self = (pipe_t *)o;

  if(winlet == self->ac)
    pipe_atom_delay(o, 0, 0, 1, at);
  else
    fts_atom_assign(self->at + winlet, at);
}

static void
pipe_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pipe_t *self = (pipe_t *)o;
  int n = ac;
  int i;

  if(n > self->ac)
  {
    n = self->ac;
  }

  for(i=0; i<n; i++)
    fts_atom_assign(self->at + i, at + i);

  if(ac > self->ac)
    pipe_atom_delay(o, 0, 0, 1, at + self->ac);
  
  pipe_delay_list(self);
}

static void
pipe_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_timebase_remove_object(fts_get_timebase(), o);
}

static void
pipe_flush(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_timebase_flush_object(fts_get_timebase(), o);
}

/****************************************************
 *
 *  class
 *
 */

static void
pipe_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pipe_t *self = (pipe_t *)o;

  if(ac > 1)
  {
    int n = ac - 1;
    int i;

    self->heap = fts_heap_new(sizeof(fts_atom_t) * n);
    self->ac = n;
    self->at = fts_heap_alloc(self->heap);

    for(i=0; i<self->ac; i++)
    {
      fts_set_void(self->at + i);
      fts_atom_assign(self->at + i, at + i);
    }
      
    pipe_atom_delay(o, 0, 0, 1, at + self->ac);

    fts_object_set_inlets_number(o, n + 1);
    fts_object_set_outlets_number(o, n);
  }
  else
  {
    self->heap = fts_heap_new(sizeof(fts_atom_t));
    self->ac = 1;
    self->at = fts_heap_alloc(self->heap);

    fts_set_int(self->at, 0);
      
    if(ac > 0)
      pipe_atom_delay(o, 0, 0, 1, at);
  }
}

static void
pipe_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{  
  pipe_t *self = (pipe_t *)o;
  int i;

  pipe_clear(o, 0, 0, 0, 0);
  
  for(i=0; i<self->ac; i++)
    fts_atom_void(self->at + i);
  
  fts_heap_free(self->at, self->heap);
}

static void
pipe_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(pipe_t), pipe_init, pipe_delete);

  fts_class_inlet_bang(cl, 0,  pipe_bang);
  fts_class_message_varargs(cl, fts_s_clear, pipe_clear);
  fts_class_message_varargs(cl, fts_s_flush, pipe_flush);

  fts_class_inlet_int(cl, 0, pipe_atom_trigger);
  fts_class_inlet_float(cl, 0, pipe_atom_trigger);
  fts_class_inlet_symbol(cl, 0, pipe_atom_trigger);
  fts_class_inlet_varargs(cl, 0, pipe_list);
  
  fts_class_inlet_int(cl, 1, pipe_atom_right);
  fts_class_inlet_float(cl, 1, pipe_atom_right);
  fts_class_inlet_symbol(cl, 1, pipe_atom_right);

  fts_class_outlet_atom(cl, 0);
}

void
pipe_config(void)
{
  fts_class_install(fts_new_symbol("pipe"), pipe_instantiate);
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
