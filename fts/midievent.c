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

#include <fts/fts.h>

/*
 * This file contains the following structures related to MIDI:
 *  - MIDI constants
 *  - MIDI events
 *  - MIDI parser 
 */

fts_symbol_t fts_midi_types[n_midi_types];
static fts_hashtable_t midi_type_hash;

/************************************************************
 *
 *  MIDI constants
 *
 */

enum midi_type
fts_midi_get_type_by_name(fts_symbol_t name)
{
  fts_atom_t k, a;
  
  fts_set_symbol(&k, name);
  fts_hashtable_get(&midi_type_hash, &k, &a);
  
  return fts_get_int(&a);
}

enum midi_type
fts_midi_get_type(const fts_atom_t *at)
{
  if(fts_is_int(at))
    return fts_get_int(at);
  else if(fts_is_symbol(at))
  {
    fts_atom_t a;

    if(fts_hashtable_get(&midi_type_hash, at, &a))
      return fts_get_int(&a);
  }

  return midi_type_any;
}

/************************************************************
 *
 *  MIDI events
 *
 */

fts_class_t *fts_midievent_type = 0;
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

static void
fts_midievent_song_position_pointer_message_init(fts_midievent_t *event, int byte1, int byte2)
{
  event->type = midi_song_position_pointer;
  fts_midievent_song_position_pointer_set_first(event,byte1);
  fts_midievent_song_position_pointer_set_second(event,byte2);
}

fts_midievent_t *
fts_midievent_song_position_pointer_message_new(int byte1, int byte2)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_object_create(fts_midievent_type, 0, 0);
  fts_midievent_song_position_pointer_message_init(event, byte1, byte2);
  return event;
}

static void
fts_midievent_song_select_message_init(fts_midievent_t *event, int byte1)
{
  event->type = midi_song_select;
  event->data.song_select = byte1;
}

fts_midievent_t *
fts_midievent_song_select_message_new(int byte1)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_object_create(fts_midievent_type, 0, 0);
  fts_midievent_song_select_message_init(event, byte1);
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
midievent_copy(fts_midievent_t *org, fts_midievent_t *copy)
{
  enum midi_type type = fts_midievent_get_type(org);

  if(fts_midievent_is_system_exclusive(copy) && !fts_midievent_is_system_exclusive(org))
    fts_array_destroy(&copy->data.system_exclusive);
  
  fts_midievent_set_type(copy, type);
  
  switch(type)
  {
    case midi_note:
    case midi_poly_pressure:
    case midi_control_change:
    case midi_pitch_bend:
      fts_midievent_channel_message_set_first(copy, fts_midievent_channel_message_get_first(org));
      fts_midievent_channel_message_set_second(copy, fts_midievent_channel_message_get_second(org));
      fts_midievent_channel_message_set_channel(copy, fts_midievent_channel_message_get_channel(org));
      break;
      
    case midi_program_change:
    case midi_channel_pressure:		
      fts_midievent_channel_message_set_first(copy, fts_midievent_channel_message_get_first(org));
      fts_midievent_channel_message_set_channel(copy, fts_midievent_channel_message_get_channel(org));
      break;
      
    case midi_system_exclusive:
      fts_array_set(&copy->data.system_exclusive, fts_array_get_size(&org->data.system_exclusive), fts_array_get_atoms(&org->data.system_exclusive));
      break;
      
    case midi_time_code:
      fts_midievent_time_code_set_type(copy, fts_midievent_time_code_get_type(org));
      fts_midievent_time_code_set_hour(copy, fts_midievent_time_code_get_hour(org));
      fts_midievent_time_code_set_minute(copy, fts_midievent_time_code_get_minute(org));
      fts_midievent_time_code_set_second(copy, fts_midievent_time_code_get_second(org));
      fts_midievent_time_code_set_frame(copy, fts_midievent_time_code_get_frame(org));
      break;
      
    case midi_song_position_pointer:
      fts_midievent_song_position_pointer_set_first(copy, fts_midievent_song_position_pointer_get_first(org));
      fts_midievent_song_position_pointer_set_second(copy, fts_midievent_song_position_pointer_get_second(org));
      break;
      
    case midi_song_select:
      fts_midievent_song_select_set(copy, fts_midievent_song_select_get(org));
      break;
      
    case midi_real_time:
      fts_midievent_real_time_set(copy, fts_midievent_real_time_get(org));
      break;
      
    default:
      break;
  }
}

static void
midievent_copy_function(const fts_atom_t *from, fts_atom_t *to)
{
  midievent_copy((fts_midievent_t *)fts_get_object(from), (fts_midievent_t *)fts_get_object(to));
}

static void
midievent_post_function(fts_object_t *o, fts_bytestream_t *stream)
{
  fts_midievent_t *this = (fts_midievent_t *)o;
  int type = fts_midievent_get_type(this);
  
  fts_spost(stream, "<midievent %s ", fts_symbol_name(fts_midi_types[type]));
  
  switch (type)
  {
    case midi_note:
    case midi_poly_pressure:
    case midi_control_change:
    case midi_pitch_bend:
      fts_spost(stream, "%d %d %d", 
                fts_midievent_channel_message_get_first(this), 
                fts_midievent_channel_message_get_second(this),
                fts_midievent_channel_message_get_channel(this));
      break;
      
    case midi_program_change:
    case midi_channel_pressure:		
      fts_spost(stream, "%d %d",
                fts_midievent_channel_message_get_first(this), 
                fts_midievent_channel_message_get_channel(this));
      break;
      
    case midi_system_exclusive:
      fts_post_atoms(fts_midievent_system_exclusive_get_size(this), fts_midievent_system_exclusive_get_atoms(this));
      break;
      
    case midi_time_code:
      fts_spost(stream, "%d %d %d %d %d",
                fts_midievent_time_code_get_type(this),
                fts_midievent_time_code_get_hour(this),
                fts_midievent_time_code_get_minute(this),
                fts_midievent_time_code_get_second(this),
                fts_midievent_time_code_get_frame(this));
      break;
      
    case midi_song_position_pointer:
      fts_spost(stream, "%d %d",
                fts_midievent_song_position_pointer_get_first(this),
                fts_midievent_song_position_pointer_get_second(this));
      break;
      
    case midi_song_select:
      fts_spost(stream, "%d", fts_midievent_song_select_get(this));
      break;
      
    case midi_real_time:
      fts_spost(stream, "%d", fts_midievent_real_time_get(this));
      break;
  }
  
  fts_spost(stream, ">");  
}

static void
midievent_array_function(fts_object_t *o, fts_array_t *array)
{
  fts_midievent_t *this = (fts_midievent_t *)o;
  int type = fts_midievent_get_type(this);
  
  fts_array_append_symbol(array, fts_midi_types[type]);
  
  /* append event data */
  if(fts_midievent_is_channel_message(this))
  {
    fts_array_append_int(array, fts_midievent_channel_message_get_first(this));
	  
    if(fts_midievent_channel_message_has_second_byte(this))
      fts_array_append_int(array, fts_midievent_channel_message_get_second(this));
    
    fts_array_append_int(array, fts_midievent_channel_message_get_channel(this));
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
  else if(fts_midievent_is_song_position_pointer(this))
  {
    fts_array_append_int(array,fts_midievent_song_position_pointer_get_first(this));
    fts_array_append_int(array,fts_midievent_song_position_pointer_get_second(this));
  }
  else if(fts_midievent_is_song_select(this))
    fts_array_append_int(array, fts_midievent_song_select_get(this));
  else if(fts_midievent_is_real_time(this))
    fts_array_append_int(array, fts_midievent_real_time_get(this));
  
  fts_return_object((fts_object_t *)array);
}

static void
midievent_set_from_midievent(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midievent_t *this = (fts_midievent_t *)o;
  
  midievent_copy((fts_midievent_t *)fts_get_object(at), this);
}

static void
midievent_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midievent_t *this = (fts_midievent_t *)o;

  if(ac > 0)
  {
    enum midi_type type = fts_midi_get_type(at);
      
    if(fts_midievent_is_system_exclusive(this))
      fts_array_destroy(&this->data.system_exclusive);

    /* set type */
    fts_midievent_set_type(this, type);

    /* set event data */
    switch (type)
    {
      default:
      case midi_note:
      case midi_poly_pressure:
      case midi_control_change:
      case midi_pitch_bend:
        switch(ac)
        {
          default:
          case 3:
            if(fts_is_number(at + 3))
              fts_midievent_channel_message_set_channel(this, fts_get_number_int(at + 3));
          case 2:	
            if(fts_is_number(at + 2))
              fts_midievent_channel_message_set_second(this, fts_get_number_int(at + 2));
          case 1:
            if(fts_is_number(at + 1))
              fts_midievent_channel_message_set_first(this, fts_get_number_int(at + 1));	
          case 0:
            break;
        }
        break;
        
      case midi_program_change:
      case midi_channel_pressure:
        fts_midievent_channel_message_set_second(this, MIDI_EMPTY_BYTE);
        
        switch(ac)
        {
          default:
          case 2:	
            if(fts_is_number(at + 2))
              fts_midievent_channel_message_set_channel(this, fts_get_number_int(at + 2));
          case 1:
            if(fts_is_number(at + 1))
              fts_midievent_channel_message_set_first(this, fts_get_number_int(at + 1));	
          case 0:
            break;
        }
        break;
        
      case midi_system_exclusive:
        fts_array_init(&this->data.system_exclusive, ac, at);
        break;
        
      case midi_time_code:
        switch(ac)
        {
          default:
          case 5:
            if(fts_is_number(at + 5))
              fts_midievent_time_code_set_frame(this, fts_get_number_int(at + 5));
          case 4:
            if(fts_is_number(at + 4))
              fts_midievent_time_code_set_second(this, fts_get_number_int(at + 4));
          case 3:
            if(fts_is_number(at + 3))
              fts_midievent_time_code_set_minute(this, fts_get_number_int(at + 3));
          case 2:
            if(fts_is_number(at + 2))
              fts_midievent_time_code_set_hour(this, fts_get_number_int(at + 2));
          case 1:
            if(fts_is_number(at + 1))
              fts_midievent_time_code_set_type(this, fts_get_number_int(at + 1));
          case 0:
            break;
        }
        break;
        
      case midi_song_position_pointer:
        switch(ac)
        {
          default:
          case 2:
            fts_midievent_song_position_pointer_set_second(this, fts_get_number_int(at + 2));
          case 1:
            fts_midievent_song_position_pointer_set_first(this, fts_get_number_int(at + 1));
          case 0:
            break;
        }
        break;
        
      case midi_song_select:
        if(fts_is_number(at + 1))
          fts_midievent_song_select_set(this, fts_get_number_int(at + 1));
        break;

      case midi_real_time:
        if(fts_is_number(at + 1))
          fts_midievent_real_time_set(this, fts_get_number_int(at + 1));
        break;
    }
  }
}
  
static void
_midievent_get_type(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midievent_t *this = (fts_midievent_t *)o;

  fts_return_symbol(fts_midi_types[this->type]);
}

static void
_midievent_set_first(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midievent_t *this = (fts_midievent_t *)o;

  if(fts_midievent_is_channel_message(this))
  {
    int byte = fts_get_number_int(at);

    if(byte < 0)
      fts_midievent_channel_message_set_first(this, 0);
    else if(byte > 127)
      fts_midievent_channel_message_set_first(this, 127);
    else
      fts_midievent_channel_message_set_first(this, byte);
  }
  else if(fts_midievent_is_song_position_pointer(this)){
    int byte = fts_get_number_int(at);
    
    if(byte < 0)
      fts_midievent_song_position_pointer_set_first(this, 0);
    else if(byte > 127)
      fts_midievent_song_position_pointer_set_first(this, 127);
    else
      fts_midievent_song_position_pointer_set_first(this, byte);
  }
  else if(fts_midievent_is_song_select(this)){
    int byte = fts_get_number_int(at);
    
    if(byte < 0)
      fts_midievent_song_select_set(this, 0);
    else if(byte > 127)
      fts_midievent_song_select_set(this, 127);
    else
      fts_midievent_song_select_set(this, byte);
  }
}

static void
_midievent_get_first(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midievent_t *this = (fts_midievent_t *)o;

  if(fts_midievent_is_channel_message(this))
    fts_return_int(fts_midievent_channel_message_get_first(this));
  
  else if(fts_midievent_is_song_position_pointer(this))
    fts_return_int(fts_midievent_song_position_pointer_get_first(this));
  
  else if(fts_midievent_is_song_select(this))
    fts_return_int(fts_midievent_song_select_get(this));
}

static void
_midievent_set_second(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midievent_t *this = (fts_midievent_t *)o;

  if(fts_midievent_is_channel_message(this) && fts_midievent_channel_message_has_second_byte(this))
  {
    int byte = fts_get_number_int(at);

    if(byte < 0)
      fts_midievent_channel_message_set_second(this, 0);
    else if(byte > 127)
      fts_midievent_channel_message_set_second(this, 127);
    else
      fts_midievent_channel_message_set_second(this, byte);
  }
  else if(fts_midievent_is_song_position_pointer(this))
  {
    int byte = fts_get_number_int(at);
    
    if(byte < 0)
      fts_midievent_song_position_pointer_set_second(this, 0);
    else if(byte > 127)
      fts_midievent_song_position_pointer_set_second(this, 127);
    else
      fts_midievent_song_position_pointer_set_second(this, byte);
  }
}

static void
_midievent_get_second(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midievent_t *this = (fts_midievent_t *)o;

  if(fts_midievent_is_channel_message(this) && fts_midievent_channel_message_has_second_byte(this))
    fts_return_int(fts_midievent_channel_message_get_second(this));
  
  else if(fts_midievent_is_song_position_pointer(this))
    fts_return_int(fts_midievent_song_position_pointer_get_second(this));
}

static void
_midievent_set_channel(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midievent_t *this = (fts_midievent_t *)o;

  if(fts_midievent_is_channel_message(this))
  {
    int channel = fts_get_number_int(at);

    if(channel < 1)
      fts_midievent_channel_message_set_second(this, 0);
    else if(channel > 16)
      fts_midievent_channel_message_set_second(this, 16);
    else
      fts_midievent_channel_message_set_second(this, channel);
  }
}

static void
_midievent_get_channel(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midievent_t *this = (fts_midievent_t *)o;

  if(fts_midievent_is_channel_message(this))
    fts_return_int(fts_midievent_channel_message_get_channel(this));
}

static void
_midievent_get_status(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midievent_t *this = (fts_midievent_t *)o;

  if(fts_midievent_is_channel_message(this))
    fts_return_int(fts_midievent_channel_message_get_status_byte(this));
  else if(fts_midievent_is_song_position_pointer(this))
    fts_return_int(fts_midievent_song_position_pointer_status_byte);
  else if(fts_midievent_is_song_select(this))
    fts_return_int(fts_midievent_song_select_status_byte);
  else if(fts_midievent_is_real_time(this))
    fts_return_int(fts_midievent_real_time_get_status_byte(this));
}

static void
midievent_dump_state(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midievent_t *this = (fts_midievent_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  enum midi_type type = fts_midievent_get_type(this);
  fts_message_t *mess;

  /* send set message with pitch and duration */
  mess = fts_dumper_message_new(dumper, fts_s_set);
  fts_message_append_int(mess, type);

  switch(type)
  {
    case midi_type_any:
      break;
      
    case midi_note:
    case midi_poly_pressure:
    case midi_control_change:
    case midi_pitch_bend:
      fts_message_append_int(mess, fts_midievent_channel_message_get_first(this));
      fts_message_append_int(mess, fts_midievent_channel_message_get_second(this));
      fts_message_append_int(mess, fts_midievent_channel_message_get_channel(this));
      break;

    case midi_program_change:
    case midi_channel_pressure:
      fts_message_append_int(mess, fts_midievent_channel_message_get_first(this));
      fts_message_append_int(mess, fts_midievent_channel_message_get_channel(this));
      break;

    case midi_system_exclusive:
      fts_message_append(mess, fts_midievent_system_exclusive_get_size(this), fts_midievent_system_exclusive_get_atoms(this));
      break;

    case midi_time_code:
      fts_message_append_int(mess, fts_midievent_time_code_get_type(this));
      fts_message_append_int(mess, fts_midievent_time_code_get_hour(this));
      fts_message_append_int(mess, fts_midievent_time_code_get_minute(this));
      fts_message_append_int(mess, fts_midievent_time_code_get_second(this));
      fts_message_append_int(mess, fts_midievent_time_code_get_frame(this));
      break;

    case midi_song_position_pointer:
      fts_message_append_int(mess, fts_midievent_song_position_pointer_get_first(this));
      fts_message_append_int(mess, fts_midievent_song_position_pointer_get_second(this));
      break;
      
    case midi_song_select:
      fts_message_append_int(mess, fts_midievent_song_select_get(this));
      break;
      
    case midi_real_time:
      fts_message_append_int(mess, fts_midievent_real_time_get(this));
      break;
      
    case n_midi_types:
      break;
  }

  fts_dumper_message_send(dumper, mess);
}

static void
midievent_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(ac > 0)
    midievent_set(o, 0, 0, ac, at);
}

static void 
midievent_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_bytestream_t* stream = fts_get_default_console_stream();
  
  if(ac > 0 && fts_is_object(at))
    stream = (fts_bytestream_t *)fts_get_object(at);

  midievent_post_function(o, stream);
  fts_post("\n");
}  

static void
midievent_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midievent_t *this = (fts_midievent_t *)o;
  
  if(fts_midievent_is_system_exclusive(this))
    fts_array_destroy(&this->data.system_exclusive);
}

static void
midievent_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_midievent_t), midievent_init, midievent_delete);

  fts_class_set_copy_function(cl, midievent_copy_function);
  fts_class_set_post_function(cl, midievent_post_function);
  fts_class_set_array_function(cl, midievent_array_function);
  
  fts_class_message_varargs(cl, fts_s_print, midievent_print);

  fts_class_message_varargs(cl, fts_s_set, midievent_set);
  fts_class_message(cl, fts_s_set, cl, midievent_set_from_midievent);

  fts_class_message_varargs(cl, fts_s_dump_state, midievent_dump_state);
  
  fts_class_message_void(cl, fts_s_type, _midievent_get_type);

  fts_class_message_number(cl, fts_new_symbol("first"), _midievent_set_first);
  fts_class_message_void(cl, fts_new_symbol("first"), _midievent_get_first);

  fts_class_message_number(cl, fts_new_symbol("second"), _midievent_set_second);
  fts_class_message_void(cl, fts_new_symbol("second"), _midievent_get_second);

  fts_class_message_number(cl, fts_new_symbol("channel"), _midievent_set_channel);
  fts_class_message_void(cl, fts_new_symbol("channel"), _midievent_get_channel);

  fts_class_message_void(cl, fts_new_symbol("status"), _midievent_get_status);
  
  /* class doc */
  fts_class_doc(cl, fts_s_midievent, "<'note'|'poly'|'ctl'|'prg'|'touch'|'bend'|'sysex'|'mtc'|'spos'|'ssel'|'rt': type> [<num: MIDI bytes (see message 'set')> ...]", "MIDI message");
  fts_class_doc(cl, fts_s_set, "<'note'> <num: note #> <num: velocity> [<num: channel>]", "set to note on/off message (velocity is 0 for note off)");
  fts_class_doc(cl, fts_s_set, "<'poly'> <num: note #> <num: pressure value> [<num: channel>]", "set to poly pressure message");
  fts_class_doc(cl, fts_s_set, "<'ctl'> <num: controller #> <num: value> [<num: channel>]", "set to control change message");
  fts_class_doc(cl, fts_s_set, "<'prg'> <num: program #> [<num: channel>]", "set to program change message");
  fts_class_doc(cl, fts_s_set, "<'touch'> <num: pressure value> [<num: channel>]", "set to channel pressure message");
  fts_class_doc(cl, fts_s_set, "<'bend'> <num: LSB> <num: MSB> [<num: channel>]", "set to pitch bend message");
  fts_class_doc(cl, fts_s_set, "<'sysex'> [<num: byte> ...]", "set to system exclusive message");
  fts_class_doc(cl, fts_s_set, "<'mtc'> <num: type> <num: hour> <num: minute> <num: second> <num: frame>", "set to MIDI time code message");
  fts_class_doc(cl, fts_s_set, "<'spos'> <num: first byte> <num: second byte>", "set to song position pointer message");
  fts_class_doc(cl, fts_s_set, "<'ssel'> <num: song #>", "set to song select message");
  fts_class_doc(cl, fts_s_set, "<'rt'> <num: real time message byte>", "set to real time message");
  fts_class_doc(cl, fts_s_set, "<midievent: other>", "set from midievent instance");
  fts_class_doc(cl, fts_s_type, NULL, "get MIDI event type");
  fts_class_doc(cl, fts_new_symbol("first"), "[<num: byte>]", "get/set first MIDI byte (channel messages only)");
  fts_class_doc(cl, fts_new_symbol("second"), "[<num: byte>]", "get/set second MIDI byte (channel messages only)");
  fts_class_doc(cl, fts_new_symbol("channel"), "[<num: byte>]", "get/set MIDI channel (channel messages only)");
  fts_class_doc(cl, fts_new_symbol("status"), NULL, "get MIDI status byte");
  fts_class_doc(cl, fts_s_print, NULL, "print MIDI event");
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
      parser->store = MIDI_EMPTY_BYTE;
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
    parser->channel = (byte & 0xf) + 1;
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

    case midiparser_status_song_position_pointer:
      
      if (parser->store == MIDI_EMPTY_BYTE)
        parser->store = byte;
      else
      {
        fts_midievent_song_position_pointer_message_init(event, parser->store, byte);
        parser->store = MIDI_EMPTY_BYTE; /* reset for running status */
        parser->event = NULL;
        return event;
      }
      break;
      
    case midiparser_status_song_select:
      
      fts_midievent_song_select_message_init(event,byte);
      parser->event = NULL;
      return event;
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

/* **********************************************************************
 *
 * Initialization
 *
 */
void
fts_kernel_midievent_init(void)
{
  fts_atom_t a;
  int i;
  
  fts_midi_types[midi_note] = fts_new_symbol("note");
  fts_midi_types[midi_poly_pressure] = fts_new_symbol("poly");
  fts_midi_types[midi_control_change] = fts_new_symbol("ctl");
  fts_midi_types[midi_program_change] = fts_new_symbol("prg");
  fts_midi_types[midi_channel_pressure] = fts_new_symbol("touch");
  fts_midi_types[midi_pitch_bend] = fts_new_symbol("bend");
  fts_midi_types[midi_system_exclusive] = fts_new_symbol("sysex");
  fts_midi_types[midi_time_code] = fts_new_symbol("mtc");
  fts_midi_types[midi_song_position_pointer] = fts_new_symbol("spos");
  fts_midi_types[midi_song_select] = fts_new_symbol("ssel");
  fts_midi_types[midi_real_time] = fts_new_symbol("rt");

  fts_hashtable_init( &midi_type_hash, FTS_HASHTABLE_SMALL);
  for(i=0; i<n_midi_types; i++)
  {
    fts_atom_t key;

    fts_set_symbol(&key, fts_midi_types[i]);
    fts_set_int(&a, i);
    fts_hashtable_put(&midi_type_hash, &key, &a);
  }

  fts_s_midievent = fts_new_symbol("midievent");
  fts_midievent_type = fts_class_install(fts_s_midievent, midievent_instantiate);
}
