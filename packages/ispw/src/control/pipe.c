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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include "fts.h"

static fts_symbol_t fts_s_clear, fts_s_flush;

typedef struct pipe_st pipe_t;
typedef struct qued_list_st qued_list_t;

struct qued_list_st{
  qued_list_t *next; /* next in que of delayed lists */
  qued_list_t *prev; /* previous in que of delayed lists */
  fts_alarm_t alarm; /* its alarm */
  pipe_t *pipe; /* pointer back to pipe */
  fts_atom_t at[1]; /* ooh!: actual size will be pipe->ac */
};

struct pipe_st{
  fts_object_t o;
  fts_symbol_t clock;
  int ac;
  fts_atom_t *at;
  double del_time;
  qued_list_t *delayed;
  qued_list_t *free;
};


static void pipe_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

/****************************************************
 *
 *  alarm
 *
 */

static void pipe_output_delayed_list(qued_list_t *out);

static void
pipe_tick(fts_alarm_t *alarm, void *x)
{
  qued_list_t *out = (qued_list_t *)x;
  pipe_output_delayed_list(out);
}

/****************************************************
 *
 *  utils
 *
 */

static qued_list_t *
pipe_allocate_new_list(pipe_t *this)
{
  qued_list_t *new;

  new = (qued_list_t *)fts_malloc(sizeof(qued_list_t) + (this->ac-1)*sizeof(fts_atom_t));
  fts_alarm_init(&(new->alarm), this->clock, pipe_tick, new);
  new->pipe = this;

  return new;
}

static qued_list_t *
pipe_get_free_list(pipe_t *this)
{
  qued_list_t *new;

  /* look to to que of free lists */
  if(this->free)
    {
      new = this->free;
      this->free = new->next;
    }
  else
    new = pipe_allocate_new_list(this);

  return new;
}

static void
pipe_free_delayed_list(pipe_t *this, qued_list_t *free)
{
  if(free->prev)
    free->prev->next = free->next;
  else
    this->delayed = free->next;

  if(free->next)
    free->next->prev = free->prev;

  /* put to que of free lists */
  free->next = this->free;
  this->free = free;

  fts_alarm_unarm(&(free->alarm));
}

static void
pipe_output_delayed_list(qued_list_t *out)
{
  fts_object_t *o = (fts_object_t *)out->pipe;
  int ac = out->pipe->ac;
  int i;

  /* output single atoms of current list */
  for(i=ac-1; i>=0; i--)
    {
      fts_atom_t *at = out->at + i;

      if(fts_is_long(at))
	fts_outlet_int(o, i, fts_get_long(at));
      else if(fts_is_float(at))
	fts_outlet_float(o, i, fts_get_float(at));
      else /* if fts_is_symbol(at) */
	fts_outlet_symbol(o, i, fts_get_symbol(at));
    }

  /* take away from que of delayed objects */
  pipe_free_delayed_list(out->pipe, out);
}

static void
pipe_delay_list(pipe_t *this)
{
  qued_list_t *new_list;
  int i;

  new_list = pipe_get_free_list(this);

  /* copy current list from pipe object */
  memcpy((char *)(new_list->at), this->at, this->ac * sizeof(fts_atom_t));

  /* set and fire alarm */
  fts_alarm_set_delay(&(new_list->alarm), this->del_time);
  fts_alarm_arm(&(new_list->alarm));

  /* insert list as first of que of delayed list */
  new_list->next = this->delayed;
  new_list->prev = 0;
  if(this->delayed)
    this->delayed->prev = new_list;
  this->delayed = new_list;
}


/****************************************************
 *
 *  object
 *
 */

static void
pipe_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pipe_t *this = (pipe_t *)o;
  fts_atom_t sat[2];

  ac--; at++; /* skip class name argument */

  /* look for clock argument */
  if(ac > 0 && fts_is_symbol(at))
    {
      this->clock = fts_get_symbol(at);
      ac--; at++; /* skip clock argument */
    }
  else
    this->clock = 0;

  /* fake default arguments */
  if(ac > 1)
    {
      this->ac = ac - 1;
      this->at = fts_malloc(this->ac * sizeof(fts_atom_t));
      memcpy((char *)(this->at), at, this->ac * sizeof(fts_atom_t));
      if(fts_is_long(at + this->ac) || fts_is_float(at + this->ac))
	this->del_time = fts_get_double_arg(ac, at, this->ac, 0);
      else
	this->del_time = 0.0;
    }
  else
    {
      this->ac = 1;
      this->at = fts_malloc(sizeof(fts_atom_t));
      fts_set_long(this->at, 0);
      if(ac == 0 || fts_is_long(at) || fts_is_float(at))
	this->del_time = fts_get_double_arg(ac, at, 0, 0);
      else
	this->del_time = 0.0;
    }

  /* init que of delayed lists */
  this->delayed = 0; /* que empty */
  this->free = 0; /* que root has no previous item */
}

static void
pipe_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{  
  pipe_t *this = (pipe_t *)o;

  pipe_clear(o, 0, 0, 0, 0);
  fts_free(this->at);
}

/****************************************************
 *
 *  methods
 *
 */

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

  this->at[0] = at[0];
  pipe_delay_list(this);
}

static void
pipe_atom_middle(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pipe_t *this = (pipe_t *)o;

  this->at[winlet] = at[0];
}

static void
pipe_atom_del_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pipe_t *this = (pipe_t *)o;
  double del_time;

  del_time = fts_get_double_arg(ac, at, 0, 0.0);

  if(del_time < 0)
    del_time = 0.0;

  this->del_time = del_time;
}

static void
pipe_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pipe_t *this = (pipe_t *)o;
  int n = this->ac;

  if(ac > n)
    {
      memcpy((char *)(this->at), at, n * sizeof(fts_atom_t));
      pipe_atom_del_time(o, 0, 0, 1, at + n);
    }
  else
    memcpy((char *)(this->at), at, ac * sizeof(fts_atom_t));
  
  pipe_delay_list(this);
}

static void
pipe_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pipe_t *this = (pipe_t *)o;

  while(this->delayed)
    pipe_free_delayed_list(this, this->delayed);
}

static void
pipe_flush(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pipe_t *this = (pipe_t *)o;
  while(this->delayed)
    pipe_output_delayed_list(this->delayed);
}


/****************************************************
 *
 *  class
 *
 */

static fts_status_t
pipe_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];
  fts_atom_t sat[2];
  int i;

  ac--; at++; /* skip class name argument */

  if(ac > 0 && fts_is_symbol(at))
    ac--; at++; /* skip clock argument */

  if (ac == 0)
    {
      ac = 2;
      fts_set_long(sat, 0);
      fts_set_long(sat + 1, 0);
      at = sat;
    }
  else if (ac == 1)
    {
      ac = 2;
      fts_set_long(sat, 0);
      sat[1] = at[0];
      at = sat;
    }

  /* initializtion */
  fts_class_init(cl, sizeof(pipe_t), ac, ac-1, 0);

  /* system  methods */

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, pipe_init);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, pipe_delete, 0, 0);

  /* user methods */

  fts_method_define(cl, 0, fts_s_bang,  pipe_bang, 0, 0);
  fts_method_define(cl, 0, fts_s_clear, pipe_clear, 0, 0);
  fts_method_define(cl, 0, fts_s_flush, pipe_flush, 0, 0);

  fts_method_define_varargs(cl, 0, fts_s_list, pipe_list);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, pipe_atom_trigger, 1, a);
  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, pipe_atom_trigger, 1, a);
  a[0] = fts_s_symbol;
  fts_method_define(cl, 0, fts_s_symbol, pipe_atom_trigger, 1, a);
  
  for(i=1; i<ac-1; i++)
    {
      a[0] = fts_s_int;
      fts_method_define(cl, i, fts_s_int, pipe_atom_middle, 1, a);
      a[0] = fts_s_float;
      fts_method_define(cl, i, fts_s_float, pipe_atom_middle, 1, a);
      a[0] = fts_s_symbol;
      fts_method_define(cl, i, fts_s_symbol, pipe_atom_middle, 1, a);
    }

  a[0] = fts_s_int;
  fts_method_define(cl, i, fts_s_int, pipe_atom_del_time, 1, a);
  a[0] = fts_s_float;
  fts_method_define(cl, i, fts_s_float, pipe_atom_del_time, 1, a);

  /* outlet */
  if(ac == 2)
    {
      if(fts_is_long(at))
	fts_outlet_type_define(cl, 0, fts_s_int, 1, a);
      else if(fts_is_float(at))
	fts_outlet_type_define(cl, 0, fts_s_float, 1, a);
      else /* if fts_is_symbol(at) */
	fts_outlet_type_define(cl, 0, fts_s_symbol, 1, a);
    }
  else
    {
      fts_outlet_type_define_varargs(cl, 0, fts_s_list);
    }

  return fts_Success;
}

void
pipe_config(void)
{
  fts_s_clear = fts_new_symbol("clear");
  fts_s_flush = fts_new_symbol("flush");

  fts_metaclass_install(fts_new_symbol("pipe"), pipe_instantiate, fts_arg_equiv);
}
