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
 * Authors: Riccardo Borghesi, Francois Dechelle, Norbert Schnell
 *
 */

#include <fts/fts.h>
#include <float.h>
#include "play.h"

static fts_hashtable_t signal_play_class_table;
static fts_symbol_t signal_play_symbol = 0;

void
signal_play_set_conv_position(signal_play_t *this, double c)
{
  if(this->begin < DBL_MAX)
    this->begin *= c / this->conv_position;

  if(this->end < DBL_MAX)
    this->end *= c / this->conv_position;

  if(this->position < DBL_MAX)
    this->position *= c / this->conv_position;

  this->conv_position = c;
}

void
signal_play_set_conv_step(signal_play_t *this, double c)
{
  this->step *= c / this->conv_step;
  this->conv_step = c;
}

/************************************************************
 *
 *  methods
 *
 */

void 
signal_play_bang_at_end(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_bang((fts_object_t *)o, 1);
}

static void
signal_play_set_object(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  signal_play_t *this = (signal_play_t *)o;
  fts_object_t *object = fts_get_object(at);

  if(this->object)
    fts_object_release(this->object);

  this->object = object;

  fts_object_refer(object);
}

static void 
signal_play_set_begin(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  signal_play_t *this = (signal_play_t *)o;
  double value = fts_get_number_float(at);
  
  if(value < 0.0)
    value = 0.0;

  this->begin = value * this->conv_position;
}

static void 
signal_play_set_end(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  signal_play_t *this = (signal_play_t *)o;
  double value = fts_get_number_float(at);
  
  if(value < 0.0)
    value = 0.0;

  this->end = value * this->conv_position;
}

static void 
signal_play_set_speed(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  signal_play_t *this = (signal_play_t *)o;
  double value = fts_get_number_float(at);
  
  if(value < 0.0)
    value = 0.0;

  this->step = value * this->conv_step;
}

static void 
signal_play_set_duration(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  signal_play_t *this = (signal_play_t *)o;
  double value = fts_get_number_float(at);
  double begin = this->begin;
  double end = this->end;
  
  if(value > 0.0)
    {
      if(end > begin)
	this->step = this->conv_step * (end - begin) / (value * this->conv_position);
      else if(begin > end)
	this->step = this->conv_step * (end - begin) / (value * this->conv_position);
    }
  else
    this->position = end;
}

static void 
signal_play_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  signal_play_t *this = (signal_play_t *)o;

  switch (ac)
    {
    default:
    case 4:
      signal_play_set_speed(o, 0, 0, 1, at + 3);
    case 3:
      signal_play_set_end(o, 0, 0, 1, at + 2);
    case 2:
      signal_play_set_begin(o, 0, 0, 1, at + 1);
    case 1:
      signal_play_set_object(o, 0, 0, 1, at);
      break;
    case 0:
      break;
    }
}

static void 
signal_play_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  signal_play_t *this = (signal_play_t *)o;

  this->position = this->begin;
  this->mode = mode_play;
}

static void 
signal_play_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  signal_play_t *this = (signal_play_t *)o;

  signal_play_set(o, 0, 0, ac, at);
  signal_play_bang(o, 0, 0, 0, 0);
}

static void 
signal_play_play(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  signal_play_t *this = (signal_play_t *)o;

  if(this->mode == mode_stop)
    this->position = this->begin;

  this->mode = mode_play;
}

static void 
signal_play_loop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  signal_play_t *this = (signal_play_t *)o;

  if(this->mode == mode_stop)
    this->position = this->begin;

  this->mode = mode_loop;
}

static void 
signal_play_cycle(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  signal_play_t *this = (signal_play_t *)o;

  if(this->mode == mode_stop)
    this->position = this->begin;

  this->mode = mode_cycle;
  this->cycle_direction = 0;
}

static void
signal_play_pause(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  signal_play_t *this = (signal_play_t *)o;

  if(this->mode != mode_stop)
    this->mode = mode_pause;
}

static void
signal_play_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  signal_play_t *this = (signal_play_t *)o;

  this->mode = mode_stop;
}

static void
signal_play_rewind(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  signal_play_t *this = (signal_play_t *)o;

  this->position = 0.0;
}

/************************************************************
 *
 *  class API
 *
 */

void
signal_play_init(signal_play_t *this, int ac, const fts_atom_t *at)
{ 
  this->position = 0.0;
  this->begin = 0.0;
  this->end = DBL_MAX;
  this->step = 1.0;

  this->conv_position = 1.0;
  this->conv_step = 1.0;

  this->mode = mode_stop;
  this->cycle_direction = 0;

  if(ac > 0 && fts_is_object(at))
    signal_play_set((fts_object_t *)this, 0, 0, ac, at);
  else
    fts_object_set_error((fts_object_t *)this, "Something to play required as first argument");

  fts_dsp_add_object((fts_object_t *)this);
}

void
signal_play_delete(signal_play_t *this)
{ 
  fts_dsp_remove_object((fts_object_t *)this);
}

void
signal_play_class_register(fts_symbol_t name, fts_instantiate_fun_t fun)
{
  fts_atom_t k, v;

  fts_set_symbol( &k, name);
  fts_set_fun(&v, (fts_fun_t)fun);
  fts_hashtable_put(&signal_play_class_table, &k, &v);
}

void
signal_play_class_init(fts_class_t *cl, fts_symbol_t type)
{
  int i;

  fts_class_init(cl, sizeof(signal_play_t), 4, 2, 0);

  fts_method_define_varargs(cl, 0, fts_s_bang, signal_play_bang);
  fts_method_define_varargs(cl, 0, fts_new_symbol("play"), signal_play_play);
  fts_method_define_varargs(cl, 0, fts_new_symbol("loop"), signal_play_loop);
  fts_method_define_varargs(cl, 0, fts_new_symbol("cycle"), signal_play_cycle);
  fts_method_define_varargs(cl, 0, fts_new_symbol("pause"), signal_play_pause);
  fts_method_define_varargs(cl, 0, fts_s_stop, signal_play_stop);
  fts_method_define_varargs(cl, 0, fts_new_symbol("rewind"), signal_play_rewind);

  fts_method_define_varargs(cl, 0, type, signal_play_set_object);
  fts_method_define_varargs(cl, 0, fts_new_symbol("begin"), signal_play_set_begin);
  fts_method_define_varargs(cl, 0, fts_new_symbol("end"), signal_play_set_end);
  fts_method_define_varargs(cl, 0, fts_new_symbol("speed"), signal_play_set_speed);

  fts_method_define_varargs(cl, 0, fts_s_set, signal_play_set);
  fts_method_define_varargs(cl, 0, fts_s_list, signal_play_list);

  fts_method_define_varargs(cl, 1, fts_s_int, signal_play_set_begin);
  fts_method_define_varargs(cl, 1, fts_s_float, signal_play_set_begin);

  fts_method_define_varargs(cl, 2, fts_s_int, signal_play_set_end);
  fts_method_define_varargs(cl, 2, fts_s_float, signal_play_set_end);

  fts_method_define_varargs(cl, 3, fts_s_int, signal_play_set_speed);
  fts_method_define_varargs(cl, 3, fts_s_float, signal_play_set_speed);

  fts_dsp_declare_outlet(cl, 0);
}

/************************************************************
 *
 *  FTS play~ class
 *
 */

static fts_status_t
signal_play_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t name = fts_get_selector(at + 1);
  fts_atom_t a, k;

  fts_set_symbol( &k, name);
  if(fts_hashtable_get( &signal_play_class_table, &k, &a))
    {
      fts_instantiate_fun_t fun = (fts_instantiate_fun_t)fts_get_fun(&a);
      
      return fun(cl, ac, at);
    }
  
  return &fts_CannotInstantiate;
}

void
signal_play_config(void)
{
  signal_play_symbol = fts_new_symbol("play~");

  fts_hashtable_init(&signal_play_class_table, 0, FTS_HASHTABLE_MEDIUM);

  fts_metaclass_install(signal_play_symbol, signal_play_instantiate, fts_arg_type_equiv);
}
