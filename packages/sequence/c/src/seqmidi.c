/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundationm; either version 2.1
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

#include <ctype.h>

#include <fts/fts.h>
#include <ftsconfig.h>
#include <string.h>
#include <fts/packages/sequence/sequence.h>
#include <fts/packages/sequence/track.h>
#include <fts/packages/sequence/note.h>
#include <fts/packages/sequence/seqsym.h>

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
  track_t *merge; 
  int track_index;
  int size; /* number of event read */
  event_t *note_is_on[n_midi_channels][n_midi_notes]; /* table for reading notes */
  int n_note_on[n_midi_channels][n_midi_notes]; /* counter for overlapping notes */
  scomark_t *last_marker;
  double last_marker_time;
  event_t *last;
} seqmidi_read_data_t;

static void
seqmidi_read_data_init(seqmidi_read_data_t *data)
{
  data->sequence = NULL;
  data->track = NULL;
  data->merge = NULL;
  data->track_index = 0;
  data->size = 0;
  data->last_marker = NULL;
  data->last_marker_time = 0.0;
  data->last = NULL;
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
  fts_set_object(&a, midievt);
  event = (event_t *)fts_object_create(event_class, 1, &a);
  
  /* claim object */
  fts_object_refer(midievt);
  
  /* add the event to track */
  track_append_event(track, time, event);
  
  data->size++;
}

static void
miditrack_read_track_end(fts_midifile_t *file)
{
  seqmidi_read_data_t *data = (seqmidi_read_data_t *)fts_midifile_get_user_data(file);
  
  /* merge to track */
  track_merge(data->merge, data->track);
}

static void
scoobtrack_read_midievent(fts_midifile_t *file, fts_midievent_t *midievt)
{
  seqmidi_read_data_t *data = (seqmidi_read_data_t *)fts_midifile_get_user_data(file);
  track_t *track = data->track;
  double time = fts_midifile_get_time(file);
  
  if(fts_midievent_is_note(midievt))
  {
    int channel = fts_midievent_channel_message_get_channel(midievt);
    int pitch = fts_midievent_channel_message_get_first(midievt);
    int velocity = fts_midievent_channel_message_get_second(midievt);
    
    if(velocity == 0 && data->note_is_on[channel - 1][pitch] != 0)
    {
      if(data->n_note_on[channel - 1][pitch] == 1)
      {
        event_t *event = data->note_is_on[channel - 1][pitch];
        scoob_t *scoob = (scoob_t *)event_get_object(event);
        
        scoob_set_duration(scoob, time - event_get_time(event));
        data->note_is_on[channel - 1][pitch] = 0;
        data->n_note_on[channel - 1][pitch] = 0;
      }
      else
        data->n_note_on[channel - 1][pitch]--;
    }
    else if(velocity > 0)
    {
      scoob_t *scoob;
      event_t *event;
      fts_atom_t a[3];
      
      if(data->note_is_on[channel - 1][pitch] != 0)
      {
        event_t *event = data->note_is_on[channel - 1][pitch];
        scoob_t *scoob = (scoob_t *)event_get_object(event);
        
        scoob_set_duration(scoob, time - event_get_time(event));
      }          
      
      /* create a scoob */
      fts_set_symbol(a, seqsym_note);
      fts_set_int(a + 1, pitch);
      fts_set_float(a + 2, 0.0);
      scoob = (scoob_t *)fts_object_create(scoob_class, 3, a);
      
      scoob_set_velocity(scoob, velocity);
      scoob_set_channel(scoob, channel);
      
      /* create a new event with the scoob */
      fts_set_object(a, (fts_object_t *)scoob);
      event = (event_t *)fts_object_create(event_class, 1, a);
      
      /* add the event to track */
      track_append_event(track, time, event);
      data->size++;
      
      /* register note as on */
      data->note_is_on[channel - 1][pitch] = event;
      data->n_note_on[channel - 1][pitch]++;
      data->last = event;
    }
  }
  else if(fts_midievent_is_poly_pressure(midievt))
  {
    int channel = fts_midievent_channel_message_get_channel(midievt);
    int value = fts_midievent_channel_message_get_second(midievt);
    
    if(data->last)
    {
      scoob_t *scoob = (scoob_t *)event_get_object(data->last);
      
      /* set scoob type (interval, rest, trill) and interval */
      scoob_set_type_by_index(scoob, channel);
      scoob_set_interval(scoob, value);
    }
  }
  else if(fts_midievent_is_program_change(midievt))
  {
    /* set cue = 100 * cue + program */
    int number = fts_midievent_channel_message_get_first(midievt);
    
    if(data->last)
    {
      scoob_t *scoob = (scoob_t *)event_get_object(data->last);
      int cue = 0;
      fts_atom_t a;
      
      fts_set_void(&a);
      scoob_property_get(scoob, seqsym_cue, &a);
      
      if(fts_is_int(&a))
        cue = fts_get_int(&a) * 100;
      
      cue += number;
      
      fts_set_int(&a, cue);
      scoob_property_set(scoob, seqsym_cue, &a);
    }
  }
}

static void
scoobtrack_read_tempo(fts_midifile_t *file, int tempo)
{
  seqmidi_read_data_t *data = (seqmidi_read_data_t *)fts_midifile_get_user_data(file);
  double time = fts_midifile_get_time(file);
  double old_tempo = 0.0;
  scomark_t *scomark = data->last_marker;
  
  if(scomark == NULL || time > data->last_marker_time)
    scomark = track_insert_marker(data->merge, time, seqsym_marker);
  
  scomark_set_tempo(scomark, 60000000.0 / (double)tempo, &old_tempo);
  
  data->last_marker = scomark;
  data->last_marker_time = time;
}

static void
scoobtrack_read_time_signature(fts_midifile_t *file, int numerator, int denominator, int clocks_per_metronome_click, int heals_per_quarter_note)
{
  seqmidi_read_data_t *data = (seqmidi_read_data_t *)fts_midifile_get_user_data(file);
  double time = fts_midifile_get_time(file);
  scomark_t *scomark = data->last_marker;
  
  if(scomark == NULL || time > data->last_marker_time)
    scomark = track_insert_marker(data->merge, time, seqsym_bar);
  else
    scomark_set_type(scomark, seqsym_bar);
  
  scomark_bar_set_meter_quotient(scomark, numerator, denominator);
  
  data->last_marker = scomark;
  data->last_marker_time = time;
}

static void
scoobtrack_read_track_end(fts_midifile_t *file)
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
        scoob_t *scoob = (scoob_t *)event_get_object(event);
        
        scoob_set_duration(scoob, time - event_get_time(event));
        data->note_is_on[i][j] = 0;
        data->n_note_on[i][j] = 0;
      }
  }
    
    track_merge(data->merge, data->track);
    data->track_index++;
}

static void
inttrack_read_midievent(fts_midifile_t *file, fts_midievent_t *midievt)
{
  seqmidi_read_data_t *data = (seqmidi_read_data_t *)fts_midifile_get_user_data(file);
  int type = fts_midievent_get_type(midievt);
  
  if(type < midi_system_exclusive)
  {
    track_t *track = data->track;
    double time = fts_midifile_get_time(file);
    int number = fts_midievent_channel_message_get_first(midievt);
    int value = number;
    event_t *event;
    fts_atom_t a;
    
    if(fts_midievent_is_control_change(midievt))
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
  fts_symbol_t name = fts_midifile_get_name(file);
  char str[STRING_SIZE];
  fts_atom_t a;
  
  if(data->track)
    fts_object_release(data->track);
  
  fts_set_symbol(&a, fts_s_midievent);
  data->track = (track_t *)fts_object_create(track_class, 1, &a);
  fts_object_refer(data->track);
  
  get_stripped_file_name_with_index(str, name, data->track_index);
  fts_object_set_name((fts_object_t *)data->track, fts_new_symbol(str));
  
  data->track_index++;
}

static void
sequence_read_track_end(fts_midifile_t *file)
{
  seqmidi_read_data_t *data = (seqmidi_read_data_t *)fts_midifile_get_user_data(file);
  
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
  
  if(track_get_type(track) == fts_midievent_type || track_get_type(track) == NULL)
  {
    read.track_end = miditrack_read_track_end;
    read.midi_event = miditrack_read_midievent;
  }
  else if(track_get_type(track) == scoob_class)
  {
    int i, j;
    
    /* set oll notes to off */
    for(i=0; i<n_midi_channels; i++)
    {
      for(j=0; j<n_midi_notes; j++)
      {
        data.note_is_on[i][j] = 0;
        data.n_note_on[i][j] = 0;
      }
    }
    
    read.track_end = scoobtrack_read_track_end;
    read.midi_event = scoobtrack_read_midievent;
    read.tempo = scoobtrack_read_tempo;
    read.time_signature = scoobtrack_read_time_signature;
  }
  else if(track_get_type(track) == fts_int_class)
  {
    read.track_end = miditrack_read_track_end;
    read.midi_event = inttrack_read_midievent;
  }
  else
    return 0;
  
  data.merge = track; /* merge all MIDI tracks */
  
  data.track = (track_t *)fts_object_create(track_class, 0, 0); /* read to temporary track */
  fts_object_refer(data.track);
  
  fts_midifile_read(file);
  
  fts_object_release(data.track);
  
  return data.size;
}

int
sequence_import_from_midifile(sequence_t *sequence, fts_midifile_t *file)
{
  fts_midifile_read_functions_t read;
  seqmidi_read_data_t data;
  
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
  
  /* be sure that this is cleaned */
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
  int size;
} seqmidi_write_data_t;

static void
seqmidi_write_note_on(fts_midifile_t *file, double time, scoob_t *scoob)
{
  seqmidi_write_data_t *data = (seqmidi_write_data_t *)fts_midifile_get_user_data(file);
  int pitch = scoob_get_pitch(scoob);
  int velocity = scoob_get_velocity(scoob);
  int channel = scoob_get_channel(scoob);
  double off_time = time + scoob_get_duration(scoob);
  long time_in_ticks = fts_midifile_time_to_ticks(file, time);
  
  if(channel <= 0)
    channel = 1;
  
  if(velocity <= 0)
    velocity = 64;
  
  fts_midifile_write_channel_message(file, time_in_ticks, midi_note, channel, pitch, velocity);
  data->size++;
  
  if(scoob_get_type_index(scoob) > scoob_note)
  {
    int type = scoob_get_type_index(scoob);
    int interval = scoob_get_interval(scoob);
    
    fts_midifile_write_channel_message(file, time_in_ticks, midi_poly_pressure, type, pitch, interval);
    data->size++;    
  }
  
  /* write cue */
  {
    fts_atom_t a;
    
    fts_set_void(&a);
    scoob_property_get(scoob, seqsym_cue, &a);
    
    if(fts_is_int(&a))
    {
      int cue = fts_get_int(&a);
      
      if(cue < 128)
      {
        /* write cue 0 to 127 */
        fts_midifile_write_channel_message(file, time_in_ticks, midi_program_change, channel, cue, MIDI_EMPTY_BYTE);
        data->size++;    
      }
      else
      {
        const int max_cue = 100000000;
        int dec;
        
        if(cue > max_cue)
          cue = max_cue;
        
        for(dec=max_cue/100; dec>=1; dec/=100)
        {
          if(cue > dec)
          {
            fts_midifile_write_channel_message(file, time_in_ticks, midi_program_change, channel, cue/dec, MIDI_EMPTY_BYTE);
            data->size++;
            cue %= dec;
          }
        }
      }
    }
  }
  
  /* schedule note off */  
  {
    event_t *off;
    fts_atom_t a;
    
    /* create new event */
    fts_set_int(&a, (pitch << 8) + channel);
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
    int off_pitch = event_get_int(off) >> 8;
    int off_channel = event_get_int(off) & 0xff;
    
    /* write note off */
    fts_midifile_write_channel_message(file, off_time_in_ticks, midi_note, off_channel, off_pitch, 0);
    data->size++;
    
    /* move off event to free track */
    track_remove_event(data->off_track, off);
    
    /* get next note off in sequence */
    off = track_get_first(data->off_track);
  }
}

int
track_export_to_midifile(track_t *track, fts_midifile_t *file)
{
  fts_class_t *track_type = track_get_type(track);
  int track_size = track_get_size(track);
  
  if(track_size <= 0)
    return 0;
  
  if(track_type == scoob_class)
  {
    seqmidi_write_data_t data;
    event_t *event;
    
    fts_midifile_set_user_data(file, &data);
    
    data.track = track;
    data.size = 0;
    
    /* create dummy track for creating note offs */
    data.off_track = (track_t *)fts_object_create(track_class, 0, 0);
    
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
      seqmidi_write_note_on(file, time, (scoob_t *)fts_get_object(&event->value));      
      
      event = event_get_next(event);
    }  
    
    /* write pending note-offs */
    if(track_get_size(data.off_track) > 0)
      seqmidi_write_note_offs(file, event_get_time(track_get_last(data.off_track)));
    
    /* delete dummy tracks */
    fts_object_destroy((fts_object_t *)(data.off_track));
    
    /* close file */
    fts_midifile_write_track_end(file);
    
    return data.size;
  }
  else if(track_type == fts_midievent_type)
  {
    seqmidi_write_data_t data;
    event_t *event;
    
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
  else if(track_type == NULL)
  {
    seqmidi_write_data_t data;
    event_t *event;
    
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
      fts_object_t *obj = fts_get_object(&event->value);
      
      if(fts_object_get_class(obj) == fts_midievent_type)
      {
        double time = event_get_time(event);
        long time_in_ticks = fts_midifile_time_to_ticks(file, time);
        
        fts_midifile_write_midievent(file, time_in_ticks, (fts_midievent_t *)obj);
        data.size++;
      }
      event = event_get_next(event);
    }  
    
    /* close file */
    fts_midifile_write_track_end(file);
    
    return data.size;
  }
  else
    return 0;
}
