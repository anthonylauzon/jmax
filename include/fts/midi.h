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
 */

#ifndef _FTS_MIDI_H_
#define _FTS_MIDI_H_

typedef struct _fts_midi_port fts_midi_port_t;

/* The function that is called by the scheduler */
FTS_API void fts_midi_poll( void);

FTS_API fts_midi_port_t *fts_midi_get_port(int idx);

/* return a pointer to a long keeping the midi time code of the port */
FTS_API double *fts_get_midi_time_code_p(int idx); 

typedef  void (* midi_action_t) (fts_midi_port_t *p, int midi_ev, void *user_data, int argc, fts_atom_t *at);

FTS_API void fts_midi_install_fun(fts_midi_port_t *port, int midi_ev, midi_action_t fun, void *user_data);
FTS_API void fts_midi_deinstall_fun(fts_midi_port_t *port, int midi_ev, midi_action_t fun, void *user_data);


/* Macros identifing the MIDI_EV  in the above call.
   Note that the n argument to the _CH macros can be between 1 and 16.
   Should add at least the modulation wheel ...
*/
#define FTS_MIDI_NOTE                       0
#define FTS_MIDI_NOTE_CH(n)                 (FTS_MIDI_NOTE + (n))
#define FTS_MIDI_POLY_AFTERTOUCH            (FTS_MIDI_NOTE + 16 + 1)
#define FTS_MIDI_POLY_AFTERTOUCH_CH(n)      (FTS_MIDI_POLY_AFTERTOUCH + (n))
#define FTS_MIDI_CONTROLLER                 (FTS_MIDI_POLY_AFTERTOUCH + 16 + 1)
#define FTS_MIDI_CONTROLLER_CH(n)           (FTS_MIDI_CONTROLLER + (n))
#define FTS_MIDI_PROGRAM_CHANGE             (FTS_MIDI_CONTROLLER + 16 + 1)
#define FTS_MIDI_PROGRAM_CHANGE_CH(n)       (FTS_MIDI_PROGRAM_CHANGE + (n))
#define FTS_MIDI_CHANNEL_AFTERTOUCH         (FTS_MIDI_PROGRAM_CHANGE + 16 + 1)
#define FTS_MIDI_CHANNEL_AFTERTOUCH_CH(n)   (FTS_MIDI_CHANNEL_AFTERTOUCH + (n))
#define FTS_MIDI_PITCH_BEND                 (FTS_MIDI_CHANNEL_AFTERTOUCH + 16 + 1)
#define FTS_MIDI_PITCH_BEND_CH(n)           (FTS_MIDI_PITCH_BEND + (n))
#define FTS_MIDI_BYTE                       (FTS_MIDI_PITCH_BEND + 16 + 1)
#define FTS_MIDI_SYSEX                      (FTS_MIDI_BYTE + 1)
#define FTS_MIDI_REALTIME                   (FTS_MIDI_SYSEX + 1)
#define FTS_MIDI_MTC                        (FTS_MIDI_REALTIME + 1)

#define MAX_FTS_MIDI_EV                     FTS_MIDI_MTC

FTS_API void fts_midi_send(fts_midi_port_t *p, long val);


/**
 * The MIDI port abstraction (fts_midiport_t()).
 *
 * The MIDI port is an abstraction of classes representing MIDI in/output devices.
 * A MIDI port represents a single MIDI connection (carrying 16 MIDI channels).
 * For multi-port MIDI sofware systems or devices (as MidiShare) \e one FTS MIDI port object
 * should represent \e one (virtual or physical) MIDI port.
 *
 * MIDI ports are FTS objects.
 *
 * Various i/o objects can refer to an FTS MIDI port in order to receive MIDI events (listeners) 
 * or send events to a virtual or physical MIDI port.
 * MIDI port listeners can be registered for a specific MIDI channel and note or contoller number.
 *
 * If an i/o object refers to a MIDI port object by a named variable,
 * the same program (patch) can be configured for completely different setups
 * simply by redefining the MIDI port object defining the variable.
 * Different MIDI ports are implemented for different platforms and MIDI i/o APIs as well as for
 * other devices or protocols implementing standard MIDI events.
 *
 * The API documented by this module permits to implement MIDI port classes.
 * It contains the MIDI port structure itself and the initialization functions for the FTS class 
 * implementing an FTS MIDI port: fts_midiport_class_init(), fts_midiport_init() and fts_midiport_delete().
 *
 * A MIDI port class allowing MIDI input, calls fts_midiport_set_input() in the objects initialization
 * and uses the provided input functions (e.g. fts_midiport_input_note()) in the routines handling incoming MIDI data.
 * These functions propagate the incoming MIDI events to the listening i/o objects refering to the MIDI port.
 *
 * For the output a MIDI port class must implement an output function for each type of MIDI events.
 * The output functions are bundeled into a structure (@ref fts_midiport_output_functions_t)
 * and declared by fts_midiport_set_output() in the objects initialization.
 *
 * @defgroup midiport MIDI port
 */

/**
 * Function called for polyphonic MIDI event (note, poly pressure change, controller value change).
 *
 * @typedef void (*fts_midiport_poly_fun_t)(fts_object_t *o, int channel, int number, int value, double time)
 *
 * @ingroup midiport
 */

/**
 * Function called for monophonic MIDI events (program change, channel pressure change, pitch bend wheel change).
 *
 * @typedef void (*fts_midiport_channel_fun_t)(fts_object_t *o, int channel, int value, double time)
 *
 * @ingroup midiport
 */

/**
 * Function called for incommig system exclusive data MIDI events.
 *
 * @typedef void (*fts_midiport_sysex_fun_t)(fts_object_t *o, int ac, const fts_atom_t *at, double time)
 *
 * @ingroup midiport
 */

/**
 * Add a system exclusive byte to an i/o buffer.
 *
 * @typedef void (*fts_midiport_sysex_byte_fun_t)(fts_object_t *o, int c)
 */

/**
 * Flush system exclusive i/o buffer.
 *
 * @typedef void (*fts_midiport_sysex_flush_fun_t)(fts_object_t *o, double time)
 *
 * @ingroup midiport
 */

typedef void (*fts_midiport_poly_fun_t)(fts_object_t *o, int channel, int number, int value, double time);
typedef void (*fts_midiport_channel_fun_t)(fts_object_t *o, int channel, int value, double time);
typedef void (*fts_midiport_sysex_fun_t)(fts_object_t *o, int ac, const fts_atom_t *at, double time);

typedef void (*fts_midiport_sysex_byte_fun_t)(fts_object_t *o, int c);
typedef void (*fts_midiport_sysex_flush_fun_t)(fts_object_t *o, double time);

typedef struct fts_midiport_listeners fts_midiport_listeners_t;

/**
 * MIDI port output functions.
 *
 * This is the structure of output functions to be implemented by a MIDI port class.
 * It contains one function for each type of MIDI event.
 * The system exclusive output is handled by two functions: 
 * The \c sysex_byte function is adding a byte to an internal buffer.
 * If a sysex block is completed the buffer is flushed and sent out with the exact time tag by \c sysex_flush.
 *
 * @code
 *   struct fts_midiport_output_functions
 *   {
 *     fts_midiport_poly_fun_t note;
 *     fts_midiport_poly_fun_t poly_pressure;
 *     fts_midiport_poly_fun_t control_change;
 *     fts_midiport_channel_fun_t program_change;
 *     fts_midiport_channel_fun_t channel_pressure;
 *     fts_midiport_channel_fun_t pitch_bend;
 *     fts_midiport_sysex_byte_fun_t sysex_byte;
 *     fts_midiport_sysex_flush_fun_t sysex_flush;
 *   };
 * @endcode
 *
 * @anchor fts_midiport_output_functions_t
 * @typedef fts_midiport_output_functions_t
 *
 * @ingroup midiport
 */

/* @code */
typedef struct fts_midiport_output_functions
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
/* @endcode */

/** 
 * @name The FTS MIDI port structure
 */
/*@{*/

/**
 * The FTS MIDI port structure.
 *
 * The FTS MIDI port "inherits" from FTS object.
 * The structure \b fts_midiport_t itself must be included by a class implementing a MIDI port:
 *
 * @code
 *   typedef struct my_midiport
 *   {
 *     fts_midiport_t port;
 *     ... 
 *   } my_midiport_t;
 * @endcode
 *
 * A MIDI port class must call \c fts_midiport_init() and \c fts_midiport_delete() 
 * in the objects \e init and \e delete Methods.
 *
 * @typedef fts_midiport_t
 *
 * @ingroup midiport
 */
typedef struct fts_midiport
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


/*@}*/ /* The FTS MIDI port structure */

/** 
 * @name Initializing a MIDI port
 *
 * Initialization of classes and objects implementing a MIDI port.
 */
/*@{*/ /* Initializing a MIDI port */

/**
 * Initialize a class implementing an FTS MIDI port.
 *
 * @fn void fts_midiport_class_init(fts_class_t *cl)
 * @param cl FTS class
 *
 * @ingroup midiport
 */

/**
 * Initialize a MIDI port structure.
 *
 * @fn void fts_midiport_init(fts_midiport_t *port)
 * @param port the MIDI port
 *
 * @ingroup midiport
 */

/**
 * Free a MIDI port structures temporary buffers.
 *
 * @fn void fts_midiport_delete(fts_midiport_t *port)
 * @param port the MIDI port
 * @warning This function doesn't free the MIDI port structure itself.
 *
 * @ingroup midiport
 */

FTS_API void fts_midiport_class_init(fts_class_t *cl);
FTS_API void fts_midiport_init(fts_midiport_t *port);
FTS_API void fts_midiport_delete(fts_midiport_t *port);

/**
 * Declare initialized MIDI port as input.
 *
 * @fn void fts_midiport_set_input(fts_midiport_t *port)
 * @param port the MIDI port
 *
 * @ingroup midiport
 */

/**
 * Declare an initialized MIDI port as output and assign it's output functions.
 *
 * @fn void fts_midiport_set_output(fts_midiport_t *port, fts_midiport_output_functions_t *functions)
 * @param port the MIDI port
 * @param functions structure of implemented output functions
 *
 * @ingroup midiport
 */

FTS_API void fts_midiport_set_input(fts_midiport_t *port);
FTS_API void fts_midiport_set_output(fts_midiport_t *port, fts_midiport_output_functions_t *functions);

/*@}*/ /* Initializing a MIDI port */

/** 
 * @name Handling incoming MIDI events
 *
 * Functions to call the associated MIDI port listeners on incoming events.
 */
/*@{*/

/**
 * Call all note event listeners of the MIDI port.
 *
 * For each incoming MIDI event a function is called by the object implementing an MIDI port, which calls all
 * listeners for a given MIDI channel and (if any) note or controller number.
 *
 * @fn void fts_midiport_input_note(fts_midiport_t *port, int channel, int number, int value, double time)
 * @param port the MIDI port itself
 * @param channel the MIDI channel of the incoming event
 * @param number the MIDI note or controller number of the incoming event
 * @param value the velocity, pressure, controller, program or pitch bend value of the incoming event
 * @param time an offset time of the incoming MIDI event (in msec) regarding the current logical (tick) time
 *
 * @ingroup midiport
 */

/**
 * Call poly pressure change event listeners of the MIDI port.
 *
 * @fn void fts_midiport_input_poly_pressure(fts_midiport_t *port, int channel, int number, int value, double time)
 * @see fts_midiport_input_note()
 *
 * @ingroup midiport
 */

/**
 * Call controller change event listeners of the MIDI port.
 *
 * @fn void fts_midiport_input_control_change(fts_midiport_t *port, int channel, int number, int value, double time)
 * @see fts_midiport_input_note()
 *
 * @ingroup midiport
 */

/**
 * Call program change event listeners of the MIDI port.
 *
 * @fn void fts_midiport_input_program_change(fts_midiport_t *port, int channel, int value, double time)
 * @see fts_midiport_input_note()
 *
 * @ingroup midiport
 */

/**
 * Call channel pressure change event listeners of the MIDI port.
 *
 * @fn void fts_midiport_input_channel_pressure(fts_midiport_t *port, int channel, int value, double time)
 * @see fts_midiport_input_note()
 *
 * @ingroup midiport
 */

/**
 * Call system exclusive data event listeners of the MIDI port.
 *
 * @fn void fts_midiport_input_pitch_bend(fts_midiport_t *port, int channel, int value, double time)
 * @see fts_midiport_input_note()
 *
 * @ingroup midiport
 */

/**
 * Add byte to the MIDI port's internal sysex buffer.
 *
 * @fn void fts_midiport_input_system_exclusive_byte(fts_midiport_t *port, int value)
 * @see fts_midiport_input_system_exclusive_call()
 *
 * @ingroup midiport
 */

/**
 * Call system exclusive data event listeners of the MIDI port with the data of the internal buffer.
 *
 * @fn void fts_midiport_input_system_exclusive_call(fts_midiport_t *port, double time)
 * @see fts_midiport_input_system_exclusive_byte()
 *
 * @ingroup midiport
 */

FTS_API void fts_midiport_input_note(fts_midiport_t *port, int channel, int number, int value, double time);
FTS_API void fts_midiport_input_poly_pressure(fts_midiport_t *port, int channel, int number, int value, double time);
FTS_API void fts_midiport_input_control_change(fts_midiport_t *port, int channel, int number, int value, double time);
FTS_API void fts_midiport_input_program_change(fts_midiport_t *port, int channel, int value, double time);
FTS_API void fts_midiport_input_channel_pressure(fts_midiport_t *port, int channel, int value, double time);
FTS_API void fts_midiport_input_pitch_bend(fts_midiport_t *port, int channel, int value, double time);

FTS_API void fts_midiport_input_system_exclusive_byte(fts_midiport_t *port, int value);
FTS_API void fts_midiport_input_system_exclusive_call(fts_midiport_t *port, double time);


#define MIDIPORT_ALL_CHANNELS 0
#define MIDIPORT_ALL_NUMBERS -1


/*@}*/ /* Handling incoming MIDI events */


/****************************************************
 *
 *  MIDI port i/o
 *
 */

/**
 * MIDI port i/o classes listening or sending events to a MIDI port.
 *
 * The API documented by this module permits to implement objects refering to MIDI port.
 * These objects can receive events from the MIDI port and send events from the MIDI port.
 *
 * An i/o object can check if a given object implements an FTS MIDI port and whether it is an input and/or an output 
 * using the functions fts_object_is_midiport(), fts_midiport_is_input() and fts_midiport_is_output().
 * The API provides functions for declaring an object as a listener to a MIDI port (see fts_bytestream_add_listener()) 
 * and output functions sending events out of a MIDI port (see fts_midiport_output_note()).
 *
 * @defgroup midiport_io MIDI port i/o
 */

/** 
 * @name Refering to a MIDI port
 *
 * Functions for objects refering to MIDI port as input (listeners) or output.
 */
/*@{*/

/**
 * Check whether an FTS object implements the MIDI port abstraction
 *
 * An object who wants to listen to or output via a MIDI port should check if the object it referes to implements the MIDI port abstraction
 *
 * @fn int fts_object_is_midiport(fts_object_t *obj)
 * @param obj the object to be checked
 * @return non-zero if object implements an FTS MIDI port 
 *
 * @ingroup midiport_io
 */

/**
 * Check whether an FTS MIDI port is an input
 *
 * @fn int fts_midiport_is_input(fts_midiport_t *port)
 * @param port the MIDI port to be checked
 * @return non-zero if port is input
 *
 * @ingroup midiport_io
 */

/**
 * Check whether an FTS MIDI port is an input
 *
 * @fn fts_midiport_is_output(fts_midiport_t *port)
 * @param port the MIDI port to be checked
 * @return non-zero if port is input
 *
 * @ingroup midiport_io
 */
FTS_API int fts_object_is_midiport(fts_object_t *obj);
FTS_API int fts_midiport_is_input(fts_midiport_t *port);
FTS_API int fts_midiport_is_output(fts_midiport_t *port);

/*@}*/ /* Refering to a MIDI port.*/

/* define functions by macros */
#define fts_midiport_is_input(p) ((p)->listeners != 0)
#define fts_midiport_is_output(p) ((p)->output != 0)


/***********************************************************************
 *
 *  MIDI port listeners
 *
 */

/** 
 * @name MIDI port listeners
 *
 * Functions used by objects listening to a MIDI port
 */
/*@{*/

/**
 * Register note event listener to a MIDI port.
 *
 * In order to receive incoming MIDI messages of a certain type and channel from a MIDI port, 
 * an FTS object registeres itself as a listener to the MIDI port using this function.
 *
 * @fn void fts_midiport_add_listener_note(fts_midiport_t *port, int chan, int num, fts_object_t *obj, fts_midiport_poly_fun_t fun)
 * @param port the listened MIDI port
 * @param chan the listened MIDI channel (1..16, MIDIPORT_ALL_CHANNELS for omni)
 * @param num the listened MIDI note or controller number (0..127, MIDIPORT_ALL_NUMBERS for omni)
 * @param obj the listening object
 * @param fun listener function to be called for incoming MIDI messages of the given MIDI type and channel
 *
 * @ingroup midiport_io
 */

/**
 * Register poly pressure change event listener to a MIDI port.
 *
 * @fn void fts_midiport_add_listener_poly_pressure(fts_midiport_t *port, int chan, int num, fts_object_t *obj, fts_midiport_poly_fun_t fun)
 * @see fts_midiport_add_listener_note()
 *
 * @ingroup midiport_io
 */

/**
 * Register controller change event listener to a MIDI port.
 *
 * @fn void fts_midiport_add_listener_control_change(fts_midiport_t *port, int chan, int num, fts_object_t *obj, fts_midiport_poly_fun_t fun)
 * @see fts_midiport_add_listener_note()
 *
 * @ingroup midiport_io
 */

/**
 * Register program change event listener to a MIDI port.
 *
 * @fn void fts_midiport_add_listener_program_change(fts_midiport_t *port, int chan, fts_object_t *obj, fts_midiport_channel_fun_t fun)
 * @see fts_midiport_add_listener_note()
 *
 * @ingroup midiport_io
 */

/**
 * Register channel pressure change event listener to a MIDI port.
 *
 * @fn void fts_midiport_add_listener_channel_pressure(fts_midiport_t *port, int chan, fts_object_t *obj, fts_midiport_channel_fun_t fun)
 * @see fts_midiport_add_listener_note()
 *
 * @ingroup midiport_io
 */

/**
 * Register system exclusive data event listener to a MIDI port.
 *
 * @fn void fts_midiport_add_listener_pitch_bend(fts_midiport_t *port, int chan, fts_object_t *obj, fts_midiport_channel_fun_t fun)
 * @see fts_midiport_add_listener_note()
 *
 * @ingroup midiport_io
 */

/**
 * Register system exclusive data event listener to a MIDI port.
 *
 * @fn void fts_midiport_add_listener_system_exclusive(fts_midiport_t *port, fts_object_t *obj, fts_midiport_sysex_fun_t fun)
 * @see fts_midiport_add_listener_note()
 *
 * @ingroup midiport_io
 */

FTS_API void fts_midiport_add_listener_note(fts_midiport_t *port, int chan, int num, fts_object_t *obj, fts_midiport_poly_fun_t fun);
FTS_API void fts_midiport_add_listener_poly_pressure(fts_midiport_t *port, int chan, int num, fts_object_t *obj, fts_midiport_poly_fun_t fun);
FTS_API void fts_midiport_add_listener_control_change(fts_midiport_t *port, int chan, int num, fts_object_t *obj, fts_midiport_poly_fun_t fun);
FTS_API void fts_midiport_add_listener_program_change(fts_midiport_t *port, int chan, fts_object_t *obj, fts_midiport_channel_fun_t fun);
FTS_API void fts_midiport_add_listener_channel_pressure(fts_midiport_t *port, int chan, fts_object_t *obj, fts_midiport_channel_fun_t fun);
FTS_API void fts_midiport_add_listener_pitch_bend(fts_midiport_t *port, int chan, fts_object_t *obj, fts_midiport_channel_fun_t fun);
FTS_API void fts_midiport_add_listener_system_exclusive(fts_midiport_t *port, fts_object_t *obj, fts_midiport_sysex_fun_t fun);

/**
 * Remove note event listener from a MIDI port.
 *
 * An object listening to a MIDI port must be removed as listener before being destroyed.
 * Typically this is done in the object's delete method.
 *
 * @fn void fts_midiport_remove_listener_note(fts_midiport_t *port, int chan, int num, fts_object_t *obj)
 * @param port the listened MIDI port
 * @param chan the listened MIDI channel (1..16, MIDIPORT_ALL_CHANNELS for omni)
 * @param num the listened MIDI note or controller number (0..127, MIDIPORT_ALL_NUMBERS for omni)
 * @param obj the listening object
 *
 * @ingroup midiport_io
 */

/**
 * Remove poly pressure change event listener from a MIDI port.
 *
 * @fn void fts_midiport_remove_listener_poly_pressure(fts_midiport_t *port, int chan, int num, fts_object_t *obj)
 * @see fts_midiport_remove_listener_note()
 *
 * @ingroup midiport_io
 */

/**
 * Remove controller change event listener from a MIDI port.
 *
 * @fn void fts_midiport_remove_listener_control_change(fts_midiport_t *port, int chan, int num, fts_object_t *obj)
 * @see fts_midiport_remove_listener_note()
 *
 * @ingroup midiport_io
 */

/**
 * Remove program change event listener from a MIDI port.
 *
 * @fn void fts_midiport_remove_listener_program_change(fts_midiport_t *port, int chan, fts_object_t *obj)
 * @see fts_midiport_remove_listener_note()
 *
 * @ingroup midiport_io
 */

/**
 * Remove channel pressure change event listener from a MIDI port.
 *
 * @fn void fts_midiport_remove_listener_channel_pressure(fts_midiport_t *port, int chan, fts_object_t *obj)
 * @see fts_midiport_remove_listener_note()
 *
 * @ingroup midiport_io
 */

/**
 * Remove system exclusive data event listener from a MIDI port.
 *
 * @fn void fts_midiport_remove_listener_pitch_bend(fts_midiport_t *port, int chan, fts_object_t *obj)
 * @see fts_midiport_remove_listener_note()
 *
 * @ingroup midiport_io
 */

/**
 * Remove system exclusive data event listener from a MIDI port.
 *
 * @fn void fts_midiport_remove_listener_system_exclusive(fts_midiport_t *port, fts_object_t *obj)
 * @see fts_midiport_remove_listener_note()
 *
 * @ingroup midiport_io
 */

FTS_API void fts_midiport_remove_listener_note(fts_midiport_t *port, int chan, int num, fts_object_t *obj);
FTS_API void fts_midiport_remove_listener_poly_pressure(fts_midiport_t *port, int chan, int num, fts_object_t *obj);
FTS_API void fts_midiport_remove_listener_control_change(fts_midiport_t *port, int chan, int num, fts_object_t *obj);
FTS_API void fts_midiport_remove_listener_program_change(fts_midiport_t *port, int chan, fts_object_t *obj);
FTS_API void fts_midiport_remove_listener_channel_pressure(fts_midiport_t *port, int chan, fts_object_t *obj);
FTS_API void fts_midiport_remove_listener_pitch_bend(fts_midiport_t *port, int chan, fts_object_t *obj);
FTS_API void fts_midiport_remove_listener_system_exclusive(fts_midiport_t *port, fts_object_t *obj);

/*@}*/ /* MIDI port listeners */

/****************************************************
 *
 *  MIDI port output
 *
 */

/** 
 * @name MIDI port output
 *
 * Functions used by objects to output events through a MIDI port
 */
/*@{*/

/**
 * Output note event via a MIDI port.
 *
 * @fn void fts_midiport_output_note(fts_midiport_t *port, int channel, int number, int value, double time)
 * @param port the MIDI port
 * @param channel the MIDI channel of the incoming event
 * @param number the MIDI note or controller number of the incoming event
 * @param value the velocity, pressure, controller, program or pitch bend value of the incoming event
 * @param time an offset time of the incoming MIDI event (in msec) regarding the current logical (tick) time
 *
 * @ingroup midiport_io
 */

/**
 * Output poly pressure change event via a MIDI port.
 *
 * @fn void fts_midiport_output_poly_pressure(fts_midiport_t *port, int channel, int number, int value, double time)
 * @see fts_midiport_output_note()
 *
 * @ingroup midiport_io
 */

/**
 * Output controller change event via a MIDI port.
 *
 * @fn void fts_midiport_output_control_change(fts_midiport_t *port, int channel, int number, int value, double time)
 * @see fts_midiport_output_note()
 *
 * @ingroup midiport_io
 */

/**
 * Output program change event via a MIDI port.
 *
 * @fn void fts_midiport_output_program_change(fts_midiport_t *port, int channel, int value, double time)
 * @see fts_midiport_output_note()
 *
 * @ingroup midiport_io
 */

/**
 * Output channel pressure change event via a MIDI port.
 *
 * @fn void fts_midiport_output_channel_pressure(fts_midiport_t *port, int channel, int value, double time)
 * @see fts_midiport_output_note()
 *
 * @ingroup midiport_io
 */

/**
 * Output system exclusive data event via a MIDI port.
 *
 * @fn void fts_midiport_output_pitch_bend(fts_midiport_t *port, int channel, int value, double time)
 * @see fts_midiport_output_note()
 *
 * @ingroup midiport_io
 */

/**
 * Add byte to the sysex buffer output buffer.
 *
 * @fn void fts_midiport_output_system_exclusive_byte(fts_midiport_t *port, int value)
 * @see fts_midiport_output_system_exclusive_call()
 *
 * @ingroup midiport_io
 */

/**
 * Flush system exclusive output buffer and output system exclusive event via a MIDI port.
 *
 * @fn void fts_midiport_output_system_exclusive_flush(fts_midiport_t *port, double time)
 * @see fts_midiport_output_system_exclusive_byte()
 *
 * @ingroup midiport_io
 */

FTS_API void fts_midiport_output_note(fts_midiport_t *port, int channel, int number, int value, double time);
FTS_API void fts_midiport_output_poly_pressure(fts_midiport_t *port, int channel, int number, int value, double time);
FTS_API void fts_midiport_output_control_change(fts_midiport_t *port, int channel, int number, int value, double time);
FTS_API void fts_midiport_output_program_change(fts_midiport_t *port, int channel, int value, double time);
FTS_API void fts_midiport_output_channel_pressure(fts_midiport_t *port, int channel, int value, double time);
FTS_API void fts_midiport_output_pitch_bend(fts_midiport_t *port, int channel, int bend, double time);
FTS_API void fts_midiport_output_system_exclusive_byte(fts_midiport_t *port, int value);
FTS_API void fts_midiport_output_system_exclusive_flush(fts_midiport_t *port, double time);

/*@}*/ /* MIDI port output */

/****************************************************
 *
 *  default MIDI port
 *
 */

FTS_API fts_midiport_t *fts_midiport_get_default(void);
FTS_API void fts_midiport_set_default( int argc, const fts_atom_t *argv);




/*****************************************************
 *
 *       MIDI Parser
 *
 */
enum parser_status 
{
  status_normal,
  status_sysex, 
  status_sysex_realtime,
  status_sysex_mtc, 
  status_sysex_mtc_frame, 
  status_mtc_quarter_frame
};

enum parser_mtc_status
{
  mtc_status_invalid, 
  mtc_status_valid, 
  mtc_status_coming
};

enum channel_message 
{
  channel_message_note_off = 0,
  channel_message_note_on,
  channel_message_poly_pressure,
  channel_message_control_change,
  channel_message_program_change,
  channel_message_channel_pressure,
  channel_message_pitch_bend
};

#define NO_ARG -1

typedef struct _fts_midiparser_
{
  fts_midiport_t head;

  enum parser_status status;

  /* channel message */
  enum channel_message message;
  int channel;	
  int arg;

  /* MIDI time code */
  enum parser_mtc_status mtc_status;
  int mtc_frame_count;
  unsigned char mtc_type;
  unsigned char mtc_hour;
  unsigned char mtc_min;
  unsigned char mtc_sec;
  unsigned char mtc_frame;

  /* the time in millisecond corresponding to the MTC received */
  double mtc_time;
} fts_midiparser_t;

FTS_API void fts_midiparser_init(fts_midiparser_t *parser);
FTS_API void fts_midiparser_byte(fts_midiparser_t *parser, unsigned char byte);




#endif
