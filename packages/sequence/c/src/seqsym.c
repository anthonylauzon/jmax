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

/* sequence components */
fts_symbol_t seqsym_sequence = 0;
fts_symbol_t seqsym_track = 0;
fts_symbol_t seqsym_eventtrk = 0;
fts_symbol_t seqsym_event = 0;

/* events */
fts_symbol_t seqsym_int = 0;
fts_symbol_t seqsym_float = 0;
fts_symbol_t seqsym_symbol = 0;
fts_symbol_t seqsym_note = 0;
fts_symbol_t seqsym_midival = 0;
fts_symbol_t seqsym_message = 0;

/* properties */
fts_symbol_t seqsym_midi_channel = 0;
fts_symbol_t seqsym_midi_velocity = 0;

/* messages */
fts_symbol_t seqsym_add = 0;
fts_symbol_t seqsym_set = 0;
fts_symbol_t seqsym_get_atoms = 0;
fts_symbol_t seqsym_print = 0;
fts_symbol_t seqsym_clear = 0;
fts_symbol_t seqsym_export_midi = 0;
fts_symbol_t seqsym_export_midi_dialog = 0;
fts_symbol_t seqsym_import_midi = 0;
fts_symbol_t seqsym_import_midi_dialog = 0;
fts_symbol_t seqsym_bmax_add_track = 0;
fts_symbol_t seqsym_bmax_add_event = 0;

/* messages to client */
fts_symbol_t seqsym_createEditor = 0;
fts_symbol_t seqsym_closeEditor = 0;
fts_symbol_t seqsym_destroyEditor = 0;
fts_symbol_t seqsym_addTracks = 0;
fts_symbol_t seqsym_deleteTracks = 0;
fts_symbol_t seqsym_moveTrack = 0;
fts_symbol_t seqsym_setName = 0;
fts_symbol_t seqsym_addEvents = 0;
fts_symbol_t seqsym_deleteEvents = 0;
fts_symbol_t seqsym_moveEvents = 0;
fts_symbol_t seqsym_highlightEvents = 0;
fts_symbol_t seqsym_openFileDialog = 0;
fts_symbol_t seqsym_lock = 0;
fts_symbol_t seqsym_unlock = 0;

void
seqsym_config(void)
{
  /* sequence components */
  seqsym_sequence = fts_new_symbol("sequence");
  seqsym_track = fts_new_symbol("track");
  seqsym_eventtrk = fts_new_symbol("eventtrk");
  seqsym_event = fts_new_symbol("event");

  /* event types */
  seqsym_int = fts_new_symbol("int");
  seqsym_float = fts_new_symbol("float");
  seqsym_symbol = fts_new_symbol("symbol");
  seqsym_note = fts_new_symbol("note");
  seqsym_midival = fts_new_symbol("midival");
  seqsym_message = fts_new_symbol("message");
  
  /* properties */
  seqsym_midi_channel = fts_new_symbol("midi_channel");
  seqsym_midi_velocity = fts_new_symbol("midi_velocity");

  /* messages */
  seqsym_add = fts_new_symbol("add");
  seqsym_set = fts_new_symbol("set");
  seqsym_get_atoms = fts_new_symbol("get_atoms");
  seqsym_print = fts_new_symbol("print");
  seqsym_clear = fts_new_symbol("clear");
  seqsym_export_midi = fts_new_symbol("export_midi");
  seqsym_export_midi_dialog = fts_new_symbol("export_midi_dialog");
  seqsym_import_midi = fts_new_symbol("import_midi");
  seqsym_import_midi_dialog = fts_new_symbol("import_midi_dialog");
  seqsym_bmax_add_track = fts_new_symbol("bmax_add_track");
  seqsym_bmax_add_event = fts_new_symbol("bmax_add_event");

  /* messages to client */
  seqsym_createEditor = fts_new_symbol("createEditor");
  seqsym_closeEditor = fts_new_symbol("closeEditor");
  seqsym_destroyEditor = fts_new_symbol("destroyEditor");
  seqsym_addTracks = fts_new_symbol("addTracks");
  seqsym_deleteTracks = fts_new_symbol("deleteTracks");
  seqsym_moveTrack = fts_new_symbol("moveTrack");
  seqsym_setName = fts_new_symbol("setName");
  seqsym_addEvents = fts_new_symbol("addEvents");
  seqsym_deleteEvents = fts_new_symbol("deleteEvents");
  seqsym_moveEvents = fts_new_symbol("moveEvents");
  seqsym_highlightEvents = fts_new_symbol("highlightEvents");
  seqsym_openFileDialog = fts_new_symbol("openFileDialog");
  seqsym_lock = fts_new_symbol("lock");
  seqsym_unlock = fts_new_symbol("unlock");
}
