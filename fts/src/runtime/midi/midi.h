/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#ifndef _FTSMIDI_H_
#define _FTSMIDI_H_

extern fts_module_t fts_midi_module;

struct _fts_midi_port;
typedef struct _fts_midi_port fts_midi_port_t;

extern fts_midi_port_t *fts_midi_get_port(int idx);

extern double *fts_get_midi_time_code_p(int idx); /*return a pointer to a long keeping
						  the midi time code of the port */

typedef  void (* midi_action_t) (fts_midi_port_t *p, int midi_ev, void *user_data, int argc, fts_atom_t *at);

extern void fts_midi_install_fun(fts_midi_port_t *port, int midi_ev, midi_action_t fun, void *user_data);
extern void fts_midi_deinstall_fun(fts_midi_port_t *port, int midi_ev, midi_action_t fun, void *user_data);
/* Macros identifing the MIDI_EV  in the above call.
   Note that the n argument to the _CH macros can be between 1 and 16.
   Should add at least the modulation wheel ...
*/


#define FTS_MIDI_NOTE                       0
#define FTS_MIDI_NOTE_CH(n)                 (FTS_MIDI_NOTE + (n))
#define FTS_MIDI_POLY_AFTERTOUCH            (FTS_MIDI_NOTE + 16 + 1)
#define FTS_MIDI_POLY_AFTERTOUCH_CH(n)      (FTS_MIDI_POLY_AFTERTOUCH + (n))
#define FTS_MIDI_CONTROLLER                 (FTS_MIDI_POLY_AFTERTOUCH + 16 + 1)
#define FTS_MIDI_CONTROLLER_CH(n)           (FTS_MIDI_CONTROLLER + (n))
#define FTS_MIDI_PROGRAM_CHANGE             (FTS_MIDI_CONTROLLER + 16 + 1)
#define FTS_MIDI_PROGRAM_CHANGE_CH(n)       (FTS_MIDI_PROGRAM_CHANGE + (n))
#define FTS_MIDI_CHANNEL_AFTERTOUCH         (FTS_MIDI_PROGRAM_CHANGE + 16 + 1)
#define FTS_MIDI_CHANNEL_AFTERTOUCH_CH(n)   (FTS_MIDI_CHANNEL_AFTERTOUCH + (n))
#define FTS_MIDI_PITCH_BEND                 (FTS_MIDI_CHANNEL_AFTERTOUCH + 16 + 1)
#define FTS_MIDI_PITCH_BEND_CH(n)           (FTS_MIDI_PITCH_BEND + (n))
#define FTS_MIDI_BYTE                       (FTS_MIDI_PITCH_BEND + 16 + 1)
#define FTS_MIDI_SYSEX                      (FTS_MIDI_BYTE + 1)
#define FTS_MIDI_REALTIME                   (FTS_MIDI_SYSEX + 1)
#define FTS_MIDI_MTC                        (FTS_MIDI_REALTIME + 1)

#define MAX_FTS_MIDI_EV                     FTS_MIDI_MTC

extern void fts_midi_send(fts_midi_port_t *p, long val);
#endif
