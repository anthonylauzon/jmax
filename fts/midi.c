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

#include <fts/fts.h>
#include <ftsconfig.h>

#include <stdlib.h>
#include <string.h>
#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#include <ftsprivate/bmaxfile.h>
#include <ftsprivate/client.h>
#include <ftsprivate/object.h>
#include <ftsprivate/variable.h>
#include <ftsprivate/midi.h>
#include <ftsprivate/audioconfig.h> /* requires audiolabel.h */
#include <ftsprivate/config.h> /* requires audioconfig.h */

/*
 * This file contains everything related to MIDI:
 *
 *  - MIDI constants
 *  - MIDI events
 *  - MIDI fifo
 *  - MIDI parser 
 *  - MIDI port (incl. midibus and midinull class)
 *  - MIDI label
 *  - MIDI manager
 *
 * MIDI files are handled in midifile.c
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
    if(fts_midievent_is_channel_message(this) <= midi_system_exclusive)
    {
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
    }
    else if(fts_midievent_is_system_exclusive(this))
      fts_array_init(&this->data.system_exclusive, ac, at);
    else if(fts_midievent_is_time_code(this))
    {
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
    }
    else if(fts_midievent_is_real_time(this))
      if(fts_is_number(at + 1))
	fts_midievent_real_time_set(this, fts_get_number_int(at + 1));
  }
}
  
static void
midievent_get_tuple(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midievent_t *this = (fts_midievent_t *)o;
  fts_tuple_t *tuple = (fts_tuple_t *)fts_object_create(fts_tuple_class, 0, 0);
  int type = fts_midievent_get_type(this);

  fts_tuple_append_symbol(tuple, fts_midi_types[type]);

  /* append event data */
  if(fts_midievent_is_channel_message(this))
  {
    fts_tuple_append_int(tuple, fts_midievent_channel_message_get_first(this));
	  
    if(fts_midievent_channel_message_has_second_byte(this))
      fts_tuple_append_int(tuple, fts_midievent_channel_message_get_second(this));

    fts_tuple_append_int(tuple, fts_midievent_channel_message_get_channel(this));
  }
  else if(fts_midievent_is_system_exclusive(this))
  {
    int size = fts_midievent_system_exclusive_get_size(this);
    fts_atom_t *atoms = fts_midievent_system_exclusive_get_atoms(this);
	  
    fts_tuple_append(tuple, size, atoms);
  }
  else if(fts_midievent_is_time_code(this))
  {
    fts_tuple_append_int(tuple, fts_midievent_time_code_get_type(this));
    fts_tuple_append_int(tuple, fts_midievent_time_code_get_hour(this));
    fts_tuple_append_int(tuple, fts_midievent_time_code_get_minute(this));
    fts_tuple_append_int(tuple, fts_midievent_time_code_get_second(this));
    fts_tuple_append_int(tuple, fts_midievent_time_code_get_frame(this));
  }
  else if(fts_midievent_is_real_time(this))
    fts_tuple_append_int(tuple, fts_midievent_real_time_get(this));

  fts_return_object((fts_object_t *)tuple);
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
}

static void
_midievent_get_first(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midievent_t *this = (fts_midievent_t *)o;

  if(fts_midievent_is_channel_message(this))
    fts_return_int(fts_midievent_channel_message_get_first(this));
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
}

static void
_midievent_get_second(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midievent_t *this = (fts_midievent_t *)o;

  if(fts_midievent_is_channel_message(this) && fts_midievent_channel_message_has_second_byte(this))
    fts_return_int(fts_midievent_channel_message_get_second(this));
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
      fts_message_append_int(mess, fts_midievent_channel_message_get_first(this));
      fts_message_append_int(mess, fts_midievent_channel_message_get_second(this));
      fts_message_append_int(mess, fts_midievent_channel_message_get_channel(this));
      break;

    case midi_program_change:
    case midi_channel_pressure:
      fts_message_append_int(mess, fts_midievent_channel_message_get_first(this));
      fts_message_append_int(mess, fts_midievent_channel_message_get_channel(this));
      break;

    case midi_pitch_bend:
      fts_message_append_int(mess, fts_midievent_channel_message_get_first(this));
      fts_message_append_int(mess, fts_midievent_channel_message_get_second(this));
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
midievent_post(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midievent_t *this = (fts_midievent_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
  int type = fts_midievent_get_type(this);

  fts_spost(stream, "(:midievent %s ", fts_midi_types[type]);

  switch (type)
  {
  case midi_note:
  case midi_poly_pressure:
  case midi_control_change:
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
      
  case midi_pitch_bend:
    fts_spost(stream, "%d %d %d",
	      fts_midievent_channel_message_get_first(this), 
	      fts_midievent_channel_message_get_second(this),
	      fts_midievent_channel_message_get_channel(this));
    break;
      
  case midi_system_exclusive:
    post_atoms(fts_midievent_system_exclusive_get_size(this), fts_midievent_system_exclusive_get_atoms(this));
    break;
      
  case midi_time_code:
    fts_spost(stream, "%d %d %d %d %d",
	      fts_midievent_time_code_get_type(this),
	      fts_midievent_time_code_get_hour(this),
	      fts_midievent_time_code_get_minute(this),
	      fts_midievent_time_code_get_second(this),
	      fts_midievent_time_code_get_frame(this));
    break;
      
  case midi_real_time:
    fts_spost(stream, "%d", fts_midievent_real_time_get(this));
    break;
  }

  fts_spost(stream, ")");  
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

  fts_class_message_varargs(cl, fts_s_post, midievent_post);

  fts_class_message_varargs(cl, fts_s_get_tuple, midievent_get_tuple);
  fts_class_message_varargs(cl, fts_s_set, midievent_set);

  fts_class_message_varargs(cl, fts_s_dump_state, midievent_dump_state);
  
  fts_class_message_void(cl, fts_s_type, _midievent_get_type);

  fts_class_message_number(cl, fts_new_symbol("first"), _midievent_set_first);
  fts_class_message_void(cl, fts_new_symbol("first"), _midievent_get_first);

  fts_class_message_number(cl, fts_new_symbol("second"), _midievent_set_second);
  fts_class_message_void(cl, fts_new_symbol("second"), _midievent_get_second);

  fts_class_message_number(cl, fts_new_symbol("channel"), _midievent_set_channel);
  fts_class_message_void(cl, fts_new_symbol("channel"), _midievent_get_channel);

  fts_class_message_void(cl, fts_new_symbol("status"), _midievent_get_status);
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

void
fts_midififo_resync(fts_midififo_t *fifo)
{
  fifo->delta = 0.0;
}

/* read next fifo entry into time base (returns pointer to atom of newly allocated entry) */
void
fts_midififo_poll(fts_midififo_t *fifo)
{
  if(fts_fifo_read_level(&fifo->data) >= sizeof(fts_midififo_entry_t)) 
  {
    fts_midififo_entry_t *entry = (fts_midififo_entry_t *)fts_fifo_read_pointer(&fifo->data);
    double time = entry->time - fts_get_time();
    double delay;
    fts_atom_t a;
      
    /* set midievent argument */
    fts_set_object(&a, entry->event);
      
    /* time == 0.0 means: send now */
    if(time > 0.0) 
    {        
      /* adjust delta time on very first fifo entry */
      if(fifo->delta == 0.0)
	fifo->delta = time;
	  
      /* translate event time to delay */
      delay = time - fifo->delta;
          	  
      /* adjust delta time */
      if(delay < 0.0) 
      {
	delay = 0.0;
	fifo->delta = time;
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
fts_midiport_add_listener(fts_midiport_t *port, enum midi_type type, int chan, int num, fts_object_t *obj, fts_method_t fun)
{
  fts_midiport_listener_t **type_list = port->listeners[type + 1]; 
  fts_midiport_listener_t **list = type_list + (num + 1) * (n_midi_channels + 1) + chan;

  add_listener(list, obj, fun);
}

void
fts_midiport_remove_listener(fts_midiport_t *port, enum midi_type type, int chan, int num, fts_object_t *obj)
{
  fts_midiport_listener_t **type_list = port->listeners[type + 1]; 
  fts_midiport_listener_t **list = type_list + (num + 1) * (n_midi_channels + 1) + chan;

  if (NULL != list)
  {
    remove_listener(list, obj);
  }
}

void
fts_midiport_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiport_t *port = (fts_midiport_t *)o;
  fts_midievent_t *event = (fts_midievent_t *)fts_get_object(at);
  int type = fts_midievent_get_type(event);
  fts_midiport_listener_t **type_list = port->listeners[type + 1]; 
  fts_midiport_listener_t *l;
  int channel = fts_midievent_channel_message_get_channel(event);

  if(type <= midi_control_change)
  {
    /* fire number listeners (note, poly pressure, control_change only) */
    int number = fts_midievent_channel_message_get_first(event); /* note or controller number */
    int onset = (number + 1) * (n_midi_channels + 1);

    /* fire callbacks for given channel and number */
    l = type_list[onset + channel];
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

  if(type <= midi_pitch_bend)
  {
    /* fire callbacks for given channel and any number */
    l = type_list[channel];
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
  fts_class_message_varargs(cl, fts_s_midievent, fts_midiport_input);
}

int
fts_object_is_midiport(fts_object_t *obj)
{
  return (fts_class_get_method_varargs(fts_object_get_class(obj), fts_s_midievent) == fts_midiport_input);
}

/****************************************************
 *
 *  internal MIDI port class
 *
 */
static fts_class_t *midibus_type = NULL;

static void
midibus_output(fts_object_t *o, fts_midievent_t *event, double time)
{
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

static void
midibus_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_midiport_t), midibus_init, midibus_delete);
}

/****************************************************
 *
 *  null MIDI port class
 *
 */
static fts_class_t *midinull_type = NULL;

static void
midinull_output(fts_object_t *o, fts_midievent_t *event, double time)
{
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

static void
midinull_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_midiport_t), midinull_init, midinull_delete);
}

/************************************************************
 *
 *  MIDI label
 *
 */

static fts_midilabel_t *
midilabel_new(fts_symbol_t name)
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
midilabel_delete(fts_midilabel_t *label)
{
  if(label->input != NULL)
    fts_object_release(label->input);

  if(label->output != NULL)
    fts_object_release(label->output);

  fts_free(label);
}

static void
midilabel_set_input(fts_midilabel_t *label, fts_midiport_t *port, fts_symbol_t name)
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
midilabel_set_output(fts_midilabel_t *label, fts_midiport_t *port, fts_symbol_t name)
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

/* current MIDI configuration */
/* static midiconfig_t *midiconfig = NULL; */

/* array of device names */
static fts_objectlist_t midiconfig_listeners;

/* array of device names */
static fts_array_t midiconfig_inputs;
static fts_array_t midiconfig_outputs;

/* list of MIDI managers */
static fts_midimanager_t *midimanagers = NULL;

/* MIDI port used for invalid references */
static fts_midiport_t *midinull = NULL;

static fts_symbol_t midiconfig_s_name;
static fts_symbol_t midiconfig_s_internal_bus;
static fts_symbol_t midiconfig_s_inputs;
static fts_symbol_t midiconfig_s_outputs;

fts_symbol_t fts_midimanager_s_get_default_input;
fts_symbol_t fts_midimanager_s_get_default_output;
fts_symbol_t fts_midimanager_s_append_input_names;
fts_symbol_t fts_midimanager_s_append_output_names;
fts_symbol_t fts_midimanager_s_get_input;
fts_symbol_t fts_midimanager_s_get_output;

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
      fts_send_message((fts_object_t *)mm, fts_midimanager_s_get_input, 3, args);
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
      fts_send_message((fts_object_t *)mm, fts_midimanager_s_get_output, 3, args);
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
    fts_send_message((fts_object_t *)mm, fts_midimanager_s_append_input_names, 1, &arg);

  fts_set_pointer(&arg, &midiconfig_outputs);
  
  for(mm = midimanagers; mm != NULL; mm = mm->next)
    fts_send_message((fts_object_t *)mm, fts_midimanager_s_append_output_names, 1, &arg);
}

static fts_symbol_t
midimanagers_get_default_input(void)
{
  fts_symbol_t name = NULL;
  fts_midimanager_t *mm;
  fts_atom_t arg;
  
  fts_set_pointer(&arg, &name);

  for(mm = midimanagers; mm != NULL && name == NULL; mm = mm->next)
    fts_send_message((fts_object_t *)mm, fts_midimanager_s_get_default_input, 1, &arg);

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
    fts_send_message((fts_object_t *)mm, fts_midimanager_s_get_default_output, 1, &arg); 

  return name;
}

static fts_midilabel_t *
midiconfig_label_get_by_index(fts_midiconfig_t *config, int index)
{
  fts_midilabel_t *label =  config->labels;

  while(label && index--)
    label = label->next;

  return label;
}

static fts_midilabel_t *
midiconfig_label_get_by_name(fts_midiconfig_t *config, fts_symbol_t name)
{
  fts_midilabel_t *label =  config->labels;

  while(label && label->name != name)
    label = label->next;

  return label;
}

static fts_midilabel_t *
midiconfig_label_insert(fts_midiconfig_t *config, int index, fts_symbol_t name)
{
  fts_midilabel_t **p = &config->labels;
  fts_midilabel_t *label = midilabel_new(name);
  int n = index;

  label->input_name = fts_s_unconnected;
  label->output_name = fts_s_unconnected;

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
    fts_set_symbol(args + 2, fts_s_unconnected);
    fts_set_symbol(args + 3, fts_s_unconnected);
    fts_client_send_message((fts_object_t *)config, fts_s_set, 4, args);
  }
  
  fts_config_set_dirty( (fts_config_t *)fts_config_get(), 1);

  return label;
}

static void
midiconfig_label_remove(fts_midiconfig_t *config, int index)
{
  fts_midilabel_t **p = &config->labels;
  int n = index;

  /* remove label and send to client */
  while((*p) && n--)
    p = &(*p)->next;

  if(*p) 
  {
    fts_midilabel_t *label = *p;
      
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

  fts_config_set_dirty( (fts_config_t *)fts_config_get(), 1);
}

static void
midiconfig_label_set_input(fts_midiconfig_t *config, fts_midilabel_t *label, int index, fts_midiport_t *midiport, fts_symbol_t name)
{
  if(midiport == NULL)
    name = fts_s_unconnected;

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

    fts_config_set_dirty( (fts_config_t *)fts_config_get(), 1);
  }
}

static void
midiconfig_label_set_output(fts_midiconfig_t *config, fts_midilabel_t *label, int index, fts_midiport_t *midiport, fts_symbol_t name)
{
  if(midiport == NULL)
    name = fts_s_unconnected;
  
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
    
    fts_config_set_dirty( (fts_config_t *)fts_config_get(), 1);
  }
}

static void
midiconfig_label_set_internal(fts_midiconfig_t *config, fts_midilabel_t *label, int index)
{
  /* create internal MIDI port */
  fts_midiport_t *port = (fts_midiport_t *)fts_object_create(midibus_type, 0, 0);

  /* set input and output to internal MIDI port */
  midiconfig_label_set_input(config, label, index, port, midiconfig_s_internal_bus);
  midiconfig_label_set_output(config, label, index, port, midiconfig_s_internal_bus);
}

static void
midiconfig_set_input(fts_midiconfig_t *config, int index, fts_symbol_t name)
{
  fts_midilabel_t *label = midiconfig_label_get_by_index(config, index);  

  /* reset output to none if it is set to internal */
  if(name != midiconfig_s_internal_bus && label->output && label->output_name == midiconfig_s_internal_bus)
    midiconfig_label_set_output(config, label, index, NULL, NULL);

  if(name == fts_s_unconnected)
    midiconfig_label_set_input(config, label, index, NULL, NULL);
  else if(name == midiconfig_s_internal_bus)
    midiconfig_label_set_internal(config, label, index);
  else
    midiconfig_label_set_input(config, label, index, midimanagers_get_input(name, label->name), name);
}

static void
midiconfig_set_output(fts_midiconfig_t *config, int index, fts_symbol_t name)
{
  fts_midilabel_t *label = midiconfig_label_get_by_index(config, index);

  /* reset output to none if it is set to internal */
  if(name != midiconfig_s_internal_bus && label->input && label->input_name == midiconfig_s_internal_bus)
    midiconfig_label_set_input(config, label, index, NULL, NULL);

  if(name == fts_s_unconnected)
    midiconfig_label_set_output(config, label, index, NULL, NULL);
  else if(name == midiconfig_s_internal_bus)
    midiconfig_label_set_internal(config, label, index);
  else
    midiconfig_label_set_output(config, label, index, midimanagers_get_output(name, label->name), name);
}

/* name utility */
static fts_symbol_t
midiconfig_get_fresh_label_name(fts_midiconfig_t *config, fts_symbol_t name)
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
    name = fts_new_symbol(new_str);
  }

  return name;
}

static void
midiconfig_restore(fts_midiconfig_t *config)
{
  fts_objectlist_cell_t *p = fts_objectlist_get_head(&midiconfig_listeners);

  while(p != NULL)
  {
    fts_objectlist_cell_t *next = fts_objectlist_get_next(p);

    fts_object_recompute(fts_objectlist_get_object(p));

    p = next;
  }
}

static void
midiconfig_update_devices(fts_midiconfig_t *config)
{
  int ac;
  fts_atom_t *at;
  
  fts_array_clear(&midiconfig_inputs);
  fts_array_clear(&midiconfig_outputs);

  fts_array_append_symbol(&midiconfig_inputs, fts_s_unconnected);
  fts_array_append_symbol(&midiconfig_outputs, fts_s_unconnected);

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
midiconfig_update_labels(fts_midiconfig_t *config)
{
  fts_midilabel_t *label = config->labels;
  int n = config->n_labels;
  int i;

  /* check inout and output midiports */
  for(i=0; i<n; i++) 
  {
    if(label->input == NULL || fts_object_get_class((fts_object_t *)label->input) != midibus_type) 
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
midiconfig_erase_labels(fts_midiconfig_t *config)
{
  fts_midilabel_t *label = config->labels;
  
  while(label != NULL)
  {
    fts_midilabel_t *next = label->next;
      
    midilabel_delete(label);
    label = next;
  }
  
  config->labels = NULL;
  config->n_labels = 0;
}

void
fts_midiconfig_set_defaults(fts_midiconfig_t* midiconfig)
{
  if(midiconfig != NULL)
  {
    fts_midilabel_t *label = midiconfig_label_get_by_index(midiconfig, 0);
      
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

    fts_config_set_dirty( (fts_config_t *)fts_config_get(), 0);
  }
}

/* midi manager API */
void
fts_midiconfig_update()
{
  fts_midiconfig_t* midiconfig = (fts_midiconfig_t*)fts_midiconfig_get();
  
  midiconfig_update_labels(midiconfig);
  midiconfig_update_devices(midiconfig);
}

void
fts_midiconfig_add_manager(fts_midimanager_t *mm)
{
  fts_midimanager_t **p = &midimanagers;
  
  while(*p != NULL)
    p = &((*p)->next);

  *p = mm;
  mm->next = NULL;

  /* update default devices */
  fts_midiconfig_set_defaults((fts_midiconfig_t*)fts_midiconfig_get());
}

/* midi objects API */
fts_midiport_t *
fts_midiconfig_get_input(fts_symbol_t name)
{
  fts_midiconfig_t* midiconfig = (fts_midiconfig_t*)fts_midiconfig_get();
  
  if(midiconfig != NULL)
  {
    fts_midilabel_t *label = midiconfig_label_get_by_name(midiconfig, name);
      
    if(label != NULL && label->input != NULL)
      return label->input;
  }

  return midinull;
}

fts_midiport_t *
fts_midiconfig_get_output(fts_symbol_t name)
{
  fts_midiconfig_t* midiconfig = (fts_midiconfig_t*)fts_midiconfig_get();
  
  if(midiconfig != NULL)
  {
    fts_midilabel_t *label = midiconfig_label_get_by_name(midiconfig, name);
      
    if(label != NULL && label->output != NULL)
      return label->output;
  }
  
  return midinull;
}

void
fts_midiconfig_add_listener(fts_object_t *obj)
{
  fts_objectlist_insert(&midiconfig_listeners, obj);
}

void
fts_midiconfig_remove_listener(fts_object_t *obj)
{
  fts_objectlist_remove(&midiconfig_listeners, obj);
}

/* fts_object_t * */
/* fts_midiconfig_get(void) */
/* { */
/*   return (fts_object_t *)midiconfig; */
/* } */

void
fts_midiconfig_set(fts_midiconfig_t *config)
{
  fts_midiconfig_t* midiconfig = (fts_midiconfig_t*)fts_midiconfig_get();
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
      
      fts_send_message( (fts_object_t *)config, fts_s_upload, 0, 0);
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
fts_class_t *fts_midiconfig_class;

static void
midiconfig_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiconfig_t *this = (fts_midiconfig_t *)o;

  midiconfig_erase_labels(this);
  midiconfig_label_insert(this, 0, fts_s_default);  

  if(fts_object_has_id( o)) 
    fts_client_send_message( o, fts_s_clear, 0, 0);
  
  fts_config_set_dirty( (fts_config_t *)fts_config_get(), 1);
}

static void
midiconfig_restore_label(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiconfig_t *this = (fts_midiconfig_t *)o;
  fts_symbol_t name = fts_get_symbol(at);
  fts_symbol_t input = fts_get_symbol(at + 1);
  fts_symbol_t output = fts_get_symbol(at + 2);
  int index = this->n_labels;
  
  if( name == fts_s_default)
    {
      if( midiconfig_label_get_by_name( this, name) == NULL)
	midiconfig_label_insert(this, 0, name);
      
      midiconfig_set_input(this, 0, input);
      midiconfig_set_output(this, 0, output);
    }  
  else
    {
      /* make sure that first label is "default" */
      if(index == 0)
	{
	  midiconfig_label_insert(this, 0, fts_s_default);
	  index = 1;
	}

      midiconfig_label_insert(this, index, name);
      midiconfig_set_input(this, index, input);
      midiconfig_set_output(this, index, output);
    }
}

static void
midiconfig_insert_label(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiconfig_t *this = (fts_midiconfig_t *)o;
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
  fts_midiconfig_t *this = (fts_midiconfig_t *)o;
  int index = fts_get_int(at);

  midiconfig_label_remove(this, index);

  midiconfig_update_labels(this);
  midiconfig_update_devices(this);
  midiconfig_restore(this);
}

static void
midiconfig_input( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiconfig_t *this = (fts_midiconfig_t *)o;
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
  fts_midiconfig_t *this = (fts_midiconfig_t *)o;
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
  fts_midiconfig_t *this = (fts_midiconfig_t *)o;
  fts_midilabel_t *label = this->labels;
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

  midiconfig_update_labels( this);
  midiconfig_update_devices( this);
}


void
fts_midiconfig_dump( fts_midiconfig_t *this, fts_bmax_file_t *f)
{
  fts_midilabel_t *label = this->labels;
  
  while(label) 
  {
    /*code insert message for each label */
    /*
      here symbol order must be inverse of restore method symbol order 
      so in restore_method (selector fts_s_label):
      label->name = fts_get_symbol(at);
      label->input_name = fts_get_symbol(at+1);
      label->output_name = fts_get_symbol(at+2);
    */
    fts_bmax_code_push_symbol(f, label->output_name);
    fts_bmax_code_push_symbol(f, label->input_name);
    fts_bmax_code_push_symbol(f, label->name);
    fts_bmax_code_push_symbol(f, fts_s_label);
    fts_bmax_code_obj_mess(f, fts_s_midi_config, 4);
    fts_bmax_code_pop_args(f, 4);
      
    label = label->next;	  
  }
}

static void
midiconfig_set_to_defaults( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiconfig_clear( o, winlet, fts_s_clear, 0, 0);
  /* this->file_name = NULL; */
  midiconfig_upload( o, winlet, fts_s_upload, 0, 0); 
  
  fts_config_set_dirty( (fts_config_t *)fts_config_get(), 0);
}

static void
midiconfig_print( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiconfig_t *this = (fts_midiconfig_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);  
  fts_midilabel_t *label = this->labels;
  fts_midimanager_t *mm;

  fts_spost(stream, "labels\n");
  for(label = this->labels; label != NULL; label = label->next) 
    fts_spost(stream, "  %s: '%s' '%s'\n", label->name, label->input_name, label->output_name);

  /* redirect to MIDI managers */
  for(mm = midimanagers; mm != NULL; mm = mm->next)
    fts_send_message((fts_object_t *)mm, fts_s_print, ac, at);
}

static void
midiconfig_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiconfig_t *this = (fts_midiconfig_t *)o;
  fts_atom_t a;
      
  this->labels = NULL;
  this->n_labels = 0;

  /* modify object description */
  fts_set_symbol(&a, midiconfig_s_name);
  fts_object_set_description(o, 1, &a);
}

static void
midiconfig_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiconfig_t *this = (fts_midiconfig_t *)o;

  midiconfig_erase_labels(this);
}

static void
midiconfig_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_midiconfig_t), midiconfig_init, midiconfig_delete);

  fts_class_message_varargs(cl, fts_s_clear, midiconfig_clear);
  fts_class_message_varargs(cl, fts_s_default, midiconfig_set_to_defaults);

  fts_class_message_varargs(cl, fts_s_label, midiconfig_restore_label);
  fts_class_message_varargs(cl, fts_s_insert, midiconfig_insert_label);
  fts_class_message_varargs(cl, fts_s_remove, midiconfig_remove_label);
  fts_class_message_varargs(cl, fts_s_input, midiconfig_input);
  fts_class_message_varargs(cl, fts_s_output, midiconfig_output);
  fts_class_message_varargs(cl, fts_s_upload, midiconfig_upload);

  fts_class_message_varargs(cl, fts_s_print, midiconfig_print);
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
  int i;
  
  fts_midi_types[midi_note] = fts_new_symbol("note");
  fts_midi_types[midi_poly_pressure] = fts_new_symbol("poly");
  fts_midi_types[midi_control_change] = fts_new_symbol("ctl");
  fts_midi_types[midi_program_change] = fts_new_symbol("prg");
  fts_midi_types[midi_channel_pressure] = fts_new_symbol("touch");
  fts_midi_types[midi_pitch_bend] = fts_new_symbol("bend");
  fts_midi_types[midi_system_exclusive] = fts_new_symbol("sysex");
  fts_midi_types[midi_time_code] = fts_new_symbol("mtc");
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

  midibus_type = fts_class_install(NULL, midibus_instantiate);
  midinull_type = fts_class_install(NULL, midinull_instantiate);
  
  midiconfig_s_name = fts_new_symbol("__midiconfig");
  midiconfig_s_internal_bus = fts_new_symbol("Internal Bus");
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
  fts_midiconfig_class = fts_class_install(midiconfig_s_name, midiconfig_instantiate);

  /* create global NULL MIDI port */
  midinull = (fts_midiport_t *)fts_object_create(midinull_type, 0, 0);
  fts_object_refer((fts_object_t *)midinull);

  fts_objectlist_init(&midiconfig_listeners);
}
