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

#ifndef _SEQUENCE_SEQSYM_H_
#define _SEQUENCE_SEQSYM_H_

#include <fts/fts.h>

/* sequence components */
FTS_API fts_symbol_t seqsym_sequence;
FTS_API fts_symbol_t seqsym_miditrack;
FTS_API fts_symbol_t seqsym_track;
FTS_API fts_symbol_t seqsym_track_editor;
FTS_API fts_symbol_t seqsym_event;
FTS_API fts_symbol_t seqsym_segment;

/* events */
FTS_API fts_symbol_t seqsym_scoob;
FTS_API fts_symbol_t seqsym_scomark;
FTS_API fts_symbol_t seqsym_enum;
FTS_API fts_symbol_t seqsym_undefined;
FTS_API fts_symbol_t seqsym_note;
FTS_API fts_symbol_t seqsym_interval;
FTS_API fts_symbol_t seqsym_rest;
FTS_API fts_symbol_t seqsym_trill;
FTS_API fts_symbol_t seqsym_unvoiced;
FTS_API fts_symbol_t seqsym_midi;
FTS_API fts_symbol_t seqsym_seqmess;
FTS_API fts_symbol_t seqsym_bar;
FTS_API fts_symbol_t seqsym_marker;
FTS_API fts_symbol_t seqsym_bar_num;
FTS_API fts_symbol_t seqsym_end;
FTS_API fts_symbol_t seqsym_double;
FTS_API fts_symbol_t seqsym_type;
FTS_API fts_symbol_t seqsym_pitch;
FTS_API fts_symbol_t seqsym_duration;
FTS_API fts_symbol_t seqsym_velocity;
FTS_API fts_symbol_t seqsym_channel;
FTS_API fts_symbol_t seqsym_tempo;
FTS_API fts_symbol_t seqsym_meter;
FTS_API fts_symbol_t seqsym_cue;
FTS_API fts_symbol_t seqsym_section;
FTS_API fts_symbol_t seqsym_offset;
FTS_API fts_symbol_t seqsym_objid;
FTS_API fts_symbol_t seqsym_object;

/* messages */
FTS_API fts_symbol_t seqsym_insert;
FTS_API fts_symbol_t seqsym_remove;
FTS_API fts_symbol_t seqsym_export_midifile;
FTS_API fts_symbol_t seqsym_export_midifile_dialog;
FTS_API fts_symbol_t seqsym_import_midifile;
FTS_API fts_symbol_t seqsym_import_midifile_dialog;
FTS_API fts_symbol_t seqsym_add_track;
FTS_API fts_symbol_t seqsym_make_track;
FTS_API fts_symbol_t seqsym_remove_track;
FTS_API fts_symbol_t seqsym_move_track;
FTS_API fts_symbol_t seqsym_add_event;
FTS_API fts_symbol_t seqsym_add_marker;
FTS_API fts_symbol_t seqsym_active;
FTS_API fts_symbol_t seqsym_dump_mess;
FTS_API fts_symbol_t seqsym_properties;
FTS_API fts_symbol_t seqsym_get_property_list;
FTS_API fts_symbol_t seqsym_append_properties;
FTS_API fts_symbol_t seqsym_editor;
FTS_API fts_symbol_t seqsym_window;
FTS_API fts_symbol_t seqsym_label;
FTS_API fts_symbol_t seqsym_zoom;
FTS_API fts_symbol_t seqsym_transp;
FTS_API fts_symbol_t seqsym_view;
FTS_API fts_symbol_t seqsym_range_mode;
FTS_API fts_symbol_t seqsym_columns;
FTS_API fts_symbol_t seqsym_table_size;
FTS_API fts_symbol_t seqsym_save_editor;
FTS_API fts_symbol_t seqsym_editor_state;
FTS_API fts_symbol_t seqsym_set_editor;
FTS_API fts_symbol_t seqsym_grid_mode;
FTS_API fts_symbol_t seqsym_markers;
FTS_API fts_symbol_t seqsym_shift;
FTS_API fts_symbol_t seqsym_stretch;
FTS_API fts_symbol_t seqsym_quantize;
FTS_API fts_symbol_t seqsym_props_to_draw;

/* messages from/to client */
FTS_API fts_symbol_t seqsym_addTracks;
FTS_API fts_symbol_t seqsym_removeTracks;
FTS_API fts_symbol_t seqsym_moveTrack;
FTS_API fts_symbol_t seqsym_setName;
FTS_API fts_symbol_t seqsym_getName;
FTS_API fts_symbol_t seqsym_makeEvent;
FTS_API fts_symbol_t seqsym_addEvent;
FTS_API fts_symbol_t seqsym_addEvents;
FTS_API fts_symbol_t seqsym_removeEvents;
FTS_API fts_symbol_t seqsym_moveEvents;
FTS_API fts_symbol_t seqsym_moveEventsFromServer;
FTS_API fts_symbol_t seqsym_highlightEvents;
FTS_API fts_symbol_t seqsym_highlightEventsAndTime;
FTS_API fts_symbol_t seqsym_openFileDialog;
FTS_API fts_symbol_t seqsym_endPaste;

#endif






