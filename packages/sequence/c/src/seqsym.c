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

/* sequence components */
fts_symbol_t seqsym_sequence = 0;
fts_symbol_t seqsym_track = 0;
fts_symbol_t seqsym_eventtrk = 0;
fts_symbol_t seqsym_event = 0;

/* events */
fts_symbol_t seqsym_noteevt = 0;
fts_symbol_t seqsym_floatevt = 0;
fts_symbol_t seqsym_intevt = 0;
fts_symbol_t seqsym_messevt = 0;

/* properties */
fts_symbol_t seqsym_midi_channel = 0;
fts_symbol_t seqsym_midi_velocity = 0;

/* messages */
fts_symbol_t seqsym_get_atoms = 0;
fts_symbol_t seqsym_export_midi = 0;
fts_symbol_t seqsym_export_midi_dialog = 0;
fts_symbol_t seqsym_import_midi = 0;
fts_symbol_t seqsym_import_midi_dialog = 0;

/* messages to client */
fts_symbol_t seqsym_createEditor = 0;
fts_symbol_t seqsym_destroyEditor = 0;
fts_symbol_t seqsym_addTracks = 0;
fts_symbol_t seqsym_deleteTracks = 0;
fts_symbol_t seqsym_addEvents = 0;
fts_symbol_t seqsym_deleteEvents = 0;
fts_symbol_t seqsym_openFileDialog = 0;

void
seqsym_config(void)
{
  /* sequence components */
  seqsym_sequence = fts_new_symbol("sequence");
  seqsym_track = fts_new_symbol("track");
  seqsym_eventtrk = fts_new_symbol("eventtrk");
  seqsym_event = fts_new_symbol("event");

  /* events */
  seqsym_noteevt = fts_new_symbol("noteevt");
  seqsym_floatevt = fts_new_symbol("floatevt");
  seqsym_intevt = fts_new_symbol("intevt");
  seqsym_messevt = fts_new_symbol("messevt");
  
  /* properties */
  seqsym_midi_channel = fts_new_symbol("midi_channel");
  seqsym_midi_velocity = fts_new_symbol("midi_velocity");

  /* messages */
  seqsym_get_atoms = fts_new_symbol("get_atoms");
  seqsym_export_midi = fts_new_symbol("export_midi");
  seqsym_export_midi_dialog = fts_new_symbol("export_midi_dialog");
  seqsym_import_midi = fts_new_symbol("import_midi");
  seqsym_import_midi_dialog = fts_new_symbol("import_midi_dialog");

  /* messages to client */
  seqsym_createEditor = fts_new_symbol("createEditor");
  seqsym_destroyEditor = fts_new_symbol("destroyEditor");
  seqsym_addTracks = fts_new_symbol("addTracks");
  seqsym_deleteTracks = fts_new_symbol("deleteTracks");
  seqsym_addEvents = fts_new_symbol("addEvents");
  seqsym_deleteEvents = fts_new_symbol("deleteEvents");
  seqsym_openFileDialog = fts_new_symbol("openFileDialog");
}
