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

#ifndef _FTS_MIDIFILE_H_
#define _FTS_MIDIFILE_H_

typedef struct _fts_midifile_ fts_midifile_t;

typedef struct _fts_midifile_read_functions_
{
  int (*header) (struct _fts_midifile_ *file);
  int (*track_start)(struct _fts_midifile_ *file);
  int (*track_end)(struct _fts_midifile_ *file);
  int (*note_on)(struct _fts_midifile_ *file, int chan, int pitch, int vol);
  int (*note_off)(struct _fts_midifile_ *file, int chan, int pitch, int vol);
  int (*poly_pressure)(struct _fts_midifile_ *file, int chan, int pitch, int press);
  int (*control_change)(struct _fts_midifile_ *file, int chan, int control, int value);
  int (*program_change)(struct _fts_midifile_ *file, int chan, int program);
  int (*channel_pressure)(struct _fts_midifile_ *file, int chan, int  press);
  int (*pitch_bend)(struct _fts_midifile_ *file, int chan, int msb, int lsb);
  int (*system_exclusive)(struct _fts_midifile_ *file, int leng, char *mess);
  int (*arbitrary)(struct _fts_midifile_ *file, int leng, char *mess);
  int (*meta_misc)(struct _fts_midifile_ *file, int type, int leng, char *mess);
  int (*sequence_number)(struct _fts_midifile_ *file, int num);
  int (*end_of_track)(struct _fts_midifile_ *file);
  int (*smpte)(struct _fts_midifile_ *file, int hr, int mn, int se, int fr, int ff);
  int (*tempo)(struct _fts_midifile_ *file);
  int (*time_signature)(struct _fts_midifile_ *file, int nn, int dd, int cc, int bb);
  int (*key_signature)(struct _fts_midifile_ *file, int sf, int mi);
  int (*sequencer_specific)(struct _fts_midifile_ *file, int type, int leng, char *mess);
  int (*text)(struct _fts_midifile_ *file, int type, int leng, char *mess);
}fts_midifile_read_functions_t;

struct _fts_midifile_
{
  FILE *fp;
  fts_symbol_t name;
  
  int format;
  int n_tracks;
  int division;
  int tempo;

  fts_midifile_read_functions_t *read;

  long currtime; /* current time in delta-time units */
  long bytes; /* writing: file offset at the beginning of the track, reading: bytes left to be read */
  long size; /* writing: file size in bytes */

  char *Msgbuff; /* message buffer */
  int Msgsize; /* Size of currently allocated Msg */
  int Msgindex; /* index of next available location in Msg */

  char *error; /* error message */

  void *user; /* user data */
};

#define fts_midifile_set_user_data(f, p) ((f)->user = (void *)(p))
#define fts_midifile_get_user_data(f) ((f)->user)

/*************************************************************
 *
 *  time
 *
 */
FTS_API long fts_midifile_seconds_to_ticks(fts_midifile_t *file, double seconds);

#define fts_midifile_get_current_time(f) ((f)->currtime)
FTS_API double fts_midifile_get_current_time_in_seconds(fts_midifile_t *file);

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

FTS_API void fts_midifile_write_tempo(fts_midifile_t *file, int tempo);

FTS_API void fts_midifile_write_note_off(fts_midifile_t *file, long time, int channel, int number, int velocity);
FTS_API void fts_midifile_write_note_on(fts_midifile_t *file, long time, int channel, int number, int velocity);
FTS_API void fts_midifile_write_poly_pressure(fts_midifile_t *file, long time, int channel, int number, int value);
FTS_API void fts_midifile_write_control_change(fts_midifile_t *file, long time, int channel, int number, int value);
FTS_API void fts_midifile_write_program_change(fts_midifile_t *file, long time, int channel, int number);
FTS_API void fts_midifile_write_channel_pressure(fts_midifile_t *file, long time, int channel, int value);
FTS_API void fts_midifile_write_pitch_bend(fts_midifile_t *file, long time, int channel, int value);
FTS_API int fts_midifile_write_meta_event(fts_midifile_t *file, long time, int type, unsigned char *data, int size);

#endif
