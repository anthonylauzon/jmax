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

typedef struct _fts_midifile_ fts_midifile_t;

/* table of user read functions */
typedef struct _fts_midifile_read_functions_
{
  void (*header) (struct _fts_midifile_ *file);
  void (*track_start)(struct _fts_midifile_ *file);
  void (*track_end)(struct _fts_midifile_ *file);
  void (*midi_event)(struct _fts_midifile_ *file, fts_midievent_t *event);
  void (*sequence_number)(struct _fts_midifile_ *file, int number);
  void (*end_of_track)(struct _fts_midifile_ *file);
  void (*smpte)(struct _fts_midifile_ *file, int type, int hour, int minute, int second, int frame, int frac);
  void (*tempo)(struct _fts_midifile_ *file, int tempo);
  void (*time_signature)(struct _fts_midifile_ *file, int numerator, int denominator, int clocks_per_metronome_click, int heals_per_quarter_note);
  void (*key_signature)(struct _fts_midifile_ *file, int n_sharps_or_flats, int major_or_minor);
  void (*text)(struct _fts_midifile_ *file, int type, int n, char *string);
}fts_midifile_read_functions_t;

/* tempo map */
typedef struct fts_midifile_tempo_map_entry
{
  int tick;
  double time; /* time current time */
  double conv; /* current factor of time/ticks so that time = te->time + te->conv * (<current tick> - te->tick) */
  struct fts_midifile_tempo_map_entry *next; /* dynamic list */
} fts_midifile_tempo_map_entry_t;

/* the midi file */
struct _fts_midifile_
{
  FILE *fp;
  fts_symbol_t name;
  
  int format;
  int n_tracks;
  int division;
  int tempo;

  fts_midifile_tempo_map_entry_t *tempo_map; /* pointer to first tempo map entry */
  fts_midifile_tempo_map_entry_t *tempo_map_pointer; /* read pointer to tempo map */

  fts_midifile_read_functions_t *read;

  double time; /* current time in msec */
  double time_conv; /* delta time / delta ticks */
  int ticks; /* current time in delta-time units */
  int bytes; /* writing: file offset at the beginning of the track, reading: bytes left to be read */
  int size; /* writing: file size in bytes */

  fts_midievent_t *system_exclusive;

  char *string; /* message buffer */
  int string_size;
  int string_alloc;

  char *error; /* error message */

  void *user; /* user data */
};

#define fts_midifile_get_name(f) ((f)->name)
#define fts_midifile_get_format(f) ((f)->format)
#define fts_midifile_get_error(f) ((f)->error)

#define fts_midifile_get_time(f) ((f)->time)

#define fts_midifile_set_user_data(f, p) ((f)->user = (void *)(p))
#define fts_midifile_get_user_data(f) ((f)->user)

/*************************************************************
 *
 *  time
 *
 */
#define fts_midifile_get_ticks(f) ((f)->ticks)
FTS_API int fts_midifile_time_to_ticks(fts_midifile_t *file, double msecs);

/*************************************************************
 *
 *  read standard MIDI files
 *
 */
FTS_API fts_midifile_t *fts_midifile_open_read(fts_symbol_t name);
FTS_API fts_midifile_t *fts_midifile_open_write(fts_symbol_t name);
FTS_API void fts_midifile_close(fts_midifile_t *file);

FTS_API void fts_midifile_read_functions_init(fts_midifile_read_functions_t *read);
#define fts_midifile_set_read_functions(f, r) ((f)->read = (r))

FTS_API int fts_midifile_read(fts_midifile_t *file);

/*************************************************************
 *
 *  write standard MIDI files
 *
 */
FTS_API int fts_midifile_write_header(fts_midifile_t *file, int format, int n_tracks, int division);
FTS_API int fts_midifile_write_track_begin(fts_midifile_t *file);
FTS_API int fts_midifile_write_track_end(fts_midifile_t *file);

FTS_API void fts_midifile_write_channel_message(fts_midifile_t *file, int ticks, enum midi_type type, int channel, int byte1, int byte2);
FTS_API void fts_midifile_write_midievent(fts_midifile_t *file, int ticks, fts_midievent_t *event);
FTS_API int fts_midifile_write_meta_event(fts_midifile_t *file, int ticks, int type, unsigned char *data, int size);
FTS_API void fts_midifile_write_tempo(fts_midifile_t *file, int tempo);

