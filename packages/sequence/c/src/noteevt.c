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
#include "seqsym.h"
#include "sequence.h"
#include "seqmidi.h"
#include "eventtrk.h"
#include "noteevt.h"

#define NOTEEVT_DEF_PITCH 64
#define NOTEEVT_DEF_DURATION 400
#define NOTEEVT_DEF_MIDI_CHANNEL 1
#define NOTEEVT_DEF_MIDI_VELOCITY 64

static void
noteevt_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  noteevt_t *this = (noteevt_t *)o;
  
  event_init(&this->head);

  this->pitch = fts_get_int_arg(ac, at, 1, NOTEEVT_DEF_PITCH);
  this->duration = fts_get_float_arg(ac, at, 2, NOTEEVT_DEF_DURATION);
  this->midi_channel = -1;
  this->midi_velocity = -1;
}

/**************************************************************
 *
 *  mandatory event methods
 *
 */

static void
noteevt_upload(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  noteevt_t *this = (noteevt_t *)o;
  fts_atom_t a[4];

  fts_set_float(a + 0, event_get_time(&this->head));
  fts_set_symbol(a + 1, seqsym_noteevt);
  fts_set_int(a + 2, this->pitch);
  fts_set_float(a + 3, this->duration);

  fts_client_upload(o, seqsym_event, 4, a);
}

static void
noteevt_move(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  /* generic event "super class" */
  event_t *this = (event_t *)o;
  float time = fts_get_float(at + 0);

  eventtrk_move_event(this, time);
}

static void
noteevt_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  noteevt_t *this = (noteevt_t *)o;

  if(ac > 0 && fts_is_number(at + 0))
    this->pitch = fts_get_number_int(at + 0);

  if(ac > 1 && fts_is_number(at + 1))
    this->duration = fts_get_number_float(at + 1);
}

void 
noteevt_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  noteevt_t *this = (noteevt_t *)o;

  post("pitch %d, duration %lf\n", this->pitch, this->duration);
}

/**************************************************************
 *
 *  set event methods
 *
 */

static void
noteevt_pitch(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  noteevt_t *this = (noteevt_t *)o;

  if(ac > 0 && fts_is_number(at + 0))
    this->pitch = fts_get_number_int(at + 0);
}

static void
noteevt_duration(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  noteevt_t *this = (noteevt_t *)o;

  if(ac > 0 && fts_is_number(at + 0))
    this->duration = fts_get_number_float(at + 0);
}

/**************************************************************
 *
 *  export to MIDI file
 *
 */

static void 
noteevt_export_midi(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  noteevt_t *this = (noteevt_t *)o;
  fts_midifile_t *file = (fts_midifile_t *)fts_get_ptr(at);
  double time = event_get_time(&this->head);
  int channel = (this->midi_channel >= 0)? this->midi_channel: NOTEEVT_DEF_MIDI_CHANNEL;
  int velocity = (this->midi_velocity >= 0)? this->midi_velocity: NOTEEVT_DEF_MIDI_VELOCITY;

  /* declare note with off time */
  seqmidi_write_note(file, time, channel, this->pitch, velocity, this->duration);
}

/**************************************************************
 *
 *  MIDI properties
 *
 */

static void
noteevt_get_midi_channel_property(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  noteevt_t *this = (noteevt_t *)obj;

  if(this->midi_channel >= 0)
    fts_set_int(value, this->midi_channel);
  else
    fts_set_void(value);
}

static void
noteevt_set_midi_channel_property(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  noteevt_t *this = (noteevt_t *)obj;

  this->midi_channel = fts_get_int(value);
}

static void
noteevt_get_midi_velocity_property(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  noteevt_t *this = (noteevt_t *)obj;

  if(this->midi_velocity >= 0)
    fts_set_int(value, this->midi_velocity);
  else
    fts_set_void(value);
}

static void
noteevt_set_midi_velocity_property(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  noteevt_t *this = (noteevt_t *)obj;

  this->midi_velocity = fts_get_int(value);
}

/**************************************************************
 *
 *  class
 *
 */
static fts_status_t
noteevt_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(noteevt_t), 0, 0, 0); 
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, noteevt_init);

  fts_class_add_daemon(cl, obj_property_get, seqsym_midi_channel, noteevt_get_midi_channel_property);
  fts_class_add_daemon(cl, obj_property_put, seqsym_midi_channel, noteevt_set_midi_channel_property);

  fts_class_add_daemon(cl, obj_property_get, seqsym_midi_velocity, noteevt_get_midi_velocity_property);
  fts_class_add_daemon(cl, obj_property_put, seqsym_midi_velocity, noteevt_set_midi_velocity_property);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("upload"), noteevt_upload);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("move"), noteevt_move);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("set"), noteevt_set);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("pitch"), noteevt_pitch);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("duration"), noteevt_duration);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("print"), noteevt_print);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("export_midi"), noteevt_export_midi);

  return fts_Success;
}

void
noteevt_config(void)
{
  fts_class_install(seqsym_noteevt, noteevt_instantiate);
}
