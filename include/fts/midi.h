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


/****************************************************
 *
 *  MIDI events
 *
 */

/**
 * The MIDI event class.
 *
 * @defgroup midievent MIDI event
 */

enum midi_type
{
  midi_type_any = -1,
  midi_note = 0,
  midi_poly_pressure,
  midi_control_change,
  midi_program_change,
  midi_channel_pressure,
  midi_pitch_bend,
  midi_system_exclusive,
  midi_time_code,
  midi_song_position_pointer,
  midi_song_select,
  midi_tune_request,
  midi_real_time,
  n_midi_types
};

enum midi_real_time_event
{
  midi_real_time_any = -1,
  midi_timing_clock = 0,
  midi_undefined_0,
  midi_start,
  midi_continue,
  midi_stop,
  midi_undefined_1,
  midi_active_sensing,
  midi_system_reset,
  n_midi_real_time_events
};

enum midi_channel 
{
  midi_channel_any = -1,
  midi_channel_1 = 0,
  midi_channel_2,
  midi_channel_3,
  midi_channel_4,
  midi_channel_5,
  midi_channel_6,
  midi_channel_7,
  midi_channel_8,
  midi_channel_9,
  midi_channel_10,
  midi_channel_11,
  midi_channel_12,
  midi_channel_13,
  midi_channel_14,
  midi_channel_15,
  midi_channel_16,
  n_midi_channels
};

enum midi_note_number 
{
  midi_note_any = -1,
  n_midi_notes = 128
};

enum midi_controller_number 
{
  midi_controller_any = -1,
  midi_controller_bank_select_msb = 0,
  midi_controller_modulation_wheel_msb,
  midi_controller_breath_control_msb,
  /* undefined controller 5 */
  midi_controller_foot_controller_msb = 4,
  midi_controller_portamento_time_msb,
  midi_controller_data_entry_msb,
  midi_controller_channel_volume_msb,
  midi_controller_balance_msb,
  /* undefined controller 9 */  
  midi_controller_pan_msb,
  midi_controller_expression_controller_msb,
  midi_controller_effect_control_1_msb,
  midi_controller_effect_control_2_msb,
  /* undefined controllers 14 and 15 */
  midi_controller_general_purpose_1_msb = 16,
  midi_controller_general_purpose_2_msb,
  midi_controller_general_purpose_3_msb,
  midi_controller_general_purpose_4_msb,
  /* undefined controllers 20 to 31 */
  midi_controller_bank_select_lsb = 32,
  midi_controller_modulation_wheel_lsb,
  midi_controller_breath_control_lsb,
  /* undefined controller 35*/  
  midi_controller_foot_controller_lsb,
  midi_controller_portamento_time_lsb,
  midi_controller_data_entry_lsb,
  midi_controller_channel_volume_lsb,
  midi_controller_balance_lsb,
  /* undefined controller 41 */  
  midi_controller_pan_lsb,
  midi_controller_expression_controller_lsb,
  midi_controller_effect_control_1_lsb,
  midi_controller_effect_control_2_lsb,
  /* undefined controllers 46 & 47*/  
  midi_controller_general_purpose_1,
  midi_controller_general_purpose_2,
  midi_controller_general_purpose_3,
  midi_controller_general_purpose_4 ,
  /* undefined controllers 52 to 63 */
  midi_controller_damper_pedal = 64,
  midi_controller_portamento,
  midi_controller_sustenuto,
  midi_controller_soft_pedal,
  midi_controller_legato_footswitch,
  midi_controller_hold,
  midi_controller_sound_variation,
  midi_controller_sound_timbre,
  midi_controller_sound_release_time,
  midi_controller_sound_attack_time,
  midi_controller_sound_brightness,
  midi_controller_sound_decay_time,
  midi_controller_sound_vibrato_rate,
  midi_controller_sound_vibrato_depth,
  midi_controller_sound_vibrato_delay,
  midi_controller_sound_undefined,
  midi_controller_general_purpose_5,
  midi_controller_general_purpose_6,
  midi_controller_general_purpose_7,
  midi_controller_general_purpose_8,
  midi_controller_portamento_control,
  /* undefined controllers 85 to 90 */
  midi_controller_reverb_send_level = 91,
  midi_controller_effects_tremolo_depth,
  midi_controller_chorus_send_level,
  midi_controller_effects_detune_depth,
  midi_controller_effects_phaser_depth,
  midi_controller_data_entry_incr,
  midi_controller_data_entry_decr,
  midi_controller_non_registered_parameter_number_lsb,
  midi_controller_non_registered_parameter_number_msb,
  midi_controller_registered_parameter_number_lsb,
  midi_controller_registered_parameter_number_msb,
  /* undefined controllers 102 to 119 */
  midi_controller_all_sound_off = 120,
  midi_controller_reset_all_controllers,
  midi_controller_local_control,
  midi_controller_all_notes_off,
  midi_controller_omni_mode_off,
  midi_controller_omni_mode_on,
  midi_controller_mono_mode,
  midi_controller_poly_mode,
  n_midi_controllers
};

#define MIDI_EMPTY_BYTE -1

typedef struct fts_midievent
{
  fts_object_t head;
  
  enum midi_type type;

  union {

    struct {
      int channel; /* channel */
      int first; /* first byte */
      int second; /* second byte (optional) */
    } channel_message;

    fts_array_t system_exclusive; /* system exclusive message */

    struct {
      int type;
      int hour;
      int minute;
      int second;
      int frame;
    } time_code;

    int song_position_pointer;
    int song_select;

    enum midi_real_time_event real_time; 
  } data;

} fts_midievent_t;

#define fts_midievent_get_type(e) ((e)->type)
#define fts_midievent_set_type(e, x) ((e)->type = (x))

/* channel events */
#define fts_midievent_is_channel_message(e) ((e)->type <= midi_pitch_bend)
#define fts_midievent_is_note(e) ((e)->type == midi_note)
#define fts_midievent_is_poly_pressure(e) ((e)->type == midi_poly_pressure)
#define fts_midievent_is_control_change(e) ((e)->type == midi_control_change)
#define fts_midievent_is_program_change(e) ((e)->type == midi_program_change)
#define fts_midievent_is_channel_pressure(e) ((e)->type == midi_channel_pressure)
#define fts_midievent_is_pitch_bend(e) ((e)->type == midi_pitch_bend)

#define fts_midievent_channel_message_get_channel(e) ((e)->data.channel_message.channel)
#define fts_midievent_channel_message_get_first(e) ((e)->data.channel_message.first)
#define fts_midievent_channel_message_get_second(e) ((e)->data.channel_message.second)

#define fts_midievent_channel_message_set_channel(e, x) ((e)->data.channel_message.channel = (x))
#define fts_midievent_channel_message_set_first(e, x) ((e)->data.channel_message.first = (x))
#define fts_midievent_channel_message_set_second(e, x) ((e)->data.channel_message.second = (x))

#define fts_midievent_channel_message_has_second_byte(e)((e)->data.channel_message.second != MIDI_EMPTY_BYTE)
#define fts_midievent_channel_message_get_status_byte(e) (144 + ((e)->type << 4) + (e)->data.channel_message.channel)

FTS_API fts_midievent_t *fts_midievent_channel_message_new(enum midi_type type, int channel, int byte1, int byte2);
FTS_API fts_midievent_t *fts_midievent_note_new(int channel, int note, int velocity);
FTS_API fts_midievent_t *fts_midievent_poly_pressure_new(int channel, int note, int value);
FTS_API fts_midievent_t *fts_midievent_control_change_new(int channel, int number, int value);
FTS_API fts_midievent_t *fts_midievent_program_change_new(int channel, int number);
FTS_API fts_midievent_t *fts_midievent_channel_pressure_new(int channel, int value);
FTS_API fts_midievent_t *fts_midievent_pitch_bend_new(int channel, int LSB, int MSB);

/* system exclusive events */
#define fts_midievent_is_system_exclusive(e) ((e)->type == midi_system_exclusive)

#define fts_midievent_system_exclusive_get_size(e) (fts_array_get_size(&(e)->data.system_exclusive))
#define fts_midievent_system_exclusive_get_atoms(e) (fts_array_get_atoms(&(e)->data.system_exclusive))

FTS_API fts_midievent_t *fts_midievent_system_exclusive_new(int ac, const fts_atom_t *at);
FTS_API void fts_midievent_system_exclusive_append(fts_midievent_t *event, int byte);

/* time code events */
#define fts_midievent_is_time_code(e) ((e)->type == midi_time_code)

#define fts_midievent_time_code_get_type(e) ((e)->data.time_code.type)
#define fts_midievent_time_code_get_hour(e) ((e)->data.time_code.hour)
#define fts_midievent_time_code_get_minute(e) ((e)->data.time_code.minute)
#define fts_midievent_time_code_get_second(e) ((e)->data.time_code.second)
#define fts_midievent_time_code_get_frame(e) ((e)->data.time_code.frame)

#define fts_midievent_time_code_set_type(e, x) ((e)->data.time_code.type = (x))
#define fts_midievent_time_code_set_hour(e, x) ((e)->data.time_code.hour = (x))
#define fts_midievent_time_code_set_minute(e, x) ((e)->data.time_code.minute = (x))
#define fts_midievent_time_code_set_second(e, x) ((e)->data.time_code.second = (x))
#define fts_midievent_time_code_set_frame(e, x) ((e)->data.time_code.frame = (x))

#define fts_midievent_time_code_get_time(e) (\
  ((double)((e)->data.time_code.hour * 60 + (e)->data.time_code.minute) * 60 + (e)->data.time_code.second) * 1000.0 + \
  ((e)->data.time_code.type == 0? (1000.0 / 24.0): ((e)->data.time_code.type == 1 ? (1000.0 / 25.0) : (1000.0 / 30.0))) * \
  (double)((e)->data.time_code.frame + 2) \
)

FTS_API fts_midievent_t *fts_midievent_time_code_new(int type, int hour, int minute, int second, int frame);

/* system real-time events */
#define fts_midievent_is_real_time(e) ((e)->type == midi_real_time)
#define fts_midievent_real_time_get(e) ((e)->data.real_time)
#define fts_midievent_real_time_set(e, x) ((e)->data.real_time = (x))
#define fts_midievent_real_time_get_status_byte(e) (248 + (e)->data.real_time)

FTS_API fts_midievent_t *fts_midievent_real_time_new(enum midi_real_time_event tag);

/* other system events */
#define fts_midievent_song_position_pointer_get(e) ((e)->data.song_position_pointer)
#define fts_midievent_song_position_pointer_set(e, x) ((e)->data.song_position_pointer = (x))
#define fts_midievent_song_select_get(e) ((e)->data.song_select)
#define fts_midievent_song_select_set(e, x) ((e)->data.song_select = (x))

FTS_API fts_metaclass_t *fts_midievent_type;
FTS_API fts_symbol_t fts_s_midievent;

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
 * @param port the MIDI port itself
 * @param event the incoming event
 * @param time an offset time of the incoming MIDI event (in msec) regarding the current logical (tick) time
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
 * An object who wants to listen to or output via a MIDI port should check if the object it referes to implements the MIDI port abstraction
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
 * @fn void fts_midiport_add_listener(fts_midiport_t *port, enum midi_type type, int id, int number, fts_object_t *obj, fts_method_t fun)
 * @param port the listened MIDI port
 * @param type type of midi event
 * @param id the listened MIDI channel (1..16, midi_channel_any for omni) or system event type
 * @param number the listened MIDI note or controller number (0..127, midi_number_any for omni)
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
 * @param id the listened MIDI channel (1..16, midi_channel_any for omni) or system event type
 * @param number the listened MIDI note or controller number (0..127, midi_number_any for omni)
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
 *  MIDI Parser
 *
 */
typedef struct _fts_midiparser_
{
  fts_midievent_t *event;

  enum midiparser_status 
  {
    midiparser_status_reset = 0,
    midiparser_status_note_off,
    midiparser_status_note_on,
    midiparser_status_poly_pressure,
    midiparser_status_control_change,
    midiparser_status_program_change,
    midiparser_status_channel_pressure,
    midiparser_status_pitch_bend,
    midiparser_status_real_time,
    midiparser_status_system_exclusive, 
    midiparser_status_system_exclusive_byte, 
    midiparser_status_system_exclusive_realtime,
    midiparser_status_system_exclusive_full_frame, 
    midiparser_status_quarter_frame,
    midiparser_status_song_position_pointer,
    midiparser_status_song_select
  } status;

  int channel;	
  int store;
  fts_array_t system_exclusive;

  enum parser_mtc_status
  {
    mtc_status_ready,
    mtc_status_forward,
    mtc_status_backward
  } mtc_status;
  
  int mtc_frame_count;
  int mtc_type;
  int mtc_hour;
  int mtc_minute;
  int mtc_second;
  int mtc_frame;

} fts_midiparser_t;

FTS_API void fts_midiparser_init(fts_midiparser_t *parser);
FTS_API void fts_midiparser_reset(fts_midiparser_t *parser);
FTS_API void fts_midiparser_set_event(fts_midiparser_t *parser, fts_midievent_t *event);
FTS_API fts_midievent_t *fts_midiparser_byte(fts_midiparser_t *parser, unsigned char byte);

/*****************************************************
 *
 *  MIDI manager
 *
 */

typedef struct _midilabel midilabel_t;

typedef struct fts_midimanager
{
  fts_object_t o;
  midilabel_t *labels;
  int n_labels;
} fts_midimanager_t;

/* MIDI manager messages */
FTS_API fts_symbol_t fts_s_midimanager;
FTS_API fts_symbol_t fts_midimanager_s_get_default_devices;
FTS_API fts_symbol_t fts_midimanager_s_append_device_names;
FTS_API fts_symbol_t fts_midimanager_s_get_input;
FTS_API fts_symbol_t fts_midimanager_s_get_output;

/* MIDI objects API */
FTS_API fts_midiport_t *fts_midimanager_get_input(fts_symbol_t name);
FTS_API fts_midiport_t *fts_midimanager_get_output(fts_symbol_t name);
FTS_API void fts_midimanger_register(fts_object_t *obj);

/* MIDI manager API */
FTS_API void fts_midimanager_update(fts_midimanager_t *mm);
FTS_API void fts_midimanager_class_init(fts_class_t *class);
FTS_API void fts_midimanager_set(fts_midimanager_t *mm);
FTS_API fts_midimanager_t *fts_midimanager_get(void);

/*****************************************************
 *
 *  MIDI fifo
 *
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

#define fts_midififo_get_events(f) ((fts_midififo_entry_t **)((f)->data.buffer))

FTS_API void fts_midififo_init(fts_midififo_t *fifo, int size);
FTS_API void fts_midififo_destroy(fts_midififo_t *fifo);
FTS_API void fts_midififo_poll(fts_midififo_t *fifo);
FTS_API fts_midievent_t *fts_midififo_get_event(fts_midififo_t *fifo);
FTS_API void fts_midififo_write(fts_midififo_t *fifo, fts_object_t *port, double time);


