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
 */

#ifndef _SEQUENCE_SEQSYM_H_
#define _SEQUENCE_SEQSYM_H_

#include <fts/fts.h>

/* sequence components */
extern fts_symbol_t seqsym_sequence;
extern fts_symbol_t seqsym_miditrack;
extern fts_symbol_t seqsym_track;
extern fts_symbol_t seqsym_event;
extern fts_symbol_t seqsym_segment;

/* events */
extern fts_symbol_t seqsym_scoob;
extern fts_symbol_t seqsym_undefined;
extern fts_symbol_t seqsym_note;
extern fts_symbol_t seqsym_interval;
extern fts_symbol_t seqsym_rest;
extern fts_symbol_t seqsym_trill;
extern fts_symbol_t seqsym_midi;
extern fts_symbol_t seqsym_seqmess;
extern fts_symbol_t seqsym_type;
extern fts_symbol_t seqsym_pitch;
extern fts_symbol_t seqsym_duration;
extern fts_symbol_t seqsym_velocity;
extern fts_symbol_t seqsym_channel;

/* messages */
extern fts_symbol_t seqsym_insert;
extern fts_symbol_t seqsym_remove;
extern fts_symbol_t seqsym_export_midifile;
extern fts_symbol_t seqsym_export_midifile_dialog;
extern fts_symbol_t seqsym_import_midifile;
extern fts_symbol_t seqsym_import_midifile_dialog;
extern fts_symbol_t seqsym_add_track;
extern fts_symbol_t seqsym_make_track;
extern fts_symbol_t seqsym_remove_track;
extern fts_symbol_t seqsym_move_track;
extern fts_symbol_t seqsym_add_event;
extern fts_symbol_t seqsym_active;
extern fts_symbol_t seqsym_dump_mess;
extern fts_symbol_t seqsym_properties;
extern fts_symbol_t seqsym_get_property_list;
extern fts_symbol_t seqsym_append_properties;

/* messages from/to client */
extern fts_symbol_t seqsym_addTracks;
extern fts_symbol_t seqsym_removeTracks;
extern fts_symbol_t seqsym_moveTrack;
extern fts_symbol_t seqsym_setName;
extern fts_symbol_t seqsym_getName;
extern fts_symbol_t seqsym_makeEvent;
extern fts_symbol_t seqsym_addEvent;
extern fts_symbol_t seqsym_addEvents;
extern fts_symbol_t seqsym_removeEvents;
extern fts_symbol_t seqsym_moveEvents;
extern fts_symbol_t seqsym_highlightEvents;
extern fts_symbol_t seqsym_highlightEventsAndTime;
extern fts_symbol_t seqsym_openFileDialog;
extern fts_symbol_t seqsym_endPaste;

#endif






