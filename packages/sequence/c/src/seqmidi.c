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
#include "seqsym.h"
#include "sequence.h"
#include "track.h"
#include "eventtrk.h"
#include "note.h"
#include "midival.h"

#define N_MIDI_CHANNELS 16
#define N_MIDI_PITCHES 128
#define N_MIDI_CONTROLLERS 128

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
  event_t *note_is_on[N_MIDI_CHANNELS][N_MIDI_PITCHES];

  /* for reading midi controllers */
  eventtrk_t *control_track;
  eventtrk_t *program_track;

} seqmidi_read_data_t;

static int
seqmidi_read_track_start(fts_midifile_t *file)
{
  seqmidi_read_data_t *data = (seqmidi_read_data_t *)fts_midifile_get_user_data(file);
  int i;

  data->note_track = 0;
  data->control_track = 0;
  data->program_track = 0;

  return 1;
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
      fts_atom_t a[1];

      /* create new track */
      fts_set_symbol(a, seqsym_note);  
      track = (eventtrk_t *)fts_object_create(eventtrk_class, 1, a);

      /* add track to sequence */
      sequence_add_track(sequence, (track_t *)track);
      
      /* store track as current */
      data->note_track = track;
    }
  
  if(vel == 0 && data->note_is_on[chan][pitch] != 0)
    {
      event_t *event = data->note_is_on[chan][pitch];
      note_t *note = (note_t *)event_get_object(event);

      note_set_duration(note, time - event_get_time(event));
      data->note_is_on[chan][pitch] = 0;
    }
  else if(vel != 0)
    {
      note_t *note;
      event_t *event;
      fts_atom_t a[3];

      /* create a note */
      fts_set_int(a + 0, pitch);
      fts_set_float(a + 1, 0.0);
      note = (note_t *)fts_object_create(note_class, 2, a);

      /* set midi properties */
      note_set_midi_channel(note, chan);
      note_set_midi_velocity(note, vel);

      /* create a new event with the note */
      fts_set_object(a, (fts_object_t *)note);
      event = (event_t *)fts_object_create(event_class, 1, a);

      /* add the event to track */
      eventtrk_add_event(track, time, event);

      /* register note as on */
      data->note_is_on[chan][pitch] = event;
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
      event_t *event = data->note_is_on[chan][pitch];
      note_t *note = (note_t *)event_get_object(event);

      note_set_duration(note, time - event_get_time(event));
      data->note_is_on[chan][pitch] = 0;
    }

  return 1;
}

static int
seqmidi_read_control_change(fts_midifile_t *file, int chan, int number, int value)
{
  seqmidi_read_data_t *data = (seqmidi_read_data_t *)fts_midifile_get_user_data(file);
  eventtrk_t *track = data->control_track;
  double time = 1000.0 * fts_midifile_get_current_time_in_seconds(file);
  event_t *event;
  midival_t *midival;
  fts_atom_t a[3];
  
  if(!track)
    {
      sequence_t *sequence = data->sequence;

      /* create new track */
      fts_set_symbol(a, seqsym_midival);  
      track = (eventtrk_t *)fts_object_create(eventtrk_class, 1, a);

      /* add track to sequence */
      sequence_add_track(sequence, (track_t *)track);
      
      /* store track as current */
      data->control_track = track;
    }
  
  fts_set_int(a + 0, value);
  fts_set_int(a + 1, number);
  fts_set_int(a + 2, chan);
  midival = (midival_t *)fts_object_create(midival_class, 3, a);
  
  fts_set_object(a, (fts_object_t *)midival);
  event = (event_t *)fts_object_create(event_class, 1, a);
  
  /* add event to track */
  eventtrk_add_event(track, time, event);

  return 1;
}

static int
seqmidi_read_program_change(fts_midifile_t *file, int chan, int program)
{
  seqmidi_read_data_t *data = (seqmidi_read_data_t *)fts_midifile_get_user_data(file);
  eventtrk_t *track = data->program_track;
  double time = 1000.0 * fts_midifile_get_current_time_in_seconds(file);
  event_t *event;
  midival_t *midival;
  fts_atom_t a[3];
  
  if(!track)
    {
      sequence_t *sequence = data->sequence;

      /* create new track */
      fts_set_symbol(a, seqsym_midival);
      track = (eventtrk_t *)fts_object_create(eventtrk_class, 1, a);

      /* add track to sequence */
      sequence_add_track(sequence, (track_t *)track);
      
      /* store track as current */
      data->program_track = track;
    }
  
  fts_set_int(a + 0, program);
  fts_set_int(a + 1, 0);
  fts_set_int(a + 2, chan);
  midival = (midival_t *)fts_object_create(midival_class, 3, a);
  
  fts_set_object(a, (fts_object_t *)midival);
  event = (event_t *)fts_object_create(event_class, 1, a);
  
  /* add event to track */
  eventtrk_add_event(track, time, event);

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
	  event_t *event = data->note_is_on[i][j];
	  note_t *note = (note_t *)event_get_object(event);

	  note_set_duration(note, time - event_get_time(event));
	  data->note_is_on[i][j] = 0;
	}

  /* close tracks */
  data->note_track = 0;
  data->control_track = 0;
  data->program_track = 0;

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
      read.control_change = seqmidi_read_control_change;
      read.program_change = seqmidi_read_program_change;

      fts_midifile_set_read_functions(file, &read);

      data.sequence = sequence;

      /* init note track */
      data.note_track = 0;

      /* set oll notes to off */
      for(i=0; i<N_MIDI_CHANNELS; i++)
	for(j=0; j<N_MIDI_PITCHES; j++)
	  data.note_is_on[i][j] = 0;

      /* init controller track */
      data.control_track = 0;
      data.program_track = 0;

      fts_midifile_set_user_data(file, &data);
      
      if(fts_midifile_read(file) <= 0)
	post("error reading MIDI file %s: %s\n", file->name, (file->error)? file->error: "unknown error");
      
      fts_midifile_close(file);

      return 1;
    }
  else
    {
      post("MIDI file not found: %s\n", fts_symbol_name(name));
      return 0;
    }
}

/**************************************************************************
 *
 *  writing MIDI files
 *
 */

/* note status pseudo event (used to keep track of note offs while writing midi files) */
#define notestat_get_channel(e) ((fts_get_int(event_get_value(e)) >> 8) & 15)
#define notestat_get_pitch(e) ((fts_get_int(event_get_value(e)) >> 1) & 127)
#define notestat_is_on(e) (fts_get_int(event_get_value(e)) & 1)
#define notestat_set_off(e) (fts_get_int(event_get_value(e)) &= 0xFFFE)
#define notestat_set_on(e) (fts_get_int(event_get_value(e)) |= 1)

static void
notestat_init(event_t *event, int channel, int pitch)
{
  int stat = ((channel & 15) << 8) + ((pitch & 127) << 1);

  event_set_int(event, stat);
}

typedef struct _seqmidi_write_data_
{
  eventtrk_t *track;
  eventtrk_t *note_off_track;
  event_t notestats[17][128]; /* matrix of note_off events (channels x pitches) */
} seqmidi_write_data_t;

static void
seqmidi_write_note_on(fts_midifile_t *file, double time, note_t *note)
{
  seqmidi_write_data_t *data = (seqmidi_write_data_t *)fts_midifile_get_user_data(file);
  int velocity = note_get_midi_velocity(note);
  int channel = note_get_midi_channel(note);
  int pitch = note_get_pitch(note);
  double off_time = time + note_get_duration(note);
  long time_in_ticks = fts_midifile_seconds_to_ticks(file, 0.001 * time);
  event_t *stat = &(data->notestats[channel][pitch]);

  if(notestat_is_on(stat))
    {
      fts_midifile_write_note_off(file, time_in_ticks, channel, pitch, 0); 
    
      eventtrk_remove_event(data->note_off_track, stat);
    
      fts_midifile_write_note_on(file, time_in_ticks, channel, pitch, velocity);
    }
  else
    fts_midifile_write_note_on(file, time_in_ticks, channel, pitch, velocity);
    
  notestat_set_on(stat);

  eventtrk_add_event(data->note_off_track, off_time, stat);
}

/* write all pending note offs until given time */
static void
seqmidi_write_note_offs(fts_midifile_t *file, double time)
{
  seqmidi_write_data_t *data = (seqmidi_write_data_t *)fts_midifile_get_user_data(file);
  event_t *stat = eventtrk_get_first(data->note_off_track);

  while(stat && event_get_time(stat) <= time)
    {
      long off_time_in_ticks = fts_midifile_seconds_to_ticks(file, 0.001 * event_get_time(stat));
      int off_channel = notestat_get_channel(stat);
      int off_pitch = notestat_get_pitch(stat);
      
      /* write note off */
      fts_midifile_write_note_off(file, off_time_in_ticks, off_channel, off_pitch, 0); 
      
      /* set note to off */
      notestat_set_off(stat);
      
      /* remove note off event from note off track */
      eventtrk_remove_event(data->note_off_track, stat);
      
      /* get next note off in sequence */
      stat = eventtrk_get_first(data->note_off_track);
    }
}

int
seqmidi_write_midifile_from_note_track(eventtrk_t *track, fts_symbol_t file_name)
{
  fts_symbol_t track_name = track_get_name(&track->head);
  fts_midifile_t *file = fts_midifile_open_write(file_name);
 
  if(file)
    {
      seqmidi_write_data_t data;
      event_t *event;
      fts_atom_t a[1];
      int i, j;

      fts_midifile_set_user_data(file, &data);

      data.track = track;
  
      fts_set_symbol(a, seqsym_export_midi);  
      data.note_off_track = (eventtrk_t *)fts_object_create(eventtrk_class, 1, a);
      
      for(i=0; i<=N_MIDI_CHANNELS; i++)
	for(j=0; j<N_MIDI_PITCHES; j++)
	  notestat_init(&(data.notestats[i][j]), i, j);
  
      fts_midifile_write_header(file, 0, 1, 384);
	
      fts_midifile_write_track_begin(file);
      
      fts_midifile_write_tempo(file, 500000);
	
      event = eventtrk_get_first(track);
      while(event)
	{
	  double time = event_get_time(event);
	  
	  seqmidi_write_note_offs(file, time);
  
	  seqmidi_write_note_on(file, time, (note_t *)fts_get_object(&event->value));
	  
	  event = event_get_next(event);
	}  

      if(eventtrk_get_size(data.note_off_track) > 0)
	seqmidi_write_note_offs(file, event_get_time(eventtrk_get_last(data.note_off_track)));
	
      fts_object_delete((fts_object_t *)(data.note_off_track));
	
      fts_midifile_write_track_end(file);

      fts_midifile_close(file);
    }
  else
    {
      post("sequence track %s: cannot open file %s\n", fts_symbol_name(track_name), fts_symbol_name(file_name));
      return 0;
    }
  return 1;
}


