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
extern fts_symbol_t seqsym_noteevt;
extern fts_symbol_t seqsym_floatevt;
extern fts_symbol_t seqsym_intevt;
extern fts_symbol_t seqsym_messevt;

/* properties */
extern fts_symbol_t seqsym_midi_channel;
extern fts_symbol_t seqsym_midi_velocity;

/* messages */
extern fts_symbol_t seqsym_get_atoms;
extern fts_symbol_t seqsym_export_midi;
extern fts_symbol_t seqsym_export_midi_dialog;
extern fts_symbol_t seqsym_import_midi;
extern fts_symbol_t seqsym_import_midi_dialog;

/* messages to client */
extern fts_symbol_t seqsym_openEditor;
extern fts_symbol_t seqsym_destroyEditor;
extern fts_symbol_t seqsym_addTracks;
extern fts_symbol_t seqsym_addEvents;
extern fts_symbol_t seqsym_dialogFileSave;

#endif

