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

#ifndef _MACOCXMIDI_H_
#define _MACOCXMIDI_H_

#include <CoreMIDI/MIDIServices.h>
#include <CoreAudio/HostTime.h>

typedef enum macosxmidi_type {macosxmidi_any = -1, macosxmidi_input = 2, macosxmidi_output = 3} macosxmidi_type_t;

typedef struct _macosxmidi_
{
  fts_midimanager_t manager;
  MIDIClientRef client;
  UInt64 delta;
  fts_hashtable_t inputs;
  fts_hashtable_t outputs;
  fts_hashtable_t sources;
  fts_hashtable_t destinations;
  fts_midififo_t fifo;
  int notify;
  int acknowledge;
} macosxmidi_t;

typedef struct _macosxmidiport_
{
  fts_midiport_t head;
  macosxmidi_t *manager;
  fts_symbol_t name;
  int id;  
  MIDIEndpointRef ref;
  MIDIPortRef port;
  fts_midiparser_t parser;
} macosxmidiport_t;

#define macosxmidiport_get_name(p) ((p)->name)
#define macosxmidiport_get_id(p) ((p)->id)
#define macosxmidiport_get_reference(p) ((p)->ref)

extern fts_class_t *macosxmidi_input_type;
extern fts_class_t *macosxmidi_output_type;

extern MIDIEndpointRef macosxmidi_get_reference_by_id(int id, macosxmidi_type_t type);

#endif
