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

#ifndef _FTS_PRIVATE_MIDI_H_
#define _FTS_PRIVATE_MIDI_H_ 1


typedef struct _midilabel
{
  fts_symbol_t name;
  fts_midiport_t *input;
  fts_midiport_t *output;
  fts_symbol_t input_name;
  fts_symbol_t output_name;
  struct _midilabel *next;
} midilabel_t;

typedef struct _midiconfig
{
  fts_object_t o;
  midilabel_t *labels;
  int n_labels;
  int dirty;
} midiconfig_t;


extern fts_class_t* midiconfig_type;

void fts_midiconfig_set(midiconfig_t* config);
void fts_midiconfig_set_defaults(midiconfig_t* config);

#endif /* _FTS_PRIVATE_MIDI_H_ */
