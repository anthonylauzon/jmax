/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <fts/fts.h>

/* sequence components */
fts_symbol_t seqsym_sequence = 0;
fts_symbol_t seqsym_track = 0;
fts_symbol_t seqsym_track_editor = 0;
fts_symbol_t seqsym_miditrack = 0;
fts_symbol_t seqsym_event = 0;
fts_symbol_t seqsym_segment = 0;

/* events */
fts_symbol_t seqsym_scoob = 0;
fts_symbol_t seqsym_scomark = 0;
fts_symbol_t seqsym_enum = 0;
fts_symbol_t seqsym_undefined = 0;
fts_symbol_t seqsym_note = 0;
fts_symbol_t seqsym_interval = 0;
fts_symbol_t seqsym_rest = 0;
fts_symbol_t seqsym_trill = 0;
fts_symbol_t seqsym_unvoiced = 0;
fts_symbol_t seqsym_midi = 0;
fts_symbol_t seqsym_seqmess = 0;
fts_symbol_t seqsym_bar = 0;
fts_symbol_t seqsym_bar_num = 0;
fts_symbol_t seqsym_end = 0;
fts_symbol_t seqsym_double = 0;
fts_symbol_t seqsym_type = 0;
fts_symbol_t seqsym_pitch = 0;
fts_symbol_t seqsym_duration = 0;
fts_symbol_t seqsym_velocity = 0;
fts_symbol_t seqsym_channel = 0;
fts_symbol_t seqsym_tempo = 0;
fts_symbol_t seqsym_meter = 0;
fts_symbol_t seqsym_cue = 0;
fts_symbol_t seqsym_section = 0;
fts_symbol_t seqsym_offset = 0;
fts_symbol_t seqsym_objid = 0;
fts_symbol_t seqsym_object = 0;

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
fts_symbol_t seqsym_add_marker = 0;
fts_symbol_t seqsym_active = 0;
fts_symbol_t seqsym_dump_mess = 0;
fts_symbol_t seqsym_properties = 0;
fts_symbol_t seqsym_get_property_list = 0;
fts_symbol_t seqsym_append_properties = 0;
fts_symbol_t seqsym_editor = 0;
fts_symbol_t seqsym_window = 0;
fts_symbol_t seqsym_label = 0;
fts_symbol_t seqsym_zoom = 0;
fts_symbol_t seqsym_transp = 0;
fts_symbol_t seqsym_view = 0;
fts_symbol_t seqsym_range_mode = 0;
fts_symbol_t seqsym_columns = 0;
fts_symbol_t seqsym_table_size = 0;
fts_symbol_t seqsym_save_editor = 0;
fts_symbol_t seqsym_editor_state = 0;
fts_symbol_t seqsym_set_editor = 0;
fts_symbol_t seqsym_markers = 0;

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
fts_symbol_t seqsym_highlightEventsAndTime = 0;
fts_symbol_t seqsym_openFileDialog = 0;
fts_symbol_t seqsym_endPaste = 0;

void
seqsym_config(void)
{
  /* sequence components */
  seqsym_sequence = fts_new_symbol("sequence");
  seqsym_miditrack = fts_new_symbol("miditrack");
  seqsym_track = fts_new_symbol("track");
  seqsym_track_editor = fts_new_symbol("track_editor");
  seqsym_event = fts_new_symbol("event");
  seqsym_segment = fts_new_symbol("segment");

  /* event types */
  seqsym_scoob = fts_new_symbol("scoob");
  seqsym_scomark = fts_new_symbol("scomark");
  seqsym_enum = fts_new_symbol("enum");
  seqsym_undefined = fts_new_symbol("undefined");
  seqsym_note = fts_new_symbol("note");
  seqsym_interval = fts_new_symbol("interval");
  seqsym_rest = fts_new_symbol("rest");
  seqsym_trill = fts_new_symbol("trill");
  seqsym_unvoiced = fts_new_symbol("unvoiced");
  seqsym_midi = fts_new_symbol("midi");
  seqsym_seqmess = fts_new_symbol("seqmess");
  seqsym_bar = fts_new_symbol("bar");
  seqsym_bar_num = fts_new_symbol("bar #");
  seqsym_end = fts_new_symbol("end");
  seqsym_double = fts_new_symbol("double");
  seqsym_type = fts_new_symbol("type");
  seqsym_pitch = fts_new_symbol("pitch");
  seqsym_duration = fts_new_symbol("duration");
  seqsym_velocity = fts_new_symbol("velocity");
  seqsym_channel = fts_new_symbol("channel");
  seqsym_tempo = fts_new_symbol("tempo");
  seqsym_meter = fts_new_symbol("meter");
  seqsym_cue = fts_new_symbol("cue");
  seqsym_section = fts_new_symbol("section");
  seqsym_offset = fts_new_symbol("offset");
  seqsym_objid = fts_new_symbol("objid");
  seqsym_object = fts_new_symbol("object");
  
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
  seqsym_add_marker = fts_new_symbol("add_marker");
  seqsym_active = fts_new_symbol("active");
  seqsym_dump_mess = fts_new_symbol("dump_mess");
  seqsym_properties = fts_new_symbol("properties");
  seqsym_get_property_list = fts_new_symbol("get_property_list");
  seqsym_append_properties = fts_new_symbol("append_properties");
  seqsym_editor = fts_new_symbol("editor");
  seqsym_window = fts_new_symbol("window");
  seqsym_label = fts_new_symbol("label");
  seqsym_zoom = fts_new_symbol("zoom");
  seqsym_transp = fts_new_symbol("transp");
	seqsym_view = fts_new_symbol("view");
	seqsym_range_mode = fts_new_symbol("range_mode");
	seqsym_columns = fts_new_symbol("columns");
	seqsym_table_size = fts_new_symbol("tableSize");
  seqsym_save_editor = fts_new_symbol("save_editor");
	seqsym_editor_state = fts_new_symbol("editor_state");
	seqsym_set_editor = fts_new_symbol("set_editor");
	seqsym_markers = fts_new_symbol("markers");
		
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
  seqsym_highlightEventsAndTime = fts_new_symbol("highlightEventsAndTime");
  seqsym_openFileDialog = fts_new_symbol("openFileDialog");
  seqsym_endPaste = fts_new_symbol("endPaste");
}
