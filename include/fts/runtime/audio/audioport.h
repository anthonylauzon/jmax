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

#ifndef _FTS_AUDIOPORT_H_
#define _FTS_AUDIOPORT_H_

#include <fts/sys.h>
#include <fts/lang.h>

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
typedef struct _fts_audioport_t {
  fts_object_t head;
  struct _fts_audioport_t *next;
  fts_symbol_t input_fun_name;
  ftl_wrapper_t input_fun;
  fts_symbol_t output_fun_name;
  ftl_wrapper_t output_fun;
  int input_channels;
  int output_channels;
  fts_object_t *audioportin;
  fts_object_t *audioportout;
  void (*idle_function)( struct _fts_audioport_t *port);
  int (*xrun_function)( struct _fts_audioport_t *port);
} fts_audioport_t;

extern void fts_audioport_init( fts_audioport_t *port);
extern void fts_audioport_delete( fts_audioport_t *port);

extern int fts_object_is_audioport( fts_object_t *obj);

#define fts_audioport_get_input_function(P) (((fts_audioport_t *)(P))->input_fun)
#define fts_audioport_get_input_function_name(P) (((fts_audioport_t *)(P))->input_fun_name)
#define fts_audioport_set_input_function(P,F) __fts_audioport_set_input_function((fts_audioport_t *)(P),fts_new_symbol(#F),F)
extern void __fts_audioport_set_input_function( fts_audioport_t *port, fts_symbol_t name, ftl_wrapper_t fun);

#define fts_audioport_get_output_function(P) (((fts_audioport_t *)(P))->output_fun)
#define fts_audioport_get_output_function_name(P) (((fts_audioport_t *)(P))->output_fun_name)
#define fts_audioport_set_output_function(P,F) __fts_audioport_set_output_function((fts_audioport_t *)(P),fts_new_symbol(#F),F)
extern void __fts_audioport_set_output_function( fts_audioport_t *port, fts_symbol_t name, ftl_wrapper_t fun);

#define fts_audioport_get_input_channels(P) (((fts_audioport_t *)(P))->input_channels)
#define fts_audioport_set_input_channels(P,C) (((fts_audioport_t *)(P))->input_channels = (C))

#define fts_audioport_get_output_channels(P) (((fts_audioport_t *)(P))->output_channels)
#define fts_audioport_set_output_channels(P,C) (((fts_audioport_t *)(P))->output_channels = (C))

/*  1)  fts_audioport_set_idle_function( my_idle_function); */
/*  2)  fts_audioport_set_idle_function( NULL); */
/*  3)   */
#define fts_audioport_set_idle_function(P,F) (((fts_audioport_t *)(P))->idle_function = (F))

extern fts_object_t *fts_audioport_get_in_object( fts_audioport_t *port, fts_object_t *owner, int outlet);
extern void fts_audioport_remove_in_object( fts_object_t *in_object);

extern fts_object_t *fts_audioport_get_out_object( fts_audioport_t *port, int inlet);
extern void fts_audioport_remove_out_object( fts_object_t *out_object);

/* dac slip report:
 the audioport xrun function returns an int saying that there has been an xrun since last call
 the dspcontrol calls fts_audioport_report_xrun to know the state of xrun
*/
#define fts_audioport_set_xrun_function(P,F) (((fts_audioport_t *)(P))->xrun_function = (F))
extern int fts_audioport_report_xrun( void);

/* Used by the DSP compiler */
extern void fts_audioport_add_input_output_objects( void);

extern void fts_audioport_set_default( int argc, const fts_atom_t *argv);
extern fts_audioport_t *fts_audioport_get_default( fts_object_t *obj);

#endif
