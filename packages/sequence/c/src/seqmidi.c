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
#include "fts.h"
#include "sequence.h"
#include "seqnote.h"

#define MAX_MIDI_CHANNELS 16
#define MAX_MIDI_PITCHES 128

static fts_symbol_t sym_note = 0;

typedef struct _seqmidi_data_
{
  sequence_t *sequence;
  sequence_track_t *track;
  seqnote_t *on[MAX_MIDI_CHANNELS][MAX_MIDI_PITCHES];
} seqmidi_data_t;

static int
prtime(fts_midifile_t *file)
{
  post("Time=%lf  ", fts_midifile_get_current_time_in_seconds(file));
  return 1;
}

static int
txt_header(fts_midifile_t *file)
{
  post("MIDI file (format %d): %d track(s), division: %d\n", file->format, file->n_tracks, file->division);

  return 1;
}

static int
txt_trackstart(fts_midifile_t *file)
{
  post("Track start\n");
  return 1;
}

static int
txt_trackend(fts_midifile_t *file)
{
  post("Track end\n");
  return 1;
}

static int
txt_noteon(fts_midifile_t *file, int chan, int pitch, int vol)
{
  prtime(file);
  post("Note on, chan=%d pitch=%d vol=%d\n",chan+1,pitch,vol);
  return 1;
}

static int
txt_noteoff(fts_midifile_t *file, int chan, int pitch, int vol)
{
  prtime(file);
  post("Note off, chan=%d pitch=%d vol=%d\n",chan+1,pitch,vol);
  return 1;
}

static int
txt_pressure(fts_midifile_t *file, int chan, int pitch, int press)
{
  prtime(file);
  post("Pressure, chan=%d pitch=%d press=%d\n",chan+1,pitch,press);
  return 1;
}

static int
txt_parameter(fts_midifile_t *file, int chan, int control, int value)
{
  prtime(file);
  post("Parameter, chan=%d c1=%d c2=%d\n",chan+1,control,value);
  return 1;
}

static int
txt_pitchbend(fts_midifile_t *file, int chan, int msb, int lsb)
{
  prtime(file);
  post("Pitchbend, chan=%d msb=%d lsb=%d\n",chan+1,msb,lsb);
  return 1;
}

static int
txt_program(fts_midifile_t *file, int chan, int program)
{
  prtime(file);
  post("Program, chan=%d program=%d\n",chan+1,program);
  return 1;
}

static int
txt_chanpressure(fts_midifile_t *file, int chan, int  press)
{
  prtime(file);
  post("Channel pressure, chan=%d pressure=%d\n",chan+1,press);
  return 1;
}

static int
txt_sysex(fts_midifile_t *file, int leng, char *mess)
{
  prtime(file);
  post("Sysex, leng=%d\n",leng);
  return 1;
}

static int
txt_metamisc(fts_midifile_t *file, int type, int leng, char *mess)
{
  prtime(file);
  post("Meta event, unrecognized, type=0x%02x leng=%d\n",type,leng);
  return 1;
}

static int
txt_metaspecial(fts_midifile_t *file, int type, int leng, char *mess)
{
  prtime(file);
  post("Meta event, sequencer-specific, type=0x%02x leng=%d\n",type,leng);
  return 1;
}

static int
txt_metatext(fts_midifile_t *file, int type, int leng, char *mess)
{
  static char *ttype[] = {
    NULL,
    "Text Event",		/* type=0x01 */
    "Copyright Notice",	/* type=0x02 */
    "Sequence/Track Name",
    "Instrument Name",	/* ...       */
    "Lyric",
    "Marker",
    "Cue Point",		/* type=0x07 */
    "Unrecognized"
  };
  int unrecognized = (sizeof(ttype)/sizeof(char *)) - 1;
  register int n, c;
  register char *p = mess;

  if ( type < 1 || type > unrecognized )
    type = unrecognized;
  prtime(file);
  post("Meta Text, type=0x%02x (%s)  leng=%d\n",type,ttype[type],leng);
  post("     Text = <");
  for ( n=0; n<leng; n++ ) {
    c = *p++;
    post( (isprint(c)||isspace(c)) ? "%c" : "\\0x%02x" , c);
  }
  post(">\n");
  return 1;
}

static int
txt_metaseq(fts_midifile_t *file, int num)
{
  prtime(file);
  post("Meta event, sequence number = %d\n",num);
  return 1;
}

static int
txt_metaeot(fts_midifile_t *file)
{
  prtime(file);
  post("Meta event, end of track\n");
  return 1;
}

static int
txt_keysig(fts_midifile_t *file, int sf, int mi)
{
  prtime(file);
  post("Key signature, sharp/flats=%d  minor=%d\n",sf,mi);
  return 1;
}

static int
txt_tempo(fts_midifile_t *file)
{
  prtime(file);
  post("Tempo, microseconds-per-MIDI-quarter-note=%d\n", file->tempo);
  return 1;
}

static int
txt_timesig(fts_midifile_t *file, int nn, int dd, int cc, int bb)
{
  int denom = 1;

  while (dd-- > 0)
    denom *= 2;

  prtime(file);
  post("Time signature=%d/%d  MIDI-clocks/click=%d  32nd-notes/24-MIDI-clocks=%d\n", nn, denom, cc, bb);

  return 1;
}

static int
txt_smpte(fts_midifile_t *file, int hr, int mn, int se, int fr, int ff)
{
  prtime(file);
  post("SMPTE, hour=%d minute=%d second=%d frame=%d fract-frame=%d\n", hr, mn, se, fr, ff);

  return 1;
}

static int
txt_arbitrary(fts_midifile_t *file, int leng, char *mess)
{
  prtime(file);
  post("Arbitrary bytes, leng=%d\n",leng);
  return 1;
}

static int
seqmidi_read_track_start(fts_midifile_t *file)
{
  seqmidi_data_t *data = (seqmidi_data_t *)fts_midifile_get_user_data(file);
  sequence_t *sequence = data->sequence;
  char s[] = "track9999";
  fts_symbol_t name;
  sequence_track_t *track;

  sprintf(s, "track%d", sequence->n_tracks);
  name = fts_new_symbol_copy(s);

  /*track = sequence_add_track(sequence, name, seqnote_symbol);@@@@*/
  track = sequence_get_track_by_name(sequence, fts_new_symbol("track0"));

  data->track = track;

  return 1;
}


static void
seqmidi_set_note_off(seqnote_t *note, double time)
{
  double duration = time - sequence_event_get_time((sequence_event_t *)note);

  seqnote_set_duration(note, duration);
}

static int
seqmidi_read_note_on(fts_midifile_t *file, int chan, int pitch, int vel)
{
  seqmidi_data_t *data = (seqmidi_data_t *)fts_midifile_get_user_data(file);
  sequence_t *sequence = data->sequence;
  sequence_track_t *track = data->track;
  double time = fts_midifile_get_current_time_in_seconds(file);

  if(vel == 0 && data->on[chan][pitch] != 0)
    {
      seqmidi_set_note_off(data->on[chan][pitch], time);
      data->on[chan][pitch] = 0;
    }
  else
    {
      fts_atom_t a[3];
      fts_object_t *note;

      fts_set_symbol(a + 0, seqnote_symbol);
      fts_set_int(a + 1, pitch);
      fts_set_float(a + 2, 0.0);
      fts_object_new(0, 3, a, &note);

      /* add event to sequence */
      sequence_add_event(sequence, track, time, (sequence_event_t *)note);

      data->on[chan][pitch] = (seqnote_t *)note;
    }
    
  return 1;
}

static int
seqmidi_read_note_off(fts_midifile_t *file, int chan, int pitch, int vel)
{
  seqmidi_data_t *data = (seqmidi_data_t *)fts_midifile_get_user_data(file);
  sequence_t *sequence = data->sequence;
  sequence_track_t *track = data->track;
  double time = fts_midifile_get_current_time_in_seconds(file);

  if(data->on[chan][pitch] != 0)
    {
      seqmidi_set_note_off(data->on[chan][pitch], time);
      data->on[chan][pitch] = 0;
    }

  return 1;
}

static int
seqmidi_read_track_end(fts_midifile_t *file)
{
  seqmidi_data_t *data = (seqmidi_data_t *)fts_midifile_get_user_data(file);
  sequence_t *sequence = data->sequence;
  sequence_track_t *track = data->track;
  double time = fts_midifile_get_current_time_in_seconds(file);
  int i, j;
  
  for(i=0; i<MAX_MIDI_CHANNELS; i++)
    for(j=0; j<MAX_MIDI_PITCHES; j++)
      if(data->on[i][j] != 0)
	{
	  seqmidi_set_note_off(data->on[i][j], time);
	  data->on[i][j] = 0;
	}
  
  return 1;  
}

int
sequence_read_midifile(sequence_t *sequence, fts_symbol_t name)
{
  char ch;
  fts_midifile_t *file = fts_midifile_open_read(name);
  fts_midifile_read_functions_t read;
  seqmidi_data_t data;
  int i, j;

  if(file)
    {    
      fts_midifile_read_functions_init(&read);
      
      read.header = txt_header;
      read.trackstart = seqmidi_read_track_start;
      read.trackend = seqmidi_read_track_end;
      read.noteon = seqmidi_read_note_on;
      read.noteoff = seqmidi_read_note_off;
      read.pressure = txt_pressure;
      read.parameter = txt_parameter;
      read.pitchbend = txt_pitchbend;
      read.program = txt_program;
      read.chanpressure = txt_chanpressure;
      read.sysex = txt_sysex;
      read.metamisc = txt_metamisc;
      read.seqnum = txt_metaseq;
      read.eot = txt_metaeot;
      read.timesig = txt_timesig;
      read.smpte = txt_smpte;
      read.tempo = txt_tempo;
      read.keysig = txt_keysig;
      read.seqspecific = txt_metaspecial;
      read.text = txt_metatext;
      read.arbitrary = txt_arbitrary;

      fts_midifile_set_read_functions(file, &read);

      data.sequence = sequence;
      for(i=0; i<MAX_MIDI_CHANNELS; i++)
	for(j=0; j<MAX_MIDI_PITCHES; j++)
	  data.on[i][j] = 0;

      fts_midifile_set_user_data(file, &data);
      
      if(fts_midifile_read(file) <= 0)
	post("error reading file %s: %s\n", file->name, (file->error)? file->error: "unknown error");
      
      fts_midifile_close(file);

      return 1;
    }
  else
    return 0;
}
