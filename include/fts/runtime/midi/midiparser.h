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

#ifndef _FTS_MIDIPARSER_H_
#define _FTS_MIDIPARSER_H_

enum parser_status 
{
  status_normal,
  status_sysex, 
  status_sysex_realtime,
  status_sysex_mtc, 
  status_sysex_mtc_frame, 
  status_mtc_quarter_frame
};

enum parser_mtc_status
{
  mtc_status_invalid, 
  mtc_status_valid, 
  mtc_status_coming
};

enum channel_message 
{
  channel_message_note_off = 0,
  channel_message_note_on,
  channel_message_poly_pressure,
  channel_message_control_change,
  channel_message_program_change,
  channel_message_channel_pressure,
  channel_message_pitch_bend
};

#define NO_ARG -1

typedef struct _fts_midiparser_
{
  fts_midiport_t head;

  enum parser_status status;

  /* channel message */
  enum channel_message message;
  int channel;	
  int arg;

  /* MIDI time code */
  enum parser_mtc_status mtc_status;
  int mtc_frame_count;
  unsigned char mtc_type;
  unsigned char mtc_hour;
  unsigned char mtc_min;
  unsigned char mtc_sec;
  unsigned char mtc_frame;

  /* the time in millisecond corresponding to the MTC received */
  double mtc_time;
} fts_midiparser_t;

FTS_API void fts_midiparser_init(fts_midiparser_t *parser);
FTS_API void fts_midiparser_byte(fts_midiparser_t *parser, unsigned char byte);

#endif
