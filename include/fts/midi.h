/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

/*****************************************************
 *
 *  MIDI fifo
 *
 */

/**
 * Simple fifo of time tagged MIDI events with their target port.
 * At the initialization the fifo is filled with (invalid) MIDI events.
 *
 * With the function fts_midififo_get_event() one gets the next event from the fifo
 * which can be initiatized in the following. The write function fts_midififo_write() 
 * completes the new fifo event with a time tag and target port and increments the 
 * fifo write pointer.
 * Reading an event from the fifo using fts_midififo_poll() directly calls the
 * target MIDI port's input function or schedules the call to the FTS timebase 
 * (depending on the time tag). The time tag of the event has to be coherent for the
 * writing thread (or function) and is automatically adjusted to the FTS time base.
 *
 * @defgroup midififo MIDI fifo
 */

typedef struct fts_midififo_entry
{
  double time;
  fts_midievent_t *event;
  fts_object_t *port;
} fts_midififo_entry_t;

typedef struct fts_midififo
{
  fts_fifo_t data;
  int size;
  double delta;
} fts_midififo_t;

/** 
 * @name MIDI fifo
 */
/*@{*/

FTS_API void fts_midififo_get_events(fts_midififo_t *fifo);
#define fts_midififo_get_events(f) ((fts_midififo_entry_t **)((f)->data.buffer))

/**
 * Initialize (and allocate) a MIDI fifo structure.
 *
 * @fn void fts_midififo_init(fts_midififo_t *fifo, int size)
 * @param fifo the MIDI fifo
 * @param size in number of events
 *
 * @ingroup midififo
 */
FTS_API void fts_midififo_init(fts_midififo_t *fifo, int size);

/**
 * Reset (and free) a MIDI fifo structure.
 *
 * @fn void void fts_midififo_reset(fts_midififo_t *fifo)
 * @param fifo the MIDI fifo
 * @warning This function doesn't free the MIDI fifo structure itself.
 *
 * @ingroup midififo
 */
FTS_API void fts_midififo_reset(fts_midififo_t *fifo);

/**
 * Poll all qued events of the MIDI fifo (calls input function of target MIDI port).
 *
 * @fn void fts_midififo_poll(fts_midififo_t *fifo)
 * @param fifo the MIDI fifo
 *
 * @ingroup midififo
 */
FTS_API void fts_midififo_poll(fts_midififo_t *fifo);

/**
 * Get the next MIDI event ready for writing.
 *
 * @fn fts_midievent_t *fts_midififo_get_event(fts_midififo_t *fifo)
 * @param fifo the MIDI fifo
 * @return next available MIDI event to be initialized or NULL if fifo is full
 *
 * @ingroup midififo
 */
FTS_API fts_midievent_t *fts_midififo_get_event(fts_midififo_t *fifo);

/**
 * Finalize the MIDI event previously returned by fts_midififo_get_event()
 * and increment the fifo's write pointer.
 *
 * @fn void fts_midififo_write(fts_midififo_t *fifo, fts_object_t *port, double time)
 * @param fifo the MIDI fifo
 * @param port target MIDI port of the MIDI event
 * @param time time tag in msec regarding the time of the writing thread
 *
 * @ingroup midififo
 */
FTS_API void fts_midififo_write(fts_midififo_t *fifo, fts_object_t *port, double time);

/**
 * Resynchronize MIDI fifo (set delta time to 0)
 *
 * @fn void fts_midififo_resync(fts_midififo_t *fifo)
 * @param fifo the MIDI fifo
 *
 * @ingroup midififo
 */
FTS_API void fts_midififo_resync(fts_midififo_t *fifo);

/*@}*/

/****************************************************
 *
 *  MIDI ports
 *
 */

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
 * implementing an FTS MIDI port: fts_midiport_class_init(), fts_midiport_init() and fts_midiport_reset().
 *
 * A MIDI port class allowing MIDI input, calls fts_midiport_set_input() in the objects initialization
 * and uses fts_midiport_input() (or a specialized function) in the routines handling incoming MIDI data.
 * These functions propagate the incoming MIDI events to the listening i/o objects refering to the MIDI port.
 *
 * An output MIDI port class must implement an output function.
 * The output function is declared by fts_midiport_set_output() in the objects initialization.
 *
 * @defgroup midiport MIDI port
 */

typedef void (*fts_midiport_output_t)(fts_object_t *o, fts_midievent_t *event, double time);

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
 * A MIDI port class must call \c fts_midiport_init() and \c fts_midiport_reset() 
 * in the objects \e init and \e delete Methods.
 *
 * @typedef fts_midiport_t
 *
 * @ingroup midiport
 */
typedef struct fts_midiport_listener
{
  fts_method_t callback;
  fts_object_t *listener;
  struct fts_midiport_listener *next;
} fts_midiport_listener_t;

typedef struct fts_midiport
{
  fts_object_t o;
  
  /* input listeners */
  fts_midiport_listener_t **listeners[n_midi_types + 1];
  
  /* output function (declared by MIDI port class) */
  fts_midiport_output_t output;
  
} fts_midiport_t;

/*@}*/ /* The FTS MIDI port structure */

/** 
 * @name Initializing a MIDI port
 *
 * Initialization of classes and objects implementing a MIDI port.
 */
/*@{*/ /* Initializing a MIDI port */

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
 * @fn void fts_midiport_reset(fts_midiport_t *port)
 * @param port the MIDI port
 * @warning This function doesn't free the MIDI port structure itself.
 *
 * @ingroup midiport
 */

FTS_API void fts_midiport_class_init(fts_class_t *cl);
FTS_API void fts_midiport_init(fts_midiport_t *port);
FTS_API void fts_midiport_reset(fts_midiport_t *port);

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
 * @fn void fts_midiport_set_output(fts_midiport_t *port, fts_midiport_output_t *function)
 * @param port the MIDI port
 * @param the implemented output function
 *
 * @ingroup midiport
 */

FTS_API void fts_midiport_set_input(fts_midiport_t *port);
FTS_API void fts_midiport_set_output(fts_midiport_t *port, fts_midiport_output_t function);

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
 * @fn void fts_midiport_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
 * @param o the MIDI port itself
 * @param winlet
 * @param s
 * @param ac
 * @param at
 *
 * @ingroup midiport
 */

FTS_API void fts_midiport_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

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
 * The API provides functions for declaring an object as a listener to a MIDI port (see fts_midiport_add_listener()) 
 * and an output function to send events out of a MIDI port (see fts_midiport_output()).
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
 * An object who wants to listen to or output via a MIDI port should check 
 * if the object it referes to implements the MIDI port abstraction
 *
 * @fn int fts_object_is_midiport(fts_object_t *obj)
 * @param obj the object to be checked
 * @return non-zero if object implements an FTS MIDI port 
 *
 * @ingroup midiport_io
 */

/**
* Initialize a class implementing an FTS MIDI port.
 *
 * @fn void fts_midiport_class_init(fts_class_t *cl)
 * @param cl FTS class
 *
 * @ingroup midiport
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
#define fts_midiport_is_input(p) ((p)->listeners[0] != 0)
#define fts_midiport_is_output(p) ((p)->output != 0)

/** 
 * @name MIDI port listeners and output function
 *
 * Functions used by objects listening to a MIDI port
 */
/*@{*/

/**
 * Register event listener to a MIDI port.
 *
 * In order to receive incoming MIDI messages of a certain type and channel from a MIDI port, 
 * an FTS object registeres itself as a listener to the MIDI port using this function.
 *
 * @fn void fts_midiport_add_listener(fts_midiport_t *port, enum midi_type type, int chan, int num, fts_object_t *obj, fts_method_t fun)
 * @param port the listened MIDI port
 * @param type type of midi event
 * @param chan the listened MIDI channel (1..16, midi_channel_any for omni) or system event type
 * @param num the listened MIDI note or controller number (0..127, midi_number_any for omni)
 * @param obj the listening object
 * @param fun callback for incoming MIDI messages of the given MIDI type, id, and number
 *
 * @ingroup midiport_io
 */

/**
 * Remove event listener from a MIDI port.
 *
 * An object listening to a MIDI port must be removed as listener before being destroyed.
 * Typically this is done in the object's delete method.
 *
 * @fn void fts_midiport_remove_listener(fts_midiport_t *port, enum midi_type type, int chan, int num, fts_object_t *obj)
 * @param port the listened MIDI port
 * @param type type of midi event
 * @param chan the listened MIDI channel (1..16, midi_channel_any for omni) or system event type
 * @param num the listened MIDI note or controller number (0..127, midi_number_any for omni)
 * @param obj the listening object
 *
 * @ingroup midiport_io
 */

FTS_API void fts_midiport_add_listener(fts_midiport_t *port, enum midi_type type, int chan, int num, fts_object_t *obj, fts_method_t fun);
FTS_API void fts_midiport_remove_listener(fts_midiport_t *port, enum midi_type type, int chan, int num, fts_object_t *obj);

/**
 * Output MIDI event via a port.
 *
 * @fn void fts_midiport_output(fts_midiport_t *port, fts_midievent_t *event, double time)
 * @param port the MIDI port
 * @param event the MIDI event
 * @param time an offset time of the incoming MIDI event (in msec) regarding the current logical (tick) time
 *
 * @ingroup midiport_io
 */

FTS_API void fts_midiport_output(fts_midiport_t *port, fts_midievent_t *event, double time);

/*@}*/ /* MIDI port listeners and output function */

/* default midi port */
FTS_API fts_midiport_t *fts_midiport_get_default(void);
FTS_API void fts_midiport_set_default( int argc, const fts_atom_t *argv);
FTS_API void fts_midiport_set_default_class( fts_symbol_t name);

/*****************************************************
 *
 *  MIDI manager
 *
 */

typedef struct fts_midimanager
{
  fts_object_t o;
  struct fts_midimanager *next;
} fts_midimanager_t;

/* MIDI manager messages */
FTS_API fts_symbol_t fts_s_midimanager;
FTS_API fts_symbol_t fts_midimanager_s_get_default_input;
FTS_API fts_symbol_t fts_midimanager_s_get_default_output;
FTS_API fts_symbol_t fts_midimanager_s_append_input_names;
FTS_API fts_symbol_t fts_midimanager_s_append_output_names;
FTS_API fts_symbol_t fts_midimanager_s_get_input;
FTS_API fts_symbol_t fts_midimanager_s_get_output;

/* MIDI objects API */
FTS_API fts_midiport_t *fts_midiconfig_get_input(fts_symbol_t name);
FTS_API fts_midiport_t *fts_midiconfig_get_output(fts_symbol_t name);
FTS_API void fts_midiconfig_add_listener(fts_object_t *obj);
FTS_API void fts_midiconfig_remove_listener(fts_object_t *obj);

/* MIDI manager API */
FTS_API void fts_midiconfig_update(void);
FTS_API void fts_midiconfig_add_manager(fts_midimanager_t *mm);

FTS_API fts_object_t *fts_midiconfig_get(void);
