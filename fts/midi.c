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

#include <fts/fts.h>
#include <ftsprivate/variable.h>
#include <stdlib.h>
#include <string.h>

/*
 * This files contains all things MIDI related:
 *
 *  - MIDI events
 *  - MIDI parser 
 *  - MIDI port
 *  - initialization of the MIDI module
 *
 * MIDI files are handled in midifile.c
 */

/************************************************************
 *
 *  MIDI events
 *
 */

fts_metaclass_t *fts_midievent_type = 0;
fts_symbol_t fts_s_midievent = 0;

static void
fts_midievent_channel_message_init(fts_midievent_t *event, enum midi_type type, int channel, int byte1, int byte2)
{
  event->type = type;
  event->data.channel_message.channel = channel;
  event->data.channel_message.first = byte1;
  event->data.channel_message.second = byte2;
}

fts_midievent_t *
fts_midievent_channel_message_new(enum midi_type type, int channel, int byte1, int byte2)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_object_create(fts_midievent_type, 0, 0);
  fts_midievent_channel_message_init(event, type, channel, byte1, byte2);
  return event;
}

fts_midievent_t *
fts_midievent_note_new(int channel, int note, int velocity)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_object_create(fts_midievent_type, 0, 0);
  fts_midievent_channel_message_init(event, midi_note, channel, note, velocity);
  return event;
}

fts_midievent_t *
fts_midievent_poly_pressure_new(int channel, int note, int value)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_object_create(fts_midievent_type, 0, 0);
  fts_midievent_channel_message_init(event, midi_poly_pressure, channel, note, value);
  return event;
}

fts_midievent_t *
fts_midievent_control_change_new(int channel, int number, int value)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_object_create(fts_midievent_type, 0, 0);
  fts_midievent_channel_message_init(event, midi_control_change, channel, number, value);
  return event;
}

fts_midievent_t *
fts_midievent_program_change_new(int channel, int number)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_object_create(fts_midievent_type, 0, 0);
  fts_midievent_channel_message_init(event, midi_program_change, channel, number, MIDI_EMPTY_BYTE);
  return event;
}

fts_midievent_t *
fts_midievent_channel_pressure_new(int channel, int value)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_object_create(fts_midievent_type, 0, 0);
  fts_midievent_channel_message_init(event, midi_channel_pressure, channel, value, MIDI_EMPTY_BYTE);
  return event;
}

fts_midievent_t *
fts_midievent_pitch_bend_new(int channel, int LSB, int MSB)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_object_create(fts_midievent_type, 0, 0);
  fts_midievent_channel_message_init(event, midi_pitch_bend, channel, LSB, MSB);
  return event;
}

static fts_midievent_t *
fts_midievent_system_exclusive_init(fts_midievent_t *event, int ac, const fts_atom_t *at)
{
  event->type = midi_system_exclusive;
  fts_array_init(&event->data.system_exclusive, ac, at);
  return event;
}

fts_midievent_t *
fts_midievent_system_exclusive_new(int ac, const fts_atom_t *at)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_object_create(fts_midievent_type, 0, 0);
  fts_midievent_system_exclusive_init(event, ac, at);
  return event;
}

void
fts_midievent_system_exclusive_append(fts_midievent_t *event, int byte)
{
  fts_atom_t a;

  fts_set_int(&a, byte & 0x7F);
  fts_array_append(&event->data.system_exclusive, 1, &a);
}

static void
fts_midievent_time_code_init(fts_midievent_t *event, int type, int hour, int minute, int second, int frame)
{
  event->type = midi_time_code;
  event->data.time_code.type = type;
  event->data.time_code.hour = hour;
  event->data.time_code.minute = minute;
  event->data.time_code.second = second;
  event->data.time_code.frame = frame;
}

fts_midievent_t *
fts_midievent_time_code_new(int type, int hour, int minute, int second, int frame)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_object_create(fts_midievent_type, 0, 0);
  fts_midievent_time_code_init(event, type, hour, minute, second, frame);
  return event;
}

static void
fts_midievent_real_time_init(fts_midievent_t *event, enum midi_real_time_event tag)
{
  event->type = midi_real_time;
  event->data.real_time = tag;
}

fts_midievent_t *
fts_midievent_real_time_new(enum midi_real_time_event tag)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_object_create(fts_midievent_type, 0, 0);
  fts_midievent_real_time_init(event, tag);
  return event;
}

/**************************************************************
 *
 *  midievent class
 *
 */

static void
midievent_set_from_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midievent_t *this = (fts_midievent_t *)o;

  if(ac > 0)
    {
      int type = fts_get_int(at + 0);
      
      if(fts_midievent_is_system_exclusive(this))
	fts_array_destroy(&this->data.system_exclusive);
      
      /* set type */
      fts_midievent_set_type(this, type);

      /* set event data */
      if(fts_midievent_is_channel_message(this) <= midi_system_exclusive)
	{
	  switch(ac)
	    {
	    default:
	    case 3:
	      if(fts_is_int(at + 3))
		fts_midievent_channel_message_set_channel(this, fts_get_int(at + 3) - 1);
	    case 2:	
	      if(fts_is_int(at + 2))
		fts_midievent_channel_message_set_second(this, fts_get_int(at + 2));
	    case 1:
	      if(fts_is_int(at + 1))
		fts_midievent_channel_message_set_first(this, fts_get_int(at + 1));	
	    case 0:
	      break;
	    }
	}
      else if(fts_midievent_is_system_exclusive(this))
	fts_array_init(&this->data.system_exclusive, ac, at);
      else if(fts_midievent_is_time_code(this))
	{
	  switch(ac)
	    {
	    default:
	    case 5:
	      if(fts_is_int(at + 5))
		fts_midievent_time_code_set_frame(this, fts_get_int(at + 5));
	    case 4:
	      if(fts_is_int(at + 4))
		fts_midievent_time_code_set_second(this, fts_get_int(at + 4));
	    case 3:
	      if(fts_is_int(at + 3))
		fts_midievent_time_code_set_minute(this, fts_get_int(at + 3));
	    case 2:
	      if(fts_is_int(at + 2))
		fts_midievent_time_code_set_hour(this, fts_get_int(at + 2));
	    case 1:
	      if(fts_is_int(at + 1))
		fts_midievent_time_code_set_type(this, fts_get_int(at + 1));
	    case 0:
	      break;
	    }
	}
      else if(fts_midievent_is_real_time(this))
	if(fts_is_int(at))
	  fts_midievent_real_time_set(this, fts_get_int(at));
    }
}
  
static void
midievent_get_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midievent_t *this = (fts_midievent_t *)o;
  fts_array_t *array = (fts_array_t *)fts_get_pointer(at);

  fts_array_append_int(array, fts_midievent_get_type(this));

  /* append event data */
  if(fts_midievent_is_channel_message(this))
    {
      fts_array_append_int(array, fts_midievent_channel_message_get_first(this));
	  
      if(fts_midievent_channel_message_has_second_byte(this))
	fts_array_append_int(array, fts_midievent_channel_message_get_second(this));

      fts_array_append_int(array, fts_midievent_channel_message_get_channel(this) + 1);
    }
  else if(fts_midievent_is_system_exclusive(this))
    {
      int size = fts_midievent_system_exclusive_get_size(this);
      fts_atom_t *atoms = fts_midievent_system_exclusive_get_atoms(this);
	  
      fts_array_append(array, size, atoms);
    }
  else if(fts_midievent_is_time_code(this))
    {
      fts_array_append_int(array, fts_midievent_time_code_get_type(this));
      fts_array_append_int(array, fts_midievent_time_code_get_hour(this));
      fts_array_append_int(array, fts_midievent_time_code_get_minute(this));
      fts_array_append_int(array, fts_midievent_time_code_get_second(this));
      fts_array_append_int(array, fts_midievent_time_code_get_frame(this));
    }
  else if(fts_midievent_is_real_time(this))
    fts_array_append_int(array, fts_midievent_real_time_get(this));
}
  
static void
midievent_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midievent_t *this = (fts_midievent_t *)o;

  ac--;
  at++;

  /* set type */
  if(ac > 0 && fts_is_int(at))
    this->type = fts_get_int(at);
  else
    this->type = midi_type_any; /* unvalid empty event type */
}

static void
midievent_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midievent_t *this = (fts_midievent_t *)o;
  int type = fts_midievent_get_type(this);

  switch (type)
    {
    case midi_note:
      post("{<note> %d %d %d}\n",
	   fts_midievent_channel_message_get_first(this), 
	   fts_midievent_channel_message_get_second(this),
	   fts_midievent_channel_message_get_channel(this) + 1);
      break;
      
    case midi_poly_pressure:
      post("{<poly pressure> %d %d %d}\n",
	   fts_midievent_channel_message_get_first(this), 
	   fts_midievent_channel_message_get_second(this),
	   fts_midievent_channel_message_get_channel(this) + 1);
      break;
      
    case midi_control_change:
      post("{<control change> %d %d %d}\n",
	   fts_midievent_channel_message_get_first(this), 
	   fts_midievent_channel_message_get_second(this),
	   fts_midievent_channel_message_get_channel(this) + 1);
      break;
      
    case midi_program_change:
      post("{<program change> %d %d}\n", 
	   fts_midievent_channel_message_get_first(this), 
	   fts_midievent_channel_message_get_channel(this) + 1);
      break;
      
    case midi_channel_pressure:		
      post("{<channel pressure> %d %d}\n", 
	   fts_midievent_channel_message_get_first(this), 
	   fts_midievent_channel_message_get_channel(this) + 1);
      break;
      
    case midi_pitch_bend:
      post("{<pitch bend> %d %d %d}\n",
	   fts_midievent_channel_message_get_first(this), 
	   fts_midievent_channel_message_get_second(this),
	   fts_midievent_channel_message_get_channel(this) + 1);
      break;
      
    case midi_system_exclusive:
	post("{<system exclusive message> ");
	post_atoms(fts_midievent_system_exclusive_get_size(this), fts_midievent_system_exclusive_get_atoms(this));
	post("}\n");
      break;
      
    case midi_time_code:
      post("{<time code (%d)> %d %d %d %d}\n",
	   fts_midievent_time_code_get_type(this),
	   fts_midievent_time_code_get_hour(this),
	   fts_midievent_time_code_get_minute(this),
	   fts_midievent_time_code_get_second(this),
	   fts_midievent_time_code_get_frame(this));
      break;
      
    case midi_song_position_pointer:
      break;
      
    case midi_song_select:
      break;
      
    case midi_tune_request:
      break;
      
    case midi_real_time:
      {
	int mess = fts_midievent_real_time_get(this);
	
	switch(mess)
	  {
	  case midi_timing_clock:
	    post("{<timing clock tick>}\n");
	    break;
	    
	  case midi_undefined_0:
	    post("{<undefined real-time message>}\n");
	    break;
	    
	  case midi_start:
	    post("{<start>}\n");
	    break;
	    
	  case midi_continue:
	    post("{<continue>}\n");
	    break;
	    
	  case midi_stop:
	    post("{<stop>}\n");
	    break;
	    
	  case midi_undefined_1:
	    post("{<undefined real-time message>}\n");
	    break;
	    
	  case midi_active_sensing:
	    post("{<active sensing>}\n");
	    break;
	    
	  case midi_system_reset:
	    post("{<reset>}\n");
	  }
      }
    }
}

static void
midievent_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midievent_t *this = (fts_midievent_t *)o;
  
  if(fts_midievent_is_system_exclusive(this))
    fts_array_destroy(&this->data.system_exclusive);
}

static fts_status_t
midievent_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_midievent_t), 0, 0, 0); 
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midievent_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, midievent_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_print, midievent_print);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_get_array, midievent_get_array);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set_from_array, midievent_set_from_array);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, midievent_set_from_array);

  return fts_Success;
}

void
midievent_config(void)
{
  fts_midievent_type = fts_class_install(fts_s_midievent, midievent_instantiate);
}

/************************************************************
 *
 *  MIDI parser
 *
 */

enum midi_status 
{
  midi_status_note_off = 128,
  midi_status_note = 144,
  midi_status_poly_pressure = 160,
  midi_status_control_change = 176,
  midi_status_program_change = 192,
  midi_status_channel_pressure = 208,
  midi_status_pitch_bend = 224,
  midi_status_system_exclusive = 240,
  midi_status_quarter_frame,
  midi_status_song_position_pointer,
  midi_status_song_select,
  midi_status_undefined_0,
  midi_status_undefined_1,
  midi_status_tune_request,
  midi_status_system_exclusive_end,
  midi_status_timing_clock,
  midi_status_undefined_2,
  midi_status_start,
  midi_status_continue,
  midi_status_stop,
  midi_status_undefined_3,
  midi_status_active_sensing,
  midi_status_system_reset
};

#define MIDI_SYSTEM_EXCLUSIVE_NON_REALTIME 0x7e 
#define MIDI_SYSTEM_EXCLUSIVE_REALTIME 0x7f

void
fts_midiparser_init(fts_midiparser_t *parser)
{  
  parser->event = NULL;

  parser->status = midiparser_status_reset;

  parser->channel = 0;	
  parser->store = MIDI_EMPTY_BYTE;

  fts_array_init(&parser->system_exclusive, 0, 0);

  parser->mtc_status = mtc_status_ready;
  parser->mtc_frame_count = 99;
  parser->mtc_type = 0;
  parser->mtc_hour = 0;
  parser->mtc_minute = 0;
  parser->mtc_second = 0;
  parser->mtc_frame = 0;
}

void
fts_midiparser_reset(fts_midiparser_t *parser)
{
  fts_array_destroy(&parser->system_exclusive);

  if(parser->event != NULL)
    fts_object_release(parser->event);
}

static fts_midievent_t *
midiparser_get_event(fts_midiparser_t *parser)
{
  if(parser->event == NULL)
    parser->event = (fts_midievent_t *)fts_object_create(fts_midievent_type, 0, 0);
  
  return parser->event;
}

fts_midievent_t *
fts_midiparser_byte(fts_midiparser_t *parser, unsigned char byte)
{
  fts_midiport_t *port = (fts_midiport_t *)parser;
  fts_midievent_t *event = midiparser_get_event(parser);

  if(byte >= midi_status_timing_clock)
    {
      /* system real-time messages */
      fts_midievent_real_time_init(event, (int)byte - midi_status_timing_clock);
      parser->event = NULL;
      return event;
    }
  else if(byte >= midi_status_system_exclusive)
    {
      /* system common messages */
      switch (byte)
	{
	case midi_status_system_exclusive:
	  parser->status = midiparser_status_system_exclusive;
	  break;
	  
	case midi_status_quarter_frame:
	  parser->status = midiparser_status_quarter_frame;
	  break;
	  
	case midi_status_song_position_pointer:
	  parser->status = midiparser_status_song_position_pointer;
	  break;
	  
	case midi_status_song_select:
	  parser->status = midiparser_status_song_select;
	  break;

	case midi_status_tune_request:
	  break;

	case midi_status_system_exclusive_end:
          /* send sysex */
          if(fts_array_get_size(&parser->system_exclusive) > 0)
            {
              fts_midievent_system_exclusive_init(event, fts_array_get_size(&parser->system_exclusive), fts_array_get_atoms(&parser->system_exclusive));
              parser->event = NULL;
              return event;
            }

          parser->status = midiparser_status_reset;
	  break;
	  
	default:
	  break;
	}
    }
  else if(byte > 127)
    {
      /* channel message status byte */
      parser->status = (byte >> 4) - 7;
      parser->channel = (byte & 0xf);
      parser->store = MIDI_EMPTY_BYTE;
    }
  else
    {	      
      /* channel message data bytes */
      switch (parser->status)
	{
	case midiparser_status_note_off:
	  
	  if (parser->store == MIDI_EMPTY_BYTE)
	    parser->store = byte;
	  else
	    {
	      fts_midievent_channel_message_init(event, midi_note, parser->channel, parser->store, 0);
              parser->store = MIDI_EMPTY_BYTE;  /* reset for running status */
              parser->event = NULL;
              return event;
	    }
	  
	  break;
	  
	case midiparser_status_note_on:
	  
	  if (parser->store == MIDI_EMPTY_BYTE)
	    parser->store = byte;
	  else
	    {
	      fts_midievent_channel_message_init(event, midi_note, parser->channel, parser->store, byte);
              parser->store = MIDI_EMPTY_BYTE; /* reset for running status */
              parser->event = NULL;
              return event;
	    }
	  
	  break;
	  
	case midiparser_status_poly_pressure:
	  
	  if (parser->store == MIDI_EMPTY_BYTE)
	    parser->store = byte;
	  else
	    {
	      fts_midievent_channel_message_init(event, midi_poly_pressure, parser->channel, parser->store, byte);
              parser->store = MIDI_EMPTY_BYTE; /* reset for running status */
              parser->event = NULL;
              return event;
	    }
	  break;
		      
	case midiparser_status_control_change:
		      
	  if (parser->store == MIDI_EMPTY_BYTE)
	    parser->store = byte;
	  else
	    {
	      fts_midievent_channel_message_init(event, midi_control_change, parser->channel, parser->store, byte);
              parser->store = MIDI_EMPTY_BYTE; /* reset for running status */
              parser->event = NULL;
              return event;
            }
	  break;
		      
	case midiparser_status_program_change:
	  {
	    fts_midievent_channel_message_init(event, midi_program_change, parser->channel, byte, MIDI_EMPTY_BYTE);
            parser->event = NULL;
            return event;
	  }
	  
	  break;
		      
	case midiparser_status_channel_pressure:		
	  {
	    fts_midievent_channel_message_init(event, midi_channel_pressure, parser->channel, byte, MIDI_EMPTY_BYTE);
            parser->event = NULL;
            return event;
          }
		      
	  break;
		      
	case midiparser_status_pitch_bend:
		      
	  if (parser->store == MIDI_EMPTY_BYTE)
	    parser->store = byte;
	  else
	    {
	      fts_midievent_channel_message_init(event, midi_pitch_bend, parser->channel, parser->store, byte);
              parser->store = MIDI_EMPTY_BYTE; /* reset for running status */
              parser->event = NULL;
              return event;
	    }
	  break;

	case midiparser_status_system_exclusive:
	  
	  switch (byte)
	    {
	    case MIDI_SYSTEM_EXCLUSIVE_REALTIME:
	      parser->status = midiparser_status_system_exclusive_realtime;
	      break;
	      
	    default:
	      /* start sysex block with vendor or sysex non real-time id */
              fts_array_clear(&parser->system_exclusive);
              fts_array_append_int(&parser->system_exclusive, byte & 0x7f);
	      parser->status = midiparser_status_system_exclusive_byte;
	    }
	  
	  break;
	  
	case midiparser_status_system_exclusive_realtime:
	  
	  switch (byte)
	    {
	    case 0x01:
	      /* start MTC full frame block */
	      parser->status = midiparser_status_system_exclusive_full_frame;
	      parser->mtc_frame_count = -1;
	      break;
	      
	    default:
	      /* sysex real-time id and byte */
              fts_array_clear(&parser->system_exclusive);
	      fts_array_append_int(&parser->system_exclusive, (int)MIDI_SYSTEM_EXCLUSIVE_REALTIME);
	      fts_array_append_int(&parser->system_exclusive, (int)byte);
	      parser->status = midiparser_status_system_exclusive_byte;
	      break;
	    }
	  
	  break;
	  
	case midiparser_status_system_exclusive_byte:
	  /* ordinary sysex byte */
	  fts_array_append_int(&parser->system_exclusive, (int)byte);
	  break;
	  
	case midiparser_status_system_exclusive_full_frame:
	  
	  /* handle MTC full frame */
	  switch (parser->mtc_frame_count)
	    {
	    case -1:
	      if(byte == 0x01)
		parser->mtc_frame_count = 0;
	      else
		{
		  /* oops was ordinary sysex message starting with 0x7f 0x01 */
                  fts_array_clear(&parser->system_exclusive);
		  fts_array_append_int(&parser->system_exclusive, (int)MIDI_SYSTEM_EXCLUSIVE_REALTIME);
		  fts_array_append_int(&parser->system_exclusive, (int)0x01);
		  fts_array_append_int(&parser->system_exclusive, (int)byte);

		  parser->status = midiparser_status_system_exclusive_byte;
		}
	      break;
	    case 0:
	      parser->mtc_type = (byte & 0x60) >> 5;
	      parser->mtc_hour = (byte & 0x1f);
	      break;
	    case 1:
	      parser->mtc_minute = byte;
	      break;
	    case 2:
	      parser->mtc_second = byte;
	      break;
	    case 3:
	      parser->mtc_frame = byte;
	      break;
	    default:
	      break;
	    }
	  
	  parser->mtc_frame_count++;
	  
	  break;
	  
	case midiparser_status_quarter_frame:
	  {
	    switch ((byte & 0xf0) >> 4)
	      {
	      case 0:
		parser->mtc_frame = (parser->mtc_frame & 0xf0) | (byte & 0x0f);

		if(parser->mtc_status == mtc_status_backward)
		  {
                    fts_midievent_time_code_init(event, parser->mtc_type, parser->mtc_hour, parser->mtc_minute, parser->mtc_second, parser->mtc_frame);
                    parser->mtc_status = mtc_status_ready;
                    parser->event = NULL;
                    return event;
		  }
		else
		  parser->mtc_status = mtc_status_forward;

		break;
	      case 1:
		parser->mtc_frame = (parser->mtc_frame & 0x0f) | (byte & 0x0f) << 4;
		break;
	      case 2:
		parser->mtc_second = (parser->mtc_second & 0xf0) | (byte & 0x0f);
		break;
	      case 3:
		parser->mtc_second = (parser->mtc_second & 0x0f) | (byte & 0x0f) << 4;
		break;
	      case 4:
		parser->mtc_minute = (parser->mtc_minute & 0xf0) | (byte & 0x0f);
		break;
	      case 5:
		parser->mtc_minute = (parser->mtc_minute & 0x0f) | (byte & 0x0f) << 4;
		break;
	      case 6:
		parser->mtc_hour = (parser->mtc_hour & 0xf0) | (byte & 0x0f);
		break;
	      case 7:
		parser->mtc_hour = (parser->mtc_hour & 0x0f) | (byte & 0x01) << 4;
		parser->mtc_type = (byte & 0x60) >> 5;

		if(parser->mtc_status == mtc_status_forward)
		  {
                    fts_midievent_time_code_init(event, parser->mtc_type, parser->mtc_hour, parser->mtc_minute, parser->mtc_second, parser->mtc_frame);
                    parser->mtc_status = mtc_status_ready;
                    parser->event = NULL;
                    return event;
		  }
		else
		  parser->mtc_status = mtc_status_backward;

		break;

	      default:
		break;
	      }
	    
	    parser->status = midiparser_status_reset;
	  }
	  
	  break;
	  
	default:
	  break;
	}
    }

  return NULL;
}

/****************************************************
 *
 *  MIDI port
 *
 */

static fts_symbol_t fts_s_midiport = 0;

/* MIDI port listeners */
static void 
add_listener(fts_midiport_listener_t **list, fts_object_t *object, fts_method_t fun)
{
  fts_midiport_listener_t *l = (fts_midiport_listener_t *)fts_malloc(sizeof(fts_midiport_listener_t));

  l->callback = fun;
  l->listener = object;
  l->next = *list;
  
  *list = l;
}

static void 
remove_listener(fts_midiport_listener_t **list, fts_object_t *o)
{
  fts_midiport_listener_t *l = *list;

  if(l)
    {
      fts_midiport_listener_t *freeme = 0;
      
      if(l->listener == o)
	{
	  freeme = l;
	  *list = l->next;
	}
      else
	{
	  while(l->next)
	    {
	      if(l->next->listener == o)
		{
		  freeme = l->next;
		  l->next = l->next->next;
		  
		  break;
		}
	      
	      l = l->next;
	    }
	}
      
      /* free removed listener */
      if(freeme)
	fts_free(freeme);
    }
}

void 
fts_midiport_add_listener(fts_midiport_t *port, enum midi_type type, int id, int number, fts_object_t *obj, fts_method_t fun)
{
  fts_midiport_listener_t **type_list = port->listeners[type + 1]; 
  fts_midiport_listener_t **list = type_list + (number + 1) * (n_midi_channels + 1) + (id + 1);

  add_listener(list, obj, fun);
}

void
fts_midiport_remove_listener(fts_midiport_t *port, enum midi_type type, int id, int number, fts_object_t *obj)
{
  fts_midiport_listener_t **type_list = port->listeners[type + 1]; 
  fts_midiport_listener_t **list = type_list + (number + 1) * (n_midi_channels + 1) + (id + 1);

  remove_listener(list, obj);
}

void
fts_midiport_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiport_t *port = (fts_midiport_t *)o;
  fts_midievent_t *event = (fts_midievent_t *)fts_get_object(at);
  int type = fts_midievent_get_type(event);
  fts_midiport_listener_t **type_list = port->listeners[type + 1]; 
  fts_midiport_listener_t *l;

  if(type <= midi_control_change)
    {
      /* note, poly pressure, control_change */
      int channel = fts_midievent_channel_message_get_channel(event);
      int number = fts_midievent_channel_message_get_first(event); /* note or controller number */
      int onset = (number + 1) * (n_midi_channels + 1);

      /* fire callbacks for given channel and number */
      l = type_list[onset + channel + 1];
      while(l)
	{
	  l->callback(l->listener, 0, 0, 1, at);
	  l = l->next;
	}

      /* fire callbacks for given channel and any number */
      l = type_list[channel + 1];
      while(l)
	{
	  l->callback(l->listener, 0, 0, 1, at);
	  l = l->next;
	}

      /* fire callbacks for any channel and given number*/
      l = type_list[onset];
      while(l)
	{
	  l->callback(l->listener, 0, 0, 1, at);
	  l = l->next;
	}
    }

  /* fire callbacks for given event type */
  l = type_list[0];
  while(l)
    {
      l->callback(l->listener, 0, 0, 1, at);
      l = l->next;
    }

  /* fire callbacks for any event */
  l = *(port->listeners[0]);
  while(l)
    {
      l->callback(l->listener, 0, 0, 1, at);
      l = l->next;
    }
}

void 
fts_midiport_output(fts_midiport_t *port, fts_midievent_t *event, double time)
{
  if(port->output != NULL)
    {
      fts_object_refer((fts_object_t *)event);

      port->output((fts_object_t *)port, event, time);

      fts_object_release((fts_object_t *)event);
    }
}

void
fts_midiport_init(fts_midiport_t *port)
{
  int type;

  for(type=midi_type_any; type<n_midi_types; type++)
    port->listeners[type + 1] = NULL;

  port->output = NULL;
}

void
fts_midiport_reset(fts_midiport_t *port)
{
  /* free listeners */
  if(port->listeners[midi_type_any])
    {
    int type;

    for(type=midi_type_any; type<n_midi_types; type++)
      fts_free(port->listeners[type + 1]);
    }
}

void
fts_midiport_set_input(fts_midiport_t *port)
{
  enum midi_type type;

  for(type=midi_type_any; type<n_midi_types; type++)
    {
    switch(type)
      {
      case midi_type_any:
        {
          fts_midiport_listener_t **any_list;

          any_list = (fts_midiport_listener_t **)fts_malloc(sizeof(fts_midiport_listener_t *));
          *any_list = 0;

          port->listeners[type + 1] = any_list;
        }
        break;

      case midi_note:
      case midi_poly_pressure:
      case midi_control_change:
        {
          /* channel and poly listeners */
          fts_midiport_listener_t **poly_list;
          int size = (n_midi_channels + 1) * (n_midi_controllers + 1);
          int i;

          poly_list = (fts_midiport_listener_t **)fts_malloc(sizeof(fts_midiport_listener_t *) * size);

          for(i=0; i<size; i++)
            poly_list[i] = 0;

          port->listeners[type + 1] = poly_list;
        }
        break;

      case midi_program_change:
      case midi_channel_pressure:
      case midi_pitch_bend:
        {
          /* channel listeners */
          fts_midiport_listener_t **chan_list;
          int i;

          chan_list = (fts_midiport_listener_t **)fts_malloc(sizeof(fts_midiport_listener_t *) * (n_midi_channels + 1));

          for(i=0; i<n_midi_channels; i++)
            chan_list[i] = 0;

          port->listeners[type + 1] = chan_list;
        }
        break;

      default:
        {
          /* system message listeners */
          fts_midiport_listener_t **sys_list;

          sys_list = (fts_midiport_listener_t **)fts_malloc(sizeof(fts_midiport_listener_t *));
          *sys_list = 0;

          port->listeners[type + 1] = sys_list;
        }
        break;
      }
    }
}

void
fts_midiport_set_output(fts_midiport_t *port, fts_midiport_output_t function)
{
  port->output = function;
}

void
fts_midiport_class_init(fts_class_t *cl)
{
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_midievent, fts_midiport_input);
}

int
fts_object_is_midiport(fts_object_t *obj)
{
  return (fts_class_get_method(fts_object_get_class(obj), fts_SystemInlet, fts_s_midievent) == fts_midiport_input);
}

/****************************************************
*
*  internal MIDI port class
*
*/
fts_metaclass_t *fts_midiport_type = NULL;

static void
midiport_output(fts_object_t *o, fts_midievent_t *event, double time)
{
  fts_midiport_t *this = (fts_midiport_t *)o;
  fts_atom_t a;

  fts_set_object(&a, event);
  fts_midiport_input(o, 0, 0, 1, &a);
}

static void
midiport_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiport_t *this = (fts_midiport_t *)o;

  fts_midiport_init(this);
  fts_midiport_set_input(this);
  fts_midiport_set_output(this, midiport_output);
}

static void
midiport_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiport_t *this = (fts_midiport_t *)o;

  fts_midiport_reset(this);
}

static fts_status_t
midiport_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_midiport_t), 1, 1, 0);

  fts_midiport_class_init(cl);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midiport_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, midiport_delete);

  return fts_Success;
}

void
midiport_config( void)
{
  fts_midiport_type = fts_class_install( fts_new_symbol("midiport"), midiport_instantiate);
}

/****************************************************
 *
 *  default MIDI port
 *
 */

static fts_midiport_t *default_midiport = 0;
static fts_symbol_t default_midiport_class = 0;

void fts_midiport_set_default( int argc, const fts_atom_t *argv)
{
  fts_object_t *obj;
  fts_atom_t a[1];

  fts_object_new_to_patcher( fts_get_root_patcher(), argc, argv, &obj);

  if (!obj)
    return;

  fts_object_get_prop( obj, fts_s_state, a);

  if ( !fts_is_object( a) || !fts_object_is_midiport( fts_get_object( a)) )
    {
      fts_object_delete_from_patcher( obj);
      return;
    }

  if (default_midiport)
    {
      fts_object_delete_from_patcher( (fts_object_t *)default_midiport);
    }

  default_midiport = (fts_midiport_t *)fts_get_object( a);
}

fts_midiport_t *fts_midiport_get_default(void)
{
  if ((default_midiport == 0) && (default_midiport_class != 0)) {
    fts_atom_t a[1];
    fts_log("[midiport]: No default midiport was installed, instanciating the default class %s\n", default_midiport_class);
    fts_set_symbol(a, default_midiport_class);
    fts_midiport_set_default(1, a);
  }
  return default_midiport;
}

void fts_midiport_set_default_class( fts_symbol_t name)
{
  default_midiport_class = name;
}

/************************************************************
 *
 *  MIDI manager
 *
 *  the fts_midimanager is an object implementing the following methods:
 *
 */

fts_symbol_t fts_s_midimanager;
fts_symbol_t fts_s_sources;
fts_symbol_t fts_s_destinations;

/* current MIDI manager & hashtable of MIDI ports */
static fts_midimanager_t *midimanager = NULL;

static fts_midilabel_t *
fts_midilabel_new(fts_symbol_t name)
{
  fts_midilabel_t *label = (fts_midilabel_t *)fts_malloc(sizeof(fts_midilabel_t));

  label->name = name;
  label->input = NULL;
  label->output = NULL;
  label->input_name = NULL;
  label->output_name = NULL;
  label->next = NULL;

  return label;
}

static void
fts_midilabel_delete(fts_midilabel_t *label)
{
  if(label->input != NULL)
    fts_object_release(label->input);

  if(label->output != NULL)
    fts_object_release(label->output);

  fts_free(label);
}

void
fts_midimanager_insert_label_at_index(fts_midimanager_t *mm, int index, fts_symbol_t name)
{
  fts_midilabel_t **p = &mm->labels;
  fts_midilabel_t *label = fts_midilabel_new(name);

  while((*p) && index--)
    p = &(*p)->next;

  label->next = (*p);
  *p = label;

  mm->n_labels++;
}

void
fts_midimanager_remove_label_at_index(fts_midimanager_t *mm, int index)
{
  fts_midilabel_t **p = &mm->labels;
  fts_midilabel_t *label;

  while((*p) && index--)
    p = &(*p)->next;

  if(*p)
    {
      label = *p;
    
      *p = (*p)->next;
      mm->n_labels--;

      fts_midilabel_delete(label);
    }
}

fts_midilabel_t *
fts_midimanager_get_label_by_index(fts_midimanager_t *mm, int index)
{
  fts_midilabel_t *label =  mm->labels;

  while(label && index--)
    label = label->next;

  return label;
}

fts_midilabel_t *
fts_midimanager_get_label_by_name(fts_midimanager_t *mm, fts_symbol_t name)
{
  fts_midilabel_t *label =  mm->labels;

  while(label && label->name != name)
    label = label->next;

  return label;
}

void
fts_midilabel_set_input(fts_midilabel_t *label, fts_midiport_t *port, fts_symbol_t name)
{
  if(label->input != NULL)
    fts_object_release(label->input);

  label->input = port;
  label->input_name = name;

  if(port != NULL)
    fts_object_refer(port);
}

void
fts_midilabel_set_output(fts_midilabel_t *label, fts_midiport_t *port, fts_symbol_t name)
{
  if(label->output != NULL)
    fts_object_release(label->output);

  label->output = port;
  label->output_name = name;

  if(port != NULL)
    fts_object_refer(port);
}

void
fts_midimanager_set_input(fts_midimanager_t *mm, int index, fts_midiport_t *midiport, fts_symbol_t name)
{
  fts_midilabel_t *label = fts_midimanager_get_label_by_index(mm, index);

  fts_midilabel_set_input(label, midiport, name);

  if(fts_object_has_id((fts_object_t *)mm)) {
    fts_atom_t args[2];
    
    fts_set_int(args + 0, index);
    fts_set_symbol(args + 1, name);
    fts_client_send_message((fts_object_t *)mm, fts_s_input, 2, args);
  }
}

void
fts_midimanager_set_output(fts_midimanager_t *mm, int index, fts_midiport_t *midiport, fts_symbol_t name)
{
  fts_midilabel_t *label = fts_midimanager_get_label_by_index(mm, index);

  fts_midilabel_set_output(label, midiport, name);

  if(fts_object_has_id((fts_object_t *)mm)) {
    fts_atom_t args[2];
    
    fts_set_int(args + 0, index);
    fts_set_symbol(args + 1, name);
    fts_client_send_message((fts_object_t *)mm, fts_s_output, 2, args);
  }
}

void
fts_midimanager_set_internal(fts_midimanager_t *mm, int index)
{
  fts_midilabel_t *label = fts_midimanager_get_label_by_index(mm, index);
  fts_midiport_t *midiport = (fts_midiport_t *)fts_object_create(fts_midiport_type, 0, 0); /* create internal MIDI port */
  fts_atom_t args[2];
  
  /* set input and output to internal MIDI port */
  fts_midilabel_set_input(label, midiport, fts_s_internal);
  fts_midilabel_set_output(label, midiport, fts_s_internal);

  fts_set_int(args + 0, index);
  fts_set_symbol(args + 1, fts_s_internal);
  fts_client_send_message((fts_object_t *)mm, fts_s_input, 2, args);

  fts_set_int(args + 0, index);
  fts_set_symbol(args + 1, fts_s_internal);
  fts_client_send_message((fts_object_t *)mm, fts_s_output, 2, args);
}

/* midi objects API */
fts_midiport_t *
fts_midimanager_get_input(fts_symbol_t name)
{
  if(midimanager != NULL)
    {
      fts_midilabel_t *label = fts_midimanager_get_label_by_name(midimanager, name);
      
      if(label)
	return label->input;
    }

  return NULL;
}

fts_midiport_t *
fts_midimanager_get_output(fts_symbol_t name)
{
  if(midimanager != NULL)
    {
      fts_midilabel_t *label = fts_midimanager_get_label_by_name(midimanager, name);
      
      if(label)
	return label->output;
    }
  
  return NULL;
}

/* name utility */
fts_symbol_t
fts_midimanager_get_fresh_label_name(fts_midimanager_t *mm, fts_symbol_t name)
{
  const char *str = name;
  int len = strlen(str);
  char *new_str = alloca((len + 10) * sizeof(char));
  int num = 0;
  int dec = 1;
  int i;

  /* separate base name and index */
  for(i=len-1; i>=0; i--)
    {
      if(len == (i + 1) && str[i] >= '0' && str[i] <= '9')
        num += (str[len = i] - '0') * dec;
      else
        new_str[i] = str[i];
  
      dec *= 10;
    }

  /* generate new label name */
  while(fts_midimanager_get_label_by_name(mm, name) != NULL)
    {
      sprintf(new_str + len, "%d", ++num);
      name = fts_new_symbol_copy(new_str);
    }

  return name;
}

/* midi manager API */
void
fts_midimanager_set(fts_midimanager_t *mm)
{
  fts_atom_t a;

  /* check methods ??? */
  midimanager = mm;

  fts_set_object(&a, midimanager);

  fts_variable_define(fts_get_root_patcher(), fts_s_midimanager);
  fts_variable_restore(fts_get_root_patcher(), fts_s_midimanager, &a, (fts_object_t *)midimanager);
}

fts_midimanager_t *
fts_midimanager_get(void)
{
  return midimanager;
}

void
fts_midimanager_update(void)
{
  fts_atom_t a;

  fts_set_object(&a, midimanager);

  fts_variable_suspend(fts_get_root_patcher(), fts_s_midimanager);
  fts_variable_restore(fts_get_root_patcher(), fts_s_midimanager, &a, (fts_object_t *)midimanager);
}

/************************************************************
 *
 *  Initialization of the midi module
 *
 */ 

void fts_midi_config(void)
{
  fts_s_midimanager = fts_new_symbol("midimanager");
  fts_s_midievent = fts_new_symbol("midievent");
  fts_s_midiport = fts_new_symbol("midiport");
  fts_s_sources = fts_new_symbol("sources");
  fts_s_destinations = fts_new_symbol("destinations");

  midievent_config();
}


