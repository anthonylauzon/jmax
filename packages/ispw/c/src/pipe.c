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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Miller Puckette.
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
  pipe_t *this = (pipe_t *)o;
  fts_atom_t *atoms = (fts_atom_t *)fts_get_pointer(at);
  int i;

  /* output single atoms of current list */
  for(i=this->ac-1; i>=0; i--)
    {
      fts_outlet_atom(o, i, atoms + i);
      fts_atom_void(atoms + i);
    }

  fts_heap_free(atoms, this->heap);
}

static void
pipe_delay_list(pipe_t *this)
{
  fts_atom_t *atoms = (fts_atom_t *)fts_heap_alloc(this->heap);
  fts_atom_t a;
  int i;
  
  for(i=0; i<this->ac; i++)
    {
      fts_atom_void(atoms + i);
      fts_atom_assign(atoms + i, this->at + i);
    }

  fts_set_pointer(&a, atoms);
  fts_timebase_add_call(fts_get_timebase(), (fts_object_t *)this, pipe_output, &a, this->delay);
}

static void
pipe_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pipe_t *this = (pipe_t *)o;

  pipe_delay_list(this);  
}

static void
pipe_atom_trigger(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pipe_t *this = (pipe_t *)o;

  fts_atom_assign(this->at, at);
  pipe_delay_list(this);
}

static void
pipe_atom_middle(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pipe_t *this = (pipe_t *)o;

  fts_atom_assign(this->at + winlet, at);
}

static void
pipe_atom_delay(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pipe_t *this = (pipe_t *)o;
  double delay = fts_get_number_float(at);

  if(delay < 0)
    delay = 0.0;

  this->delay = delay;
}

static void
pipe_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pipe_t *this = (pipe_t *)o;
  int n = ac;
  int i;

  if(n > this->ac)
    n = ac;

  for(i=0; i<n; i++)
    fts_atom_assign(this->at + i, at + i);

  if(ac > this->ac && fts_is_number(at + this->ac))
    pipe_atom_delay(o, 0, 0, 1, at + this->ac);
  
  pipe_delay_list(this);
}

static void
pipe_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pipe_t *this = (pipe_t *)o;

  fts_timebase_remove_object(fts_get_timebase(), o);
}

static void
pipe_flush(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pipe_t *this = (pipe_t *)o;

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
  pipe_t *this = (pipe_t *)o;

  if(ac > 1)
    {
      int n = ac - 1;
      int i;

      this->heap = fts_heap_new(sizeof(fts_atom_t) * n);
      this->ac = n;
      this->at = fts_heap_alloc(this->heap);

      for(i=0; i<this->ac; i++)
	{
	  fts_set_void(this->at + i);
	  fts_atom_assign(this->at + i, at + i);
	}
      
      if(fts_is_number(at + this->ac))
	pipe_atom_delay(o, 0, 0, 1, at + this->ac);
      else
	this->delay = 0.0;
    }
  else
    {
      this->heap = fts_heap_new(sizeof(fts_atom_t));
      this->ac = 1;
      this->at = fts_heap_alloc(this->heap);

      fts_set_int(this->at, 0);

      if(ac > 0 && fts_is_number(at))
	pipe_atom_delay(o, 0, 0, 1, at);
      else
	this->delay = 0.0;
    }
}

static void
pipe_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{  
  pipe_t *this = (pipe_t *)o;
  int i;

  pipe_clear(o, 0, 0, 0, 0);
  
  for(i=0; i<this->ac; i++)
    fts_atom_void(this->at + i);
  
  fts_heap_free(this->at, this->heap);
}

static fts_status_t
pipe_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int n;
  int i;

  if(ac < 2)
    n = 2;
  else
    n = ac;

  fts_class_init(cl, sizeof(pipe_t), n, n - 1, 0);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, pipe_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, pipe_delete);

  fts_method_define_varargs(cl, 0, fts_s_bang,  pipe_bang);
  fts_method_define_varargs(cl, 0, fts_s_clear, pipe_clear);
  fts_method_define_varargs(cl, 0, fts_s_flush, pipe_flush);

  fts_method_define_varargs(cl, 0, fts_s_list, pipe_list);
  fts_method_define_varargs(cl, 0, fts_s_int, pipe_atom_trigger);
  fts_method_define_varargs(cl, 0, fts_s_float, pipe_atom_trigger);
  fts_method_define_varargs(cl, 0, fts_s_symbol, pipe_atom_trigger);
  
  for(i=1; i<n-1; i++)
    {
      fts_method_define_varargs(cl, i, fts_s_int, pipe_atom_middle);
      fts_method_define_varargs(cl, i, fts_s_float, pipe_atom_middle);
      fts_method_define_varargs(cl, i, fts_s_symbol, pipe_atom_middle);
    }

  fts_method_define_varargs(cl, n - 1, fts_s_int, pipe_atom_delay);
  fts_method_define_varargs(cl, n - 1, fts_s_float, pipe_atom_delay);

  return fts_ok;
}

void
pipe_config(void)
{
  fts_metaclass_install(fts_new_symbol("pipe"), pipe_instantiate, fts_narg_equiv);
}
