/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundationm; either version 2
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
#include <ctype.h>

#include "fts.h"
#include "sequence.h"
#include "track.h"
#include "eventtrk.h"
#include "noteevt.h"
#include "noteoffevt.h"
#include "intevt.h"

#define N_MIDI_CHANNELS 16
#define N_MIDI_PITCHES 128
#define N_MIDI_CONTROLLERS 128

static fts_symbol_t sym_note = 0;

/**************************************************************************
 *
 *  reading MIDI files
 *
 */

typedef struct _seqmidi_read_data_
{
  sequence_t *sequence;

  /* for reading notes */
  eventtrk_t *note_track;
  noteevt_t *note_is_on[N_MIDI_CHANNELS][N_MIDI_PITCHES];

  /* for reading midi controllers */
  eventtrk_t *controller_tracks[N_MIDI_CONTROLLERS];
} seqmidi_read_data_t;

static int
seqmidi_read_track_start(fts_midifile_t *file)
{
  seqmidi_read_data_t *data = (seqmidi_read_data_t *)fts_midifile_get_user_data(file);
  int i;

  data->note_track = 0;

  for(i=0; i<N_MIDI_CONTROLLERS; i++)
    data->controller_tracks[i] = 0;

  return 1;
}


static void
seqmidi_set_note_off(noteevt_t *note, double time)
{
  double duration = time - event_get_time((event_t *)note);

  noteevt_set_duration(note,  duration);
}

static int
seqmidi_read_note_on(fts_midifile_t *file, int chan, int pitch, int vel)
{
  seqmidi_read_data_t *data = (seqmidi_read_data_t *)fts_midifile_get_user_data(file);
  eventtrk_t *track = data->note_track;
  double time = 1000.0 * fts_midifile_get_current_time_in_seconds(file);

  if(!track)
    {
      sequence_t *sequence = data->sequence;
      char s[] = "track9999";
      fts_symbol_t name;
      fts_atom_t a[3];
  
      sprintf(s, "track%d", sequence_get_size(sequence));
      name = fts_new_symbol_copy(s);

      /* create new track */
      fts_set_symbol(a + 0, eventtrk_symbol);
      fts_set_symbol(a + 1, name);
      fts_set_symbol(a + 2, noteevt_symbol);
      fts_object_new(0, 3, a, (fts_object_t **)&track);

      /* add track to sequence */
      sequence_add_track(sequence, (track_t *)track);
      
      /* store track as current */
      data->note_track = track;
    }
  
  if(vel == 0 && data->note_is_on[chan][pitch] != 0)
    {
      seqmidi_set_note_off(data->note_is_on[chan][pitch], time);
      data->note_is_on[chan][pitch] = 0;
    }
  else
    {
      fts_object_t *note;
      fts_atom_t a[3];

      fts_set_symbol(a + 0, noteevt_symbol);
      fts_set_int(a + 1, pitch);
      fts_set_float(a + 2, 0.0);
      fts_object_new(0, 3, a, &note);

      /* add event to track */
      eventtrk_add_event(track, time, (event_t *)note);

      data->note_is_on[chan][pitch] = (noteevt_t *)note;
    }
    
  return 1;
}

static int
seqmidi_read_note_off(fts_midifile_t *file, int chan, int pitch, int vel)
{
  seqmidi_read_data_t *data = (seqmidi_read_data_t *)fts_midifile_get_user_data(file);
  double time = 1000.0 * fts_midifile_get_current_time_in_seconds(file);

  if(data->note_is_on[chan][pitch] != 0)
    {
      seqmidi_set_note_off(data->note_is_on[chan][pitch], time);
      data->note_is_on[chan][pitch] = 0;
    }

  return 1;
}

static int
seqmidi_read_controller(fts_midifile_t *file, int chan, int ctrl_num, int value)
{
  seqmidi_read_data_t *data = (seqmidi_read_data_t *)fts_midifile_get_user_data(file);
  eventtrk_t *track = data->controller_tracks[ctrl_num];
  double time = 1000.0 * fts_midifile_get_current_time_in_seconds(file);
  fts_object_t *intevt;
  fts_atom_t a[3];
  
  if(!track)
    {
      sequence_t *sequence = data->sequence;
      char s[] = "track9999";
      fts_symbol_t name;
  
      sprintf(s, "track%d", sequence_get_size(sequence));
      name = fts_new_symbol_copy(s);

      /* create new track */
      fts_set_symbol(a + 0, eventtrk_symbol);
      fts_set_symbol(a + 1, name);
      fts_set_symbol(a + 2, intevt_symbol);
      fts_object_new(0, 3, a, (fts_object_t **)&track);

      /* add track to sequence */
      sequence_add_track(sequence, (track_t *)track);
      
      /* store track as current */
      data->controller_tracks[ctrl_num] = track;
    }
  
  fts_set_symbol(a + 0, intevt_symbol);
  fts_set_int(a + 1, value);
  fts_object_new(0, 2, a, &intevt);
  
  /* add event to track */
  eventtrk_add_event(track, time, (event_t *)intevt);
  
  return 1;
}

static int
seqmidi_read_track_end(fts_midifile_t *file)
{
  seqmidi_read_data_t *data = (seqmidi_read_data_t *)fts_midifile_get_user_data(file);
  double time = 1000.0 * fts_midifile_get_current_time_in_seconds(file);
  int i, j;
  
  /* shut down all pending note ons */
  for(i=0; i<N_MIDI_CHANNELS; i++)
    for(j=0; j<N_MIDI_PITCHES; j++)
      if(data->note_is_on[i][j] != 0)
	{
	  seqmidi_set_note_off(data->note_is_on[i][j], time);
	  data->note_is_on[i][j] = 0;
	}

  /* close tracks */
  data->note_track = 0;
  for(i=0; i<N_MIDI_CONTROLLERS; i++)
    data->controller_tracks[i] = 0;

  return 1;
}

int
sequence_read_midifile(sequence_t *sequence, fts_symbol_t name)
{
  char ch;
  fts_midifile_t *file = fts_midifile_open_read(name);
  fts_midifile_read_functions_t read;
  seqmidi_read_data_t data;
  int i, j;

  if(file)
    {    
      fts_midifile_read_functions_init(&read);
      
      read.track_start = seqmidi_read_track_start;
      read.track_end = seqmidi_read_track_end;
      read.note_on = seqmidi_read_note_on;
      read.note_off = seqmidi_read_note_off;
      read.control_change = seqmidi_read_controller;

      fts_midifile_set_read_functions(file, &read);

      data.sequence = sequence;

      /* init note track */
      data.note_track = 0;

      /* set oll notes to off */
      for(i=0; i<N_MIDI_CHANNELS; i++)
	for(j=0; j<N_MIDI_PITCHES; j++)
	  data.note_is_on[i][j] = 0;

      /* init controller track */
      for(i=0; i<N_MIDI_CONTROLLERS; i++)
	data.controller_tracks[i] = 0;

      fts_midifile_set_user_data(file, &data);
      
      if(fts_midifile_read(file) <= 0)
	post("error reading file %s: %s\n", file->name, (file->error)? file->error: "unknown error");
      
      fts_midifile_close(file);

      return 1;
    }
  else
    return 0;
}

/**************************************************************************
 *
 *  writing MIDI files
 *
 */

typedef struct _seqmidi_write_data_
{
  eventtrk_t *track;
  noteoffevt_t *note_offs[128]; /* array of note_off events */
} seqmidi_write_data_t;

eventtrk_t *
seqmidi_write_get_track(fts_midifile_t *file)
{
  seqmidi_write_data_t *data = (seqmidi_write_data_t *)fts_midifile_get_user_data(file);
  
  return data->track;
}

noteoffevt_t *
seqmidi_write_get_note_off(fts_midifile_t *file, int i)
{
  seqmidi_write_data_t *data = (seqmidi_write_data_t *)fts_midifile_get_user_data(file);
  
  return data->note_offs[i];
}

int
eventtrk_write_midifile(eventtrk_t *track, fts_symbol_t file_name)
{
  fts_symbol_t track_name = track_get_name(&track->head);
  fts_midifile_t *file = fts_midifile_open_write(file_name);
 
  if(file)
    {
      fts_symbol_t sym_export_midi = fts_new_symbol("export_midi");
      seqmidi_write_data_t data;
      fts_atom_t a[2];
      event_t *event;
      int i;

      /* init note track */
      data.track = track;
	  
      /* init note offs array */
      fts_set_symbol(a, noteoffevt_symbol);
      for(i=0; i<N_MIDI_PITCHES; i++)
	{
	  fts_set_int(a + 1, i);
	  fts_object_new(0, 2, a, (fts_object_t **)(data.note_offs + i));
	}
	  
      fts_midifile_set_user_data(file, &data);

	  /* start wrtiting the file */
      fts_midifile_write_header(file, 0, 1, 96); /* format 0, 1 track, division = 96 */
      fts_midifile_write_tempo(file, 500000); /* 120 bpm */
	  
      fts_midifile_write_track_begin(file);

      /* to send file to event */
      fts_set_ptr(a, file);

      event = eventtrk_get_first(track);
      while(event)
	{
	  event_t *next = event_get_next(event);

	  fts_send_message((fts_object_t *)event, fts_SystemInlet, sym_export_midi, 1, a);
	  event = next;
	}  
	  
      fts_midifile_write_track_end(file);	  
	  
	  /* destroy note offs array */	  
      for(i=0; i<N_MIDI_PITCHES; i++)
	fts_object_delete((fts_object_t *)(data.note_offs[i]));
	  
      fts_midifile_close(file);
    }
  else
    {
      post("sequence track %s: cannot open file %s\n", fts_symbol_name(track_name), fts_symbol_name(file_name));
      return 0;
    }

  return 1;
}
