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

/**
 * The FTS MIDI port abstraction.
 * The FTS MIDI port is an abstraction for classes representing MIDI in/output devices.
 *
 * @file midiport.h
 */
#ifndef _FTS_MIDIPORT_H_
#define _FTS_MIDIPORT_H_

#define MIDIPORT_ALL_CHANNELS 0
#define MIDIPORT_ALL_NUMBERS -1

/****************************************************
 *
 *  MIDI port abstraction
 *
 */

/**
 * The structure of an FTS MIDI port
 *
 * @typedef fts_midiport_t
 */
/* MIDI message functions */
typedef void (*fts_midiport_poly_fun_t)(fts_object_t *o, int channel, int number, int value, double time);
typedef void (*fts_midiport_channel_fun_t)(fts_object_t *o, int channel, int value, double time);
typedef void (*fts_midiport_sysex_fun_t)(fts_object_t *o, int ac, const fts_atom_t *at, double time);

typedef void (*fts_midiport_sysex_byte_fun_t)(fts_object_t *o, int c);
typedef void (*fts_midiport_sysex_flush_fun_t)(fts_object_t *o, double time);

typedef struct _fts_midiport_listeners_ fts_midiport_listeners_t;

typedef struct _fts_midiport_output_functions_ 
{
  fts_midiport_poly_fun_t note;
  fts_midiport_poly_fun_t poly_pressure;
  fts_midiport_poly_fun_t control_change;
  fts_midiport_channel_fun_t program_change;
  fts_midiport_channel_fun_t channel_pressure;
  fts_midiport_channel_fun_t pitch_bend;
  fts_midiport_sysex_byte_fun_t sysex_byte;
  fts_midiport_sysex_flush_fun_t sysex_flush;
} fts_midiport_output_functions_t;

typedef struct _fts_midiport_
{
  fts_object_t o;

  /* input listeners */
  fts_midiport_listeners_t *listeners;

  /* output functions (declared by MIDI port class) */
  fts_midiport_output_functions_t *output;

  /* system exclusive input buffer */
  fts_atom_t *sysex_at;
  int sysex_ac;
  int sysex_alloc;

} fts_midiport_t;

/**
 * Declare class implementing a FTS MIDI port.
 *
 * @fn void fts_midiport_class_init(fts_class_t *cl)
 * @param cl FTS class
 */
extern void fts_midiport_class_init(fts_class_t *cl);

/**
 * Initialize an allocated MIDI port structure
 *
 * @fn void fts_midiport_init(fts_midiport_t *port)
 * @param port the MIDI port
 */
extern void fts_midiport_init(fts_midiport_t *port);

/**
 * Free temprary MIDI port's temporary buffers.
 *
 * @fn void fts_midiport_delete(fts_midiport_t *port)
 * @param port the MIDI port
 * @warning This function doesn't free the MIDI port structure itself.
 */
extern void fts_midiport_delete(fts_midiport_t *port);

/**
 * Declare input for an initialized MIDI port.
 *
 * @fn void fts_midiport_set_input(fts_midiport_t *port)
 * @param port the MIDI port
 */
extern void fts_midiport_set_input(fts_midiport_t *port);

/**
 * Declare output and functions for an initialized MIDI port.
 *
 * @fn void fts_midiport_set_output(fts_midiport_t *port, fts_midiport_output_t functions)
 * @param port the MIDI port
 * @param mess channel message output function
 * @param sysex system exclusive output function
 */
extern void fts_midiport_set_output(fts_midiport_t *port, fts_midiport_output_functions_t *functions);

/* midiport functions called on input (calls all listeners) */
extern void fts_midiport_input_note(fts_midiport_t *port, int channel, int number, int value, double time);
extern void fts_midiport_input_poly_pressure(fts_midiport_t *port, int channel, int number, int value, double time);
extern void fts_midiport_input_control_change(fts_midiport_t *port, int channel, int number, int value, double time);
extern void fts_midiport_input_program_change(fts_midiport_t *port, int channel, int value, double time);
extern void fts_midiport_input_channel_pressure(fts_midiport_t *port, int channel, int value, double time);
extern void fts_midiport_input_pitch_bend(fts_midiport_t *port, int channel, int value, double time);

extern void fts_midiport_input_system_exclusive_byte(fts_midiport_t *port, int value);
extern void fts_midiport_input_system_exclusive_call(fts_midiport_t *port, double time);


/****************************************************
 *
 *  MIDI port i/o
 *
 */

/** 
 * Functions used by objects refering to a MIDI port
 * @name MIDI port reference functions
 */
/*@{*/

/**
 * Check if an FTS object implements the MIDI port abstraction
 *
 * An object who wants to listen to a MIDI port should check if the object it referes to implements the MIDI port abstraction
 *
 * @fn int fts_midiport_check(fts_object_t *obj)
 * @param obj the object to be checked
 * @return non-zero if object implements an FTS MIDI port 
 */
extern int fts_midiport_check(fts_object_t *obj);

/**
 * Check if an FTS MIDI port is an input
 *
 * 
 *
 * @fn int fts_midiport_is_input(fts_midiport_t *port)
 * @param port the MIDI port to be checked
 * @return non-zero if port is input
 */
#define fts_midiport_is_input(p) ((p)->listeners != 0)
#define fts_midiport_is_output(p) ((p)->output != 0)

/*@}*/


/****************************************************
 *
 *  MIDI port listeners
 *
 */

/**
 * Register listener to a MIDI port.
 *
 * In order to receive incomming MIDI messages of a certain type and channel from a MIDI port, 
 * an FTS object registeres itself as a listener to the MIDI port using this function.
 *
 * @fn void fts_midiport_add_listener_note(fts_midiport_t *port, int chan, int num, fts_object_t *obj, fts_midiport_poly_fun_t fun)
 * @param port the listened MIDI port
 * @param chan the listened MIDI channel (1..16, 0 for omni)
 * @param num the listened MIDI note or controller number (1..128, 0 for omni)
 * @param obj the listening object
 * @param fun listener function to be called for incomming MIDI messages of the given MIDI type and channel
 * @see void fts_midiport_add_listener_poly_pressure(fts_midiport_t *port, int chan, int num, fts_object_t *obj, fts_midiport_poly_fun_t fun)
 * @see void fts_midiport_add_listener_control_change(fts_midiport_t *port, int chan, int num, fts_object_t *obj, fts_midiport_poly_fun_t fun)
 * @see void fts_midiport_add_listener_program_change(fts_midiport_t *port, int chan, fts_object_t *obj, fts_midiport_channel_fun_t fun)
 * @see void fts_midiport_add_listener_channel_pressure(fts_midiport_t *port, int chan, fts_object_t *obj, fts_midiport_channel_fun_t fun)
 * @see void fts_midiport_add_listener_pitch_bend(fts_midiport_t *port, int chan, fts_object_t *obj, fts_midiport_channel_fun_t fun)
 * @see void fts_midiport_add_listener_system_exclusive(fts_midiport_t *port, fts_object_t *obj, fts_midiport_sysex_fun_t fun)
 */

/**
 * @fn void fts_midiport_add_listener_poly_pressure(fts_midiport_t *port, int chan, int num, fts_object_t *obj, fts_midiport_poly_fun_t fun)
 */

/**
 * @fn void fts_midiport_add_listener_control_change(fts_midiport_t *port, int chan, int num, fts_object_t *obj, fts_midiport_poly_fun_t fun)
 */

/**
 * @fn void fts_midiport_add_listener_program_change(fts_midiport_t *port, int chan, fts_object_t *obj, fts_midiport_channel_fun_t fun)
 */

/**
 * @fn void fts_midiport_add_listener_channel_pressure(fts_midiport_t *port, int chan, fts_object_t *obj, fts_midiport_channel_fun_t fun)
 */

/**
 * @fn void fts_midiport_add_listener_pitch_bend(fts_midiport_t *port, int chan, fts_object_t *obj, fts_midiport_channel_fun_t fun)
 */

/**
 * @fn void fts_midiport_add_listener_system_exclusive(fts_midiport_t *port, fts_object_t *obj, fts_midiport_sysex_fun_t fun)
 */

extern void fts_midiport_add_listener_note(fts_midiport_t *port, int chan, int num, fts_object_t *obj, fts_midiport_poly_fun_t fun);
extern void fts_midiport_add_listener_poly_pressure(fts_midiport_t *port, int chan, int num, fts_object_t *obj, fts_midiport_poly_fun_t fun);
extern void fts_midiport_add_listener_control_change(fts_midiport_t *port, int chan, int num, fts_object_t *obj, fts_midiport_poly_fun_t fun);
extern void fts_midiport_add_listener_program_change(fts_midiport_t *port, int chan, fts_object_t *obj, fts_midiport_channel_fun_t fun);
extern void fts_midiport_add_listener_channel_pressure(fts_midiport_t *port, int chan, fts_object_t *obj, fts_midiport_channel_fun_t fun);
extern void fts_midiport_add_listener_pitch_bend(fts_midiport_t *port, int chan, fts_object_t *obj, fts_midiport_channel_fun_t fun);
extern void fts_midiport_add_listener_system_exclusive(fts_midiport_t *port, fts_object_t *obj, fts_midiport_sysex_fun_t fun);

/**
 * Remove listener from a MIDI port.
 *
 * An object listening to a MIDI port must be removed as listener before being destroyed.
 *
 * @fn void fts_midiport_remove_listener_note(fts_midiport_t *port, int chan, int num, fts_object_t *obj)
 * @fn void fts_midiport_remove_listener_poly_pressure(fts_midiport_t *port, int chan, int num, fts_object_t *obj)
 * @fn void fts_midiport_remove_listener_control_change(fts_midiport_t *port, int chan, int num, fts_object_t *obj)
 * @fn void fts_midiport_remove_listener_program_change(fts_midiport_t *port, int chan, fts_object_t *obj)
 * @fn void fts_midiport_remove_listener_channel_pressure(fts_midiport_t *port, int chan, fts_object_t *obj)
 * @fn void fts_midiport_remove_listener_pitch_ben(fts_midiport_t *port, int chan, fts_object_t *obj)
 * @fn void fts_midiport_remove_listener_system_exclusive(fts_midiport_t *port, fts_object_t *obj)
 * @param port the listened MIDI port
 * @param chan the listened MIDI channel (1..16, 0 for omni)
 * @param num the listened MIDI note or controller number (1..128, 0 for omni)
 * @param obj the listening object
 */
extern void fts_midiport_remove_listener_note(fts_midiport_t *port, int chan, int num, fts_object_t *obj);
extern void fts_midiport_remove_listener_poly_pressure(fts_midiport_t *port, int chan, int num, fts_object_t *obj);
extern void fts_midiport_remove_listener_control_change(fts_midiport_t *port, int chan, int num, fts_object_t *obj);
extern void fts_midiport_remove_listener_program_change(fts_midiport_t *port, int chan, fts_object_t *obj);
extern void fts_midiport_remove_listener_channel_pressure(fts_midiport_t *port, int chan, fts_object_t *obj);
extern void fts_midiport_remove_listener_pitch_bend(fts_midiport_t *port, int chan, fts_object_t *obj);
extern void fts_midiport_remove_listener_system_exclusive(fts_midiport_t *port, fts_object_t *obj);

/****************************************************
 *
 *  MIDI port output
 *
 */

/* midiport functions called on output (calls all listeners) */
extern void fts_midiport_output_note(fts_midiport_t *port, int channel, int number, int value, double time);
extern void fts_midiport_output_poly_pressure(fts_midiport_t *port, int channel, int number, int value, double time);
extern void fts_midiport_output_control_change(fts_midiport_t *port, int channel, int number, int value, double time);
extern void fts_midiport_output_program_change(fts_midiport_t *port, int channel, int value, double time);
extern void fts_midiport_output_channel_pressure(fts_midiport_t *port, int channel, int value, double time);
extern void fts_midiport_output_pitch_bend(fts_midiport_t *port, int channel, int bend, double time);
extern void fts_midiport_output_system_exclusive_byte(fts_midiport_t *port, int value);
extern void fts_midiport_output_system_exclusive_flush(fts_midiport_t *port, double time);

/****************************************************
 *
 *  default MIDI port
 *
 */
typedef fts_midiport_t * (*fts_midiport_default_function_t)(void);

extern void fts_midiport_set_default_function(fts_midiport_default_function_t fun);
extern fts_midiport_t *fts_midiport_get_default(void);

extern fts_midiport_t *fts_midiport_get_default(void);
extern void fts_midiport_set_default_function(fts_midiport_default_function_t fun);

#endif
