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
#include <ftsconfig.h>
#include <ftsprivate/bmaxfile.h>
#include <ftsprivate/client.h>
#include <ftsprivate/variable.h>
#include <stdlib.h>
#include <string.h>

#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

/*
 * This file contains everything related to MIDI:
 *
 *  - MIDI events
 *  - MIDI fifo
 *  - MIDI parser 
 *  - MIDI port (incl. midibus and midinull class)
 *  - MIDI label
 *  - MIDI manager
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

fts_midievent_t *   fts_midievent_channel_message_new(enum midi_type type, int channel, int byte1, int byte2)
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

/***************************************************
*
*  midi fifo
*
*/
void
fts_midififo_init(fts_midififo_t *fifo, int size)
{
  int bytes = sizeof(fts_midififo_entry_t) * size;
  fts_midififo_entry_t *entries;
  int i;

  fts_fifo_init(&fifo->data, fts_malloc(bytes), bytes);
  entries = (fts_midififo_entry_t *)fts_fifo_get_buffer(&fifo->data);

  for(i=0; i<size; i++) 
    {
      fts_object_t *obj = fts_object_create(fts_midievent_type, 0, 0);
      fts_object_refer(obj);
      entries[i].event = (fts_midievent_t *)obj;
    }

  fifo->delta = 0.0;
  fifo->size = size;
}

void
fts_midififo_reset(fts_midififo_t *fifo)
{
  fts_midififo_entry_t *entries = (fts_midififo_entry_t *)fts_fifo_get_buffer(&fifo->data);
  int i;

  for(i=0; i<fifo->size; i++)
    fts_object_release((fts_object_t *)entries[i].event);

  fts_free((void *)fifo->data.buffer);
}

/* read next fifo entry into time base (returns pointer to atom of newly allocated entry) */
void
fts_midififo_poll(fts_midififo_t *fifo)
{
  if(fts_fifo_read_level(&fifo->data) >= sizeof(fts_midififo_entry_t)) 
    {
      fts_midififo_entry_t *entry = (fts_midififo_entry_t *)fts_fifo_read_pointer(&fifo->data);
      double now = fts_get_time();
      double delay;
      fts_atom_t a;
      
      /* set midievent argument */
      fts_set_object(&a, entry->event);
      
      /* time == 0.0 means: send now */
      if(entry->time != 0.0) 
	{
	  /* adjust delta time on very first fifo entry */
	  if(fifo->delta == 0.0)
	    fifo->delta = entry->time;
	  
	  /* translate event time to delay */
	  delay = entry->time - fifo->delta;
	  
	  /* adjust delta time */
	  if(delay < 0.0) 
	    {
	      delay = 0.0;
	      fifo->delta = entry->time;
	    }
	  
	  /* schedule midiport input call */
	  fts_timebase_add_call(fts_get_timebase(), entry->port, fts_midiport_input, &a, delay);
	} 
      else
	fts_midiport_input(entry->port, 0, 0, 1, &a);
      
      fts_object_release(entry->event);
      
      /* insert a new midievent into fifo and claim it */
      entry->event = (fts_midievent_t *)fts_object_create(fts_midievent_type, 0, 0);
      fts_object_refer(entry->event);
      
      fts_fifo_incr_read(&fifo->data, sizeof(fts_midififo_entry_t));
    }
}

fts_midievent_t *
fts_midififo_get_event(fts_midififo_t *fifo)
{
  if(fts_fifo_write_level(&fifo->data) >= sizeof(fts_midififo_entry_t)) 
    {
      fts_midififo_entry_t *entry = (fts_midififo_entry_t *)fts_fifo_write_pointer(&fifo->data);
      return entry->event;
    } 
  else
    return NULL;
}

void
fts_midififo_write(fts_midififo_t *fifo, fts_object_t *port, double time)
{
  fts_midififo_entry_t *entry = (fts_midififo_entry_t *)fts_fifo_write_pointer(&fifo->data);
  
  /* set midport */
  entry->port = port;
  fts_object_refer(port);

  /* set time */
  entry->time = time;

  /* send entry */
  fts_fifo_incr_write(&fifo->data, sizeof(fts_midififo_entry_t));
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

static fts_midievent_t *
midiparser_get_event(fts_midiparser_t *parser)
{
  if(parser->event == NULL)
    parser->event = (fts_midievent_t *)fts_object_create(fts_midievent_type, 0, 0);

  return parser->event;
}

void
fts_midiparser_set_event(fts_midiparser_t *parser, fts_midievent_t *event)
{
  if(event != parser->event) 
    {
      if(parser->event != NULL)
	fts_object_release(parser->event);
      
      parser->event = event;
      
      if(event != NULL)
	fts_object_refer(event);
    }
}

void
fts_midiparser_reset(fts_midiparser_t *parser)
{
  fts_array_destroy(&parser->system_exclusive);

  if(parser->event != NULL)
    fts_object_release(parser->event);

  parser->event = NULL;
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
static fts_metaclass_t *midibus_type = NULL;

static void
midibus_output(fts_object_t *o, fts_midievent_t *event, double time)
{
  fts_midiport_t *this = (fts_midiport_t *)o;
  fts_atom_t a;

  fts_set_object(&a, event);
  fts_midiport_input(o, 0, 0, 1, &a);
}

static void
midibus_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiport_t *this = (fts_midiport_t *)o;

  fts_midiport_init(this);
  fts_midiport_set_input(this);
  fts_midiport_set_output(this, midibus_output);
}

static void
midibus_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiport_t *this = (fts_midiport_t *)o;

  fts_midiport_reset(this);
}

static fts_status_t
midibus_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_midiport_t), 0, 0, 0);

  fts_midiport_class_init(cl);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midibus_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, midibus_delete);

  return fts_Success;
}

/****************************************************
 *
 *  null MIDI port class
 *
 */
static fts_metaclass_t *midinull_type = NULL;

static void
midinull_output(fts_object_t *o, fts_midievent_t *event, double time)
{
  fts_midiport_t *this = (fts_midiport_t *)o;
}

static void
midinull_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiport_t *this = (fts_midiport_t *)o;

  fts_midiport_init(this);
  fts_midiport_set_input(this);
  fts_midiport_set_output(this, midinull_output);
}

static void
midinull_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiport_t *this = (fts_midiport_t *)o;

  fts_midiport_reset(this);
}

static fts_status_t
midinull_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_midiport_t), 0, 0, 0);

  fts_midiport_class_init(cl);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midinull_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, midinull_delete);

  return fts_Success;
}

/************************************************************
 *
 *  MIDI label
 *
 */

typedef struct _midilabel
{
  fts_symbol_t name;
  fts_midiport_t *input;
  fts_midiport_t *output;
  fts_symbol_t input_name;
  fts_symbol_t output_name;
  struct _midilabel *next;
} midilabel_t;

static midilabel_t *
midilabel_new(fts_symbol_t name)
{
  midilabel_t *label = (midilabel_t *)fts_malloc(sizeof(midilabel_t));

  label->name = name;
  label->input = NULL;
  label->output = NULL;
  label->input_name = NULL;
  label->output_name = NULL;
  label->next = NULL;

  return label;
}

static void
midilabel_delete(midilabel_t *label)
{
  if(label->input != NULL)
    fts_object_release(label->input);

  if(label->output != NULL)
    fts_object_release(label->output);

  fts_free(label);
}

static void
midilabel_set_input(midilabel_t *label, fts_midiport_t *port, fts_symbol_t name)
{
  if(port != label->input) 
    {
      if(label->input != NULL)
	fts_object_release(label->input);
      
      label->input = port;
      label->input_name = name;
      
      if(port != NULL)
	fts_object_refer(port);
    }
}

static void
midilabel_set_output(midilabel_t *label, fts_midiport_t *port, fts_symbol_t name)
{
  if(port != label->output) 
    {
      if(label->output != NULL)
	fts_object_release(label->output);
      
      label->output = port;
      label->output_name = name;
      
      if(port != NULL)
	fts_object_refer(port);
    }
}

/************************************************************
 *
 *  MIDI configuration
 *
 */

typedef struct _midiconfig
{
  fts_object_t o;
  midilabel_t *labels;
  int n_labels;

  int dirty;
  fts_symbol_t file_name;

} midiconfig_t;

/* current MIDI configuration */
static midiconfig_t *midiconfig = NULL;

/* array of device names */
static fts_array_t midiconfig_inputs;
static fts_array_t midiconfig_outputs;

/* list of MIDI managers */
static fts_midimanager_t *midimanagers = NULL;

/* MIDI port used for invalid references */
static fts_midiport_t *midinull = NULL;

static fts_symbol_t midiconfig_s_name;
static fts_symbol_t midiconfig_s_internal_bus;
static fts_symbol_t midiconfig_s_unconnected;
static fts_symbol_t midiconfig_s_inputs;
static fts_symbol_t midiconfig_s_outputs;

fts_symbol_t fts_midimanager_s_get_default_input;
fts_symbol_t fts_midimanager_s_get_default_output;
fts_symbol_t fts_midimanager_s_append_input_names;
fts_symbol_t fts_midimanager_s_append_output_names;
fts_symbol_t fts_midimanager_s_get_input;
fts_symbol_t fts_midimanager_s_get_output;

static void midiconfig_set_dirty(midiconfig_t *this, int is_dirty);

/* handle midimanagers */
static fts_midiport_t *
midimanagers_get_input(fts_symbol_t device_name, fts_symbol_t label_name)
{
  fts_midiport_t *port = NULL;

  if(device_name != NULL)
    {
      fts_midimanager_t *mm;
      fts_atom_t args[3];
      
      fts_set_pointer(args + 0, &port);
      fts_set_symbol(args + 1, device_name);
      fts_set_symbol(args + 2, label_name);
      
      for(mm = midimanagers; mm != NULL && port == NULL; mm = mm->next)
	fts_send_message((fts_object_t *)mm, fts_SystemInlet, fts_midimanager_s_get_input, 3, args);
    }

  return port;      
}

static fts_midiport_t *
midimanagers_get_output(fts_symbol_t device_name, fts_symbol_t label_name)
{
  fts_midiport_t *port = NULL;

  if(device_name != NULL)
    {
      fts_midimanager_t *mm;
      fts_atom_t args[3];
      
      fts_set_pointer(args + 0, &port);
      fts_set_symbol(args + 1, device_name);
      fts_set_symbol(args + 2, label_name);
      
      for(mm = midimanagers; mm != NULL && port == NULL; mm = mm->next)
	fts_send_message((fts_object_t *)mm, fts_SystemInlet, fts_midimanager_s_get_output, 3, args);
    }
      
  return port;
}

static void
midimanagers_get_device_names(void)
{
  fts_midimanager_t *mm;
  fts_atom_t arg;
  
  fts_set_pointer(&arg, &midiconfig_inputs);
  
  for(mm = midimanagers; mm != NULL; mm = mm->next)
    fts_send_message((fts_object_t *)mm, fts_SystemInlet, fts_midimanager_s_append_input_names, 1, &arg);

  fts_set_pointer(&arg, &midiconfig_outputs);
  
  for(mm = midimanagers; mm != NULL; mm = mm->next)
    fts_send_message((fts_object_t *)mm, fts_SystemInlet, fts_midimanager_s_append_output_names, 1, &arg);
}

static fts_symbol_t
midimanagers_get_default_input(void)
{
  fts_symbol_t name = NULL;
  fts_midimanager_t *mm;
  fts_atom_t arg;
  
  fts_set_pointer(&arg, &name);

  for(mm = midimanagers; mm != NULL && name == NULL; mm = mm->next)
    fts_send_message((fts_object_t *)mm, fts_SystemInlet, fts_midimanager_s_get_default_input, 1, &arg);

  return name;
}

static fts_symbol_t
midimanagers_get_default_output(void)
{
  fts_symbol_t name = NULL;
  fts_midimanager_t *mm;
  fts_atom_t arg;
  
  fts_set_pointer(&arg, &name);

  for(mm = midimanagers; mm != NULL && name == NULL; mm = mm->next)
    fts_send_message((fts_object_t *)mm, fts_SystemInlet, fts_midimanager_s_get_default_output, 1, &arg); 

  return name;
}

static midilabel_t *
midiconfig_label_get_by_index(midiconfig_t *config, int index)
{
  midilabel_t *label =  config->labels;

  while(label && index--)
    label = label->next;

  return label;
}

static midilabel_t *
midiconfig_label_get_by_name(midiconfig_t *config, fts_symbol_t name)
{
  midilabel_t *label =  config->labels;

  while(label && label->name != name)
    label = label->next;

  return label;
}

static midilabel_t *
midiconfig_label_insert(midiconfig_t *config, int index, fts_symbol_t name)
{
  midilabel_t **p = &config->labels;
  midilabel_t *label = midilabel_new(name);
  int n = index;

  label->input_name = midiconfig_s_unconnected;
  label->output_name = midiconfig_s_unconnected;

  /* inset label to list */
  while((*p) && n--)
    p = &(*p)->next;

  label->next = (*p);
  *p = label;

  config->n_labels++;

  /* send new label to client */
  if(fts_object_has_id((fts_object_t *)config)) 
    {
      fts_atom_t args[4];
      
      fts_set_int(args, index);
      fts_set_symbol(args + 1, name);
      fts_set_symbol(args + 2, midiconfig_s_unconnected);
      fts_set_symbol(args + 3, midiconfig_s_unconnected);
      fts_client_send_message((fts_object_t *)config, fts_s_set, 4, args);
    }
  
  midiconfig_set_dirty( config, 1);

  return label;
}

static void
midiconfig_label_remove(midiconfig_t *config, int index)
{
  midilabel_t **p = &config->labels;
  int n = index;

  /* remove label and send to client */
  while((*p) && n--)
    p = &(*p)->next;

  if(*p) 
    {
      midilabel_t *label = *p;
      
      *p = (*p)->next;
      config->n_labels--;
      
      midilabel_delete(label);
    }

  /* send remove to client */
  if(fts_object_has_id((fts_object_t *)config)) 
    {
      fts_atom_t arg;
      
      fts_set_int(&arg, index);
      fts_client_send_message((fts_object_t *)config, fts_s_remove, 1, &arg);
    }

  midiconfig_set_dirty( config, 1);
}

static void
midiconfig_label_set_input(midiconfig_t *config, midilabel_t *label, int index, fts_midiport_t *midiport, fts_symbol_t name)
{
  if(midiport == NULL)
    name = midiconfig_s_unconnected;

  if(midiport != label->input || name != label->input_name) 
    {
      midilabel_set_input(label, midiport, name);
      
      if(fts_object_has_id((fts_object_t *)config)) 
	{
	  fts_atom_t args[2];
	  
	  fts_set_int(args + 0, index);
	  fts_set_symbol(args + 1, name);
	  fts_client_send_message((fts_object_t *)config, fts_s_input, 2, args);
	}

      midiconfig_set_dirty( config, 1);
    }
}

static void
midiconfig_label_set_output(midiconfig_t *config, midilabel_t *label, int index, fts_midiport_t *midiport, fts_symbol_t name)
{
  if(midiport == NULL)
    name = midiconfig_s_unconnected;
  
  if(midiport != label->output || name != label->output_name) 
    {
      midilabel_set_output(label, midiport, name);
      
      if(fts_object_has_id((fts_object_t *)config)) 
	{
	  fts_atom_t args[2];
	  
	  fts_set_int(args + 0, index);
	  fts_set_symbol(args + 1, name);
	  fts_client_send_message((fts_object_t *)config, fts_s_output, 2, args);
	}
    
      midiconfig_set_dirty( config, 1);
    }
}

static void
midiconfig_label_set_internal(midiconfig_t *config, midilabel_t *label, int index)
{
  /* create internal MIDI port */
  fts_midiport_t *port = (fts_midiport_t *)fts_object_create(midibus_type, 0, 0);

  /* set input and output to internal MIDI port */
  midiconfig_label_set_input(config, label, index, port, midiconfig_s_internal_bus);
  midiconfig_label_set_output(config, label, index, port, midiconfig_s_internal_bus);
}

static void
midiconfig_set_input(midiconfig_t *config, int index, fts_symbol_t name)
{
  midilabel_t *label = midiconfig_label_get_by_index(config, index);  

  /* reset output to none if it is set to internal */
  if(name != midiconfig_s_internal_bus && label->output && label->output_name == midiconfig_s_internal_bus)
    midiconfig_label_set_output(config, label, index, NULL, NULL);

  if(name == midiconfig_s_unconnected)
    midiconfig_label_set_input(config, label, index, NULL, NULL);
  else if(name == midiconfig_s_internal_bus)
    midiconfig_label_set_internal(config, label, index);
  else
    midiconfig_label_set_input(config, label, index, midimanagers_get_input(name, label->name), name);
}

static void
midiconfig_set_output(midiconfig_t *config, int index, fts_symbol_t name)
{
  midilabel_t *label = midiconfig_label_get_by_index(config, index);

  /* reset output to none if it is set to internal */
  if(name != midiconfig_s_internal_bus && label->input && label->input_name == midiconfig_s_internal_bus)
    midiconfig_label_set_input(config, label, index, NULL, NULL);

  if(name == midiconfig_s_unconnected)
    midiconfig_label_set_output(config, label, index, NULL, NULL);
  else if(name == midiconfig_s_internal_bus)
    midiconfig_label_set_internal(config, label, index);
  else
    midiconfig_label_set_output(config, label, index, midimanagers_get_output(name, label->name), name);
}

/* name utility */
static fts_symbol_t
midiconfig_get_fresh_label_name(midiconfig_t *config, fts_symbol_t name)
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
  while(midiconfig_label_get_by_name(config, name) != NULL) 
    {
      sprintf(new_str + len, "%d", ++num);
      name = fts_new_symbol_copy(new_str);
    }

  return name;
}

static void
midiconfig_restore(midiconfig_t *config)
{
  fts_atom_t a;

  fts_set_object(&a, midiconfig);
  fts_variable_suspend(fts_get_root_patcher(), midiconfig_s_name);
  fts_variable_restore(fts_get_root_patcher(), midiconfig_s_name, &a, (fts_object_t *)config);
}

static void
midiconfig_update_devices(midiconfig_t *config)
{
  int ac;
  fts_atom_t *at;
  
  fts_array_clear(&midiconfig_inputs);
  fts_array_clear(&midiconfig_outputs);

  fts_array_append_symbol(&midiconfig_inputs, midiconfig_s_unconnected);
  fts_array_append_symbol(&midiconfig_outputs, midiconfig_s_unconnected);

  fts_array_append_symbol(&midiconfig_inputs, midiconfig_s_internal_bus);
  fts_array_append_symbol(&midiconfig_outputs, midiconfig_s_internal_bus);

  /* get device names from all midimanagers */
  midimanagers_get_device_names();

  ac = fts_array_get_size(&midiconfig_inputs);
  at = fts_array_get_atoms(&midiconfig_inputs);
  fts_client_send_message((fts_object_t *)config, midiconfig_s_inputs, ac, at);
  
  ac = fts_array_get_size(&midiconfig_outputs);
  at = fts_array_get_atoms(&midiconfig_outputs);
  fts_client_send_message((fts_object_t *)config, midiconfig_s_outputs, ac, at);
}

static void
midiconfig_update_labels(midiconfig_t *config)
{
  midilabel_t *label = config->labels;
  int n = config->n_labels;
  int i;

  /* check inout and output midiports */
  for(i=0; i<n; i++) 
    {
      if(label->input == NULL || fts_object_get_metaclass((fts_object_t *)label->input) != midibus_type) 
	{
	  fts_midiport_t *input = NULL;
	  fts_midiport_t *output = NULL;
	  
	  if(label->input != NULL)
	    {
	      input = midimanagers_get_input(label->input_name, label->name);
	      
	      if(input != label->input) 
		midiconfig_label_set_input(config, label, i, input, label->input_name);
	    }	  

	  if(label->output != NULL)
	    {
	      output = midimanagers_get_output(label->output_name, label->name);
	      
	      if(output != label->output) 
		midiconfig_label_set_output(config, label, i, output, label->output_name);
	    }
	}
      
      label = label->next;
    }
}

static void
midiconfig_erase_labels(midiconfig_t *config)
{
  midilabel_t *label = config->labels;
  
  while(label != NULL)
    {
      midilabel_t *next = label->next;
      
      midilabel_delete(label);
      label = next;
    }
  
  config->labels = NULL;
  config->n_labels = 0;
}

static void
midiconfig_set_defaults()
{
  if(midiconfig != NULL)
    {
      midilabel_t *label = midiconfig_label_get_by_index(midiconfig, 0);
      
      if(label == NULL)
	label = midiconfig_label_insert(midiconfig, 0, fts_s_default);
      
      if(label->input == NULL)
	{
	  fts_symbol_t name = midimanagers_get_default_input();
	  fts_midiport_t *port = midimanagers_get_input(name, fts_s_default);
	  
	  midiconfig_label_set_input(midiconfig, label, 0, port, name);
	}

      if(label->output == NULL)
	{
	  fts_symbol_t name = midimanagers_get_default_output();
	  fts_midiport_t *port = midimanagers_get_output(name, fts_s_default);
	  
	  midiconfig_label_set_output(midiconfig, label, 0, port, name);
	}

      midiconfig_set_dirty( midiconfig, 0);
    }
}

/* midi manager API */
void
fts_midiconfig_update()
{
  midiconfig_update_labels(midiconfig);
  midiconfig_update_devices(midiconfig);
}

void
fts_midiconfig_add_manager(fts_midimanager_t *mm)
{
  fts_midimanager_t **p = &midimanagers;
  fts_atom_t a;
  
  while(*p != NULL)
    *p = (*p)->next;

  *p = mm;
  mm->next = NULL;

  /* update default devices */
  midiconfig_set_defaults();
}

/* midi objects API */
fts_midiport_t *
fts_midiconfig_get_input(fts_symbol_t name)
{
  if(midiconfig != NULL)
    {
      midilabel_t *label = midiconfig_label_get_by_name(midiconfig, name);
      
      if(label != NULL && label->input != NULL)
	return label->input;
    }

  return midinull;
}

fts_midiport_t *
fts_midiconfig_get_output(fts_symbol_t name)
{
  if(midiconfig != NULL)
    {
      midilabel_t *label = midiconfig_label_get_by_name(midiconfig, name);
      
      if(label != NULL && label->output != NULL)
	return label->output;
    }
  
  return midinull;
}

void
fts_midiconfig_add_listener(fts_object_t *obj)
{
  fts_variable_add_user(fts_get_root_patcher(), midiconfig_s_name, obj);
}

fts_object_t *
fts_midiconfig_get(void)
{
  return (fts_object_t *)midiconfig;
}

static void
midiconfig_set(midiconfig_t *config)
{
  if(config != NULL)
    {
      fts_object_refer((fts_object_t *)config);
 
      if( (midiconfig != NULL) && fts_object_has_id( (fts_object_t *)midiconfig))
	{
	  fts_atom_t a;
      
	  if( ! fts_object_has_id( (fts_object_t *)config))
	    fts_client_register_object(  (fts_object_t *)config, fts_get_client_id( (fts_object_t *)midiconfig));
	  
	  fts_set_int(&a, fts_get_object_id( (fts_object_t *)config));
	  fts_client_send_message(  (fts_object_t *) object_get_client( (fts_object_t *)config), fts_s_midi_config, 1, &a);
      
	  fts_send_message( (fts_object_t *)config, fts_SystemInlet, fts_s_upload, 0, 0);
	}
    }
  
  if(midiconfig != NULL)
    fts_object_release((fts_object_t *)midiconfig);
    
  midiconfig = config;
}

/****************************************************
 *
 *  MIDI configuration class
 *
 */
static fts_metaclass_t *midiconfig_type = NULL;

static void
midiconfig_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiconfig_t *this = (midiconfig_t *)o;

  midiconfig_erase_labels(this);
  midiconfig_label_insert(this, 0, fts_s_default);  

  if(fts_object_has_id( o)) 
    fts_client_send_message( o, fts_s_clear, 0, 0);
  
  /*midiconfig_set_dirty( this, 1);*/
}

static void
midiconfig_restore_label(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiconfig_t *this = (midiconfig_t *)o;
  fts_symbol_t name = fts_get_symbol(at);
  fts_symbol_t input = fts_get_symbol(at + 1);
  fts_symbol_t output = fts_get_symbol(at + 2);
  int index = this->n_labels;
  
  /* make sure that first label is "default" */
  if(index == 0 && name != fts_s_default)
    {
      midiconfig_label_insert(this, 0, fts_s_default);
      index = 1;
    }
    
  midiconfig_label_insert(this, index, name);
  midiconfig_set_input(this, index, input);
  midiconfig_set_output(this, index, output);
}

static void
midiconfig_insert_label(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiconfig_t *this = (midiconfig_t *)o;
  int index = fts_get_int(at);
  fts_symbol_t name = fts_get_symbol(at + 1);
  
  /* check if name is not already used */
  if(midiconfig_label_get_by_name(this, name) != NULL)
    name = midiconfig_get_fresh_label_name(this, name);

  midiconfig_label_insert(this, index, name);
}

static void
midiconfig_remove_label(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiconfig_t *this = (midiconfig_t *)o;
  int index = fts_get_int(at);

  midiconfig_label_remove(this, index);

  midiconfig_update_labels(this);
  midiconfig_update_devices(this);
  midiconfig_restore(this);
}

static void
midiconfig_input( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiconfig_t *this = (midiconfig_t *)o;
  int index = fts_get_int(at);
  fts_symbol_t name = fts_get_symbol(at + 1);

  midiconfig_set_input(this, index, name);

  midiconfig_update_labels(this);
  midiconfig_update_devices(this);
  midiconfig_restore(this);
}

static void
midiconfig_output( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiconfig_t *this = (midiconfig_t *)o;
  int index = fts_get_int(at);
  fts_symbol_t name = fts_get_symbol(at + 1);

  midiconfig_set_output(this, index, name);

  midiconfig_update_labels(this);
  midiconfig_update_devices(this);
  midiconfig_restore(this);
}

static void
midiconfig_upload( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiconfig_t *this = (midiconfig_t *)o;
  midilabel_t *label = this->labels;
  int i;

  /* upload labels with inputs and outputs */
  for(i=0; i<this->n_labels; i++) 
    {
      fts_atom_t args[4];
      
      fts_set_int(args, i);
      fts_set_symbol(args + 1, label->name);
      fts_set_symbol(args + 2, label->input_name);
      fts_set_symbol(args + 3, label->output_name);
      fts_client_send_message((fts_object_t *)this, fts_s_insert, 4, args);
      
      label = label->next;
    }

  midiconfig_update_labels(this);
  midiconfig_update_devices(this);

  if( this->file_name != NULL)
    {
      fts_atom_t a[1];
      fts_set_symbol( a, this->file_name);
      fts_client_send_message((fts_object_t *)this, fts_s_name, 1, a);
    }
}

static void
midiconfig_load( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiconfig_t *this = (midiconfig_t *)o;
  fts_symbol_t file_name = fts_get_symbol(at);
  fts_symbol_t project_dir = fts_project_get_dir();
  fts_object_t *obj = NULL;
  char path[MAXPATHLEN];

  fts_make_absolute_path(project_dir, file_name, path, MAXPATHLEN);

  obj = fts_binary_file_load(path, (fts_object_t *)fts_get_root_patcher(), 0, 0);

  if(obj != NULL && fts_object_get_metaclass(obj) == midiconfig_type) 
    {
      /* replace current config by loaded config */
      midiconfig_set((midiconfig_t *)obj);

      ((midiconfig_t *)obj)->file_name = fts_new_symbol_copy( path);
      
      midiconfig_set_dirty( (midiconfig_t *)obj, 0);
    }
  else
    fts_log( "midiconfig load: cannot read MIDI configuration from file %s\n", file_name);
}

static void
midiconfig_save( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiconfig_t *this = (midiconfig_t *)o;
  fts_symbol_t file_name = fts_get_symbol(at);
  fts_symbol_t project_dir = fts_project_get_dir();
  char path[MAXPATHLEN];
  fts_bmax_file_t f;

  fts_make_absolute_path(project_dir, file_name, path, MAXPATHLEN);

  if (fts_bmax_file_open(&f, path, 0, 0, 0) >= 0)
    {
      midilabel_t *label = this->labels;
      int i;

      fts_bmax_code_new_object(&f, o, -1);
      
      while(label) 
	{
	  /* code insert message for each label */
	  fts_bmax_code_push_symbol(&f, label->output_name);
	  fts_bmax_code_push_symbol(&f, label->input_name);
	  fts_bmax_code_push_symbol(&f, label->name);
	  fts_bmax_code_obj_mess(&f, fts_SystemInlet, fts_s_restore, 3);
	  fts_bmax_code_pop_args(&f, 3);

	  label = label->next;	  
	}

      fts_bmax_code_return(&f);
      fts_bmax_file_close(&f);
    
      midiconfig_set_dirty( this, 0);
    }
  else
    fts_log( "midiconfig save: cannot open file %s\n", file_name);
}

static void
midiconfig_set_to_defaults( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiconfig_t *this = (midiconfig_t *)o;
  midiconfig_clear( o, winlet, fts_s_clear, 0, 0);
  this->file_name = NULL;
  midiconfig_upload( o, winlet, fts_s_upload, 0, 0); 
  
  midiconfig_set_dirty( this, 0);
}

/* set midiconfig as dirty or as saved.
 * A "setDirty" message is sent to the client after is_dirty flag changed
 */
static void 
midiconfig_set_dirty(midiconfig_t *this, int is_dirty)
{
  if(this->dirty != is_dirty)
    {
      this->dirty = is_dirty;

      if ( fts_object_has_id( (fts_object_t *)this))
	{
	  fts_atom_t a[1];
	  
	  fts_set_int(&a[0], is_dirty);
	  fts_client_send_message((fts_object_t *)this, fts_s_set_dirty, 1, a);
	}
    }
}


static void
midiconfig_print( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiconfig_t *this = (midiconfig_t *)o;
  midilabel_t *label = this->labels;

  post("labels\n");
  for(label = this->labels; label != NULL; label = label->next) 
    post("  %s: '%s' '%s'\n", label->name, label->input_name, label->output_name);
}

static void
midiconfig_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiconfig_t *this = (midiconfig_t *)o;
  fts_atom_t a;
      
  this->labels = NULL;
  this->n_labels = 0;

  this->dirty = 0;
  this->file_name = NULL;

  /* modify object description */
  fts_set_symbol(&a, midiconfig_s_name);
  fts_object_set_description(o, 1, &a);
}

static void
midiconfig_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiconfig_t *this = (midiconfig_t *)o;

  midiconfig_erase_labels(this);
}

static fts_status_t
midiconfig_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(midiconfig_t), 0, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midiconfig_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, midiconfig_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_clear, midiconfig_clear);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_default, midiconfig_set_to_defaults);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_restore, midiconfig_restore_label);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_insert, midiconfig_insert_label);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_remove, midiconfig_remove_label);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_input, midiconfig_input);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_output, midiconfig_output);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_upload, midiconfig_upload);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_load, midiconfig_load);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_save, midiconfig_save);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_print, midiconfig_print);

  return fts_Success;
}

/* temporary object for debugging */
static void
mm_redirect( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midimanager_t *mm;
      
  /* redirect to MIDI config */
  fts_send_message((fts_object_t *)midiconfig, fts_SystemInlet, s, ac, at);
  
  /* redirect to MIDI managers */
  for(mm = midimanagers; mm != NULL; mm = mm->next)
    fts_send_message((fts_object_t *)mm, fts_SystemInlet, s, ac, at);
}

static fts_status_t
mm_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_object_t), 1, 0, 0);

  fts_method_define_varargs(cl, 0, fts_s_anything, mm_redirect);

  return fts_Success;
}

/************************************************************
 *
 *  Initialization of the midi module
 *
 */ 
void
fts_midi_config(void)
{
  fts_atom_t a;

  fts_s_midievent = fts_new_symbol("midievent");
  fts_midievent_type = fts_class_install(fts_s_midievent, midievent_instantiate);

  midibus_type = fts_class_install(NULL, midibus_instantiate);
  midinull_type = fts_class_install(NULL, midinull_instantiate);
  
  midiconfig_s_name = fts_new_symbol("__midiconfig");
  midiconfig_s_internal_bus = fts_new_symbol("Internal Bus");
  midiconfig_s_unconnected = fts_new_symbol("-");
  midiconfig_s_inputs = fts_new_symbol("inputs");
  midiconfig_s_outputs = fts_new_symbol("outputs");  
  fts_array_init(&midiconfig_inputs, 0, 0);
  fts_array_init(&midiconfig_outputs, 0, 0);

  /* midi manager interface messages */
  fts_midimanager_s_get_default_input = fts_new_symbol("_midimanager_get_default_input");
  fts_midimanager_s_get_default_output = fts_new_symbol("_midimanager_get_default_output");
  fts_midimanager_s_append_input_names = fts_new_symbol("_midimanager_append_input_names");
  fts_midimanager_s_append_output_names = fts_new_symbol("_midimanager_append_output_names");
  fts_midimanager_s_get_input = fts_new_symbol("_midimanager_get_input");
  fts_midimanager_s_get_output = fts_new_symbol("_midimanager_get_output");

  /* MIDI configuration class */
  midiconfig_type = fts_class_install(midiconfig_s_name, midiconfig_instantiate);

  /* create global NULL MIDI port */
  midinull = (fts_midiport_t *)fts_object_create(midinull_type, 0, 0);
  fts_object_refer((fts_object_t *)midinull);

  /* create first default midi configuration */
  midiconfig_set((midiconfig_t *)fts_object_create(midiconfig_type, 0, 0));
  midiconfig_set_defaults();

  /* define global midiconfig variable */
  fts_set_object(&a, midiconfig);
  fts_variable_define(fts_get_root_patcher(), midiconfig_s_name);
  fts_variable_restore(fts_get_root_patcher(), midiconfig_s_name, &a, (fts_object_t *)midiconfig);

  fts_class_install(fts_new_symbol("mm"), mm_instantiate);
}
