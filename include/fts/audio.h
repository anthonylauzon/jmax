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

/** 
 * Audio
 *
 * The FTS audio I/O handling
 *
 * @defgroup audio audio
 */


#define FTS_AUDIO_INPUT 0
#define FTS_AUDIO_OUTPUT 1

/* Forward declarations */
typedef struct fts_audiolabel fts_audiolabel_t;
typedef struct fts_audioport fts_audioport_t;

/**
 * The audio port.
 *
 * An audio port is a class that implements the native audio I/O by providing functions
 * calling the platform native audio API.
 * 
 * A class implementing a audio port must inherit from fts_audioport_t:
 * @code
 *   typedef struct my_audioport {
 *     fts_audioport_t port;
 *     ... 
 *   } my_audioport_t;
 * @endcode
 *
 * Audio ports for the differents platforms are implemented in packages and are loaded
 * dynamically. FTS kernel contains no platform-dependent code for audio.
 *
 * @typedef fts_audioport_t
 *
 * @ingroup audio
 */

/**
 * The audioport I/O function calls the native audio layer to read/write the samples buffers
 * in the native format.
 *
 * @param port the audioport
 * @param buffers an array of buffers to samples
 * @param buffsize the size of the buffers to be read/written
 */
typedef void (*fts_audioport_io_fun_t)( fts_audioport_t *port, float **buffers, int buffsize);

typedef int (*fts_audioport_xrun_fun_t)( fts_audioport_t *port);

#define FTS_AUDIOPORT_MAX_CHANNELS 64

struct fts_audioport_direction {
  int valid;
  int open;
  fts_audioport_io_fun_t io_fun;
  int channels;
  int *channel_used;
  float **buffers;
  int used;
};

struct fts_audioport {
  fts_object_t head;
  fts_symbol_t name;
  struct fts_audioport *next;
  struct fts_audioport_direction inout[2];

  int (*xrun_function)( struct fts_audioport *port);
};


/** 
 * Init an audioport
 * 
 * @param port the port
 * @ingroup audio
 */
FTS_API void fts_audioport_init( fts_audioport_t *port);

/** 
 * Delete an audioport
 * 
 * @param port the port
 * @ingroup audio
 */
FTS_API void fts_audioport_delete( fts_audioport_t *port);


#define fts_audioport_get_channels( port, direction) \
  ((port)->inout[(direction)].channels)

/** 
 * Set number of channels of an audioport
 * 
 * @param port the audioport
 * @param direction the direction
 * @param channels the number of channels
 * @ingroup audio
 */
FTS_API void fts_audioport_set_channels( fts_audioport_t *port, int direction, int channels);


/** 
 * Test if audioport have given channel used
 * 
 * @param port the audioport
 * @param direction the direction
 * @param channel the channel
 * @return 1 if channel is used, 0 if not
 * @ingroup audio
 */
FTS_API int fts_audioport_is_channel_used( fts_audioport_t *port, int direction, int channel);

#define fts_audioport_set_valid(port, direction) \
  ((port)->inout[(direction)].valid = 1)
#define fts_audioport_unset_valid(port, direction) \
  ((port)->inout[(direction)].valid = 0)
#define fts_audioport_is_valid(port, direction) \
  ((port)->inout[(direction)].valid)

#define fts_audioport_set_open(port, direction) \
  ((port)->inout[direction].open = 1)
#define fts_audioport_unset_open(port, direction) \
  ((port)->inout[direction].open = 0)
#define fts_audioport_is_open(port, direction) \
  ((port)->inout[direction].open)

#define fts_audioport_set_io_fun( port, direction, fun) \
  ((port)->inout[(direction)].io_fun = (fun))
#define fts_audioport_get_io_fun( port, direction) \
  ((port)->inout[(direction)].io_fun)

#define fts_audioport_set_xrun_fun( port, fun) \
  ((port)->xrun_fun = (fun))

/**
 * 
 * The audio label
 *
 * @typedef fts_audiolabel_t
 *
 * @ingroup audio
 */

struct fts_audiolabel_direction {
  fts_symbol_t port_name;
  fts_audioport_t *port;
  int channel;
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
 * Find an audiolabel by name
 * 
 * @param name the name of the wanted audiolabel
 * @return an audiolabel, or NULL if there is no audiolabel with a such name
 * @ingroup audio
 */
FTS_API fts_audiolabel_t *fts_audiolabel_get( fts_symbol_t name);

/** 
 * Copy input buffer of an audiolabel to given buffer
 * 
 * @param label the label
 * @param buff the buffer to fill
 * @param buffsize the buffer size
 * @ingroup audio
 */
FTS_API void fts_audiolabel_input( fts_audiolabel_t *label, float *buff, int buffsize);

/** 
 * Copy given buffer to output of an audiolabel
 * 
 * @param label the label
 * @param buff the buffer to copy
 * @param buffsize the buffer size
 * @ingroup audio
 */
FTS_API void fts_audiolabel_output( fts_audiolabel_t *label, float *buff, int buffsize);

/**
 * Audiolabels have listeners, that are called when audiolabels are created/deleted.
 * The native audio packages can use these listeners to be informed when labels are created and
 * create new audioports. 
 */
/** 
 * Add a listener of label addition and label suppression
 * 
 * @param listener the listener object
 * @param label_added the method called when a label is added
 * @param label_removed the method called when a lebel is removed
 * @ingroup audio
 */
FTS_API void fts_audiolabel_add_listener( fts_object_t *listener, fts_method_t label_added, fts_method_t label_removed);

/** 
 * Remove a listener of label addition and label suppression 
 * 
 * @param listener the listener object
 * @ingroup audio
 */
FTS_API void fts_audiolabel_remove_listener( fts_object_t *listener);


/**
 * The FTS audiomanager
 *
 * Maintains the list of audio port names
 *
 */

/**
 * Get an audioport by name.
 *
 * @param name the name of the audioport
 * @return the audioport, or NULL if there is no audioport with a such name
 * @ingroup audio
 */
FTS_API fts_audioport_t *fts_audiomanager_get_port( fts_symbol_t name);

/**
 * Register a new audioport.
 *
 * @param name the registered name  the audioport
 * @param port the audioport
 * @ingroup audio
 */
FTS_API void fts_audiomanager_put_port( fts_symbol_t name, fts_audioport_t *port);

/**
 * Remove an audioport that was already registered using fts_audiomanager_put_port()
 *
 * @param name the name of the audioport to unregister
 * @ingroup audio
 */
FTS_API void fts_audiomanager_remove_port( fts_symbol_t name);


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
