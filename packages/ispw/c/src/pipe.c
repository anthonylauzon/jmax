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

static fts_symbol_t fts_s_clear, fts_s_flush;

typedef struct _pipe_
{
  fts_object_t o;
  int ac;
  fts_atom_t *at;
  double del_time;
  fts_heap_t *heap;
  fts_timer_t *timer;
} pipe_t;

static void
pipe_delay_list(pipe_t *this)
{
  fts_atom_t *at = (fts_atom_t *)fts_heap_alloc(this->heap);
  int i;
  
  for(i=0; i<this->ac; i++)
    {
      fts_set_void(at + i);
      fts_atom_assign(at + i, this->at + i);
    }

  /* reset alarm to first list member */
  fts_timer_set_delay(this->timer, this->del_time, at);
}

/****************************************************
 *
 *  methods
 *
 */

static void
pipe_alarm(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pipe_t *this = (pipe_t *)o;
  int i;

  /* output single atoms of current list */
  for(i=this->ac-1; i>=0; i--)
    {
      fts_outlet_send(o, i, fts_get_selector(at + i), 1, at + i);
      fts_atom_void(at + i);
    }

  fts_heap_free(at, this->heap);
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
pipe_atom_del_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pipe_t *this = (pipe_t *)o;
  double del_time = fts_get_number_float(at);

  if(del_time < 0)
    del_time = 0.0;

  this->del_time = del_time;
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
    pipe_atom_del_time(o, 0, 0, 1, at + this->ac);
  
  pipe_delay_list(this);
}

static void
pipe_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pipe_t *this = (pipe_t *)o;

  fts_timer_reset(this->timer);
}

static void
pipe_flush(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pipe_t *this = (pipe_t *)o;

  fts_timer_flush(this->timer);
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

  ac--; 
  at++;

  /* fake default arguments */
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
	pipe_atom_del_time(o, 0, 0, 1, at + this->ac);
      else
	this->del_time = 0.0;
    }
  else
    {
      this->heap = fts_heap_new(sizeof(fts_atom_t));
      this->ac = 1;
      this->at = fts_heap_alloc(this->heap);

      fts_set_int(this->at, 0);

      if(ac > 0 && fts_is_number(at))
	pipe_atom_del_time(o, 0, 0, 1, at);
      else
	this->del_time = 0.0;
    }

  this->timer = fts_timer_new(o, 0);
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
  fts_atom_t sat[2];
  int i;

  ac--; 
  at++;

  if(ac <= 1)
    ac = 2;

  fts_class_init(cl, sizeof(pipe_t), ac, ac - 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, pipe_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, pipe_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_timer_alarm, pipe_alarm);

  fts_method_define_varargs(cl, 0, fts_s_bang,  pipe_bang);
  fts_method_define_varargs(cl, 0, fts_s_clear, pipe_clear);
  fts_method_define_varargs(cl, 0, fts_s_flush, pipe_flush);

  fts_method_define_varargs(cl, 0, fts_s_list, pipe_list);
  fts_method_define_varargs(cl, 0, fts_s_int, pipe_atom_trigger);
  fts_method_define_varargs(cl, 0, fts_s_float, pipe_atom_trigger);
  fts_method_define_varargs(cl, 0, fts_s_symbol, pipe_atom_trigger);
  
  for(i=1; i<ac-1; i++)
    {
      fts_method_define_varargs(cl, i, fts_s_int, pipe_atom_middle);
      fts_method_define_varargs(cl, i, fts_s_float, pipe_atom_middle);
      fts_method_define_varargs(cl, i, fts_s_symbol, pipe_atom_middle);
    }

  fts_method_define_varargs(cl, i, fts_s_int, pipe_atom_del_time);
  fts_method_define_varargs(cl, i, fts_s_float, pipe_atom_del_time);

  return fts_Success;
}

void
pipe_config(void)
{
  fts_s_clear = fts_new_symbol("clear");
  fts_s_flush = fts_new_symbol("flush");

  fts_metaclass_install(fts_new_symbol("pipe"), pipe_instantiate, fts_arg_equiv);
}
