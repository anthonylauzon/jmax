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
#ifndef _SEQSYM_H_
#define _SEQSYM_H_

#include "fts.h"

/* sequence components */
extern fts_symbol_t seqsym_sequence;
extern fts_symbol_t seqsym_track;
extern fts_symbol_t seqsym_eventtrk;
extern fts_symbol_t seqsym_event;

/* events */
extern fts_symbol_t seqsym_float;
extern fts_symbol_t seqsym_int;
extern fts_symbol_t seqsym_symbol;
extern fts_symbol_t seqsym_note;
extern fts_symbol_t seqsym_midival;
extern fts_symbol_t seqsym_message;

/* properties */
extern fts_symbol_t seqsym_midi_channel;
extern fts_symbol_t seqsym_midi_velocity;

/* messages */
extern fts_symbol_t seqsym_add;
extern fts_symbol_t seqsym_set;
extern fts_symbol_t seqsym_get_atoms;
extern fts_symbol_t seqsym_print;
extern fts_symbol_t seqsym_clear;
extern fts_symbol_t seqsym_export_midi;
extern fts_symbol_t seqsym_export_midi_dialog;
extern fts_symbol_t seqsym_import_midi;
extern fts_symbol_t seqsym_import_midi_dialog;
extern fts_symbol_t seqsym_bmax_add_track;
extern fts_symbol_t seqsym_bmax_add_event;

/* messages to client */
extern fts_symbol_t seqsym_createEditor;
extern fts_symbol_t seqsym_destroyEditor;
extern fts_symbol_t seqsym_addTracks;
extern fts_symbol_t seqsym_deleteTracks;
extern fts_symbol_t seqsym_moveTrack;
extern fts_symbol_t seqsym_setName;
extern fts_symbol_t seqsym_addEvents;
extern fts_symbol_t seqsym_deleteEvents;
extern fts_symbol_t seqsym_moveEvents;
extern fts_symbol_t seqsym_highlightEvents;
extern fts_symbol_t seqsym_openFileDialog;
extern fts_symbol_t seqsym_lock;
extern fts_symbol_t seqsym_unlock;

#endif

