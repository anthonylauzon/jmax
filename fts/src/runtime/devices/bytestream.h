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

#ifndef _FTS_BYTESTREAM_H_
#define _FTS_BYTESTREAM_H_

#include "sys.h"
#include "lang.h"

/**
 * The byte stream abstraction (fts_bytestream_t()).
 *
 * The byte stream is an abstraction of classes representing byte stream in/output devices like
 * serial lines and TCP or UDP sockets.
 *
 * FTS byte streams are FTS objects.
 *
 * Various i/o objects can refer to an FTS byte stream in order to receive bytes (listeners) 
 * or send bytes to an internal or external device or file.
 * Mulitple MIDI port listeners can be registered for a single stream.
 *
 * If an i/o object refers to byte stream objects by a named variable,
 * the same program (patch) can be configured for completely different setups
 * simply by redefining the byte stream object defining the variable.
 *
 * Different implementations of FTS byte streams are developed for different platforms
 * and byte stream protocols.
 * In many cases i/o objects implement a protocol (encoding and decoding)
 * in order to send more complex data structures via a serial byte stream.
 *
 * The API documented by this module permits to implement FTS byte stream classes.
 * It contains the byte stream structure itself and the initialization functions for the FTS class 
 * implementing an FTS byte stream: fts_bytestream_class_init(), fts_bytestream_init().
 *
 * A byte stream class allowing input, calls fts_bytestream_set_input() in the objects initialization
 * and uses the provided input function fts_bytestream_input() in the routines handling incoming data.
 * These functions propagate the incoming data to the listening i/o objects refering to the byte stream.
 *
 * For the output a byte stream class must implement two output functions for sending single characters 
 * and entire strings to the bytestream as well as an optional function to flush eventual output buffers.
 * The functions are declared by fts_bytestream_set_output() in the objects initialization.
 *
 * @defgroup bytestream byte stream
 */

typedef struct fts_bytestream fts_bytestream_t;

/**
 * Call back function declared by an object listening to a byte stream.
 *
 * @typedef void (*fts_bytestream_callback_t)(fts_object_t *listener, int n, const unsigned char* c)
 *
 * @ingroup bytestream_io
 */

typedef void (*fts_bytestream_callback_t)(fts_object_t *listener, int n, const unsigned char* c);

typedef struct fts_bytestream_listener
{
  fts_bytestream_callback_t callback;
  fts_object_t *listener;
  struct fts_bytestream_listener *next;
} fts_bytestream_listener_t;

/**
 * Send a string to a byte stream.
 *
 * @typedef void (*fts_bytestream_output_t)(fts_bytestream_t *stream, int n, const unsigned char *c)
 *
 * @ingroup bytestream
 */

/**
 * Send a single charakter to a byte stream.
 *
 * @typedef void (*fts_bytestream_output_char_t)(fts_bytestream_t *stream, unsigned char c)
 *
 * @ingroup bytestream
 */

/**
 * Flush byte streams output buffer.
 *
 * This function is optional and has not be declared only for byte streams using an output buffer
 *
 * @typedef void (*fts_bytestream_flush_t)(fts_bytestream_t *stream)
 *
 * @ingroup bytestream
 */

typedef void (*fts_bytestream_output_t)(fts_bytestream_t *stream, int n, const unsigned char *c);
typedef void (*fts_bytestream_output_char_t)(fts_bytestream_t *stream, unsigned char c);
typedef void (*fts_bytestream_flush_t)(fts_bytestream_t *stream);

/** 
 * @name The FTS byte stream structure
 */
/*@{*/

/**
 * The FTS byte stream structure.
 *
 * The FTS byte stream "inherits" from FTS object.
 * The structure \b fts_bytestream_t itself must be included by a class implementing a byte stream:
 *
 * @code
 *   typedef struct my_bytestream
 *   {
 *     fts_bytestream_t stream;
 *     ... 
 *   } my_bytestream_t;
 * @endcode
 *
 * A byte stream class must call \c fts_bytestream_init() it the objects \e init Method.
 *
 * @typedef fts_bytestream_t
 *
 * @ingroup bytestream
 */
struct fts_bytestream
{
  fts_object_t o;
  int input; /* flag telling if bytestream is input */
  fts_bytestream_listener_t *listeners; /* list of objects listening to byte stream */
  fts_bytestream_output_t output; /* stream output function */
  fts_bytestream_output_char_t output_char; /* stream output function for single char */
  fts_bytestream_flush_t flush; /* stream flush function (when output buffered) */
};

/*@}*/ /* The FTS byte stream structure */

/** 
 * @name Initializing a byte stream
 *
 * Initialization of classes and objects implementing a byte stream.
 */
/*@{*/ /* Initializing a byte stream */

/**
 * Initialize a class implementing an FTS byte stream.
 *
 * @fn void fts_bytestream_class_init(fts_class_t *cl)
 * @param cl FTS class
 *
 * @ingroup bytestream
 */

/**
 * Initialize a byte stream structure.
 *
 * @fn void fts_bytestream_init(fts_bytestream_t *stream)
 * @param stream the byte stream
 *
 * @ingroup bytestream
 */

extern void fts_bytestream_class_init(fts_class_t *cl);
extern void fts_bytestream_init(fts_bytestream_t *stream);

/**
 * Declare initialized byte stream as input.
 *
 * @fn void fts_bytestream_set_input(fts_bytestream_t *stream)
 * @param stream the byte stream
 *
 * @ingroup bytestream
 */

/**
 * Declare an initialized byte stream as output and assign it's output functions.
 *
 * @fn void fts_bytestream_set_output(fts_bytestream_t *stream, fts_bytestream_output_t output, fts_bytestream_output_char_t output_char, fts_bytestream_flush_t flush)
 * @param stream the byte stream
 * @param functions structure of implemented output functions
 *
 * @ingroup bytestream
 */

extern void fts_bytestream_set_input(fts_bytestream_t *stream);
extern void fts_bytestream_set_output(fts_bytestream_t *stream, 
				       fts_bytestream_output_t output, fts_bytestream_output_char_t output_char, fts_bytestream_flush_t flush);

/*@}*/ /* Initializing a byte stream */

/** 
 * @name Handling incoming data
 */
/*@{*/

/**
 * Call all listeners of the byte stream.
 *
 * For incoming data a function is called by the object implementing an byte stream, which calls all listeners.
 *
 * @fn void fts_bytestream_input(fts_bytestream_t *stream, int n, const unsigned char *c)
 * @param stream the byte stream itself
 * @param n the number of characters
 * @param c the characters
 *
 * @ingroup bytestream
 */

extern void fts_bytestream_input(fts_bytestream_t *stream, int n, const unsigned char *c);

/*@}*/ /* Handling incoming data */


/****************************************************
 *
 *  byte stream i/o
 *
 */

/**
 * Byte stream i/o classes listening or sending data to a byte stream.
 *
 * The API documented by this module permits to implement objects refering to byte streams.
 * These objects can receive data from the bytestream and send data to the byte stream.
 *
 * An i/o object can check if a given object implements an FTS byte stream and whether it is an input and/or an output 
 * using the functions fts_bytestream_check(), fts_bytestream_is_input() and fts_bytestream_is_output().
 * The API provides functions for declaring an object as a listener to a byte stream (see fts_bytestream_add_listener()) 
 * and output functions sending events to a byte stream (see fts_bytestream_output()).
 *
 * @defgroup bytestream_io byte stream i/o
 */

/** 
 * @name Refering to a byte stream
 *
 * Functions for objects refering to byte stream as input (listeners) or output.
 */
/*@{*/

/**
 * Check whether an FTS object implements the byte stream abstraction
 *
 * An object who wants to listen to or output via a byte stream should check if the object it referes to implements the byte stream abstraction
 *
 * @fn int fts_bytestream_check(fts_object_t *obj)
 * @param obj the object to be checked
 * @return non-zero if object implements an FTS byte stream 
 *
 * @ingroup bytestream_io
 */

/**
 * Check whether an FTS byte stream is an input
 *
 * @fn int fts_bytestream_is_input(fts_bytestream_t *stream)
 * @param stream the byte stream to be checked
 * @return non-zero if stream is input
 *
 * @ingroup bytestream_io
 */

/**
 * Check whether an FTS byte stream is an input
 *
 * @fn fts_bytestream_is_output(fts_bytestream_t *stream)
 * @param stream the byte stream to be checked
 * @return non-zero if stream is input
 *
 * @ingroup bytestream_io
 */

extern int fts_bytestream_check(fts_object_t *obj);
extern int fts_bytestream_is_input(fts_bytestream_t *stream);
extern int fts_bytestream_is_output(fts_bytestream_t *stream);

/*@}*/ /* Refering to a byte stream.*/

/* define functions by macros */
#define fts_bytestream_is_input(s) ((s)->input != 0)
#define fts_bytestream_is_output(s) ((s)->output != 0)

/** 
 * @name Byte stream listeners
 */
/*@{*/

/**
 * Register listener to a byte stream.
 *
 * In order to receive incoming data from a byte stream, an FTS object registers itself as a listener to the byte stream.
 *
 * @fn void fts_bytestream_add_listener(fts_bytestream_t *stream, fts_object_t *obj, fts_bytestream_callback_t fun)
 * @param stream the listened byte stream
 * @param obj the listening object
 * @param fun listener function to be called for incoming data
 * @see fts_bytestream_remove_listener()
 *
 * @ingroup bytestream_io
 */

/**
 * Remove listener from a byte stream.
 *
 * An object listening to a byte stream must be removed as listener before being destroyed.
 * Typically this is done in the object's delete method.
 *
 * @fn void fts_bytestream_remove_listener(fts_bytestream_t *stream, fts_object_t *obj)
 * @param stream the listened byte stream
 * @param obj the listening object
 *
 * @ingroup bytestream_io
 */

extern void fts_bytestream_add_listener(fts_bytestream_t *stream, fts_object_t *obj, fts_bytestream_callback_t fun);
extern void fts_bytestream_remove_listener(fts_bytestream_t *stream, fts_object_t *obj);

/*@}*/ /* Byte stream listeners */

/** 
 * @name Output functions
 */
/*@{*/

/**
 * Send a string to a byte stream.
 *
 * @fn void fts_bytestream_output(fts_bytestream_t *stream, int n, const unsigned char *c);
 * @param stream the byte stream
 * @param n the number of characters
 * @param c the characters
 * @see fts_bytestream_output_char
 * @see fts_bytestream_flush
 *
 * @ingroup bytestream_io
 */

/**
 * Send a single character to a byte stream.
 *
 * @fn void fts_bytestream_output_char(fts_bytestream_t *stream, unsigned char c)
 * @param stream the byte stream
 * @param c the character
 * @see fts_bytestream_output
 * @see fts_bytestream_flush
 *
 * @ingroup bytestream_io
 */

/**
 * Flush byte streams output buffer (if any)
 *
 * @fn void fts_bytestream_flush(fts_bytestream_t *stream)
 * @param stream the byte stream
 * @see fts_bytestream_output
 * @see fts_bytestream_output_char
 *
 * @ingroup bytestream_io
 */

extern void fts_bytestream_output(fts_bytestream_t *stream, int n, const unsigned char *c);
extern void fts_bytestream_output_char(fts_bytestream_t *stream, unsigned char c);
extern void fts_bytestream_flush(fts_bytestream_t *stream);

/*@}*/ /* Output functions */

/* define functions by macros */
#define fts_bytestream_output(s, n, c) ((s)->output((s), (n), (c)))
#define fts_bytestream_output_char(s, c) ((s)->output_char((s), (c)))
#define fts_bytestream_flush(s) {if((s)->flush) (s)->flush(s);}

#endif
