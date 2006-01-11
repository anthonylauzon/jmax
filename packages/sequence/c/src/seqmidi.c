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
#define ABSOLUTE_END 1.7976931348623157e+308

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
  double next_bar_time;
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
  data->next_bar_time = ABSOLUTE_END;
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
    
    if(velocity == 0 && data->note_is_on[channel - 1][pitch] != NULL)
    {
      /* note off */
      if(data->n_note_on[channel - 1][pitch] == 1)
      {
        event_t *e = data->note_is_on[channel - 1][pitch];
        scoob_t *s = (scoob_t *)event_get_object(e);
        
        scoob_set_duration(s, time - event_get_time(e));
        data->note_is_on[channel - 1][pitch] = NULL;
        data->n_note_on[channel - 1][pitch] = 0;
      }
      else
        data->n_note_on[channel - 1][pitch]--;
    }
    else if(velocity > 0)
    {
      /* note on */
      scoob_t *scoob;
      event_t *event;
      fts_atom_t a[3];
      
      if(data->note_is_on[channel - 1][pitch] != NULL)
      {
        event_t *e = data->note_is_on[channel - 1][pitch];
        scoob_t *s = (scoob_t *)event_get_object(e);
        double dur = time - event_get_time(e);
        
        if(dur > 0.0)
          scoob_set_duration(s, dur);
        else
          track_remove_event(track, e);
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
  scomark_t *scomark = data->last_marker;
  double old_tempo = 0.0;
  
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
      if(data->note_is_on[i][j] != NULL)
      {
        event_t *event = data->note_is_on[i][j];
        scoob_t *scoob = (scoob_t *)event_get_object(event);
        
        scoob_set_duration(scoob, time - event_get_time(event));
        data->note_is_on[i][j] = NULL;
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

/**************************************************************************
 *
 *  sequence MIDI import methods
 *
 */
static int
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

static fts_method_status_t
sequence_import_midifile(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sequence_t *this = (sequence_t *)o;
  
  if(ac == 1 && fts_is_symbol(at))
  {
    fts_symbol_t name = fts_get_symbol(at);
    fts_midifile_t *file = fts_midifile_open_read(name);
    
    if(file)
    {
      int size = sequence_import_from_midifile(this, file);
      char *error = fts_midifile_get_error(file);
      
      if(error)
        fts_object_error(o, "import: read error in \"%s\" (%s)", fts_symbol_name(name), error);
      else if(size <= 0)
        fts_object_error(o, "import: couldn't get any data from \"%s\"", fts_symbol_name(fts_midifile_get_name(file)));
      
      fts_midifile_close(file);
      
      if(sequence_editor_is_open(this))
        sequence_upload(this);
      
      /* fts_name_update(o); */
    }
    else
      fts_object_error(o, "import: cannot open \"%s\"", fts_symbol_name(name));
  }
  
  return fts_ok;
}

/******************************************************
 *
 *  track MIDI import/export
 *
 */
static int
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
        data.note_is_on[i][j] = NULL;
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

static int
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

/* default import handler: midifile */
static fts_method_status_t
track_import_midifile(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *)o;
  
  if(ac > 0  &&  fts_is_symbol(at))
  {
    fts_symbol_t name = fts_get_symbol(at);
    fts_midifile_t *file = fts_midifile_open_read(name);
    fts_class_t *type = track_get_type(self);
    
    if (type == fts_midievent_type  ||  type == scoob_class  ||
        type == fts_int_class       ||  type == NULL)
    {
      if (file != NULL)
      {
        int size;
        char *error;
        int i;
        
        /* clear track and markers(!) */
        track_clear(self);
        
        /* get import options */
        for(i=1; i<ac; i+=2)
        {
          if(fts_is_symbol(at + i) && fts_is_number(at + i + 1))
          {
            fts_symbol_t sym = fts_get_symbol(at + i);
            
            if(sym == seqsym_track)
            {
              int n = fts_get_number_int(at + i + 1);
              
              if(n >= 0)
                fts_midifile_select_track(file, n);
            }
            else if(sym == seqsym_channel)
            {
              int n = fts_get_number_int(at + i + 1);
              
              if(n > 0)
                fts_midifile_select_channel(file, n);
            }
          }
        }
        
        size  = track_import_from_midifile(self, file);
        error = fts_midifile_get_error(file);
        
        if (!error && size > 0)   /* set return value: sucess */
        {
          if (self->markers)
            marker_track_renumber_bars(self->markers, 
                                       track_get_first(self->markers), 
                                       FIRST_BAR_NUMBER, 0);
          
          track_update_editor(self);
          fts_set_object(ret, o);
        }
        else
          fts_object_error(o, "import: coudn't read any MIDI data from file \"%s\"", fts_symbol_name(name));        
        
        fts_midifile_close(file);
      }
      else
        fts_object_error(o, "import: cannot open MIDI file \"%s\"", fts_symbol_name(name));
    }
    else
      fts_object_error(o, "cannot import MIDI file to %s track", fts_symbol_name(fts_class_get_name(type)));
  }
  else
    fts_object_error(o, "import: file name argument missing");
  
  return fts_ok;
}


/* exporting */

/* default export handler: midifile */
static fts_method_status_t
track_export_midifile(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *) o;
  
  if (ac > 0  &&  fts_is_symbol(at))
  {
    fts_symbol_t  sym  = fts_get_symbol(at);
    const char   *name = fts_symbol_name(sym);
    fts_midifile_t *file = fts_midifile_open_write(sym);
    
    if(file != NULL)
    {
      int size = track_export_to_midifile(self, file);
      char *error = fts_midifile_get_error(file);
      
      if(error != NULL)
        fts_object_error(o, "export: write error in file \"%s\" (%s)", name, error);
      else if (size <= 0)
        fts_object_error(o, "export: coudn't write any MIDI data to file \"%s\"", name);
      else
        fts_set_object(ret, o);
      
      fts_midifile_close(file);
    }
    else
      fts_object_error(o, "export: cannot open MIDI file \"%s\"", name);    
  }
  else
    fts_object_error(o, "export: file name argument missing");
  
  return fts_ok;
}

/******************************************************
 *
 *  marker track text file import
 *
 */
/* (this is not MIDI - could be in another file such as "seqtext.c")
import text label file as exported by audacity into marker track.
format: lines of time [s] (no leading space!), tab or space, 
label text until newline
*/
static fts_method_status_t
marker_track_import_labels_txt(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *) o;
  
  if (track_is_marker(self)  &&  ac > 0  &&  fts_is_symbol(at))
  {
    fts_symbol_t      filename = fts_get_symbol(at);
    fts_atomfile_t  *file;
    fts_atom_t        a;
    char              c;
    double            time = 0.0;
    fts_memorystream_t *memstream;
    enum { wTIME, wTEXT, wERROR } waitingfor = wTIME;
    
    memstream = (fts_memorystream_t *) fts_object_create(fts_memorystream_class, 0, NULL);
    fts_object_refer((fts_object_t *) memstream);
    
    /* check file name for .txt? */
    
    if (!(file = fts_atomfile_open_read(filename)))
    { /* we were responsible for this file, but can't open it: 
      don't return void */
      fts_post("can't open label text file '%s'\n", fts_symbol_name(filename));
      fts_set_object(ret, o);     
      return fts_ok;
    }
    
    while (waitingfor != wERROR  &&  fts_atomfile_read(file, &a, &c))
    {
      switch (waitingfor)
      {
        case wTIME:
          if (fts_is_number(&a))
          {
            time = fts_get_number_float(&a) * 1000.;  /* convert to millisec */
            
            /* prepare collection of label */
            fts_memorystream_reset(memstream);
            waitingfor = wTEXT;
          }
          else
          {
            waitingfor = wERROR;
          }
          break;
          
        case wTEXT:
          fts_spost_atoms((fts_bytestream_t *) memstream, 1, &a);
          
          if (c == '\n' || c == '\r')
          {
            /* end of label: create marker event, set label */
            event_t      *ev;
            scomark_t    *mrk;
            char         *lab;
            
            mrk = marker_track_insert_marker(self, time, seqsym_marker, &ev);
            
            /* get and zero-terminate label string (NOT KOSHER!) */
            lab = (char *) fts_memorystream_get_bytes(memstream);
            lab[fts_memorystream_get_size(memstream)] = 0;
            
            scomark_set_label(mrk, fts_new_symbol(lab));
            
            waitingfor = wTIME;
          }
            else
              fts_spost((fts_bytestream_t *) memstream, "%c", c);
          break;
          
        default:
          waitingfor = wERROR;
          break;
      }
    }
    
    fts_object_release(memstream); 
    fts_atomfile_close(file);
  }
  /* else: no marker track or wrong args -> don't handle this file */
  fts_set_object(ret, (fts_object_t *) self);       
  return fts_ok;
}

/******************************************************
*
*  bpf track text file import
*
*/
/* (this is not MIDI - could be in another file such as "seqtext.c")
import text bpf file as exported by f0 into float track.
format: lines of time [s] (no leading space!), tab or space, 
float text, anything until newline
*/
static fts_method_status_t
float_track_import_bpf_txt(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *) o;
  
  if((track_get_type(self) == fts_float_class)  &&  ac > 0  &&  fts_is_symbol(at))
  {
    fts_symbol_t      filename = fts_get_symbol(at);
    fts_atomfile_t   *file;
    fts_atom_t        a;
    char              c;
    double            time = 0.0;
    fts_atom_t        value;
    enum { wTIME, wFLOAT, wEOL, wERROR } waitingfor = wTIME;
    event_t         * event;
        
    if (!(file = fts_atomfile_open_read(filename)))
    { /* we were responsible for this file, but can't open it: 
      don't return void */
      fts_post("can't open bpf text file '%s'\n", fts_symbol_name(filename));
      fts_set_object(ret, o);     
      return fts_ok;
    }
    
    /* clear track */
    track_clear_and_upload(self);

    
    // do not stop on error, try to read anything readable:
    while (fts_atomfile_read(file, &a, &c))
    {
      switch (waitingfor)
      {
        case wTIME:
          if(fts_is_number(&a))
          {
            time = fts_get_number_float(&a) * 1000.;  /* convert to millisec */
            waitingfor = wFLOAT;
          }
          else
          {
            // ignore this line
            waitingfor = wEOL;
          }
          break;
          
        case wFLOAT:
          if(fts_is_number(&a))
          {
            fts_set_float(&value, fts_get_number_float(&a));
            event = (event_t *)fts_object_create(event_class, 1, &value);
            track_add_event_and_upload(self, time, event);
            waitingfor = wEOL;
            // do not break, because EOL can follow
          }
          else
          {
            // ignore this line
            waitingfor = wEOL;
            break;
          }
          
        case wEOL:
          if (c == '\n' || c == '\r')
          {
            waitingfor = wTIME;
          }
          // else: still waiting for EOL
          break;
          
        default:
          waitingfor = wERROR;
          break;
      }
    }
    
    fts_atomfile_close(file);
  }
  /* else: no float track or wrong args -> don't handle this file */

  fts_set_object(ret, (fts_object_t *) self);       
  return fts_ok;
}



// import_format helper functions
static enum format_wait_t { wHEAD, wROW, wEOL, wERROR } format_wait;

static int
import_format_txt_init(const fts_object_t *o, int ac, const fts_atom_t *at, fts_atom_t *ret,
                       const fts_symbol_t filename, fts_atomfile_t ** file,
                       int * time_index, int * m_max_index)
{
  
  fts_post("ac = %d (waiting for the fix)\n", ac);
  
  if (!(*file = fts_atomfile_open_read(filename)))
  { /* we were responsible for this file, but can't open it: 
    don't return void */
    fts_post("can't open fmats text file '%s'\n", fts_symbol_name(filename));
    fts_set_object(ret, o);     
    return 0;
  }
  
  if(ac > 3)
  {
    if (! (fts_is_number(at+3) && (*time_index = fts_get_number_int(at+3)) >= 0))
    {
      fts_post("bad time index (import argument number(2))\n");
      fts_set_object(ret, o);     
      return 0;        
    }
  }
  
  if(ac > 2)
  {
    if(!(fts_is_number(at+2) && (*m_max_index = fts_get_number_int(at+2)) >= 0))
    {
      fts_post("bad number of rows index (import argument number(1))\n");
      fts_set_object(ret, o);     
      return 0;        
    }
    else if(*time_index == *m_max_index)
    {
      fts_post("time index and number of rows index must differ: they are both set to %d\n",
               *time_index);
      fts_set_object(ret, o);     
      return 0;        
    }
    
  }
    
  return 1;
}

static void
read_format_txt_header(const fts_atom_t * a, const char * c, double * time, int * n, int * m,
                       const int * n_max, int * m_max, int * m_max_index, int * time_index, 
                       fmat_t ** partials, enum format_wait_t * waitingfor)
{        
  ++ *n;
  
  if(fts_is_number(a))
  {
    if(*n == *m_max_index)
    {
      *m_max = fts_get_number_int(a);
    }
    else if(*n == *time_index)
    {
      *time = fts_get_number_float(a) * 1000.;  /* convert to millisec */
    }
  }          
  
  if (*c == '\n' || *c == '\r')
  {
    *n = -1;
    
    if(*m_max > 0)
    {
      *partials = fmat_create(*m_max, *n_max);
      *m = 0;
      *waitingfor = wROW;
    }
  }
  return;
}

static void
import_format_txt_header(track_t * self, int * n_head, int * n_head_max, fmat_t ** headerValues,
                         const fts_atom_t * a, const char * c, double * time, int * n, int * m,
                         const int * n_max, int * m_max, int * m_max_index, int * time_index, 
                         fmat_t ** partials, enum format_wait_t * waitingfor)
{        
  fts_atom_t        value;
  event_t         * currentEvent;
    
  ++ *n;
  
  if(fts_is_number(a))
  {
    if(*n == *m_max_index)
    {
      *m_max = fts_get_number_int(a);
    }
    else if(*n == *time_index)
    {
      *time = fts_get_number_float(a) * 1000.;  /* convert to millisec */
    }
    else // extraneous header value
    {
      if(++ *n_head >= *n_head_max)
      {
        *n_head_max = *n_head + 1;
        fmat_reshape(*headerValues, 1, *n_head_max);
      }
      fmat_set_element(*headerValues, 0, *n_head, fts_get_number_float(a));
    }     
  }
  
  if (*c == '\n' || *c == '\r')
  {
    *n = -1;
    *n_head = -1;
    
    fts_set_object(&value, *headerValues);
    currentEvent = (event_t *)fts_object_create(event_class, 1, &value);
    track_add_event_and_upload(self, *time, currentEvent);
    
    *headerValues = fmat_create(1, *n_head_max);    
    
    if(*m_max > 0)
    {      
      *partials = fmat_create(*m_max, *n_max);
      *m = 0;
      *waitingfor = wROW;
    }
  }
  return;
}


static int
import_format_txt_row(track_t * self, const fts_atom_t * a, const char * c, double * time, 
                      int * n, int * m, int * n_max, const int * m_max, 
                      fmat_t ** partials, enum format_wait_t * waitingfor)
{
  fts_atom_t        value;
  event_t         * currentEvent;

  if(fts_is_number(a))
  {
    ++ *n;
    if(*n >= *n_max)
    {
      // the first row is used to determine the actual number of columns
      if(*m == 0)              
      {
        *n_max = *n + 1;
        fmat_reshape(*partials, *m_max, *n_max);
      }
      else // each row must have the same number of columns
      {
        return 0;            
      }
    }    
    
    fmat_set_element(*partials, *m, *n, fts_get_number_float(a));
    
    // last column
    if (*c == '\n' || *c == '\r')
    {
      if(++ *m >= *m_max) // last row
      {
        fts_set_object(&value, *partials);
        currentEvent = (event_t *)fts_object_create(event_class, 1, &value);
        track_add_event_and_upload(self, *time, currentEvent);
        *waitingfor = wHEAD;
      }
      *n = -1;
    }
    
  }
  else
  {
    return 0;
  }
  
  return 1;
}  



/******************************************************
*
*  fmat track format text file import
*
*/
/* (this is not MIDI - could be in another file such as "seqtext.c")
import text bpf file as exported by f0 into float track.
format: time and number of rows, then one line by row (any number of float columns)
*/
static fts_method_status_t
fmat_track_import_format_txt(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  track_t *self = (track_t *) o;
  
  if((track_get_type(self) == fmat_class)  &&  ac > 0  &&  fts_is_symbol(at+0))
  {
    fts_symbol_t      filename = fts_get_symbol(at+0);
    fts_atomfile_t  * file = NULL; // input file
    fts_atom_t        a; // current read element
    char              c; // separator
    double            time = -1.;
    int               m = -1; // current row
    int               n = -1; // current column
    int               m_max = 0; // current number of rows (read from header lines)
    int               n_max = 0; // current nuber of columns (first matrix decides)
    int               m_max_index = 0; // default position for the m_max information
    int               time_index = 1; // default position for the time information
    enum format_wait_t waitingfor = wHEAD;
    fmat_t          * partials = NULL;
    

    if(import_format_txt_init(o, ac, at, ret, filename, &file, &time_index, &m_max_index) == 0)
    {
      // an error occured
      return fts_ok;
    }
        
    /* clear track */
    track_clear_and_upload(self);
    
    // stop on error, do not risk a mess
    while (waitingfor != wERROR && fts_atomfile_read(file, &a, &c))
    {
      switch (waitingfor)
      {
        case wHEAD:
          read_format_txt_header(&a, &c, &time, &n, &m, &n_max, &m_max,
                                     &m_max_index, &time_index, &partials, &waitingfor);           
          break;
  
          
        case wROW:
          if(import_format_txt_row(self, &a, &c, &time, &n, &m, &n_max, &m_max,
                               &partials, &waitingfor) != 0)
          {
            break;
          }
          // else do not break, it is an error

        default:
          waitingfor = wERROR;
          // do not break, it is an error
          
        case wERROR:
          fts_post("error while reading fmats text file '%s' at time %f\n",
                   fts_symbol_name(filename), time * 0.001);
          break;
      }
    }
    
    fts_atomfile_close(file);
  }
  // else: no fmat track or wrong args -> do not handle this file
  fts_set_object(ret, (fts_object_t *) self);       
  return fts_ok;
}

/******************************************************
*
*  sequence format text file import (for extended format text file)
*
*/
/* (this is not MIDI - could be in another file such as "seqtext.c")
import text bpf file as exported by f0 into float track.
format: time, number of rows and extraneous floats, then one line by row (any number of float columns)
*/
static fts_method_status_t
sequence_import_format_txt(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sequence_t *self = (sequence_t *) o;
  
  if(ac > 0  &&  fts_is_symbol(at+0))
  {
    track_t         * partialsTrack;
    track_t         * headerTrack;
    
    int               n_head = -1;
    int               n_head_max = 0;
    fmat_t          * headerValues = fmat_create(1, 0);

    fts_symbol_t      filename = fts_get_symbol(at+0);
    fts_atomfile_t  * file = NULL; // input file
    fts_atom_t        a; // current read element
    char              c; // separator
    double            time = -1.;
    int               m = -1; // current row
    int               n = -1; // current column
    int               m_max = 0; // current number of rows (read from header lines)
    int               n_max = 0; // current nuber of columns (first matrix decides)
    int               m_max_index = 0; // default position for the m_max information
    int               time_index = 2; // default position for the time information
    enum format_wait_t waitingfor = wHEAD;
    fmat_t          * partialsFrame = NULL;
    
    
    if(import_format_txt_init(o, ac, at, ret, filename, &file, &time_index, &m_max_index) == 0)
    {
      // an error occured
      return fts_ok;
    }
    
    // clear sequence
    while(sequence_get_size(self) > 0)
    {
      sequence_remove_track(self, sequence_get_first_track(self));
    }

    // create two tracks
    partialsTrack = (track_t *) fts_object_create(track_class, 0, 0);
    fts_object_refer((fts_object_t *) partialsTrack);
    partialsTrack->type = fmat_class;
    sequence_add_track(self, partialsTrack);

    headerTrack = (track_t *) fts_object_create(track_class, 0, 0);
    fts_object_refer((fts_object_t *) headerTrack);
    sequence_add_track(self, headerTrack);
    headerTrack->type = fmat_class;
    
    // TODO sequence_upload(self);
    
    // stop on error, do not risk a mess
    while (waitingfor != wERROR && fts_atomfile_read(file, &a, &c))
    {
      switch (waitingfor)
      {
        case wHEAD:
          import_format_txt_header(headerTrack, &n_head, &n_head_max, &headerValues,
                                   &a, &c, &time, &n, &m, &n_max, &m_max,
                                   &m_max_index, &time_index,
                                   &partialsFrame, &waitingfor);           
          break;
          
          
        case wROW:
          if(import_format_txt_row(partialsTrack, &a, &c,
                                   &time, &n, &m, &n_max, &m_max,
                                   &partialsFrame, &waitingfor) != 0)
          {
            break;
          }
          // else do not break, it is an error
          
        default:
          waitingfor = wERROR;
          // do not break, it is an error
          
        case wERROR:
          fts_post("error while reading fmats text file '%s' at time %f\n",
                   fts_symbol_name(filename), time * 0.001);
          break;
      }
    }
    
    fts_atomfile_close(file);
  }
  // else: no fmat track or wrong args -> do not handle this file
  fts_set_object(ret, (fts_object_t *) self);       
  return fts_ok;
}


FTS_MODULE_INIT(seqfiles)
{
  /* track class MIDI file import/export (registered first, so it will be the last handler tried) */
  fts_midifile_import_handler(track_class, track_import_midifile);
  fts_midifile_export_handler(track_class, track_export_midifile);
  
  /* marker track import/export */
  fts_class_import_handler(track_class, fts_new_symbol("labels"), marker_track_import_labels_txt);  

  /* float track text import/ */
  fts_class_import_handler(track_class, fts_new_symbol("bpf"), float_track_import_bpf_txt);  
  fts_class_import_handler(track_class, fts_new_symbol("f0"), float_track_import_bpf_txt);  

  /* fmat track text import */
  fts_class_import_handler(track_class, fts_new_symbol("format"), fmat_track_import_format_txt);
  
  fts_class_import_handler(multitrack_class, fts_new_symbol("extformat"), sequence_import_format_txt);
  fts_class_import_handler(multitrack_class, fts_new_symbol("format"), sequence_import_format_txt);

  
  /* sequence class MIDI file import */
  fts_midifile_import_handler(multitrack_class, sequence_import_midifile);
}