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


#define FTS_AUDIO_INPUT 0
#define FTS_AUDIO_OUTPUT 1

/**
 * The audio port.
 *
 * A class implementing a audio port must inherit from fts_audioport_t:
 *
 * @code
 *   typedef struct my_audioport
 *   {
 *     fts_audioport_t port;
 *     ... 
 *   } my_audioport_t;
 * @endcode
 *
 * @typedef fts_audioport_t
 *
 * @ingroup audioport
 */

typedef struct fts_audiolabel fts_audiolabel_t;

typedef struct fts_audioport fts_audioport_t;

/**
 * The audioport IO function calls the native audio layer to read/write a buffer
 * of samples in the native format.
 * Its argument is the audioport. Buffers are allocated by the port.
 */
typedef void (*fts_audioport_io_fun_t)( fts_audioport_t *port);

/** 
 * The audioport copy function copies the samples in the native format to a float buffer
 * for a given channel.
 */
typedef void (*fts_audioport_copy_fun_t)( fts_audioport_t *port, float *buff, int buffsize, int channel);

typedef void (*fts_audioport_mute_fun_t)( fts_audioport_t *port, int channel);
typedef int (*fts_audioport_xrun_fun_t)( fts_audioport_t *port);

#define FTS_AUDIOPORT_MAX_CHANNELS 64

struct fts_audioport_direction {
  int valid;
  fts_audioport_io_fun_t io_fun;
  fts_audioport_copy_fun_t copy_fun;
  fts_audioport_mute_fun_t mute_fun;
  unsigned int used_channels;
  int max_channels;
  fts_audiolabel_t *labels;
};

struct fts_audioport {
  fts_object_t head;
  fts_symbol_t name;
  struct fts_audioport *next;
  struct fts_audioport_direction inout[2];
  void (*idle_function)( struct fts_audioport *port);
  int (*xrun_function)( struct fts_audioport *port);
};

FTS_API void fts_audioport_init( fts_audioport_t *port);
FTS_API void fts_audioport_delete( fts_audioport_t *port);

#define fts_audioport_set_max_channels( port, direction, max_channel) \
  ((port)->inout[(direction)].max_channels = (max_channels))

#define fts_audioport_set_io_fun( port, direction, fun) \
  ((port)->inout[(direction)].io_fun = (fun))
#define fts_audioport_set_copy_fun( port, direction, fun) \
  ((port)->inout[(direction)].copy_fun = (fun))
#define fts_audioport_set_mute_fun( port, direction, fun) \
  ((port)->inout[(direction)].mute_fun = (fun))

#define fts_audioport_set_idle_fun( port, fun) \
  ((port)->mute_fun = (fun))
#define fts_audioport_set_xrun_fun( port, fun) \
  ((port)->idle_fun = (fun))


/**
 * 
 * The audio label
 *
 * @typedef fts_audiolabel_t
 *
 * @ingroup audiolabel
 */

struct fts_audiolabel_direction {
  fts_symbol_t port_name;
  fts_audioport_t *port;
  int channel;
  struct fts_audiolabel *next_same_port;
};

struct fts_audiolabel {
  fts_object_t o;
  fts_symbol_t name;
  struct fts_audiolabel *next;
  struct fts_audiolabel_direction inout[2];
};

#define fts_audiolabel_get_name( label) ((label)->name)

#define fts_audiolabel_get_port_name( label, direction) ((label)->inout[(direction)].port_name)
#define fts_audiolabel_get_port( label, direction) ((label)->inout[(direction)].port)
#define fts_audiolabel_get_channel( label, direction) ((label)->inout[(direction)].channel)

/**
 * Audiolabels have listeners, that are called when audiolabels are created/deleted.
 * The native audio packages can use these listeners to be informed when labels are created and
 * create new audioports. 
 */
FTS_API void fts_audiolabel_add_listener( fts_object_t *listener, fts_method_t label_added, fts_method_t label_removed);
FTS_API void fts_audiolabel_remove_listener( fts_object_t *listener);


/**
 * The FTS audiomanager
 *
 * Maintains the list of audio port names
 *
 * @ingroup audiomanager
 */

/**
 * Get an audioport by name.
 *
 * @ingroup audiomanager
 */
FTS_API fts_audioport_t *fts_audiomanager_get_port( fts_symbol_t name);

/**
 * Register a new audioport.
 *
 * @ingroup audiomanager
 */
FTS_API void fts_audiomanager_put_port( fts_symbol_t name, fts_audioport_t *port);

/**
 * Remove an audioport.
 *
 * @ingroup audiomanager
 */
FTS_API void fts_audiomanager_remove_port( fts_symbol_t name);


