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
#include "seqsym.h"
#include "sequence.h"
#include "eventtrk.h"
#include "note.h"

fts_class_t *note_class = 0;

/**************************************************************
 *
 *  mandatory event methods
 *
 */

static void
note_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  note_t *this = (note_t *)o;
  
  switch(ac)
    {
    case 4:
      this->midi_channel = fts_get_number_int(at + 3);
    case 3:
      this->midi_velocity = fts_get_number_int(at + 2);
    case 2:
      this->duration = fts_get_number_float(at + 1);
    case 1:
      this->pitch = fts_get_number_int(at + 0);
    default:
      break;
    }
}

void 
note_get_atoms(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  note_t *this = (note_t *)o;
  int *n = fts_get_ptr(at);
  fts_atom_t *a = fts_get_ptr(at + 1);

  fts_set_int(a, this->pitch);
  fts_set_float(a + 1, (float) this->duration);

  if(this->midi_channel < 0)
    *n = 2;
  else
    {
      fts_set_int(a + 2, this->midi_velocity);
      fts_set_int(a + 3, this->midi_channel);
      *n = 4;
    }
}

/**************************************************************
 *
 *  MIDI properties
 *
 */

static void
note_get_midi_velocity_property(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  note_t *this = (note_t *)obj;

  if(this->midi_velocity >= 0)
    fts_set_int(value, this->midi_velocity);
  else
    fts_set_void(value);
}

static void
note_set_midi_velocity_property(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  note_t *this = (note_t *)obj;

  this->midi_velocity = fts_get_int(value);
}

static void
note_get_midi_channel_property(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  note_t *this = (note_t *)obj;

  if(this->midi_channel >= 0)
    fts_set_int(value, this->midi_channel);
  else
    fts_set_void(value);
}

static void
note_set_midi_channel_property(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  note_t *this = (note_t *)obj;

  this->midi_channel = fts_get_int(value);
}

/**************************************************************
 *
 *  class
 *
 */

static void
note_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  note_t *this = (note_t *)o;
  
  this->pitch = fts_get_int_arg(ac, at, 1, NOTE_DEF_PITCH);
  this->duration = fts_get_float_arg(ac, at, 2, NOTE_DEF_DURATION);
  this->midi_velocity = fts_get_int_arg(ac, at, 3, NOTE_DEF_MIDI_VELOCITY);
  this->midi_channel = fts_get_int_arg(ac, at, 4, NOTE_DEF_MIDI_CHANNEL);
}

static fts_status_t
note_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(note_t), 0, 0, 0); 
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, note_init);

  fts_class_add_daemon(cl, obj_property_get, seqsym_midi_velocity, note_get_midi_velocity_property);
  fts_class_add_daemon(cl, obj_property_put, seqsym_midi_velocity, note_set_midi_velocity_property);

  fts_class_add_daemon(cl, obj_property_get, seqsym_midi_channel, note_get_midi_channel_property);
  fts_class_add_daemon(cl, obj_property_put, seqsym_midi_channel, note_set_midi_channel_property);

  fts_method_define_varargs(cl, fts_SystemInlet, seqsym_set, note_set);
  fts_method_define_varargs(cl, fts_SystemInlet, seqsym_get_atoms, note_get_atoms);

  return fts_Success;
}

void
note_config(void)
{
  fts_class_install(seqsym_note, note_instantiate);
  note_class = fts_class_get_by_name(seqsym_note);
}
