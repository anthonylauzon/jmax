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
#ifndef _NOTE_H_
#define _NOTE_H_

#include <fts/fts.h>
#include "event.h"

extern fts_class_t *note_class;

typedef struct _note_
{
  fts_object_t head;
  int pitch;
  double duration;
  int midi_velocity; /* optinal property for MIDI notes (will get an fts property) */
  int midi_channel; /* optinal property for MIDI notes (will get an fts property) */
} note_t;

#define NOTE_DEF_PITCH 64
#define NOTE_DEF_DURATION 100
#define NOTE_DEF_MIDI_VELOCITY 64
#define NOTE_DEF_MIDI_CHANNEL 1

#define note_set_pitch(n, x) ((n)->pitch = (x))
#define note_get_pitch(n) ((n)->pitch)

#define note_set_duration(n, x) ((n)->duration = (x))
#define note_get_duration(n) ((n)->duration)

#define note_set_midi_velocity(n, x) ((n)->midi_velocity = (x))
#define note_get_midi_velocity(n) (((n)->midi_velocity >= 0)? (n)->midi_velocity: NOTE_DEF_MIDI_VELOCITY)

#define note_set_midi_channel(n, x) ((n)->midi_channel = (x))
#define note_get_midi_channel(n) (((n)->midi_channel >= 0)? (n)->midi_channel: NOTE_DEF_MIDI_CHANNEL)

#endif
