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

#include <fts/fts.h>
#include <ftsconfig.h>
#if HAVE_STRING_H
#include <string.h>
#endif
#include "seqsym.h"
#include "sequence.h"
#include "track.h"
#include "note.h"

#define STRING_SIZE 1024

static void
get_stripped_file_name_with_index(char *name_str, fts_symbol_t name, int index)
{
  const char *s = fts_symbol_name(name);
  int size;
  int i = strlen(s) - 1;

  while(i > 0 && s[i] != '/' && s[i] != '.')
    i--;

  if(s[i] == '.')
    size = i;
  else
    size = strlen(s);

  while(i > 0 && s[i - 1] != '/')
    i--;

  size -= i;

  if(size > STRING_SIZE - 5)
    size = STRING_SIZE - 5;

  snprintf(name_str, size + 1, "%s", s + i);
  snprintf(name_str + size, 4, "-%d", index);
}

/**************************************************************************
 *
 *  read utilities
 *
 */
typedef struct _seqmidi_read_data_
{
  sequence_t *sequence;
  track_t *track;
  int track_index;

  event_t *note_is_on[n_midi_channels][n_midi_notes]; /* table for reading notes */

  double tempo; /* relative tempo */

  int size; /* number of event read */
} seqmidi_read_data_t;

static void
seqmidi_read_data_init(seqmidi_read_data_t *data)
{
  data->sequence = 0;
  data->track = 0;
  data->track_index = 0;
  data->size = 0;
} 

static void
miditrack_read_midievent(fts_midifile_t *file, fts_midievent_t *midievt)
{
  seqmidi_read_data_t *data = (seqmidi_read_data_t *)fts_midifile_get_user_data(file);
  track_t *track = data->track;
  double time = fts_midifile_get_time(file);
  event_t *event;
  fts_atom_t a;
    
  /* create a new event with the event */
  fts_set_object_with_type(&a, midievt, fts_s_midievent);
  event = (event_t *)fts_object_create(event_class, 1, &a);

  /* claim object */
  fts_object_refer(midievt);
  
  /* add the event to track */
  track_append_event(track, time, event);

  data->size++;
}

static void
notetrack_read_midievent(fts_midifile_t *file, fts_midievent_t *midievt)
{
  seqmidi_read_data_t *data = (seqmidi_read_data_t *)fts_midifile_get_user_data(file);
  track_t *track = data->track;
  double time = fts_midifile_get_time(file);

  if(fts_midievent_is_note(midievt))
    {
      int channel = fts_midievent_channel_message_get_channel(midievt);
      int pitch = fts_midievent_channel_message_get_first(midievt);
      int velocity = fts_midievent_channel_message_get_second(midievt);
      
      if(velocity == 0 && data->note_is_on[channel][pitch] != 0)
	{
	  event_t *event = data->note_is_on[channel][pitch];
	  note_t *note = (note_t *)event_get_object(event);
	  
	  note_set_duration(note, time - event_get_time(event));
	  data->note_is_on[channel][pitch] = 0;
	}
      else if(velocity != 0)
	{
	  note_t *note;
	  event_t *event;
	  fts_atom_t a[3];
	  
	  /* create a note */
	  fts_set_int(a + 0, pitch);
	  fts_set_float(a + 1, 0.0);
	  note = (note_t *)fts_object_create(note_class, 2, a);
	  
	  /* create a new event with the note */
	  fts_set_object(a, (fts_object_t *)note);
	  event = (event_t *)fts_object_create(event_class, 1, a);
	  
	  /* add the event to track */
	  track_append_event(track, time, event);
	  data->size++;
	  
	  /* register note as on */
	  data->note_is_on[channel][pitch] = event;
	}
    }
}

static void
notetrack_read_track_end(fts_midifile_t *file)
{
  seqmidi_read_data_t *data = (seqmidi_read_data_t *)fts_midifile_get_user_data(file);
  double time = fts_midifile_get_time(file);
  int i, j;
  
  /* shut down all pending note ons */
  for(i=0; i<n_midi_channels; i++)
    {
      for(j=0; j<n_midi_notes; j++)
	if(data->note_is_on[i][j] != 0)
	  {
	    event_t *event = data->note_is_on[i][j];
	    note_t *note = (note_t *)event_get_object(event);
	    
	    note_set_duration(note, time - event_get_time(event));
	    data->note_is_on[i][j] = 0;
	  }
    }

  data->track_index++;
}

static void
inttrack_read_midievent(fts_midifile_t *file, fts_midievent_t *midievt)
{
  seqmidi_read_data_t *data = (seqmidi_read_data_t *)fts_midifile_get_user_data(file);
  int type = fts_midievent_get_type(midievt);

  if(type < midi_type_system)
    {
      track_t *track = data->track;
      double time = fts_midifile_get_time(file);
      int channel = fts_midievent_channel_message_get_channel(midievt);
      int number = fts_midievent_channel_message_get_first(midievt);
      int value = number;
      event_t *event;
      fts_atom_t a;
      
      if(type <= fts_midievent_is_control_change(midievt))
	value = fts_midievent_channel_message_get_second(midievt);

      fts_set_int(&a, value);
      event = (event_t *)fts_object_create(event_class, 1, &a);
      
      /* add event to track */
      track_append_event(track, time, event);
    }
}

static void
sequence_read_track_start(fts_midifile_t *file)
{
  seqmidi_read_data_t *data = (seqmidi_read_data_t *)fts_midifile_get_user_data(file);
  double time = fts_midifile_get_time(file);
  fts_symbol_t name = fts_midifile_get_name(file);
  char str[STRING_SIZE];
  fts_atom_t a;

  if(data->track)
    fts_object_release(data->track);

  fts_set_symbol(&a, fts_s_midievent);
  data->track = (track_t *)fts_object_create(track_class, 1, &a);
  fts_object_refer(data->track);

  get_stripped_file_name_with_index(str, name, data->track_index);
  track_set_name(data->track, fts_new_symbol_copy(str));

  data->track_index++;
}

static void
sequence_read_track_end(fts_midifile_t *file)
{
  seqmidi_read_data_t *data = (seqmidi_read_data_t *)fts_midifile_get_user_data(file);
  double time = fts_midifile_get_time(file);

  if(data->track)
    {
      if(data->sequence && track_get_size(data->track) > 0)
	sequence_add_track(data->sequence, data->track);
      
      fts_object_release(data->track);
      data->track = 0;
    }
}

/**************************************************************************
 *
 *  import MIDI files to track
 *
 */
int
track_import_from_midifile(track_t *track, fts_midifile_t *file)
{
  fts_midifile_read_functions_t read;
  seqmidi_read_data_t data;
      
  seqmidi_read_data_init(&data);
  fts_midifile_set_user_data(file, &data);
  fts_midifile_read_functions_init(&read);
  fts_midifile_set_read_functions(file, &read);
  
  if(track_get_type(track) == fts_s_midievent)
    {
      data.track = track;
      read.midi_event = miditrack_read_midievent;
    } 
  else if(track_get_type(track) == seqsym_note)
    {
      int i, j;

      data.track = track;

      /* set oll notes to off */
      for(i=0; i<n_midi_channels; i++)
	for(j=0; j<n_midi_notes; j++)
	  data.note_is_on[i][j] = 0;
      
      read.track_end = notetrack_read_track_end;
      read.midi_event = notetrack_read_midievent;
    }
  else if(track_get_type(track) == fts_s_int)
    {
      read.midi_event = inttrack_read_midievent;
    }
  else
    return 0;
 
  fts_midifile_read(file);
  
  return data.size;
}

int
sequence_import_from_midifile(sequence_t *sequence, fts_midifile_t *file)
{
  fts_midifile_read_functions_t read;
  seqmidi_read_data_t data;
  int i, j;
      
  seqmidi_read_data_init(&data);
  fts_midifile_set_user_data(file, &data);
  fts_midifile_read_functions_init(&read);
  fts_midifile_set_read_functions(file, &read);
  
  data.sequence = sequence;
  data.track_index = 0;
  
  read.midi_event = miditrack_read_midievent;
  read.track_start = sequence_read_track_start;
  read.track_end = sequence_read_track_end;
  
  fts_midifile_read(file);
  
  if(data.track)
    fts_object_release(data.track);

  return data.size;
}

/**************************************************************************
 *
 *  writing MIDI files
 *
 */

typedef struct _seqmidi_write_data_
{
  track_t *track;
  track_t *off_track;
  track_t *free_track;
  int size;
} seqmidi_write_data_t;

static void
seqmidi_write_note_on(fts_midifile_t *file, double time, note_t *note)
{
  seqmidi_write_data_t *data = (seqmidi_write_data_t *)fts_midifile_get_user_data(file);
  int pitch = note_get_pitch(note);
  double off_time = time + note_get_duration(note);
  long time_in_ticks = fts_midifile_time_to_ticks(file, time);
  event_t *off = track_get_first(data->free_track);

  fts_midifile_write_channel_message(file, time_in_ticks, midi_type_note, 0, pitch, 64);
  data->size++;

  /* schedule note off */
  if(off)
    {
      /* prevent free event being destroyed when moved to off_track */
      fts_object_refer(off);

      /* set pitch and move free event to off track */
      event_set_int(off, pitch);
      track_remove_event(data->free_track, off);
      track_add_event(data->off_track, off_time, off);

      /* remove reference */
      fts_object_release(off);
    }
  else
    {
      fts_atom_t a;

      /* create new event */
      fts_set_int(&a, pitch);
      off = (event_t *)fts_object_create(event_class, 1, &a);

      /* add event to note on track */
      track_add_event(data->off_track, off_time, off);
    }  
}

/* write all pending note offs until given time */
static void
seqmidi_write_note_offs(fts_midifile_t *file, double time)
{
  seqmidi_write_data_t *data = (seqmidi_write_data_t *)fts_midifile_get_user_data(file);
  event_t *off = track_get_first(data->off_track);

  while(off && event_get_time(off) <= time)
    {
      long off_time_in_ticks = fts_midifile_time_to_ticks(file, event_get_time(off));
      int off_pitch = event_get_int(off);
      
      /* write note off */
      fts_midifile_write_channel_message(file, off_time_in_ticks, midi_type_note, 0, off_pitch, 0);
      data->size++;

      /* prevent off event being destroyed when moved to free_track */
      fts_object_refer(off);

      /* move off event to free track */
      track_remove_event(data->off_track, off);
      track_append_event(data->free_track, 0.0, off);
      
      /* remove reference */
      fts_object_release(off);

      /* get next note off in sequence */
      off = track_get_first(data->off_track);
    }
}

int
track_export_to_midifile(track_t *track, fts_midifile_t *file)
{
  fts_symbol_t track_type = track_get_type(track);
  int track_size = track_get_size(track);

  if(track_size <= 0)
    return 0;

  if(track_type == seqsym_note)
    {
      seqmidi_write_data_t data;
      event_t *event;
      fts_atom_t a[1];
      int i, j;
      
      fts_midifile_set_user_data(file, &data);
      
      data.track = track;
      data.size = 0;
      
      /* create dummy track for creating note offs */
      fts_set_symbol(a, seqsym_export_midifile);  
      data.off_track = (track_t *)fts_object_create(track_class, 1, a);
      data.free_track = (track_t *)fts_object_create(track_class, 1, a);
      
      /* write file header */
      fts_midifile_write_header(file, 0, 1, 384);
      fts_midifile_write_track_begin(file);
      fts_midifile_write_tempo(file, 500000);
      
      /* write events */
      event = track_get_first(track);
      while(event)
	{
	  double time = event_get_time(event);
	  
	  seqmidi_write_note_offs(file, time);
	  seqmidi_write_note_on(file, time, (note_t *)fts_get_object(&event->value));      
	  
	  event = event_get_next(event);
	}  
      
      /* write pending note-offs */
      if(track_get_size(data.off_track) > 0)
	seqmidi_write_note_offs(file, event_get_time(track_get_last(data.off_track)));
      
      /* delete dummy tracks */
      fts_object_destroy((fts_object_t *)(data.off_track));
      fts_object_destroy((fts_object_t *)(data.free_track));
      
      /* close file */
      fts_midifile_write_track_end(file);
      
      return data.size;
    }
  else if(track_type == fts_s_midievent)
    {
      seqmidi_write_data_t data;
      event_t *event;
      fts_atom_t a[1];
      int i, j;
      
      fts_midifile_set_user_data(file, &data);
      
      data.track = track;
      data.size = 0;
      
      /* write file header */
      fts_midifile_write_header(file, 0, 1, 384);
      fts_midifile_write_track_begin(file);
      fts_midifile_write_tempo(file, 500000);
      
      /* write events */
      event = track_get_first(track);
      while(event)
	{
	  double time = event_get_time(event);
	  long time_in_ticks = fts_midifile_time_to_ticks(file, time);
	  
	  fts_midifile_write_midievent(file, time_in_ticks, (fts_midievent_t *)fts_get_object(&event->value));
	  data.size++;

	  event = event_get_next(event);
	}  
      
      /* close file */
      fts_midifile_write_track_end(file);
      
      return data.size;
    }
  else
    return 0;
}


