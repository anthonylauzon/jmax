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

#ifndef _FTS_MIDIPORT_H_
#define _FTS_MIDIPORT_H_

#include "fts.h"

/****************************************************
 *
 *  MIDI status
 *
 */
enum _fts_midi_status_ 
{
  fts_midi_status_note_off = 0, 
  fts_midi_status_note_on, 
  fts_midi_status_poly_pressure, 
  fts_midi_status_control_change, 
  fts_midi_status_program_change, 
  fts_midi_status_channel_pressure, 
  fts_midi_status_pitch_bend,
  fts_midi_status_system_exclusive,
  fts_midi_status_n
};

typedef enum _fts_midi_status_ fts_midi_status_t;

typedef void (*fts_midiport_channel_message_callback_t)(fts_object_t *listener, int channel, int x, int y, double time);
typedef void (*fts_midiport_system_exclusive_callback_t)(fts_object_t *listener, int size, char *buf, double time);

typedef union _fts_midiport_callback_
{
  fts_midiport_channel_message_callback_t channel_message;
  fts_midiport_system_exclusive_callback_t system_exclusive;
} fts_midiport_callback_t;

typedef struct _fts_midiport_listener_
{
  fts_midiport_callback_t callback;
  fts_object_t *listener;
  struct _fts_midiport_listener_ *next;
} fts_midiport_listener_t;

typedef struct _fts_midiport_
{
  fts_object_t o;
  fts_midiport_listener_t *channel_message_listeners[fts_midi_status_n - 1][17]; /* 16 channels(1..16) + omni (0) */
  fts_midiport_listener_t *system_exclusive_listeners; /* 16 channels(1..16) + omni (0) */
} fts_midiport_t;

extern void fts_midiport_add_listener(fts_midiport_t *port, fts_midi_status_t status, int channel, fts_object_t *listener, fts_midiport_callback_t fun);
extern void fts_midiport_remove_listener(struct _fts_midiport_ *port, fts_midi_status_t status, int channel, fts_object_t *listener);

#endif
