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

#include <fts/fts.h>

typedef struct _midiout_
{
  fts_object_t o;
  fts_midiport_t *port;
  int channel;
  int number;
} midiout_t;

static fts_symbol_t sym_omni = 0;
static fts_midievent_t *midievent_general_midi = 0;

static void
midiout_send_event(midiout_t *this, fts_midievent_t *event, double time)
{
  fts_object_refer((fts_object_t *)event);

  if(this->port != NULL)
    fts_midiport_output(this->port, event, time);
  else
    fts_outlet_object((fts_object_t *)this, 0, (fts_object_t *)event);

  fts_object_release((fts_object_t *)event);
}

static void
midiout_set_channel(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;
  int channel = fts_get_number_int(at);
  
  if(channel < 1)
    this->channel = 1;
  else if(channel > 16)
    this->channel = 16;
  else
    this->channel = channel;
}

static void
midiout_set_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;
  int number = fts_get_number_int(at);
  
  if(number < 0)
    this->number = 0;
  else if(number > 127)
    this->number = 127;
  else
    this->number = number;
}

static void
midiout_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;
  fts_midievent_t *event = (fts_midievent_t *)fts_get_object(at);

  fts_midiport_output(this->port, event, 0.0);
}

static void
midiout_panic(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiout_t *this = (midiout_t *)o;
  int i;
  
  for(i=0; i<n_midi_channels; i++)
    fts_midiport_output(this->port, fts_midievent_control_change_new(i, 120, 0), 0.0);
}

static void
midiout_general_midi(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiout_t *this = (midiout_t *)o;

  fts_midiport_output(this->port, midievent_general_midi, 0.0);
}

static void
noteout_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;

  switch (ac)
    {
    default:
    case 3:
      if(fts_is_number(at + 2))
	midiout_set_channel(o, 0, 0, 1, at + 2);
    case 2:
      if(fts_is_number(at + 1))
	midiout_set_number(o, 0, 0, 1, at + 1);
    case 1:
      if(fts_is_number(at + 0))
	{
	  int note = fts_get_number_int(at);

	  if(note < 0)
	    note = 0;
	  else if(note > 127)
	    note = 127;
	    
	  midiout_send_event(this, fts_midievent_note_new(this->channel, note, this->number), 0.0);
	}
    case 0:
      break;
    }
}

static void
noteout_all_off(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiout_t *this = (midiout_t *)o;
  int channel = 0;

  if(ac > 0 && fts_is_number(at))
    channel = fts_get_number_int(at);

  if(channel > 16)
    channel = 16;

  if(channel <= 0)
    {
      int i;

      for(i=0; i<n_midi_channels; i++)
	midiout_send_event(this, fts_midievent_control_change_new(i, 123, 0), 0.0);
    }
  else
    {
      midiout_set_channel(o, 0, 0, 1, at);
      midiout_send_event(this, fts_midievent_control_change_new(channel, 123, 0), 0.0);
    }    
}

static void
polyout_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;

  switch (ac)
    {
    default:
    case 3:
      if(fts_is_number(at + 2))
	midiout_set_channel(o, 0, 0, 1, at + 2);
    case 2:
      if(fts_is_number(at + 1))
	midiout_set_number(o, 0, 0, 1, at + 1);
    case 1:
      if(fts_is_number(at + 0))
	{
	  int value = fts_get_number_int(at);
	    
	  if(value < 0)
	    value = 0;
	  else if(value > 127)
	    value = 127;
	    
	  midiout_send_event(this, fts_midievent_poly_pressure_new(this->channel, this->number, value), 0.0);
	}
    case 0:
      break;
    }
}

static void
ctlout_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;

  switch (ac)
    {
    default:
    case 3:
      if(fts_is_number(at + 2))
	midiout_set_channel(o, 0, 0, 1, at + 2);
    case 2:
      if(fts_is_number(at + 1))
	midiout_set_number(o, 0, 0, 1, at + 1);
    case 1:
      if(fts_is_number(at + 0))
	{
	  int value = fts_get_number_int(at);
	  
	  if(value < 0)
	    value = 0;
	  else if(value > 127)
	    value = 127;
	  
	  midiout_send_event(this, fts_midievent_control_change_new(this->channel, this->number, value), 0.0);
	}
    case 0:
      break;
    }
}

static void
pgmout_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;

  switch (ac)
    {
    default:
    case 2:
      if(fts_is_number(at + 1))
	midiout_set_channel(o, 0, 0, 1, at + 1);
    case 1:
      if(fts_is_number(at + 0))
	{
	  int value = fts_get_number_int(at);
	    
	  if(value < 0)
	    value = 0;
	  else if(value > 127)
	    value = 127;
	    
	  midiout_send_event(this, fts_midievent_program_change_new(this->channel, value), 0.0);
	}
    case 0:
      break;
    }
}

static void
touchout_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;

  switch (ac)
    {
    default:
    case 2:
      if(fts_is_number(at + 1))
	midiout_set_channel(o, 0, 0, 1, at + 1);
    case 1:
      if(fts_is_number(at + 0))
	{
	  int value = fts_get_number_int(at);
	    
	  if(value < 0)
	    value = 0;
	  else if(value > 127)
	    value = 127;
	    
	  midiout_send_event(this, fts_midievent_channel_pressure_new(this->channel, value), 0.0);
	}
    case 0:
      break;
    }
}

static void
bendout_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;

  switch (ac)
    {
    default:
    case 2:
      if(fts_is_number(at + 1))
	midiout_set_channel(o, 0, 0, 1, at + 1);
    case 1:
      if(fts_is_number(at))
	{
	  int value = fts_get_number_int(at);
	    
	  if(value < 0)
	    value = 0;
	  else if(value > 127)
	    value = 127;
	    
	  midiout_send_event(this, fts_midievent_pitch_bend_new(this->channel, 0, value), 0.0);
	}
    case 0:
      break;
    }
}

static void
xbendout_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;

  switch (ac)
    {
    default:
    case 2:
      if(fts_is_number(at + 1))
	midiout_set_channel(o, 0, 0, 1, at + 1);
    case 1:
      if(fts_is_number(at))
	{
	  int value = fts_get_number_int(at);
	  
	  if(value < 0)
	    value = 0;
	  else if(value > 16383)
	    value = 16383;
	  
	  midiout_send_event(this, fts_midievent_pitch_bend_new(this->channel, value & 127, value >> 7), 0.0);
	}
    case 0:
      break;
    }
}

static void
bendout_range(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;
  fts_midievent_t *event[3];
  int range = 2;

  switch (ac)
    {
    default:
    case 2:
      if(fts_is_number(at + 1))
	midiout_set_channel(o, 0, 0, 1, at + 1);
    case 1:
      if(fts_is_number(at))
	{
	  range = fts_get_number_int(at);

	  if(range < 0)
	    range = 0;
	  else if(range > 24)
	    range = 24;
	}
    case 0:
      break;
    }  

  midiout_send_event(this, fts_midievent_control_change_new(this->channel, 100, 0), 0.0);
  midiout_send_event(this, fts_midievent_control_change_new(this->channel, 101, 0), 0.0);
  midiout_send_event(this, fts_midievent_control_change_new(this->channel, 6, range), 0.0);
}

/************************************************************
 *
 *  class
 *
 */

static void
midiout_get_number_and_channel(fts_object_t *o, int ac, const fts_atom_t *at)
{ 
  if(ac >= 2 && fts_is_number(at)) 
    {
      midiout_set_number(o, 0, 0, 1, at);
      
      /* skip number argument */
      ac--;
      at++;
    }
  
  if(ac >= 1 && fts_is_number(at)) 
    midiout_set_channel(o, 0, 0, 1, at);
}

static void
midiout_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiout_t *this = (midiout_t *)o;

  this->port = NULL;
  this->channel = 0;
  this->number = 0;

  if(ac > 0 && fts_is_symbol(at)) 
    {
      fts_symbol_t label = fts_get_symbol(at);

      if(label != fts_s_minus)
	{
	  this->port = fts_midiconfig_get_output(label);      
	  midiout_get_number_and_channel(o, ac - 1, at + 1);
	}
    }
  else
    {
      this->port = fts_midiconfig_get_output(fts_s_default);
      midiout_get_number_and_channel(o, ac, at);
    }
  
  if(this->port)
    fts_midiconfig_add_listener(o);
}

static void
midiout_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(midiout_t), midiout_init, 0);

  fts_class_method_varargs(cl, fts_new_symbol("panic"), midiout_panic);
  fts_class_method_varargs(cl, fts_new_symbol("GM"), midiout_general_midi);
  fts_class_method_varargs(cl, fts_new_symbol("gm"), midiout_general_midi);

  fts_class_method_varargs(cl, fts_s_midievent, midiout_send);
}

static void
noteout_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(midiout_t), midiout_init, 0);

  fts_class_method_varargs(cl, fts_new_symbol("off"), noteout_all_off);

  fts_class_inlet_int(cl, 0, noteout_send);
  fts_class_inlet_float(cl, 0, noteout_send);
  fts_class_inlet_varargs(cl, 0, noteout_send);
  
  fts_class_inlet_int(cl, 1, midiout_set_number);
  fts_class_inlet_float(cl, 1, midiout_set_number);
  
  fts_class_inlet_int(cl, 2, midiout_set_channel);
  fts_class_inlet_float(cl, 2, midiout_set_channel);
}

static void
polyout_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(midiout_t), midiout_init, 0);

  fts_class_inlet_int(cl, 0, polyout_send);
  fts_class_inlet_float(cl, 0, polyout_send);
  fts_class_inlet_varargs(cl, 0, polyout_send);
  
  fts_class_inlet_int(cl, 1, midiout_set_number);
  fts_class_inlet_float(cl, 1, midiout_set_number);
  
  fts_class_inlet_int(cl, 2, midiout_set_channel);
  fts_class_inlet_float(cl, 2, midiout_set_channel);
}

static void
ctlout_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(midiout_t), midiout_init, 0);

  fts_class_inlet_int(cl, 0, ctlout_send);
  fts_class_inlet_float(cl, 0, ctlout_send);
  fts_class_inlet_varargs(cl, 0, ctlout_send);
  
  fts_class_inlet_int(cl, 1, midiout_set_number);
  fts_class_inlet_float(cl, 1, midiout_set_number);
  
  fts_class_inlet_int(cl, 2, midiout_set_channel);
  fts_class_inlet_float(cl, 2, midiout_set_channel);
}

static void
pgmout_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(midiout_t), midiout_init, 0);

  fts_class_inlet_int(cl, 0, pgmout_send);
  fts_class_inlet_float(cl, 0, pgmout_send);
  fts_class_inlet_varargs(cl, 0, pgmout_send);
  
  fts_class_inlet_int(cl, 1, midiout_set_channel);
  fts_class_inlet_float(cl, 1, midiout_set_channel);
}

static void
touchout_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(midiout_t), midiout_init, 0);

  fts_class_inlet_int(cl, 0, touchout_send);
  fts_class_inlet_float(cl, 0, touchout_send);
  fts_class_inlet_varargs(cl, 0, touchout_send);
  
  fts_class_inlet_int(cl, 1, midiout_set_channel);
  fts_class_inlet_float(cl, 1, midiout_set_channel);
}


static void
bendout_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(midiout_t), midiout_init, 0);

  fts_class_method_varargs(cl, fts_new_symbol("range"), bendout_range);

  fts_class_inlet_int(cl, 0, bendout_send);
  fts_class_inlet_float(cl, 0, bendout_send);
  fts_class_inlet_varargs(cl, 0, bendout_send);
  
  fts_class_inlet_int(cl, 1, midiout_set_channel);
  fts_class_inlet_float(cl, 1, midiout_set_channel);
}

static void
xbendout_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(midiout_t), midiout_init, 0);

  fts_class_method_varargs(cl, fts_new_symbol("range"), bendout_range);

  fts_class_inlet_int(cl, 0, xbendout_send);
  fts_class_inlet_float(cl, 0, xbendout_send);
  fts_class_inlet_varargs(cl, 0, xbendout_send);
  
  fts_class_inlet_int(cl, 1, midiout_set_channel);
  fts_class_inlet_float(cl, 1, midiout_set_channel);
}

void
midiout_config(void)
{
  sym_omni = fts_new_symbol("omni");

  midievent_general_midi = fts_midievent_system_exclusive_new(0, 0);
  fts_midievent_system_exclusive_append(midievent_general_midi, 126);
  fts_midievent_system_exclusive_append(midievent_general_midi, 0);
  fts_midievent_system_exclusive_append(midievent_general_midi, 9);
  fts_midievent_system_exclusive_append(midievent_general_midi, 1);
  fts_object_refer(midievent_general_midi); /* claim forever */

  fts_class_install(fts_new_symbol("midiout"), midiout_instantiate);

  fts_class_install(fts_new_symbol("noteout"), noteout_instantiate);
  fts_class_install(fts_new_symbol("polyout"), polyout_instantiate);
  fts_class_install(fts_new_symbol("ctlout"), ctlout_instantiate);
  fts_class_install(fts_new_symbol("pgmout"), pgmout_instantiate);
  fts_class_install(fts_new_symbol("touchout"), touchout_instantiate);
  fts_class_install(fts_new_symbol("bendout"), bendout_instantiate);
  fts_class_install(fts_new_symbol("xbendout"), xbendout_instantiate);
}
