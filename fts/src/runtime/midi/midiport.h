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

typedef struct _fts_midiport_ fts_midiport_t;

/****************************************************
 *
 *  MIDI status
 *
 */
enum _fts_midi_status_ 
{
  fts_midi_status_note = 0, 
  fts_midi_status_poly_pressure, 
  fts_midi_status_control_change, 
  fts_midi_status_program_change, 
  fts_midi_status_channel_pressure, 
  fts_midi_status_pitch_bend,
  fts_midi_status_system_exclusive,
  fts_midi_status_n
};

typedef enum _fts_midi_status_ fts_midi_status_t;

/****************************************************
 *
 *  MIDI in callbacks and listeners
 *
 */
typedef void (*fts_midiport_channel_message_callback_t)(fts_object_t *listener, int channel, int x, int y, double time);
typedef void (*fts_midiport_system_exclusive_callback_t)(fts_object_t *listener, int ac, const fts_atom_t *at, double time);

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

/****************************************************
 *
 *  MIDI out functions
 *
 */
typedef void (*fts_midiport_channel_message_output_t)(fts_midiport_t *port, int status, int channel, int x, int y, double time);
typedef void (*fts_midiport_system_exclusive_output_t)(fts_midiport_t *port, int ac, const fts_atom_t *at, double time);
typedef int (*fts_midiport_system_exclusive_get_t)(fts_midiport_t *port);

/****************************************************
 *
 *  MIDI port
 *
 */
extern fts_symbol_t fts_s__superclass;
extern fts_symbol_t fts_s_midiport;

struct _fts_midiport_
{
  fts_object_t o;

  /* call backs */
  fts_midiport_listener_t *channel_message_listeners[fts_midi_status_n - 1][17]; /* 16 channels(1..16) + omni (0) */
  fts_midiport_listener_t *system_exclusive_listeners; /* 16 channels(1..16) + omni (0) */

  /* MIDI output functions */
  fts_midiport_channel_message_output_t channel_message_output;
  fts_midiport_system_exclusive_output_t system_exclusive_output;

  /* system exclusive message atoms */
  fts_atom_t *sysex_at;
  int sysex_ac;
  int sysex_alloc;
};

extern void fts_midiport_init(fts_midiport_t *port, fts_midiport_channel_message_output_t chmess_out, fts_midiport_system_exclusive_output_t sysex_out);
extern void fts_midiport_class_init(fts_class_t *cl);

extern void fts_midiport_add_listener(fts_midiport_t *port, fts_midi_status_t status, int channel, fts_object_t *listener, fts_midiport_callback_t fun);
extern void fts_midiport_remove_listener(fts_midiport_t *port, fts_midi_status_t status, int channel, fts_object_t *listener);

extern int fts_midiport_has_superclass(fts_object_t *obj);

/****************************************************
 *
 *  MIDI in/out functions
 *
 */

extern void fts_midiport_channel_message(fts_midiport_t *port, fts_midi_status_t status, int channel, int x, int y, double time);
extern void fts_midiport_system_exclusive(fts_midiport_t *port, double time);
extern void fts_midiport_system_exclusive_add_byte(fts_midiport_t *port, int value);

#define fts_midiport_output_channel_message(p, s, c, x, y, t) ((p)->channel_message_output((p), (s), (c), (x), (y), (t)))
#define fts_midiport_output_system_exclusive(p, n, a, t) ((p)->system_exclusive_output((p), (n), (a), (t)))

/****************************************************
 *
 *  default MIDI port
 *
 */
typedef fts_midiport_t * (*fts_midiport_default_function_t)(void);

extern void fts_midiport_set_default_function(fts_midiport_default_function_t fun);
extern fts_midiport_t *fts_midiport_get_default(void);

extern fts_midiport_t *fts_midiport_get_default(void);
extern void fts_midiport_set_default_function(fts_midiport_default_function_t fun);

#endif
