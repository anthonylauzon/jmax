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

typedef struct _fts_bytestream_ fts_bytestream_t;

/****************************************************
 *
 *  callbacks and listeners
 *
 */
typedef void (*fts_bytestream_callback_t)(fts_object_t *listener, int n, const unsigned char* c);
typedef void (*fts_bytestream_output_t)(fts_bytestream_t *stream, int n, const unsigned char *c);
typedef void (*fts_bytestream_output_char_t)(fts_bytestream_t *stream, unsigned char c);
typedef void (*fts_bytestream_flush_t)(fts_bytestream_t *stream);

typedef struct _fts_bytestream_listener_
{
  fts_bytestream_callback_t callback;
  fts_object_t *listener;
  struct _fts_bytestream_listener_ *next;
} fts_bytestream_listener_t;

/****************************************************
 *
 *  byte stream
 *
 */
struct _fts_bytestream_
{
  fts_object_t o;
  int input; /* flag telling if bytestream is input */
  fts_bytestream_listener_t *listeners; /* list of objects listening to byte stream */
  fts_bytestream_output_t output; /* stream output function */
  fts_bytestream_output_char_t output_char; /* stream output function for single char */
  fts_bytestream_flush_t flush; /* stream flush function (when output buffered) */
};

extern void fts_bytestream_init(fts_bytestream_t *stream);
extern void fts_bytestream_set_input(fts_bytestream_t *stream);
extern void fts_bytestream_set_output(fts_bytestream_t *stream, 
				       fts_bytestream_output_t output, fts_bytestream_output_char_t output_char, fts_bytestream_flush_t flush);
extern void fts_bytestream_class_init(fts_class_t *cl);

extern void fts_bytestream_add_listener(fts_bytestream_t *stream, fts_object_t *listener, fts_bytestream_callback_t fun);
extern void fts_bytestream_remove_listener(fts_bytestream_t *stream, fts_object_t *listener);

extern int fts_bytestream_has_superclass(fts_object_t *obj);

#define fts_bytestream_is_input(s) ((s)->input != 0)
#define fts_bytestream_is_output(s) ((s)->output != 0)

/* function called by bytestream class on input (call all listeners) */
extern void fts_bytestream_input(fts_bytestream_t *stream, int n, const unsigned char *c);

/* generic stream output functions (called by out object to output bytes to stream) */
#define fts_bytestream_output(s, n, c) ((s)->output((s), (n), (c)))
#define fts_bytestream_output_char(s, c) ((s)->output_char((s), (c)))
#define fts_bytestream_flush(s) {if((s)->flush) (s)->flush(s);}

#endif
