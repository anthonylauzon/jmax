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

fts_class_t *midievent_class = 0;
fts_symbol_t fts_s_midievent = 0;

fts_midievent_t *
fts_midievent_channel_message_new(enum midi_type type, int channel, int byte1, int byte2)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_object_create(midievent_class, 0, 0);

  event->type = type;
  event->id = channel;
  event->data.channel_message.first = byte1;
  event->data.channel_message.second = byte2;

  return event;
}

fts_midievent_t *
fts_midievent_note_new(int channel, int note, int velocity)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_object_create(midievent_class, 0, 0);

  event->type = midi_type_note;
  event->id = channel;
  event->data.channel_message.first = note;
  event->data.channel_message.second = velocity;

  return event;
}

fts_midievent_t *
fts_midievent_poly_pressure_new(int channel, int note, int value)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_object_create(midievent_class, 0, 0);

  event->type = midi_type_poly_pressure;
  event->id = channel;
  event->data.channel_message.first = note;
  event->data.channel_message.second = value;

  return event;
}

fts_midievent_t *
fts_midievent_control_change_new(int channel, int number, int value)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_object_create(midievent_class, 0, 0);

  event->type = midi_type_control_change;
  event->id = channel;
  event->data.channel_message.first = number;
  event->data.channel_message.second = value;

  return event;
}

fts_midievent_t *
fts_midievent_program_change_new(int channel, int number)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_object_create(midievent_class, 0, 0);

  event->type = midi_type_program_change;
  event->id = channel;
  event->data.channel_message.first = number;
  event->data.channel_message.second = MIDI_EMPTY_BYTE;

  return event;
}

fts_midievent_t *
fts_midievent_channel_pressure_new(int channel, int value)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_object_create(midievent_class, 0, 0);

  event->type = midi_type_channel_pressure;
  event->id = channel;
  event->data.channel_message.first = value;
  event->data.channel_message.second = MIDI_EMPTY_BYTE;

  return event;
}

fts_midievent_t * 
fts_midievent_pitch_bend_new(int channel, int LSB, int MSB)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_object_create(midievent_class, 0, 0);

  event->type = midi_type_pitch_bend;
  event->id = channel;
  event->data.channel_message.first = LSB;
  event->data.channel_message.second = MSB;

  return event;
}

fts_midievent_t * 
fts_midievent_system_exclusive_new(void)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_object_create(midievent_class, 0, 0);

  event->type = midi_type_system;
  event->id = midi_system_exclusive;
  fts_array_init(&event->data.system_exclusive, 0, 0);

  return event;
}

void
fts_midievent_system_exclusive_append(fts_midievent_t *event, int byte)
{
  fts_atom_t a;

  fts_set_int(&a, byte & 0x7F);
  fts_array_append(&event->data.system_exclusive, 1, &a);
}

fts_midievent_t * 
fts_midievent_time_code_new(int type, int hour, int minute, int second, int frame)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_object_create(midievent_class, 0, 0);

  event->type = midi_type_system;
  event->id = midi_time_code;
  event->data.time_code.type = type;
  event->data.time_code.hour = hour;
  event->data.time_code.minute = minute;
  event->data.time_code.second = second;
  event->data.time_code.frame = frame;

  return event;
}

fts_midievent_t * 
fts_midievent_real_time_new(enum midi_real_time_event tag)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_object_create(midievent_class, 0, 0);

  event->type = midi_type_system;
  event->id = midi_real_time;
  event->data.real_time = tag;

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
  int type = fts_get_int(at + 0);
  
  if(fts_midievent_is_system_exclusive(this))
    fts_array_destroy(&this->data.system_exclusive);

  /* set type */
  fts_midievent_set_type(this, type);

  /* set event data */
  if(fts_midievent_is_channel_message(this) <= midi_type_system)
    {
      fts_midievent_channel_message_set_first(this, fts_get_int(at + 1));
      fts_midievent_channel_message_set_second(this, fts_get_int(at + 2));
      fts_midievent_channel_message_set_channel(this, fts_get_int(at + 3) - 1);
    }
  else if(fts_midievent_is_system_exclusive(this))
    fts_array_init(&this->data.system_exclusive, ac - 1, at + 1);
  else if(fts_midievent_is_time_code(this))
    {
      fts_midievent_time_code_set_type(this, fts_get_int(at + 1));
      fts_midievent_time_code_set_hour(this, fts_get_int(at + 2));
      fts_midievent_time_code_set_minute(this, fts_get_int(at + 3));
      fts_midievent_time_code_set_second(this, fts_get_int(at + 4));
      fts_midievent_time_code_set_frame(this, fts_get_int(at + 5));
    }
  else if(fts_midievent_is_real_time(this))
    fts_midievent_real_time_set(this, fts_get_int(at + 1));
}

static void
midievent_get_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midievent_t *this = (fts_midievent_t *)o;
  fts_array_t *array = fts_get_array(at);

  /* set type */
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

  this->type = midi_type_any; /* event type */
  this->id = midi_system_any; /* channel or system type id */
}

static void
midievent_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midievent_t *this = (fts_midievent_t *)o;
  int type = this->type;
  
  if(fts_midievent_is_channel_message(this))
    {
      int byte1 = this->data.channel_message.first;
      int byte2 = this->data.channel_message.second;
      
      switch (this->type)
	{
	case midi_type_note:
	  post("{note (%d) %d %d}\n", this->id, byte1, byte2);
	  break;
	  
	case midi_type_poly_pressure:
	  post("{poly pressure (%d) %d %d}\n", this->id, byte1, byte2);
	  break;
	  
	case midi_type_control_change:
	  post("{control change (%d) %d %d}\n", this->id, byte1, byte2);
	  break;
	  
	case midi_type_program_change:
	  post("{program change (%d) %d}\n", this->id, byte1);
	  break;
	  
	case midi_type_channel_pressure:		
	  post("{channel pressure (%d) %d}\n", this->id, byte1);
	  break;
	  
	case midi_type_pitch_bend:
	  post("{pitch bend (%d) %d %d}\n", this->id, byte1, byte2);
	  break;

	default:
	  break;	  
	}
    }
  else
    post("midi system message");
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
  fts_class_install(fts_s_midievent, midievent_instantiate);
  midievent_class = fts_class_get_by_name(fts_s_midievent);
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

#define RANGE_CH(n) (((n) < 1)? 1: (((n) > 16)? 16: (n)))
#define RANGE_VALUE(n) (((n) < 0)? 0: (((n) > 127)? 127: (n)))

#define SYSEX_BLOCK_SIZE 512

void
fts_midiparser_init(fts_midiparser_t *parser)
{  
  fts_midiport_init(&parser->port);

  parser->status = midiparser_status_reset;

  parser->channel = 0;	
  parser->store = MIDI_EMPTY_BYTE;
  parser->system_exclusive;

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
  if(parser->system_exclusive)
    fts_object_release(parser->system_exclusive);

  fts_midiport_reset(&parser->port);
}

static void
midiparser_system_exclusive_start(fts_midiparser_t *parser)
{
  fts_midievent_t *sysex_event = parser->system_exclusive;

  if(sysex_event)
    fts_object_release(sysex_event);

  sysex_event = fts_midievent_system_exclusive_new();
  fts_object_refer(sysex_event);
}

static void
midiparser_system_exclusive_byte(fts_midiparser_t *parser, int byte)
{
  fts_midievent_system_exclusive_append(parser->system_exclusive, byte);
}

static void
midiparser_system_exclusive_send(fts_midiparser_t *parser)
{
  fts_midievent_t *sysex_event = parser->system_exclusive;

  if(sysex_event)
    {
      if(fts_midievent_system_exclusive_get_size(sysex_event) > 0)
	fts_midiport_input(&parser->port, sysex_event, 0.0);
      
      fts_object_release(sysex_event);
      parser->system_exclusive = 0;
    }
}

static void
midiparser_mtc_send(fts_midiparser_t *parser)
{
  fts_midievent_t *mtc_event = 
    fts_midievent_time_code_new(parser->mtc_type, parser->mtc_hour, parser->mtc_minute, parser->mtc_second, parser->mtc_frame);

  fts_midiport_input(&parser->port, mtc_event, 0.0);  
}

void
fts_midiparser_byte(fts_midiparser_t *parser, unsigned char byte)
{
  fts_midiport_t *port = (fts_midiport_t *)parser;

  if(byte >= midi_status_timing_clock)
    {
      /* system real-time messages */
      fts_midievent_t *event = fts_midievent_real_time_new((int)byte - midi_status_timing_clock + 1);
      
      fts_midiport_input(port, event, 0.0);
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
	  midiparser_system_exclusive_send(parser);
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
	      fts_midievent_t *event = fts_midievent_note_new(parser->channel, parser->store, 0);

	      fts_midiport_input(port, event, 0.0);
	      
	      /* reset for running status */
	      parser->store = MIDI_EMPTY_BYTE;
	    }
	  
	  break;
	  
	case midiparser_status_note_on:
	  
	  if (parser->store == MIDI_EMPTY_BYTE)
	    parser->store = byte;
	  else
	    {
	      fts_midievent_t *event = fts_midievent_note_new(parser->channel, parser->store, byte);

	      fts_midiport_input(port, event, 0.0);
	      
	      /* reset for running status */
	      parser->store = MIDI_EMPTY_BYTE;
	    }
	  
	  break;
	  
	case midiparser_status_poly_pressure:
	  
	  if (parser->store == MIDI_EMPTY_BYTE)
	    parser->store = byte;
	  else
	    {
	      fts_midievent_t *event = fts_midievent_poly_pressure_new(parser->channel, parser->store, byte);

	      fts_midiport_input(port, event, 0.0);
	      
	      /* reset for running status */
	      parser->store = MIDI_EMPTY_BYTE;
	    }
	  break;
		      
	case midiparser_status_control_change:
		      
	  if (parser->store == MIDI_EMPTY_BYTE)
	    parser->store = byte;
	  else
	    {
	      fts_midievent_t *event = fts_midievent_control_change_new(parser->channel, parser->store, byte);

	      fts_midiport_input(port, event, 0.0);
			  
	      /* reset for running status */
	      parser->store = MIDI_EMPTY_BYTE;
	    }
	  break;
		      
	case midiparser_status_program_change:
	  {
	    fts_midievent_t *event = fts_midievent_program_change_new(parser->channel, byte);
	    
	    fts_midiport_input(port, event, 0.0);
	  }
	  
	  break;
		      
	case midiparser_status_channel_pressure:		
	  {
	    fts_midievent_t *event = fts_midievent_channel_pressure_new(parser->channel, byte);
	    
	    fts_midiport_input(port, event, 0.0);
	  }
		      
	  break;
		      
	case midiparser_status_pitch_bend:
		      
	  if (parser->store == MIDI_EMPTY_BYTE)
	    parser->store = byte;
	  else
	    {
	      fts_midievent_t *event = fts_midievent_pitch_bend_new(parser->channel, parser->store, byte);

	      fts_midiport_input(port, event, 0.0);
			  
	      /* reset for running status */
	      parser->store = MIDI_EMPTY_BYTE;
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
	      midiparser_system_exclusive_start(parser);
	      midiparser_system_exclusive_byte(parser, (int)byte);
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
	      midiparser_system_exclusive_start(parser);
	      midiparser_system_exclusive_byte(parser, (int)MIDI_SYSTEM_EXCLUSIVE_REALTIME);
	      midiparser_system_exclusive_byte(parser, (int)byte);
	      parser->status = midiparser_status_system_exclusive_byte;
	      break;
	    }
	  
	  break;
	  
	case midiparser_status_system_exclusive_byte:
	  
	  /* ordinary sysex byte */
	  midiparser_system_exclusive_byte(parser, (int)byte);

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
		  midiparser_system_exclusive_start(parser);
		  midiparser_system_exclusive_byte(parser, (int)MIDI_SYSTEM_EXCLUSIVE_REALTIME);
		  midiparser_system_exclusive_byte(parser, (int)0x01);
		  midiparser_system_exclusive_byte(parser, (int)byte);

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
		    midiparser_mtc_send(parser);
		    parser->mtc_status = mtc_status_ready;
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
		    midiparser_mtc_send(parser);
		    parser->mtc_status = mtc_status_ready;
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
}

/****************************************************
 *
 *  MIDI port class
 *
 */

static fts_symbol_t fts_s__superclass = 0;
static fts_symbol_t fts_s_midiport = 0;

void 
fts_midiport_class_init(fts_class_t *cl)
{
  fts_atom_t a[1];

  fts_set_symbol(a, fts_s_midiport);

  fts_class_put_prop(cl, fts_s__superclass, a); /* set _superclass property to "midiport" */
}

void
fts_midiport_init(fts_midiport_t *port)
{
  int type;
  
  for(type=midi_type_any; type<n_midi_types; type++)
    port->listeners[type + 1] = 0;
  
  port->output = 0;
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

	case midi_type_note:
	case midi_type_poly_pressure:
	case midi_type_control_change:
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

	case midi_type_program_change:
	case midi_type_channel_pressure:
	case midi_type_pitch_bend:
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

	case midi_type_system:
	  {
	    /* system message listeners */
	    fts_midiport_listener_t **sys_list;
	    int i;
	    
	    sys_list = (fts_midiport_listener_t **)fts_malloc(sizeof(fts_midiport_listener_t *) * n_midi_system_types);
	    
	    for(i=0; i<n_midi_system_types; i++)
	      sys_list[i] = 0;

	    port->listeners[type + 1] = sys_list;
	  }
	  break;

	default:
	  break;
	}
    }
}

void
fts_midiport_set_output(fts_midiport_t *port, fts_midiport_function_t function)
{
  port->output = function;
}

int
fts_object_is_midiport(fts_object_t *obj)
{
  fts_atom_t a[1];

  fts_object_get_prop(obj, fts_s__superclass, a);

  if(fts_is_symbol(a) && fts_get_symbol(a) == fts_s_midiport)
    return 1;
  else
    return 0;
}

/****************************************************
 *
 *  MIDI port listeners
 *
 */

static void 
add_listener(fts_midiport_listener_t **list, fts_object_t *object, fts_midiport_function_t fun)
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
fts_midiport_add_listener(fts_midiport_t *port, enum midi_type type, int id, int number, fts_object_t *obj, fts_midiport_function_t fun)
{
  fts_midiport_listener_t **list = &port->listeners[type + 1][(number + 1) * (n_midi_channels + 1) + (id + 1)];

  add_listener(list, obj, fun);
}

void
fts_midiport_remove_listener(fts_midiport_t *port, enum midi_type type, int id, int number, fts_object_t *obj)
{
  fts_midiport_listener_t **list = &port->listeners[type + 1][(number + 1) * (n_midi_channels + 1) + (id + 1)];

  remove_listener(list, obj);
}

void
fts_midiport_input(fts_midiport_t *port, fts_midievent_t *event, double time)
{
  int type = fts_midievent_get_type(event);
  int id = fts_midievent_get_id(event);
  fts_midiport_listener_t **type_list = port->listeners[type + 1]; 
  fts_midiport_listener_t *l; 

  fts_object_refer((fts_object_t *)event);

  if(type <= midi_type_control_change)
    {
      /* note, poly pressure, control_change */
      int number = fts_midievent_channel_message_get_first(event); /* note or controller number */
      int onset = (number + 1) * (n_midi_channels + 1);

      /* fire callbacks for given channel and number */
      l = type_list[onset + id + 1];
      while(l)
	{
	  l->callback(l->listener, event, time);
	  l = l->next;
	}

      /* fire callbacks for any channel and given number*/
      l = type_list[onset];
      while(l)
	{
	  l->callback(l->listener, event, time);
	  l = l->next;
	}
    }

  /* fire callbacks for given system type or given channel and any number */
  l = type_list[id + 1];
  while(l)
    {
      l->callback(l->listener, event, time);
      l = l->next;
    }

  /* fire callbacks for any system type or any channel and any number */
  l = type_list[0];
  while(l)
    {
      l->callback(l->listener, event, time);
      l = l->next;
    }

  /* fire callbacks for any event */
  l = port->listeners[0][0];
  while(l)
    {
      l->callback(l->listener, event, time);
      l = l->next;
    }

  fts_object_release((fts_object_t *)event);
}

void 
fts_midiport_output(fts_midiport_t *port, fts_midievent_t *event, double time)
{
  fts_object_refer((fts_object_t *)event);
  
  port->output((fts_object_t *)port, event, time);

  fts_object_release((fts_object_t *)event);
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
    fts_log("[midiport]: No default midiport was installed, instanciating the default class %s\n", fts_symbol_name(default_midiport_class));
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
 *  Initialization of the midi module
 */ 

void fts_midi_config(void)
{
  fts_s_midievent = fts_new_symbol("midievent");
  fts_s_midiport = fts_new_symbol("midiport");
  fts_s__superclass = fts_new_symbol("_superclass");

  midievent_config();
}
