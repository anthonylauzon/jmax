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

#include <fts/fts.h>
#include <sequence/c/include/sequence.h>
#include <sequence/c/include/track.h>
#include <sequence/c/include/event.h>
#include <sequence/c/include/seqsym.h>

typedef struct _seqfind_
{
  fts_object_t head;
  track_t *track;
  fts_symbol_t mode;
} seqfind_t;

static fts_symbol_t sym_first = 0;
static fts_symbol_t sym_all = 0;

/************************************************************
 *
 *  user methods
 *
 */

static void 
seqfind_find_single(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqfind_t *this = (seqfind_t *)o;
  event_t *event = track_get_first(this->track);
	  
  while(event)
    {
      fts_atom_t *value = event_get_value(event);

      if(fts_atom_compare(value, at))
	{
	  fts_outlet_float(o, 1, (float)event_get_time(event));
	  fts_outlet_varargs(o, 0, 1, value);

	  if(this->mode == sym_first)
	    break;
	}
      
      event = event_get_next(event);
    }  
}

static void
seqfind_find_varargs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(ac == 1)
    seqfind_find_single(o, 0, 0, 1, at);
  else if(ac > 1)
    {
      fts_object_t *tuple = fts_object_create(fts_tuple_class, NULL, ac, at);
      fts_atom_t a;
      
      fts_set_object(&a, tuple);
      seqfind_find_single(o, 0, 0, 1, &a);
    }
}

static void
seqfind_set_track(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqfind_t *this = (seqfind_t *)o;

  fts_object_release(this->track);
  this->track = (track_t *)fts_get_object(at);
  fts_object_refer(this->track);
}

static void
seqfind_set_mode(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqfind_t *this = (seqfind_t *)o;
  
  if(ac > 0 && fts_is_symbol(at))
    {
      fts_symbol_t mode = fts_get_symbol(at);
      
      if(mode == sym_first)
	this->mode = sym_first;
      else if(mode == sym_all)
	this->mode = sym_all;
      else
	fts_object_signal_runtime_error(o, "doesn't understand mode %s", mode);
    }
  else
    fts_object_signal_runtime_error(o, "symbol argument required for message mode");
}

static void
seqfind_set_mode_prop(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  seqfind_set_mode(o, 0, 0, 1, value);
}

/************************************************************
 *
 *  class
 *
 */

static void
seqfind_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqfind_t *this = (seqfind_t *)o;

  this->track = 0;
  this->mode = sym_first;

  if(fts_is_a(at, track_type))
    {
      this->track = (track_t *)fts_get_object(at);
      fts_object_refer(this->track);
    }
  else
    fts_object_set_error(o, "argument of event track required");
}

static void 
seqfind_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqfind_t *this = (seqfind_t *)o;

  if(this->track)
    fts_object_release(this->track);
}

static void
seqfind_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(seqfind_t), seqfind_init, seqfind_delete);
  
  fts_class_message_varargs(cl, fts_new_symbol("mode"), seqfind_set_mode);
  fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("mode"), seqfind_set_mode_prop);

  fts_class_inlet_number(cl, 0, seqfind_find_single);
  fts_class_inlet_symbol(cl, 0, seqfind_find_single);
  fts_class_inlet_varargs(cl, 0, seqfind_find_varargs);
  fts_class_inlet(cl, 1, track_type, seqfind_set_track);

  fts_class_outlet_varargs(cl, 0);
  fts_class_outlet_float(cl, 1);
}

void
seqfind_config(void)
{
  sym_first = fts_new_symbol("first");
  sym_all = fts_new_symbol("all");

  fts_class_install(fts_new_symbol("find"), seqfind_instantiate);
}
