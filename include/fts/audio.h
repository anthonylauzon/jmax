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
 * The FTS audio port structure.
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
 * A audio port class must call \c fts_audioport_init() and \c fts_audioport_delete() 
 * in the objects \e init and \e delete methods.
 *
 * @typedef fts_audioport_t
 *
 * @ingroup audioport
 */
struct fts_audioport_direction {
  int channels;
  fts_symbol_t io_function_name;
  ftl_wrapper_t io_function;
  fts_object_t *dsp_object;
  fts_object_t *dispatcher;
};
  
  
typedef struct fts_audioport {
  fts_object_t head;
  struct fts_audioport *next;
  struct fts_audioport_direction input;
  struct fts_audioport_direction output;
  void (*idle_function)( struct fts_audioport *port);
  int (*xrun_function)( struct fts_audioport *port);
} fts_audioport_t;

FTS_API void fts_audioport_init( fts_audioport_t *port);
FTS_API void fts_audioport_delete( fts_audioport_t *port);

FTS_API int fts_object_is_audioport( fts_object_t *obj);

#define fts_audioport_get_input_function(P) (((fts_audioport_t *)(P))->input.io_function)
#define fts_audioport_get_input_function_name(P) (((fts_audioport_t *)(P))->input.io_function_name)
#define fts_audioport_set_input_function(P,F) __fts_audioport_set_input_function((fts_audioport_t *)(P),fts_new_symbol(#F),F)
FTS_API void __fts_audioport_set_input_function( fts_audioport_t *port, fts_symbol_t name, ftl_wrapper_t fun);

#define fts_audioport_get_output_function(P) (((fts_audioport_t *)(P))->output.io_function)
#define fts_audioport_get_output_function_name(P) (((fts_audioport_t *)(P))->output.io_function_name)
#define fts_audioport_set_output_function(P,F) __fts_audioport_set_output_function((fts_audioport_t *)(P),fts_new_symbol(#F),F)
FTS_API void __fts_audioport_set_output_function( fts_audioport_t *port, fts_symbol_t name, ftl_wrapper_t fun);

#define fts_audioport_get_input_channels(P) (((fts_audioport_t *)(P))->input.channels)
FTS_API void fts_audioport_set_input_channels( fts_audioport_t *port, int channels);

#define fts_audioport_get_output_channels(P) (((fts_audioport_t *)(P))->output.channels)
FTS_API void fts_audioport_set_output_channels( fts_audioport_t *port, int channels);

FTS_API void fts_audioport_add_input_object( fts_audioport_t *port, int channel, fts_object_t *object);
FTS_API void fts_audioport_remove_input_object( fts_audioport_t *port, int channel, fts_object_t *object);

#define fts_audioport_get_output_dispatcher(P) ((P)->output.dispatcher)

/*  1)  fts_audioport_set_idle_function( my_idle_function); */
/*  2)  fts_audioport_set_idle_function( NULL); */
/*  3)  corresponding to default (idle function created by fts_audioport_idle() */
#define fts_audioport_set_idle_function(P,F) (((fts_audioport_t *)(P))->idle_function = (F))

FTS_API void fts_audioport_idle( fts_word_t *args);

/* dac slip report:
 the audioport xrun function returns an int saying that there has been an xrun since last call
 the dspcontrol calls fts_audioport_report_xrun to know the state of xrun
*/
#define fts_audioport_set_xrun_function(P,F) (((fts_audioport_t *)(P))->xrun_function = (F))

FTS_API int fts_audioport_report_xrun( void);

FTS_API fts_audioport_t *fts_audioport_get_default( fts_object_t *obj);
FTS_API void fts_audioport_set_default_class( fts_symbol_t name);

