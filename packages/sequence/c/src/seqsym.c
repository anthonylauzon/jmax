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

#include <fts/fts.h>

/* sequence components */
fts_symbol_t seqsym_sequence = 0;
fts_symbol_t seqsym_track = 0;
fts_symbol_t seqsym_miditrack = 0;
fts_symbol_t seqsym_event = 0;
fts_symbol_t seqsym_segment = 0;

/* events */
fts_symbol_t seqsym_note = 0;
fts_symbol_t seqsym_midi = 0;
fts_symbol_t seqsym_seqmess = 0;
fts_symbol_t seqsym_pitch = 0;
fts_symbol_t seqsym_duration = 0;
fts_symbol_t seqsym_velocity = 0;
fts_symbol_t seqsym_channel = 0;

/* messages */
fts_symbol_t seqsym_insert = 0;
fts_symbol_t seqsym_remove = 0;
fts_symbol_t seqsym_export_midifile = 0;
fts_symbol_t seqsym_export_midifile_dialog = 0;
fts_symbol_t seqsym_import_midifile = 0;
fts_symbol_t seqsym_import_midifile_dialog = 0;
fts_symbol_t seqsym_add_track = 0;
fts_symbol_t seqsym_make_track = 0;
fts_symbol_t seqsym_remove_track = 0;
fts_symbol_t seqsym_move_track = 0;
fts_symbol_t seqsym_add_event = 0;
fts_symbol_t seqsym_active = 0;
fts_symbol_t seqsym_dump_mess = 0;

/* messages from/to client */
fts_symbol_t seqsym_addTracks = 0;
fts_symbol_t seqsym_removeTracks = 0;
fts_symbol_t seqsym_moveTrack = 0;
fts_symbol_t seqsym_setName = 0;
fts_symbol_t seqsym_getName = 0;
fts_symbol_t seqsym_makeEvent = 0;
fts_symbol_t seqsym_addEvent = 0;
fts_symbol_t seqsym_addEvents = 0;
fts_symbol_t seqsym_removeEvents = 0;
fts_symbol_t seqsym_moveEvents = 0;
fts_symbol_t seqsym_highlightEvents = 0;
fts_symbol_t seqsym_openFileDialog = 0;
fts_symbol_t seqsym_endPaste = 0;

void
seqsym_config(void)
{
  /* sequence components */
  seqsym_sequence = fts_new_symbol("sequence");
  seqsym_miditrack = fts_new_symbol("miditrack");
  seqsym_track = fts_new_symbol("track");
  seqsym_event = fts_new_symbol("event");
  seqsym_segment = fts_new_symbol("segment");

  /* event types */
  seqsym_note = fts_new_symbol("note");
  seqsym_midi = fts_new_symbol("midi");
  seqsym_seqmess = fts_new_symbol("seqmess");
  seqsym_pitch = fts_new_symbol("pitch");
  seqsym_duration = fts_new_symbol("duration");
  seqsym_velocity = fts_new_symbol("velocity");
  seqsym_channel = fts_new_symbol("channel");
  
  /* messages */
  seqsym_insert = fts_new_symbol("insert");
  seqsym_remove = fts_new_symbol("remove");
  seqsym_export_midifile = fts_new_symbol("export_midifile");
  seqsym_export_midifile_dialog = fts_new_symbol("export_midifile_dialog");
  seqsym_import_midifile = fts_new_symbol("import_midifile");
  seqsym_import_midifile_dialog = fts_new_symbol("import_midifile_dialog");
  seqsym_add_track = fts_new_symbol("add_track");
  seqsym_make_track = fts_new_symbol("make_track");
  seqsym_remove_track = fts_new_symbol("remove_track");
  seqsym_move_track = fts_new_symbol("move_track");
  seqsym_add_event = fts_new_symbol("add_event");
  seqsym_active = fts_new_symbol("active");
  seqsym_dump_mess = fts_new_symbol("dump_mess");
  
  /* messages to client */
  seqsym_addTracks = fts_new_symbol("addTracks");
  seqsym_removeTracks = fts_new_symbol("removeTracks");
  seqsym_moveTrack = fts_new_symbol("moveTrack");
  seqsym_setName = fts_new_symbol("setName");
  seqsym_getName = fts_new_symbol("getName");
  seqsym_makeEvent = fts_new_symbol("makeEvent");
  seqsym_addEvent = fts_new_symbol("addEvent");
  seqsym_addEvents = fts_new_symbol("addEvents");
  seqsym_removeEvents = fts_new_symbol("removeEvents");
  seqsym_moveEvents = fts_new_symbol("moveEvents");
  seqsym_highlightEvents = fts_new_symbol("highlightEvents");
  seqsym_openFileDialog = fts_new_symbol("openFileDialog");
  seqsym_endPaste = fts_new_symbol("endPaste");
}
