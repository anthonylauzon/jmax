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

#include <fts/fts.h>

typedef struct 
{
  fts_object_t o;
  int velocity;
  int channel;
  double duration;
} noteoff_t;

static void
noteoff_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  noteoff_t *this = (noteoff_t *)o;
  int note = fts_get_int(at);
  int pitch = note >> 4;
  int channel = (note & 0x0f) + 1;

  fts_outlet_int(o, 2, channel);
  fts_outlet_int(o, 1, 0);
  fts_outlet_int(o, 0, pitch);
}

static void
noteoff_set_duration(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  noteoff_t *this = (noteoff_t *)o;
  double duration = fts_get_number_float(at);

  if(duration > 0.0)
    this->duration = duration;
  else
    this->duration = 0.0;
}

static void
noteoff_set_channel(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  noteoff_t *this = (noteoff_t *)o;
  int channel = fts_get_number_int(at);

  if(channel < 1)
    this->channel = 1;
  else if(channel > 16)
    this->channel = 16;
  else
    this->channel = channel;
}

static void
noteoff_set_velocity(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  noteoff_t *this = (noteoff_t *)o;

  this->velocity = fts_get_number_int(at);
}

static void
noteoff_pitch(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  noteoff_t *this = (noteoff_t *)o;

  if(ac > 0 && fts_is_number(at) && this->velocity != 0)
    {
      int pitch = fts_get_number_int(at);
      int note = (pitch << 4) + ((this->channel - 1) & 0x0f);
      fts_atom_t a;

      fts_set_int(&a, note);
      fts_timebase_add_call(fts_get_timebase(), o, noteoff_output, &a, this->duration);
      
      /* send the output messages */
      fts_outlet_int(o, 2, this->channel);
      fts_outlet_int(o, 1, this->velocity);
      fts_outlet_int(o, 0, pitch);
    }
}

static void
noteoff_atoms(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  noteoff_t *this = (noteoff_t *)o;

  switch(ac)
    {
    default:
    case 4:
      if(fts_is_number(at + 3))
	noteoff_set_duration(o, 0, 0, 1, at + 3);
    case 3:
      if(fts_is_number(at + 2))
	noteoff_set_channel(o, 0, 0, 1, at + 2);
    case 2:
      if(fts_is_number(at + 1))
	noteoff_set_velocity(o, 0, 0, 1, at + 1);
    case 1:
      noteoff_pitch(o, 0, 0, 1, at);
    case 0:
      break;
    }      
}

static void
noteoff_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  noteoff_t *this = (noteoff_t *)o;

  fts_timebase_flush_object(fts_get_timebase(), o);
}

static void
noteoff_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  noteoff_t *this = (noteoff_t *)o;

  fts_timebase_remove_object(fts_get_timebase(), o);
}


static void
noteoff_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  noteoff_t *this = (noteoff_t *)o;

  switch(ac)
    {
    default:
    case 3:
      noteoff_set_duration(o, 0, 0, 1, at + 2);
    case 2:
      noteoff_set_channel(o, 0, 0, 1, at + 1);
    case 1:
      noteoff_set_velocity(o, 0, 0, 1, at + 0);
    case 0:
      break;
    }
}

static fts_status_t
noteoff_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(noteoff_t), 4, 3, 0); 

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, noteoff_init);

  fts_method_define_varargs(cl, 0, fts_s_clear, noteoff_clear);
  fts_method_define_varargs(cl, 0, fts_s_stop, noteoff_stop);

  fts_method_define_varargs(cl, 0, fts_s_int, noteoff_pitch);
  fts_method_define_varargs(cl, 0, fts_s_float, noteoff_pitch);

  fts_method_define_varargs(cl, 0, fts_s_list, noteoff_atoms);

  fts_method_define_varargs(cl, 1, fts_s_int, noteoff_set_velocity);
  fts_method_define_varargs(cl, 1, fts_s_float, noteoff_set_velocity);

  fts_method_define_varargs(cl, 2, fts_s_int, noteoff_set_channel);
  fts_method_define_varargs(cl, 2, fts_s_float, noteoff_set_channel);

  fts_method_define_varargs(cl, 3, fts_s_int, noteoff_set_duration);
  fts_method_define_varargs(cl, 3, fts_s_float, noteoff_set_duration);

  fts_outlet_type_define_varargs(cl, 0, fts_s_int);
  fts_outlet_type_define_varargs(cl, 1, fts_s_int);
  fts_outlet_type_define_varargs(cl, 2, fts_s_int);

  return fts_ok;
}


void
noteoff_config(void)
{
  fts_class_install(fts_new_symbol("noteoff"), noteoff_instantiate);
}
