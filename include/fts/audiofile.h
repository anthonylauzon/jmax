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

/***********************************************
 *
 *  API for audio files
 */

typedef struct _fts_audiofile_t fts_audiofile_t;
typedef struct _fts_audiofile_loader_t fts_audiofile_loader_t;

/***********************************************
 *
 *  Audio file loader
 */

struct _fts_audiofile_loader_t {
  /* returns 0 if no errors occured, a non null value otherwise */
  int (*open_write)(fts_audiofile_t* aufile);

  /* returns 0 if no errors occured, a non null value otherwise */
  int (*open_read)(fts_audiofile_t* aufile);

  /* set buffer length returns 0 if no errors occured, a non null value otherwise */
  int (*buffer_length)(fts_audiofile_t* aufile, unsigned int length);

  /* returns the number of frames written or -1 in case an error occured */
  int (*write)(fts_audiofile_t* aufile, float** buf, int nbuf, unsigned int buflen);

  /* returns the number of frames read, 0 if the file reached the end or -1 in case an error occured */
  int (*read)(fts_audiofile_t* aufile, float** buf, int nbuf, unsigned int buflen);

  /* returns 0 if no errors occured, a non null value otherwise */
  int (*seek)(fts_audiofile_t* aufile, unsigned int offset);

  /* returns 0 if no errors occured, a non null value otherwise */
  int (*close)(fts_audiofile_t* aufile);
};

FTS_API fts_audiofile_loader_t* fts_audiofile_loader;
FTS_API int fts_audiofile_set_loader(char* name, fts_audiofile_loader_t* loader);

/***********************************************
 *
 *  Audio file 
 */

/* file formats */
FTS_API fts_symbol_t fts_s_dot_aiff;
FTS_API fts_symbol_t fts_s_dot_wav;
FTS_API fts_symbol_t fts_s_dot_snd;
FTS_API fts_symbol_t fts_s_dot_raw;

/* sample formats */
FTS_API fts_symbol_t fts_s_int8;
FTS_API fts_symbol_t fts_s_int16;
FTS_API fts_symbol_t fts_s_int24;
FTS_API fts_symbol_t fts_s_int32;
FTS_API fts_symbol_t fts_s_uint8;
FTS_API fts_symbol_t fts_s_uint16;
FTS_API fts_symbol_t fts_s_uint24;
FTS_API fts_symbol_t fts_s_uint32;
FTS_API fts_symbol_t fts_s_float32;
FTS_API fts_symbol_t fts_s_float64;

enum audiofile_sample_format
{
  audiofile_sample_format_null = -1, 
  audiofile_int8 = 0, 
  audiofile_int16, 
  audiofile_int24, 
  audiofile_int32, 
  audiofile_uint8, 
  audiofile_uint16, 
  audiofile_uint24, 
  audiofile_uint32, 
  audiofile_float32, 
  audiofile_float64,
  n_audiofile_sample_formats
};

enum audiofile_format
{
  audiofile_file_format_null = -1, 
  audiofile_raw = 0,
  audiofile_aiff,
  audiofile_wave,
  audiofile_snd,
  n_audiofile_file_formats
};

struct _fts_audiofile_t 
{
  fts_symbol_t filename;
  fts_symbol_t mode;
  int channels;
  int sample_rate;
  enum audiofile_sample_format sample_format;
  unsigned int bytes_per_sample;
  enum audiofile_format file_format;
  unsigned int frames;
  void *handle;
  char *error;
};

FTS_API fts_audiofile_t *fts_audiofile_open_write(fts_symbol_t filename, int channels, int sr, fts_symbol_t sample_format);
FTS_API fts_audiofile_t *fts_audiofile_open_read(fts_symbol_t filename);
FTS_API void fts_audiofile_close(fts_audiofile_t* aufile);

#define fts_audiofile_write(_f,_b,_n,_l) ((fts_audiofile_loader != NULL)? (*fts_audiofile_loader->write)(_f,_b,_n,_l) : -1)
#define fts_audiofile_read(_f,_b,_n,_l ) ((fts_audiofile_loader != NULL)? (*fts_audiofile_loader->read)(_f,_b,_n,_l) : -1)
#define fts_audiofile_seek(_f,_n) ((fts_audiofile_loader != NULL)? (*fts_audiofile_loader->seek)(_f,_n) : -1)

#define fts_audiofile_is_valid(_f) (((_f) != NULL) && (_f)->error == NULL)

#define fts_audiofile_get_filename(_f) (_f)->filename
#define fts_audiofile_get_sample_rate(_f) (_f)->sample_rate
#define fts_audiofile_get_num_channels(_f) (_f)->channels
#define fts_audiofile_get_sample_format(_f) (_f)->sample_format
#define fts_audiofile_get_bytes_per_sample(_f) (_f)->bytes_per_sample
#define fts_audiofile_get_num_frames(_f) (_f)->frames
#define fts_audiofile_get_handle(_f) (_f)->handle
#define fts_audiofile_get_error(_f) (_f)->error

#define fts_audiofile_set_sample_rate(_f,_v) { (_f)->sample_rate = _v; }
#define fts_audiofile_set_num_channels(_f,_v) { (_f)->channels = _v; }
#define fts_audiofile_set_file_format(_f,_v) { (_f)->file_format = _v; }
#define fts_audiofile_set_bytes_per_sample(_f,_v) { (_f)->bytes_per_sample = _v; }
#define fts_audiofile_set_num_frames(_f,_v) { (_f)->frames = _v; }
#define fts_audiofile_set_handle(_f,_v) { (_f)->handle = _v; }
#define fts_audiofile_set_error(_f,_v) {(_f)->error = _v; }

FTS_API void fts_audiofile_set_file_format_by_suffix(fts_audiofile_t* aufile, fts_symbol_t suffix);
FTS_API void fts_audiofile_set_sample_format_by_name(fts_audiofile_t* aufile, fts_symbol_t name);

