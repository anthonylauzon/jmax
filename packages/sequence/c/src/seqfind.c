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
#include "seqsym.h"
#include "sequence.h"
#include "track.h"
#include "event.h"

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
seqfind_find(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqfind_t *this = (seqfind_t *)o;
  event_t *event = track_get_first(this->track);
	  
  while(event)
    {
      fts_atom_t *value = event_get_value(event);

      if(fts_atom_equals(value, at))
	{
	  fts_outlet_float(o, 1, (float)event_get_time(event));
	  fts_outlet_send(o, 0, fts_get_selector(value), 1, value);

	  if(this->mode == sym_first)
	    break;
	}
      
      event = event_get_next(event);
    }  
}

static void 
seqfind_any_value(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqfind_t *this = (seqfind_t *)o;

  if(ac == 1 && fts_get_selector(at) == s)
    seqfind_find(o, winlet, s, 1, at);
  else
    fts_object_signal_runtime_error(o, "doesn't understand message %s", fts_symbol_name(s));
}

static void
seqfind_set_track(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqfind_t *this = (seqfind_t *)o;

  fts_object_release(this->track);
  this->track = track_atom_get(at);
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
	fts_object_signal_runtime_error(o, "doesn't understand mode %s", fts_symbol_name(mode));
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

  ac--;
  at++;

  this->track = 0;
  this->mode = sym_first;

  if(track_atom_is(at))
    {
      this->track = (track_t *)fts_get_object(at);
      fts_object_refer(this->track);
    }
  else
    fts_object_set_error(o, "Argument of event track required");
}

static void 
seqfind_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqfind_t *this = (seqfind_t *)o;

  if(this->track)
    fts_object_release(this->track);
}

static fts_status_t
seqfind_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(seqfind_t), 2, 2, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, seqfind_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, seqfind_delete);
  
  fts_method_define_varargs(cl, 0, fts_new_symbol("mode"), seqfind_set_mode);
  fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("mode"), seqfind_set_mode_prop);

  fts_method_define_varargs(cl, 0, fts_s_int, seqfind_find);
  fts_method_define_varargs(cl, 0, fts_s_float, seqfind_find);
  fts_method_define_varargs(cl, 0, fts_s_symbol, seqfind_find);
  fts_method_define_varargs(cl, 0, fts_s_list, seqfind_find);
  fts_method_define_varargs(cl, 0, fts_s_anything, seqfind_any_value);
  
  fts_method_define_varargs(cl, 1, seqsym_track, seqfind_set_track);
  
  return fts_Success;
}

void
seqfind_config(void)
{
  sym_first = fts_new_symbol("first");
  sym_all = fts_new_symbol("all");

  fts_class_install(fts_new_symbol("find"), seqfind_instantiate);
}
