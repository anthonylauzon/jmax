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
 */

#ifndef _ALSAMIDI_H_
#define _ALSAMIDI_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <alsa/asoundlib.h>

typedef struct _alsamidi_
{
  fts_midimanager_t manager;
  fts_array_t inputs;
  fts_array_t outputs;
  fts_hashtable_t devices;
} alsamidi_t;

/* MIDI status bytes */
#define STATUS_BYTE_SYSEX 0xf0
#define STATUS_BYTE_SYSEX_END 0xf7
#define ALSA_SYSEX_BUFFER_LENGTH 512

typedef struct _alsarawmidiport_
{
  fts_midiport_t head;
  fts_midiparser_t parser;
  alsamidi_t *manager;
  fts_symbol_t name;
  fts_symbol_t hw_name;
  snd_rawmidi_t *handle_in;
  snd_rawmidi_t *handle_out;
  int fd;
  unsigned char receive_buffer[ALSA_SYSEX_BUFFER_LENGTH]; /* system exclusive output buffer */
  int sysex_head;
} alsarawmidiport_t;

extern fts_class_t *alsarawmidiport_type;

#endif
